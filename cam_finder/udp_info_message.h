/*
 * Copyright (C) 2018 by Rodrigo Antonio de Araujo
 */
#ifndef UDP_RADAR_UDP_INFO_MESSAGE_H_
#define UDP_RADAR_UDP_INFO_MESSAGE_H_

#include <string>

namespace camfinder {

std::string query_stream_info_message(
    const std::string& host,
    const std::string& path,
    const std::string& username,
    const std::string& password);

}

#endif  // UDP_RADAR_UDP_INFO_MESSAGE_H_
