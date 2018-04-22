/*
 * Copyright (C) 2018 by Rodrigo Antonio de Araujo
 */
#ifndef CAM_FINDER_NETWORK_UTILS_H_
#define CAM_FINDER_NETWORK_UTILS_H_

#include <string>
#include <list>

namespace camfinder {

std::list<std::string> get_local_addresses();

}  // namespace camfinder

#endif  // CAM_FINDER_NETWORK_UTILS_H_
