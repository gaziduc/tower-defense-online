//
// Created by Gazi on 4/27/2024.
//

#ifndef CONSTANTS_H
#define CONSTANTS_H



class ServerConstants {
public:
    static constexpr int MAX_LOCAL_IPS = 25;
    static constexpr int MAX_CLIENTS = 2;

    // If you change the following lines, update src/Constants.h
    static constexpr int INITIAL_PLAYER_HEALTH = 500;
    static constexpr int INITIAL_PLAYER_MONEY = 200;
    static constexpr int MAX_MESSAGE_SIZE = 1024;
    static constexpr char WORD_DELIMITER = ' ';
    static constexpr char MESSAGE_DELIMITER = '\n';
    inline static const std::string MESSAGE_NEW_ENTITY = "NEW_ENTITY";
    inline static const std::string MESSAGE_START = "START";
};



#endif //CONSTANTS_H
