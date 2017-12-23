//
// Created by neild47 on 17-12-22.
//

#ifndef MYGAME_AIPLAYER_H
#define MYGAME_AIPLAYER_H

#include "MaJiangList.h"
#include "Player.h"

#define POSITION 20

class AIOppoPlayer : public Player {
public:
    void mopai(MaJiang *majiang);

    void display() override;

    void chupai(MaJiang *mj = nullptr);

    void displayAll();
};


#endif //MYGAME_AIPLAYER_H
