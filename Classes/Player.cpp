//
// Created by neild47 on 17-12-22.
//

#include "Player.h"

void Player::mopai(MaJiang *majiang) {
    if (majiang != nullptr) {
        majiang->setColor(Color3B::YELLOW);
        this->playerMaJiang.pushBack(majiang);
    }
}

void Player::sort() {
    this->playerMaJiang.sort();
}

void Player::resetColor() {
    this->playerMaJiang.resetColor(Color3B::WHITE);
}

bool Player::isHupai(MaJiangType type) {
    return playerMaJiang.isHuPai(type);
}

bool Player::isPeng(MaJiangType type) {
    return playerMaJiang.isPeng(type);
}

bool Player::isGang(MaJiangType type) {
    return playerMaJiang.isGang(type);
}

bool Player::isChi(MaJiangType type) {
    return playerMaJiang.isChi(type) != 0;
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
        m->setTexture(MaJiang::getFilePathByType(m->maJiangType)->getCString());
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
        m->setTexture(MaJiang::getFilePathByType(m->maJiangType)->getCString());
        outPlayerMaJiang.pushBack(m);
        playerMaJiang.eraseObject(m);
    }
}

void Player::chi(MaJiang *mj) {
    int position = playerMaJiang.isChi(mj->maJiangType);
    if (position == 0) {
        return;
    }
    //先只考虑只有一种吃牌情况
    mj->setScale(1);
    playerMaJiang.pushBack(mj);
    int startType = BEIMIAN;
    if (position & 0b100) {
        startType = mj->maJiangType;
    } else if (position & 0b010) {
        startType = mj->maJiangType - 1;
    } else if (position & 0b001) {
        startType = mj->maJiangType - 2;
    }
    if (startType != BEIMIAN) {
        for (int i = 0; i < 3; ++i) {
            auto m = playerMaJiang.getByType(MaJiangType(startType + i));
            m->setColor(Color3B::GREEN);
            m->setScale(0.5f);
            m->setTexture(MaJiang::getFilePathByType(m->maJiangType)->getCString());
            outPlayerMaJiang.pushBack(m);
            playerMaJiang.eraseObject(m);
        }
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

void Player::hupai(MaJiang *mj) {
    if (mj != nullptr) {
        mj->setColor(Color3B::RED);
        playerMaJiang.pushBack(mj);
    }
}



