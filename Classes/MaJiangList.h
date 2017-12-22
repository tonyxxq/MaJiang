//
// Created by neild47 on 17-12-21.
//

#ifndef MYGAME_MAJIANGLIST_H
#define MYGAME_MAJIANGLIST_H

#include "cocos2d.h"
#include "MaJiang.h"

USING_NS_CC;

class MaJiangList {
private:
    Vector<MaJiang *> majiangs;
    int consumerIter;
    bool checkSequenceAndTriplet(int *card);
public:
    MaJiang *consume();

    void pushBack(MaJiang *maJiang);

    Vector<MaJiang *>::iterator begin();

    Vector<MaJiang *>::iterator end();

    Vector<MaJiang *>::const_iterator cbegin();

    Vector<MaJiang *>::const_iterator cend();

    void sort();

    ssize_t size();

    void shuffle();

    void eraseObject(MaJiang *object, bool removeAll = false);

    void display(float y, Direction direction = Direction::UP);

    void resetColor(const Color3B &color);

    MaJiang* getByType(MaJiangType type);

    bool isHuPai();

    bool isPeng(MaJiangType type);

    bool isGang(MaJiangType type);

    MaJiang *contain(const Vec2& location);
};


#endif //MYGAME_MAJIANGLIST_H
