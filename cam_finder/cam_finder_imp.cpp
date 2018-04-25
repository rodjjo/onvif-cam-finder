/*
 * Copyright (C) 2018 by Rodrigo Antonio de Araujo
 */
#include <sstream>
#include <string>

#include "cam_finder/cam_finder_imp.h"
#include "cam_finder/cam_finder_http.h"
#include "cam_finder/cam_finder_parsers.h"
#include "cam_finder/request_messages.h"

namespace camfinder {

CamFinderImp::CamFinderImp(
    const char *listen_address,
    const char *multicast_address,
    unsigned int port,
    device_info_handler_t handler
): listen_address_(listen_address),
   multicast_address_(multicast_address),
   port_(port),
   handler_(handler) {
}

CamFinderImp::~CamFinderImp() {
    stop();
}

void CamFinderImp::find_cameras() {
    if (!thread_) {
        return;
    }

    boost::asio::io_service io_service;

    std::shared_ptr<boost::asio::ip::udp::socket> socket(
        new boost::asio::ip::udp::socket(io_service));

    auto v4_multi_address = boost::asio::ip::address_v4::from_string(
                multicast_address_.c_str());

    boost::system::error_code error;
    socket->open(boost::asio::ip::udp::v4(), error);

    if (error) {
        return;
    }

    // TTL
    socket->set_option(boost::asio::ip::multicast::hops(4));

    socket->set_option(
        boost::asio::ip::multicast::enable_loopback(false));

    socket->set_option(
        boost::asio::socket_base::reuse_address(true));

    auto v4_listen_address = boost::asio::ip::address_v4::from_string(
        listen_address_.c_str());

    auto listen_endpoint = boost::asio::ip::udp::endpoint(
        boost::asio::ip::udp::v4(), port_);

    socket->bind(listen_endpoint);

    auto endpoint = boost::asio::ip::udp::endpoint(
        v4_multi_address, port_);

    const char* discovery_message = get_discovery_message();

    socket->async_send_to(boost::asio::buffer(
            discovery_message, strlen(discovery_message) + 1), endpoint,
                [socket] (
                    const boost::system::error_code& error,
                    std::size_t transferred) {
                    if (error) {
                        printf("Sent Error %d\n", error.value());
                    } else {
                        printf("Sent Success\n");
                    }
                });

    io_service.run_one();
}

void CamFinderImp::start() {
    if (thread_) {
        return;
    }
    work_.reset(new boost::asio::io_service::work(io_service_));
    thread_.reset(new boost::thread([this]() {
        io_service_.reset();
        start_discovery();
        io_service_.run();
    }));
}

void CamFinderImp::stop() {
    if (thread_) {
        work_.reset();
        io_service_.stop();
        thread_->join();
        thread_.reset();
    }
}

void CamFinderImp::start_discovery() {
    auto v4_listen_address = boost::asio::ip::address_v4::from_string(
            listen_address_.c_str());

    auto listen_endpoint = boost::asio::ip::udp::endpoint(
        boost::asio::ip::udp::v4(), port_);

    auto v4_multi_address = boost::asio::ip::address_v4::from_string(
            multicast_address_.c_str());

    std::shared_ptr<boost::asio::ip::udp::socket> socket(
        new boost::asio::ip::udp::socket(io_service_));

    boost::system::error_code error;
    socket->open(listen_endpoint.protocol(), error);

    if (error) {
        handler_(std::string(), stream_list_t(), error.value());
    }

    socket->set_option(
        boost::asio::ip::udp::socket::reuse_address(true));

    socket->bind(listen_endpoint);

    socket->set_option(
        boost::asio::ip::multicast::join_group(
            v4_multi_address, v4_listen_address));

    std::shared_ptr<array_2k> buffer(new array_2k());

    receive_discovery(socket, buffer);
}

void CamFinderImp::receive_discovery(
    std::shared_ptr<boost::asio::ip::udp::socket> socket,
    std::shared_ptr<array_2k> buffer
) {
    std::shared_ptr<boost::asio::ip::udp::endpoint> remote_endpoint(
        new boost::asio::ip::udp::endpoint());

    socket->async_receive_from(
        boost::asio::buffer(*buffer.get()),
        *remote_endpoint.get(), [
            this, remote_endpoint, socket, buffer
        ] (const boost::system::error_code& error, std::size_t transferred) {
            if (error) {
                handler_(std::string(), stream_list_t(), error.value());
            } else {
                auto data = std::string(
                    buffer->begin(), buffer->begin() + transferred);

                receive_discovery(socket, buffer);

                std::size_t tag_start = data.find("<d:XAddrs>");

                if (tag_start == std::string::npos) {
                    return;
                }

                std::size_t tag_end = data.find("</d:XAddrs>");

                if (tag_end == std::string::npos) {
                    return;
                }

                query_profiles(
                    data.substr(tag_start + 10, tag_end - tag_start - 10),
                    "", "");
            }
        });
}


void CamFinderImp::query_profiles(
    const std::string& device_url,
    const std::string& username,
    const std::string& password
) {
    printf("querying profiles...\n");
    http::post(
        &io_service_,
        device_url,
        query_profiles_message(username, password),
        [this, device_url, username, password] (
            const std::string& response,
            int ec
        ) {
            if (ec || response.empty()) {
                printf("error getting profiles %d...\n", ec);
                handler_(device_url, stream_list_t(), 0);
                return;
            }

            std::shared_ptr<profile_list_t> profiles(
                new profile_list_t(parser::get_profiles(response)));

            std::shared_ptr<stream_list_t> streams(new stream_list_t());

            printf("Finding streams ...\n");

            query_profiles_streams(
                device_url,
                username,
                password,
                profiles,
                streams,
                [this, device_url] (
                    std::shared_ptr<profile_list_t> profiles,
                    std::shared_ptr<stream_list_t> streams,
                    int error
                ) {
                    handler_(device_url, *streams.get(), 0);
                });
        });
}

void CamFinderImp::query_profiles_streams(
        const std::string& device_url,
        const std::string& username,
        const std::string& password,
        std::shared_ptr<profile_list_t> profiles,
        std::shared_ptr<stream_list_t> streams,
        query_stream_handler_t handler) {
    if (profiles->empty()) {
        handler(profiles, streams, 0);
        return;
    }

    auto current_profile = *profiles->begin();
    profiles->pop_front();

    http::post(
        &io_service_,
        device_url,
        query_profiles_stream_message(
            username, password, current_profile.token),
        [this, device_url, username, password,
                handler, current_profile, streams, profiles] (
            const std::string& response,
            int ec
        ) {
            if (!ec) {
                stream_info_t stream_info = parser::get_stream_info(
                        current_profile.token, current_profile.name, response);
                if (!stream_info.stream_uri.empty()) {
                    streams->push_back(stream_info);
                }
            }

            query_profiles_streams(
                device_url,
                username,
                password,
                profiles,
                streams,
                handler);
        });
}

}  // namespace camfinder
