/*
 * Copyright (C) 2018 by Rodrigo Antonio de Araujo
 */

#include <string>
#include <boost/thread.hpp>
#include <boost/date_time.hpp>
#include "cam_finder/camfinder/camfinder.h"


int main() {
    bool finished = false;

    auto finder = camfinder::build(
        "0.0.0.0", "239.255.255.250", 3702, [&finished] (
            const std::string& device,
            const camfinder::stream_list_t& streams,
            int) {
                printf("Device: %s\n", device.c_str());
                for (const auto & s : streams) {
                    printf("profile %s: %s\n\tResolution: %sx%s @ %sfps\n",
                        s.profile_name.c_str(), s.stream_uri.c_str(),
                        s.width.c_str(), s.height.c_str(), s.fps.c_str());
                }
                finished = true;
            });

    printf("Starting...\n");
    finder->start();

    boost::this_thread::sleep(boost::posix_time::milliseconds(2000));

    printf("Sending...\n");
    finder->search();
    printf("Waiting reception...\n");

    while (!finished) {
        boost::this_thread::sleep(boost::posix_time::milliseconds(1));
    }

    printf("Finishing...\n");

    return 0;
}