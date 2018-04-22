/*
 * Copyright (C) 2018 by Rodrigo Antonio de Araujo
 */
#ifndef UDP_RADAR_UDPRADAR_UDPRADAR_H_
#define UDP_RADAR_UDPRADAR_UDPRADAR_H_

#include <memory>
#include <functional>
#include <string>
#include <boost/noncopyable.hpp>


namespace udpradar {


typedef std::function<void(
    const std::string& device, int error_code)> ReceiverHandler;


class UdpRadar {
 public:
    virtual ~UdpRadar();
    virtual void ws_discovery() = 0;
    virtual void start() = 0;
    virtual void stop() = 0;
};


std::shared_ptr<UdpRadar> build(
    const char *listen_address,
    const char *multicast_address,
    unsigned int port,
    ReceiverHandler handler);


}  // namespace udpradar

#endif  // UDP_RADAR_UDPRADAR_UDPRADAR_H_
