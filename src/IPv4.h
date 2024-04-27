//
// Created by Gazi on 4/27/2024.
//

#ifndef IPV4_H
#define IPV4_H

#include <SDL_net.h>
#include <string>

class IPv4 {
private:
    Uint32 _first_num;
    Uint32 _second_num;
    Uint32 _third_num;
    Uint32 _fourth_num;

public:
    IPv4(IPaddress* ip_address);

    std::string to_string();
};



#endif //IPV4_H
