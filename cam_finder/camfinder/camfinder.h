/*
 * Copyright (C) 2018 by Rodrigo Antonio de Araujo
 */
#ifndef CAM_FINDER_CAMFINDER_CAMFINDER_H_
#define CAM_FINDER_CAMFINDER_CAMFINDER_H_

#include <memory>
#include <functional>
#include <string>
#include <list>
#include <boost/noncopyable.hpp>


namespace camfinder {

typedef std::list<std::string> stream_list_t;

typedef std::function<void(
    const std::string& device,
    const stream_list_t& streams,
    int error_code)> ReceiverHandler;


class CamFinder {
 public:
    virtual ~CamFinder();
    virtual void find_cameras() = 0;
    virtual void query_device(
                const std::string& device_url,
                const std::string& username,
                const std::string& password) = 0;
    virtual void start() = 0;
    virtual void stop() = 0;
};


std::shared_ptr<CamFinder> build(
    const char *listen_address,
    const char *multicast_address,
    unsigned int port,
    ReceiverHandler handler);


std::string query_stream_info_message(
    const std::string& host,
    const std::string& path,
    const std::string& username,
    const std::string& password);

}  // namespace camfinder

#endif  // CAM_FINDER_CAMFINDER_CAMFINDER_H_
