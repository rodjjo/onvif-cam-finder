/*
 * Copyright (C) 2018 by Rodrigo Antonio de Araujo
 */
#ifndef CAM_FINDER_CAM_FINDER_PARSERS_H_
#define CAM_FINDER_CAM_FINDER_PARSERS_H_

#include <string>
#include <list>
#include "cam_finder/camfinder/camfinder.h"
#include "cam_finder/cam_finder_types.h"


namespace camfinder {
namespace parser {

profile_list_t get_profiles(const std::string& xml);

stream_info_t get_stream_info(
    const std::string& profile_token,
    const std::string& profile_name,
    const std::string& xml);

}  // namespace parser
}  // namespace camfinder

#endif  // CAM_FINDER_CAM_FINDER_PARSERS_H_
