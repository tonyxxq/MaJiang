//
// Created by neild47 on 17-12-22.
//

#ifndef MYGAME_HOSTPLAYER_H
#define MYGAME_HOSTPLAYER_H

#include "Player.h"
#define PLAYER1_MAJIANGS_POSITTION 20

class HostPlayer : public Player{
public:
    void display() override;

    bool chupai(const Vec2 &location);
};


#endif //MYGAME_HOSTPLAYER_H
