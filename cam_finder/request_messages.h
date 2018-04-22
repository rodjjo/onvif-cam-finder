/*
 * Copyright (C) 2018 by Rodrigo Antonio de Araujo
 */
#ifndef CAM_FINDER_REQUEST_MESSAGES_H_
#define CAM_FINDER_REQUEST_MESSAGES_H_

#include <string>

namespace camfinder {

std::string query_stream_info_message(
    const std::string& host,
    const std::string& path,
    const std::string& username,
    const std::string& password);

const char* get_discovery_message();

}

#endif  // CAM_FINDER_REQUEST_MESSAGES_H_
