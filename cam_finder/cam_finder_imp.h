/*
 * Copyright (C) 2018 by Rodrigo Antonio de Araujo
 */
#ifndef CAM_FINDER_CAM_FINDER_IMP_H_
#define CAM_FINDER_CAM_FINDER_IMP_H_

#include <list>
#include <memory>
#include <string>
#include <functional>
#include <boost/asio.hpp>
#include <boost/noncopyable.hpp>
#include <boost/thread.hpp>
#include "cam_finder/camfinder/camfinder.h"
#include "cam_finder/cam_finder_types.h"


namespace camfinder {

typedef std::array<char, 2048> array_2k;

typedef std::function<void(
    std::shared_ptr<profile_list_t> profiles,
    std::shared_ptr<stream_list_t> streams,
    int error
)> query_stream_handler_t;

class CamFinderImp: public CamFinder, public boost::noncopyable {
 public:
    CamFinderImp(
        const char *listen_address,
        const char *multicast_address,
        unsigned int port,
        device_info_handler_t handler);
    ~CamFinderImp();
    void search() override;
    void query_profiles(
            const std::string& device_url,
            const std::string& username,
            const std::string& password) override;

    void start() override;
    void stop() override;

 private:
    void discovery();
    void receive_discovery(
        std::shared_ptr<boost::asio::ip::udp::socket> socket,
        std::shared_ptr<array_2k> buffer);

    void query_profiles_streams(
        const std::string& device_url,
        const std::string& username,
        const std::string& password,
        std::shared_ptr<profile_list_t> profiles,
        std::shared_ptr<stream_list_t> streams,
        query_stream_handler_t handler);

 private:
    std::string listen_address_;
    std::string multicast_address_;
    unsigned int port_;
    device_info_handler_t handler_;
    boost::asio::io_service io_service_;
    std::shared_ptr<boost::asio::io_service::work> work_;
    std::shared_ptr<boost::thread> thread_;
};


}  // namespace camfinder

#endif  // CAM_FINDER_CAM_FINDER_IMP_H_
