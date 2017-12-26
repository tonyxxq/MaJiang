//
// Created by neild47 on 17-12-22.
//

#include "AIOppoPlayer.h"

void AIOppoPlayer::mopai(MaJiang *majiang) {
    if (majiang != nullptr) {
        majiang->setTexture(MaJiang::getFilePathByType(MaJiangType::BEIMIAN).getCString());
        playerMaJiang.pushBack(majiang);
    }
}

void AIOppoPlayer::display() {
    float position = Director::getInstance()->getVisibleSize().height - 112 - AIOPPO_POSITION;
    playerMaJiang.display(position);
    outPlayerMaJiang.display(position - 94, DOWN); // 94 = 112*0.5 + (150-112)
}

void AIOppoPlayer::chupai(MaJiang *mj) {

    // 基本ai算法， 能杠则杠， 能碰则碰，能吃则吃, 否则出单牌
    if (mj != nullptr) {
        if (isGang(mj->maJiangType)) {
            gang(mj);
        } else if (isPeng(mj->maJiangType)) {
            peng(mj);
        } else if (isChi(mj->maJiangType)) {
            mj->setTexture(MaJiang::getFilePathByType(MaJiangType::BEIMIAN).getCString());
            chi(mj);
        }
    }
    //吃碰杠完了还要出牌

    auto clicked = *(playerMaJiang.begin());

    int mark[MaJiangType::HONGZHONG + 1] = {0};
    for (const auto &m : playerMaJiang) {
        mark[m->maJiangType]++;
    }
    for (int i = 1; i <= MaJiangType::HONGZHONG; ++i) {
        if (mark[i] == 1) {
            clicked = playerMaJiang.getByType(MaJiangType(i));
            break;
        }
    }

    CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("sound/chupai.wav");
    clicked->resetTexture();
    clicked->setScale(0.5f);
    playerMaJiang.resetColor(Color3B::WHITE);

    playerMaJiang.eraseObject(clicked);
    outPlayerMaJiang.pushBack(clicked);
}

void AIOppoPlayer::displayAll() {
    for (auto mj:playerMaJiang) {
        auto texturePath = MaJiang::getFilePathByType(mj->maJiangType);
        mj->setTexture(texturePath.getCString());
    }
    this->display();
}
