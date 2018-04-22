/*
 * Copyright (C) 2018 by Rodrigo Antonio de Araujo
 */

#include <string>
#include <boost/thread.hpp>
#include <boost/date_time.hpp>
#include "cam_finder/camfinder/camfinder.h"


int main() {
    printf("%s\n", camfinder::query_stream_info_message
        ("", "", "admin", "admin").c_str());
    return 0;
    bool finished = false;

    auto finder = camfinder::build(
            "0.0.0.0", "239.255.255.250", 3702, [&finished] (
                const std::string& device,
                const camfinder::stream_list_t&,
                int) {
                    printf("Device: %s\n", device.c_str());
                    finished = true;
                });

    printf("Starting...\n");
    finder->start();

    boost::this_thread::sleep(boost::posix_time::milliseconds(2000));

    printf("Sending...\n");
    finder->find_cameras();
    printf("Waiting reception...\n");

    while (!finished) {
        boost::this_thread::sleep(boost::posix_time::milliseconds(1));
    }

    boost::this_thread::sleep(boost::posix_time::milliseconds(2000));

    return 0;
}