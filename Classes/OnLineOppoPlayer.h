//
// Created by neild47 on 17-12-26.
//

#ifndef MYGAME_ONLINEOPPOPLAYER_H
#define MYGAME_ONLINEOPPOPLAYER_H

#include "Player.h"

#define ONLINEOPPO_POSITION 20

class OnLineOppoPlayer : public Player {
public:
    void mopai(MaJiang *majiang) override;

    void display() override;

    void chupai(MaJiangType mjt = BEIMIAN);
};


#endif //MYGAME_ONLINEOPPOPLAYER_H
