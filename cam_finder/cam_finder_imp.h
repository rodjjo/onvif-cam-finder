/*
 * Copyright (C) 2018 by Rodrigo Antonio de Araujo
 */
#ifndef CAM_FINDER_CAM_FINDER_IMP_H_
#define CAM_FINDER_CAM_FINDER_IMP_H_

#include <string>
#include <memory>
#include <boost/asio.hpp>
#include <boost/noncopyable.hpp>
#include <boost/thread.hpp>
#include "cam_finder/camfinder/camfinder.h"


namespace camfinder {

typedef std::array<char, 2048> array_2k;

class CamFinderImp: public CamFinder, public boost::noncopyable {
 public:
    CamFinderImp(
        const char *listen_address,
        const char *multicast_address,
        unsigned int port,
        ReceiverHandler handler);
    ~CamFinderImp();
    void find_cameras() override;
    void start() override;
    void stop() override;

 private:
    void start_receive();
    void receive(
        std::shared_ptr<boost::asio::ip::udp::socket> socket,
        std::shared_ptr<array_2k> buffer);

 private:
    std::string listen_address_;
    std::string multicast_address_;
    unsigned int port_;
    ReceiverHandler handler_;
    boost::asio::io_service io_service_;
    std::shared_ptr<boost::asio::io_service::work> work_;
    std::shared_ptr<boost::thread> thread_;
};


}  // namespace camfinder

#endif  // CAM_FINDER_CAM_FINDER_IMP_H_
