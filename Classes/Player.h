//
// Created by neild47 on 17-12-22.
//

#ifndef MYGAME_PLAYER_H
#define MYGAME_PLAYER_H

#include "MaJiangList.h"
#include "SimpleAudioEngine.h"


class Player {
protected:
    MaJiangList playerMaJiang;
    MaJiangList outPlayerMaJiang;
public:
    virtual void mopai(MaJiang *majiang);

    virtual void display() = 0;

    void resetColor();

    void sort();

    bool isHupai();

    bool isPeng(MaJiangType type);
    bool isGang(MaJiangType type);

    void peng(MaJiang *mj);
    void gang(MaJiang *mj);

    MaJiangType getLastOutType();

    MaJiang *popLastOutMaJiang();

    void pushToOutMaJiangList(MaJiang *item, ...);
};


#endif //MYGAME_PLAYER_H
