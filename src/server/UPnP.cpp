//
// Created by Gazi on 5/11/2024.
//

#include "UPnP.h"
#include "../Constants.h"
#include "../Colors.h"

#include <iostream>
#include <miniupnpc/miniupnpc.h>
#include <miniupnpc/upnpcommands.h>


void UPnP::handle_upnp(int port) {
    std::cout << "Starting discovering UPnP Internet Gateway Devices so you don't need to port-forward manually." << std::endl;

    int error = 0;
    UPNPDev *upnp_devlist = upnpDiscover(2000, nullptr, nullptr, UPNP_LOCAL_PORT_ANY, false, 2, &error);
    if (upnp_devlist == nullptr) {
        std::cout << Colors::red() << "Discovered nothing." << std::endl;
    }

    char lan_address[64];
    UPNPUrls upnp_urls;
    IGDdatas upnp_data;
    // look up possible "status" values, the number "1" indicates a valid IGD was found
    int status = UPNP_GetValidIGD(upnp_devlist, &upnp_urls, &upnp_data, lan_address, sizeof(lan_address));
    if (status == 0)  {
       std::cout << Colors::red() << "No IGD found." << Colors::reset() << std::endl;
    } else if (status == 1) {
       std::cout << Colors::green() << "A valid connected IGD has been found!" << Colors::reset() << std::endl;
    } else if (status == 2) {
       std::cout << Colors::red() << "A valid IGD has been found but it reported as not connected." << Colors::reset() << std::endl;
    } else if (status == 3) {
       std::cout << Colors::red() << "An UPnP device has been found but was not recognized as an IGD." << Colors::reset() << std::endl;
    }

    const char* port_str = std::to_string(port).c_str();

    // add a new port mapping
    error = UPNP_AddPortMapping(
        upnp_urls.controlURL,
        upnp_data.first.servicetype,
        port_str, // external (WAN) port requested
        port_str, // internal (LAN) port to which packets will be redirected
        lan_address, // internal (LAN) address to which packets will be redirected
        Constants::PROJECT_NAME_SERVER.c_str(), // text description to indicate why or who is responsible for the port mapping
        "TCP", // protocol must be either TCP or UDP
        nullptr, // remote (peer) host address or nullptr for no restriction
        "0" // port map lease duration (in seconds) or zero for "as long as possible"
    );

    if (error == 0) {
        // get the external (WAN) IP address
        char wan_address[64];
        UPNP_GetExternalIPAddress(upnp_urls.controlURL, upnp_data.first.servicetype, wan_address);
        std::cout << "Clients on the internet can join you with:" << std::endl;
        std::cout << Colors::yellow() << wan_address << Colors::reset() << ":" <<  Colors::green() << port << Colors::reset() << std::endl;
    } else {
        std::cout << Colors::red() << "Could not add port mapping for TCP protocol on port " << port << Colors::reset() << std::endl;
    }

    FreeUPNPUrls(&upnp_urls);
    freeUPNPDevlist(upnp_devlist);
}
