//
// Created by Gazi on 5/6/2024.
//

#ifndef NETWORKUTILS_H
#define NETWORKUTILS_H



class NetworkUtils {
public:
    static float read_float(char* buffer);
    static void write_float(float number, char* buffer_to_write_to);
};



#endif //NETWORKUTILS_H
