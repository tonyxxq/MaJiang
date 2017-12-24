//
// Created by neild47 on 17-12-21.
//

#include "MaJiangList.h"

MaJiang *MaJiangList::consume() {
    if (consumerIter == majiangs.size()) {
        return nullptr;
    }
    return *(majiangs.begin() + (consumerIter++));
}

void MaJiangList::pushBack(MaJiang *maJiang) {
    majiangs.pushBack(maJiang);
}

Vector<MaJiang *>::iterator MaJiangList::begin() {
    return majiangs.begin();
}

Vector<MaJiang *>::iterator MaJiangList::end() {
    return majiangs.end();
}

Vector<MaJiang *>::const_iterator MaJiangList::cbegin() {
    return majiangs.cbegin();
}

Vector<MaJiang *>::const_iterator MaJiangList::cend() {
    return majiangs.cend();
}

void MaJiangList::sort() {
    std::sort(majiangs.begin(), majiangs.end(), [](MaJiang *m1, MaJiang *m2) {
        return m1->maJiangType < m2->maJiangType;
    });
}

ssize_t MaJiangList::size() {
    return majiangs.size();
}

void MaJiangList::shuffle() {
    long seed = std::time(NULL);
    std::shuffle(majiangs.begin(), majiangs.end(), std::default_random_engine(seed));
}

void MaJiangList::eraseObject(MaJiang *object, bool removeAll) {
    majiangs.eraseObject(object, removeAll);
}

void MaJiangList::display(float y, Direction direction) {
    if (majiangs.size() == 0) {
        return;
    }
    //display
    int row_count = 0;
    auto playOneTmp = majiangs.cbegin();
    auto sizePlayOne = static_cast<int>(majiangs.size()) + 24; // +24:因为for循环的一开始就会进入if,为了简化代码
    if (direction == Direction::UP) {
        y -= (*playOneTmp)->getContentSize().height * (*playOneTmp)->getScale() + 10;//与上一步同样的道理
    } else {
        y += (*playOneTmp)->getContentSize().height * (*playOneTmp)->getScale() + 10;//与上一步同样的道理
    }
    float start = 0;

    for (int l = 0; l < static_cast<int>(majiangs.size()); ++l) {
        if (row_count % 24 == 0) {
            sizePlayOne -= 24;

            if (sizePlayOne > 24) {
                start =
                        (Director::getInstance()->getVisibleSize().width -
                         24 * (*playOneTmp)->getContentSize().width * (*playOneTmp)->getScale()) / 2;
            } else {
                start =
                        (Director::getInstance()->getVisibleSize().width -
                         sizePlayOne * (*playOneTmp)->getContentSize().width * (*playOneTmp)->getScale()) / 2;
            }
            if (direction == Direction::UP) {
                y += (*playOneTmp)->getContentSize().height * (*playOneTmp)->getScale() + 10;
            } else {
                y -= (*playOneTmp)->getContentSize().height * (*playOneTmp)->getScale() + 10;
            }
        }

        row_count++;
        (*playOneTmp)->setVisible(true);
        (*playOneTmp)->setPosition(Vec2(start, y));
        start += (*playOneTmp)->getContentSize().width * (*playOneTmp)->getScale();
        playOneTmp++;
    }
}

void MaJiangList::resetColor(const Color3B &color) {
    for (auto tmp :majiangs) {
        tmp->setColor(color);
    }
}

bool MaJiangList::isHuPai(MaJiangType type) {
//    if (majiangs.size() % 3 != 2) {
//        return false;
//    }

    int pai[HONGZHONG + 1] = {0};
    for (auto mj:majiangs) {
        pai[mj->maJiangType]++;
    }
    if (type != BEIMIAN) {
        pai[type]++;
    }

    for (int i = 1; i <= HONGZHONG; ++i) {
        if (pai[i] >= 2) { //对子
            pai[i] -= 2;
            if (checkSequenceAndTriplet(pai)) {
                return true;
            }
            pai[i] += 2;
        }
    }

    return false;
}

bool MaJiangList::checkSequenceAndTriplet(int *pai) {
    int i;
    for (i = 1; i <= HONGZHONG; ++i) {
        if (pai[i] != 0) {
            break;
        }
    }
    if (i == HONGZHONG + 1) { // 递归结束
        return true;
    }

    for (i = 1; i <= HONGZHONG; ++i) {
        bool sequenceWan = i >= WAN_1 && (i + 2) <= WAN_9 && pai[i] >= 1 && pai[i + 1] >= 1 && pai[i + 2] >= 1;
        bool sequenceTong = i >= TONG_1 && (i + 2) <= TONG_9 && pai[i] >= 1 && pai[i + 1] >= 1 && pai[i + 2] >= 1;
        bool sequenceTiao = i >= TIAO_1 && (i + 2) <= TIAO_9 && pai[i] >= 1 && pai[i + 1] >= 1 && pai[i + 2] >= 1;
        if (sequenceWan || sequenceTong || sequenceTiao) { //顺子
            pai[i]--;
            pai[i + 1]--;
            pai[i + 2]--;
            if (checkSequenceAndTriplet(pai)) {
                return true;
            }
            pai[i]++;
            pai[i + 1]++;
            pai[i + 2]++;
        }
        if (pai[i] >= 3) { //刻子
            pai[i] -= 3;
            if (checkSequenceAndTriplet(pai)) {
                return true;
            }
            pai[i] += 3;
        }
    }
    return false;
}

MaJiang *MaJiangList::contain(const Vec2 &location) {
    for (auto item : majiangs) {
        if (item->getBoundingBox().containsPoint(location)) {
            return item;
        }
    }
    return nullptr;
}

MaJiang *MaJiangList::getByType(MaJiangType type) {
    for (auto item : majiangs) {
        if (item->maJiangType == type) {
            return item;
        }
    }
    return nullptr;
}

bool MaJiangList::isPeng(MaJiangType type) {
    int count = 0;
    for (auto mj:majiangs) {
        if (mj->maJiangType == type) {
            count++;
        }
    }

    return count >= 2;
}

bool MaJiangList::isGang(MaJiangType type) {
    int count = 0;
    for (auto mj:majiangs) {
        if (mj->maJiangType == type) {
            count++;
        }
    }

    return count >= 3;
}

int MaJiangList::isChi(MaJiangType type) {
    int chiPosition = 0b000;

    int pai[HONGZHONG + 1] = {0};
    for (auto mj:majiangs) {
        pai[mj->maJiangType]++;
    }

    if ((type >= WAN_1 && type + 2 <= WAN_9 || type >= TONG_1 && type + 2 <= TONG_9 ||
         type >= TIAO_1 && type + 2 <= TIAO_9)
        && pai[type + 1] >= 1 && pai[type + 2] >= 1) { //011
        chiPosition |= 0b100;
    }
    if ((type + 1 <= WAN_9 && type - 1 >= WAN_1 || type + 1 <= TONG_9 && type - 1 >= TONG_1 ||
         type + 1 <= TIAO_9 && type - 1 >= TIAO_1)
        && pai[type - 1] >= 1 && pai[type + 1] >= 1) {//101
        chiPosition |= 0b010;
    }
    if ((type - 2 >= WAN_1 && type <= WAN_9 || type - 2 >= TONG_1 && type <= TONG_9 ||
         type - 2 >= TIAO_1 && type <= TIAO_9)
        && pai[type - 2] >= 1 && pai[type - 1] >= 1) { // 110
        chiPosition |= 0b001;
    }

    return chiPosition;
}


