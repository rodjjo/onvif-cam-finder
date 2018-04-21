/*
 * Copyright (C) 2018 by Rodrigo Antonio de Araujo
 */
#include <sstream>
#include <string>
#include "udp_radar/udp_radar_imp.h"


namespace udpradar {

UdpRadarImp::UdpRadarImp(
    const char *listen_address,
    const char *multicast_address,
    unsigned int port,
    ReceiverHandler handler
): listen_address_(listen_address),
   multicast_address_(multicast_address),
   port_(port),
   handler_(handler) {
}

UdpRadarImp::~UdpRadarImp() {
    stop();
}

void UdpRadarImp::send(const void *buffer, unsigned int size) {
    if (!thread_) {
        return;
    }
    std::shared_ptr<boost::asio::ip::udp::socket> socket(
        new boost::asio::ip::udp::socket(io_service_));
    boost::asio::socket_base::reuse_address reuse_addr_option(true);
    boost::asio::socket_base::broadcast broad_cast_option(true);

    boost::system::error_code error;
    socket->open(boost::asio::ip::udp::v4(), error);

    if (error) {
        return;
    }

    socket->set_option(reuse_addr_option);

    socket->set_option(
        boost::asio::ip::multicast::join_group(
            boost::asio::ip::address_v4::from_string(
                multicast_address_.c_str())));

    auto endpoint = boost::asio::ip::udp::endpoint(
        boost::asio::ip::address_v4::from_string(
            multicast_address_.c_str()), port_);

    std::shared_ptr<unsigned char> data(
        new unsigned char[size], []( unsigned char *p ) { delete[] p; } );

    socket->async_send_to(boost::asio::buffer(data.get(), size), endpoint,
                [data, socket] (
                    const boost::system::error_code& error,
                    std::size_t transferred) {
                });
}

void UdpRadarImp::start() {
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

void UdpRadarImp::stop() {
    if (thread_) {
        work_.reset();
        thread_->join();
        thread_.reset();
    }
}

void UdpRadarImp::start_receive() {
    auto endpoint = boost::asio::ip::udp::endpoint(
        boost::asio::ip::address_v4::from_string(
                listen_address_.c_str()), port_);

    std::shared_ptr<boost::asio::ip::udp::socket> socket(
        new boost::asio::ip::udp::socket(io_service_, endpoint));

    boost::system::error_code error;
    socket->open(boost::asio::ip::udp::v4(), error);

    if (error) {
        return;
    }

    boost::asio::socket_base::reuse_address reuse_addr_option(true);
    socket->set_option(reuse_addr_option);

    socket->set_option(
        boost::asio::ip::multicast::join_group(
            boost::asio::ip::address_v4::from_string(
                multicast_address_.c_str())));

    std::shared_ptr<array_64> buffer(new array_64());

    receive(socket, buffer);
}

void UdpRadarImp::receive(
    std::shared_ptr<boost::asio::ip::udp::socket> socket,
    std::shared_ptr<array_64> buffer
) {
    std::shared_ptr<boost::asio::ip::udp::endpoint> remote_endpoint(
        new boost::asio::ip::udp::endpoint());

    socket->async_receive_from(
        boost::asio::buffer(*buffer.get()),
        *remote_endpoint.get(), [
            this, remote_endpoint, socket, buffer
        ] (const boost::system::error_code& error, std::size_t transferred) {
            if (error) {
                handler_(NULL, 0, error.value());
            } else {
                receive(socket, buffer);

                std::stringstream content;
                content << &(*(buffer.get()));
                auto data = content.str();

                handler_(data.data(), data.size(), 0);
            }
        });
}

}  // namespace udpradar
