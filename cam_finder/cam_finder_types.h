/*
 * Copyright (C) 2018 by Rodrigo Antonio de Araujo
 */
#ifndef CAM_FINDER_CAM_FINDER_TYPES_H_
#define CAM_FINDER_CAM_FINDER_TYPES_H_

#include <string>
#include <list>
#include "cam_finder/camfinder/camfinder.h"


namespace camfinder {

typedef struct {
    std::string name;
    std::string token;
} profile_t;

typedef std::list<profile_t> profile_list_t;


}   // namespace camfinder

#endif  // CAM_FINDER_CAM_FINDER_TYPES_H_
