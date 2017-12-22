//
// Created by neild47 on 17-12-22.
//

#include "Player.h"

void Player::mopai(MaJiang *majiang) {
    if (majiang != nullptr) {
        majiang->setColor(Color3B(255, 255, 0));
        this->playerMaJiang.pushBack(majiang);
    }
}

void Player::sort() {
    this->playerMaJiang.sort();
}

void Player::resetColor() {
    this->playerMaJiang.resetColor(Color3B::WHITE);
}

bool Player::isHupai() {
    return playerMaJiang.isHuPai();
}

bool Player::isPeng(MaJiangType type) {
    return playerMaJiang.isPeng(type);
}

bool Player::isGang(MaJiangType type) {
    return playerMaJiang.isGang(type);
}

MaJiangType Player::getLastOutType() {
    return (*(--outPlayerMaJiang.cend()))->maJiangType;
}

MaJiang *Player::popLastOutMaJiang() {
    auto majiang = *(--outPlayerMaJiang.cend());
    outPlayerMaJiang.eraseObject(majiang);
    return majiang;
}

void Player::peng(MaJiang *mj) {
    MaJiangType type = mj->maJiangType;
    if (!isPeng(type)) {
        return;
    }
    outPlayerMaJiang.pushBack(mj);
    mj->setColor(Color3B::GREEN);

    MaJiang *m = nullptr;
    for (int i = 0; i < 2; ++i) {
        m = playerMaJiang.getByType(type);
        m->setColor(Color3B::GREEN);
        m->setScale(0.5f);
        outPlayerMaJiang.pushBack(m);
        playerMaJiang.eraseObject(m);
    }
}

void Player::gang(MaJiang *mj) {
    MaJiangType type = mj->maJiangType;
    if (!isPeng(type)) {
        return;
    }
    outPlayerMaJiang.pushBack(mj);
    mj->setColor(Color3B::GREEN);

    MaJiang *m = nullptr;
    for (int i = 0; i < 3; ++i) {
        m = playerMaJiang.getByType(type);
        m->setColor(Color3B::GREEN);
        m->setScale(0.5f);
        outPlayerMaJiang.pushBack(m);
        playerMaJiang.eraseObject(m);
    }
}

void Player::pushToOutMaJiangList(MaJiang *item, ...) {
    va_list args;
    va_start(args, item);
    while (item) {
        outPlayerMaJiang.pushBack(item);
        item = va_arg(args, MaJiang*);
    }
    va_end(args);
}


