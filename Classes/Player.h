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

    void displayAll();

    void resetColor();

    void sort();

    bool isHupai(MaJiangType type = BEIMIAN);

    bool isPeng(MaJiangType type);

    bool isGang(MaJiangType type);

    int isChi(MaJiangType type);

    virtual void peng(MaJiang *mj);

    virtual void gang(MaJiang *mj);

    void hupai(MaJiang *mj);

    virtual void chi(MaJiang *mj, int chiPosition = 0);

    MaJiangType getLastOutType();

    MaJiang *popLastOutMaJiang();

    MaJiang *popLastPlayerMaJiang();

    void pushToOutMaJiangList(MaJiang *item, ...);

    void pushToPlayerMaJiangList(MaJiang *item, ...);
};


#endif //MYGAME_PLAYER_H
