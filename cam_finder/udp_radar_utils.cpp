/*
 * Copyright (C) 2018 by Rodrigo Antonio de Araujo
 */
#include "cam_finder/udp_radar_utils.h"
#include <boost/asio.hpp>

namespace udpradar {

std::list<std::string> get_local_addresses() {
    using boost::asio::ip::tcp;
    std::list<std::string> result;
    boost::asio::io_service io_service;

    tcp::resolver resolver(io_service);
    tcp::resolver::query query(boost::asio::ip::host_name(), "");
    tcp::resolver::iterator it = resolver.resolve(query);

    while (it != tcp::resolver::iterator()) {
        boost::asio::ip::address addr = (it++)->endpoint().address();
        if (!addr.is_v6()) {
            result.push_back(addr.to_string());
        }
    }
}

}  // namespace udpradar
