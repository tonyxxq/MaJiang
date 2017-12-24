//
// Created by neild47 on 17-12-20.
//

#include "MaJiang.h"

MaJiang *MaJiang::create(MaJiangType type) {
    auto *sprite = new(std::nothrow) MaJiang();
    __String filename = MaJiang::getFilePathByType(type);
    if (sprite && sprite->initWithFile(filename.getCString())) {
        sprite->autorelease();
        sprite->maJiangType = type;
        return sprite;
    }
    CC_SAFE_DELETE(sprite);
    return nullptr;
}

__String MaJiang::getFilePathByType(MaJiangType type) {
    __String filename("");
    if (type >= WAN_1 && type <= WAN_9) {
        filename.initWithFormat("majiang/m%d.png", type);
    } else if (type >= TONG_1 && type <= TONG_9) {
        filename.initWithFormat("majiang/p%d.png", type - TONG_1 + 1);
    } else if (type >= TIAO_1 && type <= TIAO_9) {
        filename.initWithFormat("majiang/s%d.png", type - TIAO_1 + 1);
    } else if (type >= DONGFENG && type <= HONGZHONG) {
        filename.initWithFormat("majiang/z%d.png", type - DONGFENG + 1);
    } else {
        filename.initWithFormat("majiang/pai.png");
    }
    return filename;
}
