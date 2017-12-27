//
// Created by neild47 on 17-12-20.
//

#ifndef MYGAME_MAJIANG_H
#define MYGAME_MAJIANG_H

#include "cocos2d.h"
#include "MaJiangType.h"

USING_NS_CC;


class MaJiang : public Sprite {
public:
    MaJiangType maJiangType;
    void resetTexture();
    void hideTexture();
    static MaJiang *create(MaJiangType type);
    static __String getFilePathByType(MaJiangType type);

private:
};


#endif //MYGAME_MAJIANG_H
