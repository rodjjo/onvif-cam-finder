/*
 * Copyright (C) 2018 by Rodrigo Antonio de Araujo
 */
#include <sstream>
#include <string>

#include "cam_finder/cam_finder_imp.h"
#include "cam_finder/request_messages.h"

namespace camfinder {

CamFinderImp::CamFinderImp(
    const char *listen_address,
    const char *multicast_address,
    unsigned int port,
    ReceiverHandler handler
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

                query_device(
                    data.substr(tag_start + 10, tag_end - tag_start - 10),
                    "", "");
            }
        });
}


void CamFinderImp::query_device(
                const std::string& device_url,
                const std::string& username,
                const std::string& password
) {
    handler_(device_url, stream_list_t(), 0);
    // TODO(Rodrigo): query camip profiles
    // TODO(Rodrigo): for each profile query camip video stream uris
    // TODO(Rodrigo): notify discovered information
}

}  // namespace camfinder
