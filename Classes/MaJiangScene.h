#ifndef __MAJIANG_SCENE_H__
#define __MAJIANG_SCENE_H__

#include "cocos2d.h"
#include "MaJiangType.h"
#include "MaJiang.h"
#include "MaJiangList.h"
#include "SimpleAudioEngine.h"
#include "AIOppoPlayer.h"
#include "HostPlayer.h"


class MaJiangScene : public cocos2d::Layer {
    enum MenuItemTag {
        PENG = 101,
        GANG,
        CHI,
        GUO,
        HU,
        MENU,
    };
private:
    MaJiangList allMaJiang;//全局的麻将
    HostPlayer hostPlayer;
    AIOppoPlayer oppoPlayer;
    bool menuEnable = false;

    bool isMyTurn = true;
public:
    // there's no 'id' in cpp, so we recommend returning the class instance pointer
    static cocos2d::Scene *createScene();

    // Here's a difference. Method 'init' in cocos2d-x returns bool, instead of returning 'id' in cocos2d-iphone
    virtual bool init();

    // implement the "static create()" method manually
    CREATE_FUNC(MaJiangScene)

    void onEnter() override;

    void onExit() override;;

    void chi(Ref *ref);
    void peng(Ref *ref);

    void gang(Ref *ref);
    void guo(Ref *ref);
    void hu(Ref *ref);

    void disableAllChoice();
};

#endif // __MAJIANG_SCENE_H__
