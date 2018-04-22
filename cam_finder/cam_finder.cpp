/*
 * Copyright (C) 2018 by Rodrigo Antonio de Araujo
 */
#include "cam_finder/camfinder/camfinder.h"
#include "cam_finder/cam_finder_imp.h"

namespace camfinder {

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


}  // namespace camfinder
