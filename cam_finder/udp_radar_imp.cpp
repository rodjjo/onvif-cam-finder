/*
 * Copyright (C) 2018 by Rodrigo Antonio de Araujo
 */
#include <sstream>
#include <string>
#include <tinyxml2.h>

#include "cam_finder/udp_radar_imp.h"


#define XML_DISCOVERY  \
    "<?xml version=\"1.0\" ?>\n"  \
    "<s:Envelope xmlns:a=\"http://schemas.xmlsoap.org/ws/2004/08/addressing\"" \
        " xmlns:d=\"http://schemas.xmlsoap.org/ws/2005/04/discovery\"" \
        " xmlns:s=\"http://www.w3.org/2003/05/soap-envelope\">\n" \
    "        <s:Header>\n" \
    "            <a:Action>" \
                  "http://schemas.xmlsoap.org/ws/2005/04/discovery/Probe" \
                "</a:Action>\n" \
    "                <a:MessageID>" \
      "urn:uuid:2b0bf1e1-d725-49a9-834a-52656c4b5011</a:MessageID>\n" \
    "                <a:To>" \
      "urn:schemas-xmlsoap-org:ws:2005:04:discovery</a:To>\n" \
    "        </s:Header>\n" \
    "        <s:Body>\n" \
    "                <d:Probe/>\n" \
    "        </s:Body>\n" \
    "</s:Envelope>\n"


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

    socket->async_send_to(boost::asio::buffer(
            XML_DISCOVERY, strlen(XML_DISCOVERY) + 1), endpoint,
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
        start_receive();
        io_service_.run();
    }));
}

void CamFinderImp::stop() {
    if (thread_) {
        work_.reset();
        thread_->join();
        thread_.reset();
    }
}

void CamFinderImp::start_receive() {
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

    receive(socket, buffer);
}

void CamFinderImp::receive(
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

                receive(socket, buffer);

                std::size_t tag_start = data.find("<d:XAddrs>");

                if (tag_start == std::string::npos) {
                    return;
                }

                std::size_t tag_end = data.find("</d:XAddrs>");

                if (tag_end == std::string::npos) {
                    return;
                }

                handler_(data.substr(tag_start + 10, tag_end - tag_start - 10),
                        stream_list_t(), 0);
            }
        });
}

}  // namespace camfinder
