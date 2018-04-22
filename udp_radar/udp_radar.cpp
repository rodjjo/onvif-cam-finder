/*
 * Copyright (C) 2018 by Rodrigo Antonio de Araujo
 */
#include "udp_radar/camfinder/udpradar.h"
#include "udp_radar/udp_radar_imp.h"

namespace udpradar {

CamFinder::~CamFinder() {
}

std::shared_ptr<CamFinder> build(
    const char *listen_address,
    const char *multicast_address,
    unsigned int port,
    ReceiverHandler handler
) {
    return std::shared_ptr<CamFinder>(
        new CamFinderImp(
            listen_address, multicast_address, port, handler));
}


}  // namespace udpradar
