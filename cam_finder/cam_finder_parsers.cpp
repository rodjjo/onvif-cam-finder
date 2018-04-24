/*
 * Copyright (C) 2018 by Rodrigo Antonio de Araujo
 */
#include "cam_finder/cam_finder_parsers.h"


namespace camfinder {
namespace parser {

profile_list_t get_profiles(const std::string& xml) {
    return profile_list_t();
}

stream_info_t get_stream_info(
    const std::string& profile_token,
    const std::string& profile_name,
    const std::string& xml
) {
    return stream_info_t();
}

}  // namespace parser
}  // namespace camfinder
