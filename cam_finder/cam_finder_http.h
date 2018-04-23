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
    const std::string& response, const boost::system::error_code& ec
)> http_handler_t;

class Http: public boost::noncopyable {
 public:
    explicit Http(boost::asio::io_service* io_service);
    virtual ~Http();
    void post(
        const std::string& device_url,
        const std::string& xml,
        http_handler_t handler);
 private:
    boost::asio::io_service* io_service_;
};

}  // namespace camfinder

#endif  // CAM_FINDER_CAM_FINDER_HTTP_H_
