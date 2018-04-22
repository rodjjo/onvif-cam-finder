/*
 * Copyright (C) 2018 by Rodrigo Antonio de Araujo
 */
#ifndef UDP_RADAR_UDPRADAR_UDPRADAR_H_
#define UDP_RADAR_UDPRADAR_UDPRADAR_H_

#include <memory>
#include <functional>
#include <string>
#include <list>
#include <boost/noncopyable.hpp>


namespace udpradar {


typedef std::list<std::string> stream_list_t;

typedef std::function<void(
    const std::string& device,
    const stream_list_t& streams,
    int error_code)> ReceiverHandler;


class CamFinder {
 public:
    virtual ~CamFinder();
    virtual void find_cameras() = 0;
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

}  // namespace udpradar

#endif  // UDP_RADAR_UDPRADAR_UDPRADAR_H_