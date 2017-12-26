#ifndef __MAJIANG_MULTIPLAYERSCENE_H__
#define __MAJIANG_MULTIPLAYERSCENE_H__

#include "cocos2d.h"
#include "MaJiangType.h"
#include "MaJiang.h"
#include "MaJiangList.h"
#include "SimpleAudioEngine.h"
#include "AIOppoPlayer.h"
#include "HostPlayer.h"
#include "SocketConnector.h"
#include "OnLineOppoPlayer.h"


class MaJiangMultiPlayerScene : public cocos2d::Layer {
    enum MenuItemTag {
        CHI = 101,
        PENG,
        GANG,
        HU,
        GUO,
        LEFT, MIDDLE, RIGHT,
        MENU,
    };
private:
    SocketConnector socketConnector;
    MaJiangList allMaJiang;//全局的麻将
    HostPlayer hostPlayer;
    OnLineOppoPlayer oppoPlayer;
    std::thread *readOppoThread;
    bool menuEnable = false;

    bool isMyTurn = false;

    void initMenu();

    void showHuPai();

    void chupaiProcess(const Vec2 &location);

public:
    // there's no 'id' in cpp, so we recommend returning the class instance pointer
    static cocos2d::Scene *createScene();

    // Here's a difference. Method 'init' in cocos2d-x returns bool, instead of returning 'id' in cocos2d-iphone
    virtual bool init();

    // implement the "static create()" method manually
    CREATE_FUNC(MaJiangMultiPlayerScene)

    void onEnter() override;

    void onExit() override;;

    void chi(Ref *ref);

    void chiLeft(Ref *ref);//left
    void chiMiddle(Ref *ref);//middle
    void chiRight(Ref *ref);//right

    void peng(Ref *ref);

    void gang(Ref *ref);

    void guo(Ref *ref);

    void hu(Ref *ref);

    void disableAllChoice();

    void onMouseUp(EventMouse *event);

    void selectToChi(int chiPosition);

    bool tryHuPai(Player *player1, Player *player2 = nullptr);

    void onEnterTransitionDidFinish() override;

    void doReadOppo();
};

#endif // __MAJIANG_SCENE_H__
