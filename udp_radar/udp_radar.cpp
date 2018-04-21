/*
 * Copyright (C) 2018 by Rodrigo Antonio de Araujo
 */
#include "udp_radar/udpradar/udpradar.h"
#include "udp_radar/udp_radar_imp.h"

namespace udpradar {

UdpRadar::~UdpRadar() {
}

std::shared_ptr<UdpRadar> build(
    const char *listen_address,
    const char *multicast_address,
    unsigned int port,
    ReceiverHandler handler
) {
    return std::shared_ptr<UdpRadar>(
        new UdpRadarImp(
            listen_address, multicast_address, port, handler));
}


}  // namespace udpradar
