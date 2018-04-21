/*
 * Copyright (C) 2018 by Rodrigo Antonio de Araujo
 */
#include <string>
#include <boost/thread.hpp>
#include <boost/date_time.hpp>
#include "udp_radar/udpradar/udpradar.h"


/*
probe

<?xml version="1.0" ?>
<s:Envelope xmlns:a="http://schemas.xmlsoap.org/ws/2004/08/addressing" xmlns:d="http://schemas.xmlsoap.org/ws/2005/04/discovery" xmlns:s="http://www.w3.org/2003/05/soap-envelope">
        <s:Header>
                <a:Action>http://schemas.xmlsoap.org/ws/2005/04/discovery/Probe</a:Action>
                <a:MessageID>urn:uuid:2b0bf1e1-d725-49a9-834a-52656c4b5011</a:MessageID>
                <a:To>urn:schemas-xmlsoap-org:ws:2005:04:discovery</a:To>
        </s:Header>
        <s:Body>
                <d:Probe/>
        </s:Body>
</s:Envelope>


by:

<?xml version="1.0" ?>
<s:Envelope xmlns:a="http://schemas.xmlsoap.org/ws/2004/08/addressing" xmlns:d="http://schemas.xmlsoap.org/ws/2005/04/discovery" xmlns:s="http://www.w3.org/2003/05/soap-envelope">
        <s:Header>
                <a:Action>http://schemas.xmlsoap.org/ws/2005/04/discovery/Bye</a:Action>
                <a:MessageID>urn:uuid:1008e2fc-4e59-4856-b363-871814006230</a:MessageID>
                <a:To>urn:schemas-xmlsoap-org:ws:2005:04:discovery</a:To>
                <d:AppSequence InstanceId="599839350" MessageNumber="1"/>
        </s:Header>
        <s:Body>
                <d:Bye>
                        <a:EndpointReference>
                                <a:Address>urn:uuid:c3cb00a7-823b-496b-a525-9387dad33cfe</a:Address>
                        </a:EndpointReference>
                </d:Bye>
        </s:Body>
</s:Envelope>


*/


std::string send_message(
    "<?xml version=\"1.0\" ?><s:Envelope xmlns:a=\"http://schemas.xmlsoap.org/ws/2004/08/addressing\" xmlns:d=\"http://schemas.xmlsoap.org/ws/2005/04/discovery\" xmlns:s=\"http://www.w3.org/2003/05/soap-envelope\"><s:Header><a:Action>http://schemas.xmlsoap.org/ws/2005/04/discovery/Probe</a:Action><a:MessageID>urn:uuid:2b0bf1e1-d725-49a9-834a-52656c4b5011</a:MessageID><a:To>urn:schemas-xmlsoap-org:ws:2005:04:discovery</a:To></s:Header><s:Body><d:Probe/></s:Body></s:Envelope>");


int main() {
    bool finished = false;
    auto radar = udpradar::build(
            "0.0.0.0", "239.255.255.250", 3702, [&finished] (
                const void* data, unsigned int size, int error) {
                    printf("Message received back");
                    finished = true;
                });
    printf("Starting...\n");
    radar->start();
    printf("Sending...\n");
    radar->send(send_message.c_str(), send_message.size() + 1);
    printf("Waiting reception...\n");
    while (!finished) {
        boost::this_thread::sleep(boost::posix_time::milliseconds(1));
    }

    boost::this_thread::sleep(boost::posix_time::milliseconds(2000));
    return 0;
}
