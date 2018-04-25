/*
 * Copyright (C) 2018 by Rodrigo Antonio de Araujo
 */
#ifndef CAM_FINDER_REQUEST_MESSAGES_H_
#define CAM_FINDER_REQUEST_MESSAGES_H_

#include <string>

namespace camfinder {

std::string query_profiles_message(
    const std::string& username,
    const std::string& password);

std::string query_profiles_stream_message(
    const std::string& username,
    const std::string& password,
    const std::string& profile_token);

const char* get_discovery_message();

}

#endif  // CAM_FINDER_REQUEST_MESSAGES_H_
