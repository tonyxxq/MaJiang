//
// Created by neild47 on 17-12-26.
//

#ifndef MYGAME_SOCKETCONNECTOR_H
#define MYGAME_SOCKETCONNECTOR_H

#include "MaJiangCommand.h"
#include "sys/socket.h"
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include <iostream>

#define SERV_IP "127.0.0.1"
#define SERV_PORT 8001

#define FRAME_LEN_POSITION 0
#define FRAME_CMD_POSITION 1
#define FRAME_DATA_POSITION 2

using namespace std;

class SocketConnector {
private:
    int connects();
    int fd = -1;
public:
    SocketConnector();

    int *getFrame();
    bool getIsHost();
    void putFrame(int *frame);

    void closeSock();
};


#endif //MYGAME_SOCKETCONNECTOR_H
