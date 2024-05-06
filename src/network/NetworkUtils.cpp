//
// Created by Gazi on 5/6/2024.
//

#include "NetworkUtils.h"

#include <SDL.h>
#include <SDL_net.h>

float NetworkUtils::read_float(char* buffer) {
    Uint32 data = SDLNet_Read32(buffer);
    float res = 0.0;
    memcpy(&res, &data, sizeof(data));
    return res;
}

void NetworkUtils::write_float(float number, char* buffer_to_write_to) {
    Uint32 unsigned_num = 0;
    memcpy(&unsigned_num, &number, sizeof(number));
    SDLNet_Write32(unsigned_num, buffer_to_write_to);
}