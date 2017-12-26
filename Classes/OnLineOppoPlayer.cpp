//
// Created by neild47 on 17-12-26.
//

#include "OnLineOppoPlayer.h"

void OnLineOppoPlayer::mopai(MaJiang *majiang) {
    Player::mopai(majiang);
}

void OnLineOppoPlayer::display() {
    float position = Director::getInstance()->getVisibleSize().height - 112 - ONLINEOPPO_POSITION;
    playerMaJiang.display(position);
    outPlayerMaJiang.display(position - 94, DOWN); // 94 = 112*0.5 + (150-112)
}

void OnLineOppoPlayer::chupai(MaJiangType mjt) {
    auto mj = playerMaJiang.getByType(mjt);
    if (mj != nullptr) {
        playerMaJiang.eraseObject(mj);
        mj->setScale(0.5);
        mj->resetTexture();
        outPlayerMaJiang.pushBack(mj);
    }
}
