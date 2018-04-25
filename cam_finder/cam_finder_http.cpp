/*
 * Copyright (C) 2018 by Rodrigo Antonio de Araujo
 */
#include "cam_finder/cam_finder_http.h"

namespace camfinder {
namespace http {

typedef std::shared_ptr<boost::asio::streambuf> buffer_t;

buffer_t compose_request(
    const std::string& server,
    const std::string& path,
    const std::string& xml
) {
    buffer_t buffer(
        new boost::asio::streambuf());

    std::ostream request_stream(buffer.get());

    request_stream << "POST " << path << " HTTP/1.0\r\n";
    request_stream << "Host: " << server << "\r\n";
    request_stream << "Accept: */*\r\n";
    request_stream << "Connection: close\r\n\r\n";
    request_stream << xml;

    return buffer;
}

typedef std::function<void(
    const boost::system::error_code& error,
    boost::asio::ip::tcp::resolver::iterator iterator
)> resolver_handler_t;

void resolve(
    boost::asio::io_service* io_service,
    const std::string& server,
    resolver_handler_t handler
) {
    const char *address = server.c_str();
    const char *separator = strstr(address, ":");
    const char *service =  separator ? separator + 1 : "http";
    std::string host;
    if (separator) {
        host = std::string(address, separator);
    } else {
        host = server;
    }

    boost::asio::ip::tcp::resolver::query query(host, service);

    std::shared_ptr<boost::asio::ip::tcp::resolver> resolver(
        new boost::asio::ip::tcp::resolver(*io_service));

    resolver->async_resolve(query, [resolver, handler] (
        const boost::system::error_code& error,
        boost::asio::ip::tcp::resolver::iterator iterator
    ) {
        handler(error, iterator);
    });
}


typedef std::shared_ptr<boost::asio::ip::tcp::socket> tcp_socket_t;

typedef std::function<void(
    int error,
    tcp_socket_t socket
)> send_handler_t;

void send_request(
    boost::asio::io_service* io_service,
    boost::asio::ip::tcp::resolver::iterator iterator,
    buffer_t buffer,
    send_handler_t handler
) {
    std::shared_ptr<boost::asio::ip::tcp::socket> socket(
        new boost::asio::ip::tcp::socket(*io_service));

     boost::asio::async_connect(
        *socket.get(), iterator, [handler, buffer, socket] (
            const boost::system::error_code& err,
            boost::asio::ip::tcp::resolver::iterator resolver
        ) {
            if (err) {
                handler(err.value(), tcp_socket_t());
                return;
            }

            boost::asio::async_write(
                *socket.get(), *buffer.get(), [buffer, handler, socket] (
                    const boost::system::error_code& err,
                    std::size_t bytestransfered
                ) {
                    handler(err.value(), socket);
                });
        });
}

typedef std::function<void(
    int error,
    const std::string& response
)> response_handler_t;


std::string parse_http_response(std::stringstream* stream) {
    auto &data = *stream;

    unsigned int status_code = 0;
    data.seekp(0);
    std::string http_version;
    data >> http_version;
    data >> status_code;

    if (status_code < 200 || status_code >= 300) {
        return std::string();
    }

    std::string line;
    std::getline(data, line);

    if (!data.good() || http_version.substr(0, 5) != "HTTP/") {
        return std::string();
    }

    size_t separator_position;

    // skip headers
    while (data.good()) {
        std::getline(data, line);
        if (line.empty() || line == "\r" || line == "\r\n") {
            break;
        }
    }

    return data.str().substr(data.tellg());
}

void receive_response(
    tcp_socket_t socket,
    std::shared_ptr<boost::asio::streambuf> buffer,
    std::shared_ptr<std::stringstream> data,
    response_handler_t handler
) {
     boost::asio::async_read(
            *socket.get(),
            *buffer.get(),
            boost::asio::transfer_at_least(1),
            [socket, buffer, data, handler] (
                const boost::system::error_code& err, std::size_t transf
            ) {
                if (err.value() == boost::asio::error::eof) {
                    socket->shutdown(
                        boost::asio::ip::tcp::socket::shutdown_both);
                    boost::system::error_code ec;
                    socket->close(ec);
                    handler(0, parse_http_response(data.get()));
                } else if (err) {
                    handler(err.value(), std::string());
                } else {
                    if (transf) {
                        (*data.get()) << &(*buffer.get());
                    }
                    receive_response(socket, buffer, data, handler);
                }
            });
}

bool split_url(
    const std::string& device_url, std::string *server, std::string *path
) {
    const char *url = device_url.c_str();
    const char *p1 = strstr(url, "http://");

    if (!p1) {
        return false;
    }

    const char *p2 = strstr(p1 + 7, "/");
    if (p2) {
        *server = std::string(p1 + 7, p2);
        *path = p2;
    } else {
        *server = p1;
        *path = "/";
    }

    return true;
}

void post(
    boost::asio::io_service* io_service,
    const std::string& device_url,
    const std::string& xml,
    http_handler_t handler) {
    std::string server;
    std::string path;

    if (!split_url(device_url, &server, &path)) {
        handler("", 0);
        return;
    }

    buffer_t request(compose_request(server, path, xml));

    resolve(io_service, server, [io_service, request, handler] (
        const boost::system::error_code& error,
        boost::asio::ip::tcp::resolver::iterator iterator
    ) {
        if (error) {
            handler("", error.value());
            return;
        }

        send_request(io_service, iterator, request, [handler] (
            int error,
            tcp_socket_t socket
        ) {
            if (error) {
                handler("", error);
                return;
            }

            std::shared_ptr<boost::asio::streambuf> buffer(
                new boost::asio::streambuf());
            std::shared_ptr<std::stringstream> data(
                new std::stringstream());

            receive_response(socket, buffer, data, [handler] (
                int error,
                const std::string& response
            ) {
                if (error) {
                    handler("", error);
                    return;
                }

                handler(response, 0);
            });
        });
    });
}


}  // namespace http
}  // namespace camfinder
