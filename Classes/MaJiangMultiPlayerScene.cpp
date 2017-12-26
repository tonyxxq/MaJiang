#include "MaJiangMultiPlayerScene.h"
#include "SocketConnector.h"

USING_NS_CC;

//extern const char* PHYSICSCONTACT_EVENT_NAME;

Scene *MaJiangMultiPlayerScene::createScene() {
    auto scene = Scene::create();
    auto layer = MaJiangMultiPlayerScene::create();
    scene->addChild(layer);

    return scene;
}

bool MaJiangMultiPlayerScene::init() {
    if (!Layer::init()) {
        return false;
    }

    Size visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();

    auto background = Sprite::create("background.png");
    background->setAnchorPoint(Vec2(0, 0));
    this->addChild(background);

    int *pai_ptr = socketConnector.getFrame();
    log("getframe..");
    for (int i = 0; i < pai_ptr[0] - 2; ++i) {
        auto majiang = MaJiang::create(MaJiangType(pai_ptr[i + 2]));
        majiang->setAnchorPoint(Vec2::ZERO);
        majiang->setVisible(false);
        this->addChild(majiang);
        allMaJiang.pushBack(majiang);
    }
    delete[] pai_ptr;
    log("host start...");

    bool isHost = socketConnector.getIsHost();
    if (isHost) {
        isMyTurn = true;
        for (int k = 0; k < 13; ++k) {
            auto majiang = allMaJiang.consume();
            hostPlayer.mopai(majiang);
        }
        for (int i = 0; i < 12; ++i) {
            auto majiang = allMaJiang.consume();
            oppoPlayer.mopai(majiang);
        }
    } else {
        for (int k = 0; k < 13; ++k) {
            auto majiang = allMaJiang.consume();
            oppoPlayer.mopai(majiang);
        }
        for (int i = 0; i < 12; ++i) {
            auto majiang = allMaJiang.consume();
            hostPlayer.mopai(majiang);
        }
    }

    oppoPlayer.sort();
    hostPlayer.sort();

    hostPlayer.resetColor();
//    oppoPlayer.resetColor();

    hostPlayer.display();
    oppoPlayer.display();

    initMenu();
    log("init...");

    return true;
}

void MaJiangMultiPlayerScene::initMenu() {
    auto visibleSize = Director::getInstance()->getVisibleSize();

    //初始化"吃碰杠胡过"菜单
    int fontSize = 60;
    TTFConfig ttfConfig("fonts/STLITI.ttf", fontSize, GlyphCollection::DYNAMIC);
    ttfConfig.bold = true;
    float labelYPosition = PLAYER1_MAJIANGS_POSITTION + (*allMaJiang.begin())->getContentSize().height + 20;
    auto labelXPosition = visibleSize.width / 2;
    typedef void (MaJiangMultiPlayerScene::*callBackFuncPointer)(cocos2d::Ref *);
    callBackFuncPointer labelCallBackFunc[] = {&MaJiangMultiPlayerScene::chi, &MaJiangMultiPlayerScene::peng,
                                               &MaJiangMultiPlayerScene::gang,
                                               &MaJiangMultiPlayerScene::hu,
                                               &MaJiangMultiPlayerScene::guo, &MaJiangMultiPlayerScene::chiLeft,
                                               &MaJiangMultiPlayerScene::chiMiddle,
                                               &MaJiangMultiPlayerScene::chiRight};
    std::string labels[] = {"吃", "碰", "杠", "胡", "过", "左", "中", "右"};
    auto menu = Menu::create();
    fontSize += 20;
    for (int i = 0; i <= RIGHT - CHI; ++i) {
        if (i == LEFT - CHI) {
            labelXPosition = visibleSize.width / 2;
        }
        auto label = Label::createWithTTF(ttfConfig, labels[i]);
        label->setColor(Color3B::RED);
        auto labelMenuItem = MenuItemLabel::create(label, std::bind(labelCallBackFunc[i], this, std::placeholders::_1));

        labelMenuItem->setAnchorPoint(Vec2::ZERO);
        labelMenuItem->setPosition(Vec2(labelXPosition, labelYPosition));
        labelMenuItem->setVisible(false);
        labelMenuItem->setEnabled(false);
        labelMenuItem->setTag(MenuItemTag(CHI + i));
        labelXPosition += fontSize;

        menu->addChild(labelMenuItem);
    }

    menu->setPosition(Vec2::ZERO);
    menu->setTag(MenuItemTag::MENU);
    this->addChild(menu);
}

//多人游戏主流程
void MaJiangMultiPlayerScene::chupaiProcess(const Vec2 &location) { //事件处理已经保证了轮到我出牌才会进入这个函数
    log("process...");
    if (hostPlayer.chupai(location)) {
        isMyTurn = false;
        int frame[] = {3, MaJiangCommand::CHUPAI, hostPlayer.getLastOutType()};
        socketConnector.putFrame(frame);

        hostPlayer.display();
    }
}

void MaJiangMultiPlayerScene::doReadOppo() {
    while (true) {
        if (!isMyTurn) {
            int *frame = socketConnector.getFrame();
            if (frame[1] == MaJiangCommand::CHUPAI) {
                oppoPlayer.chupai(MaJiangType(frame[2]));
                isMyTurn = true;
                oppoPlayer.display();
            }
        } else {
            sleep(1);
        }
    }
}

void MaJiangMultiPlayerScene::peng(Ref *ref) {
    auto majiang = oppoPlayer.popLastOutMaJiang();
    hostPlayer.peng(majiang);

    hostPlayer.display();
    oppoPlayer.display();
    disableAllChoice();
}

void MaJiangMultiPlayerScene::gang(Ref *ref) {
    auto majiang = oppoPlayer.popLastOutMaJiang();
    hostPlayer.gang(majiang);

    hostPlayer.display();
    oppoPlayer.display();
    disableAllChoice();
}

