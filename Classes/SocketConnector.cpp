//
// Created by neild47 on 17-12-26.
//

#include "SocketConnector.h"


int SocketConnector::connects() {
    int sfd;
    struct sockaddr_in serv_addr{};

    sfd = socket(AF_INET, SOCK_STREAM, 0);

    bzero(&serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(SERV_PORT);
    inet_pton(AF_INET, SERV_IP, &serv_addr.sin_addr.s_addr);

    connect(sfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr));
    return sfd;
}


int *SocketConnector::getFrame() {
    if (fd == -1) {
        fd = connects();
    }
    int sfd = fd;

    int pai[512];
    if (read(sfd, &pai[FRAME_LEN_POSITION], sizeof(int)) <= 0) return nullptr;

    if (read(sfd, &pai[FRAME_CMD_POSITION], sizeof(int) * (pai[0] - 1)) <= 0) return nullptr;

    auto pai_ptr = new int[pai[0]];
    memcpy(pai_ptr, pai, pai[0] * sizeof(int));

    return pai_ptr;
}

bool SocketConnector::getIsHost() {
    auto data = getFrame();
    if (data[1] != ISHOST) {
        return false;
    }
    bool isHost = data[2] == 1;
    delete[] data;
    return isHost;
}

SocketConnector::SocketConnector() {}

void SocketConnector::closeSock() {
    if (fd != -1) {
        close(fd);
        fd = -1;
    }
}

void SocketConnector::putFrame(int *frame) {
    if (fd != -1) {
        write(fd, frame, sizeof(int) * frame[0]);
    }
}
