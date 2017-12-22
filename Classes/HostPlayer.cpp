//
// Created by neild47 on 17-12-22.
//

#include "HostPlayer.h"

void HostPlayer::display() {
    playerMaJiang.display(PLAYER1_MAJIANGS_POSITTION);
    outPlayerMaJiang.display(PLAYER1_MAJIANGS_POSITTION + 150);
}

bool HostPlayer::chupai(const Vec2 &location) {
    auto clicked = playerMaJiang.contain(location);
    if (clicked == nullptr) {
        return false;
    }
    CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("sound/chupai.wav");
    clicked->setScale(0.5f);
    playerMaJiang.resetColor(Color3B::WHITE);

    playerMaJiang.eraseObject(clicked);
    outPlayerMaJiang.pushBack(clicked);
    return true;
}
