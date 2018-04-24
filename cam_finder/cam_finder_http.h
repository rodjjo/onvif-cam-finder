/*
 * Copyright (C) 2018 by Rodrigo Antonio de Araujo
 */
#ifndef CAM_FINDER_CAM_FINDER_HTTP_H_
#define CAM_FINDER_CAM_FINDER_HTTP_H_

#include <functional>
#include <string>
#include <boost/asio.hpp>
#include <boost/noncopyable.hpp>

namespace camfinder {


typedef std::function<void(
    const std::string& response, int error
)> http_handler_t;

namespace http {

void post(
    boost::asio::io_service* io_service,
    const std::string& device_url,
    const std::string& xml,
    http_handler_t handler);

}  // namespace http

}  // namespace camfinder

#endif  // CAM_FINDER_CAM_FINDER_HTTP_H_