void MaJiangMultiPlayerScene::selectToChi(int chiPosition) {
    menuEnable = true;
    auto menu = dynamic_cast<Menu *>(this->getChildByTag(MENU));

    for (int position_tmp = RIGHT; position_tmp >= LEFT; --position_tmp) {
        if (chiPosition & 0b001) {
            auto menuItem = dynamic_cast<MenuItem *>(menu->getChildByTag(position_tmp));
            menuItem->setVisible(true);
            menuItem->setEnabled(true);
        }
        chiPosition >>= 1;
    }
}

void MaJiangMultiPlayerScene::chi(Ref *ref) {
    int chiPosition = hostPlayer.isChi(oppoPlayer.getLastOutType());
    if (chiPosition == 1 || chiPosition == 2 || chiPosition == 4) {
        auto mj = oppoPlayer.popLastOutMaJiang();
        hostPlayer.chi(mj);
    } else {
        disableAllChoice();
        selectToChi(chiPosition);
        return;
    }

    hostPlayer.display();
    oppoPlayer.display();
    disableAllChoice();
}

void MaJiangMultiPlayerScene::hu(Ref *ref) {
    tryHuPai(&hostPlayer, &oppoPlayer);
    disableAllChoice();
}

void MaJiangMultiPlayerScene::guo(Ref *ref) {
    //摸牌
    auto newMaJiang = allMaJiang.consume();
    hostPlayer.mopai(newMaJiang);
    hostPlayer.sort();
    hostPlayer.display();
    if (hostPlayer.isHupai()) {
        showHuPai();
    }
    disableAllChoice();
}

void MaJiangMultiPlayerScene::onMouseUp(EventMouse *event) {

    if (!isMyTurn || menuEnable) {
        return;
    }
    auto location = event->getLocationInView();

    chupaiProcess(location);
}

bool MaJiangMultiPlayerScene::tryHuPai(Player *player1, Player *player2) {
    std::function<void(void)> func = [player1]() {//记录数据...
        auto userDefaule = UserDefault::getInstance();
        if (typeid(*player1) == typeid(AIOppoPlayer)) {
            auto score = userDefaule->getIntegerForKey("aiPlayerScore", 0);
            score++;
            userDefaule->setIntegerForKey("aiPlayerScore", score);
        } else {
            auto score = userDefaule->getIntegerForKey("HostPlayerScore", 0);
            score++;
            userDefaule->setIntegerForKey("HostPlayerScore", score);
        }
    };

    if (player2 != nullptr) {
        if (player1->isHupai(player2->getLastOutType())) {//被点炮
            auto mj = player2->popLastOutMaJiang();
            mj->setScale(1);
            player1->hupai(mj);
            showHuPai();
            func();
            return true;
        }
    } else {
        if (player1->isHupai()) {//自摸
            auto mj = player1->popLastPlayerMaJiang();
            player1->hupai(mj);
            showHuPai();
            func();
            return true;
        }
    }
    return false;
}

void MaJiangMultiPlayerScene::showHuPai() {
    oppoPlayer.sort();
    hostPlayer.sort();
    oppoPlayer.display();
    hostPlayer.display();

    auto hule = Sprite::create("hule.png");
    hule->setAnchorPoint(Vec2::ZERO);
    this->addChild(hule);
    Director::getInstance()->getEventDispatcher()->removeAllEventListeners();
    Director::getInstance()->pause();
}

void MaJiangMultiPlayerScene::chiLeft(Ref *ref) {
    auto mj = oppoPlayer.popLastOutMaJiang();
    hostPlayer.chi(mj, 0b100);

    hostPlayer.display();
    oppoPlayer.display();
    disableAllChoice();
}

void MaJiangMultiPlayerScene::disableAllChoice() {
    menuEnable = false;
    auto menu = dynamic_cast<Menu *>(this->getChildByTag(MenuItemTag::MENU));
    MenuItem *item = nullptr;
    for (int i = MenuItemTag::CHI; i <= MenuItemTag::RIGHT; ++i) {
        item = dynamic_cast<MenuItem *>(menu->getChildByTag(MenuItemTag(i)));
        if (item != nullptr) {
            item->setVisible(false);
            item->setEnabled(false);
        }
    }
}

void MaJiangMultiPlayerScene::chiMiddle(Ref *ref) {
    auto mj = oppoPlayer.popLastOutMaJiang();
    hostPlayer.chi(mj, 0b010);

    hostPlayer.display();
    oppoPlayer.display();
    disableAllChoice();
}

void MaJiangMultiPlayerScene::chiRight(Ref *ref) {
    auto mj = oppoPlayer.popLastOutMaJiang();
    hostPlayer.chi(mj, 0b001);

    hostPlayer.display();
    oppoPlayer.display();
    disableAllChoice();
}

void MaJiangMultiPlayerScene::onEnter() {
    Node::onEnter();
    auto mouseListener = EventListenerMouse::create();
    mouseListener->onMouseUp = CC_CALLBACK_1(MaJiangMultiPlayerScene::onMouseUp, this);

    Director::getInstance()->getEventDispatcher()->addEventListenerWithFixedPriority(mouseListener, 1);
}

void MaJiangMultiPlayerScene::onExit() {
    Node::onExit();
    Director::getInstance()->getEventDispatcher()->removeAllEventListeners();
}

void MaJiangMultiPlayerScene::onEnterTransitionDidFinish() {
    Node::onEnterTransitionDidFinish();
    log("enter...");
    readOppoThread = new std::thread(CC_CALLBACK_0(MaJiangMultiPlayerScene::doReadOppo, this));
    readOppoThread->detach();
    log("enter...");
}










