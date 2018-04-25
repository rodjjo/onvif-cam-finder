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


typedef struct {
    std::string profile_token;
    std::string profile_name;
    std::string stream_uri;
    std::string width;
    std::string height;
    std::string fps;
} stream_info_t;

typedef std::list<stream_info_t> stream_list_t;

typedef std::function<void(
    const std::string& device,
    const stream_list_t& streams,
    int error_code)> device_info_handler_t;


class CamFinder {
 public:
    virtual ~CamFinder();
    virtual void search() = 0;
    virtual void query_profiles(
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
    device_info_handler_t handler);

}  // namespace camfinder

#endif  // CAM_FINDER_CAMFINDER_CAMFINDER_H_
