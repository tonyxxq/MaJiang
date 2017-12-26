#include "MaJiangScene.h"

USING_NS_CC;

//extern const char* PHYSICSCONTACT_EVENT_NAME;

Scene *MaJiangScene::createScene() {
    auto scene = Scene::create();
    auto layer = MaJiangScene::create();
    scene->addChild(layer);

    return scene;
}

bool MaJiangScene::init() {
    if (!Layer::init()) {
        return false;
    }

    Size visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();

    auto background = Sprite::create("background.png");
    background->setAnchorPoint(Vec2(0, 0));
    this->addChild(background);

    //创建所有的麻将实例
    for (int i = WAN_1; i <= HONGZHONG; ++i) {
        for (int j = 0; j < 4; ++j) {
            auto majiang = MaJiang::create(MaJiangType(i));
            majiang->setAnchorPoint(Vec2::ZERO);
            majiang->setVisible(false);
            this->addChild(majiang);
            allMaJiang.pushBack(majiang);
        }
    }

    allMaJiang.shuffle();

    for (int k = 0; k < 13; ++k) {
        auto majiang = allMaJiang.consume();
        oppoPlayer.mopai(majiang);

        majiang = allMaJiang.consume();
        hostPlayer.mopai(majiang);
    }

    auto majiang = allMaJiang.consume();//host 14张牌
    hostPlayer.mopai(majiang);

    oppoPlayer.sort();
    hostPlayer.sort();

    hostPlayer.resetColor();
//    oppoPlayer.resetColor();

    hostPlayer.display();
    oppoPlayer.display();

    initMenu();

    return true;
}

void MaJiangScene::initMenu() {
    auto visibleSize = Director::getInstance()->getVisibleSize();

    //初始化"吃碰杠胡过"菜单
    int fontSize = 60;
    TTFConfig ttfConfig("fonts/STLITI.ttf", fontSize, GlyphCollection::DYNAMIC);
    ttfConfig.bold = true;
    float labelYPosition = PLAYER1_MAJIANGS_POSITTION + (*allMaJiang.begin())->getContentSize().height + 20;
    auto labelXPosition = visibleSize.width / 2;
    typedef void (MaJiangScene::*callBackFuncPointer)(cocos2d::Ref *);
    callBackFuncPointer labelCallBackFunc[] = {&MaJiangScene::chi, &MaJiangScene::peng, &MaJiangScene::gang,
                                               &MaJiangScene::hu,
                                               &MaJiangScene::guo, &MaJiangScene::chiLeft, &MaJiangScene::chiMiddle,
                                               &MaJiangScene::chiRight};
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

//单人游戏主流程
void MaJiangScene::chupaiProcess(const Vec2 &location) {
    if (hostPlayer.chupai(location)) {//我先出牌
        //AI摸牌，出牌
        if (!tryHuPai(&oppoPlayer, &hostPlayer)) {
            if (oppoPlayer.isGang(hostPlayer.getLastOutType()) || oppoPlayer.isPeng(hostPlayer.getLastOutType()) ||
                oppoPlayer.isChi(hostPlayer.getLastOutType())) {
                auto mj = hostPlayer.popLastOutMaJiang();
                oppoPlayer.chupai(mj);
            } else {
                oppoPlayer.mopai(allMaJiang.consume());

                if (!tryHuPai(&oppoPlayer)) {//ai判断自摸
                    oppoPlayer.chupai();
                }
            }
        }

        oppoPlayer.sort();
        oppoPlayer.display();

        //判断是否可以吃胡碰杠
        bool isGang, isPeng, isChi, isHu;
        isChi = hostPlayer.isChi(oppoPlayer.getLastOutType()) != 0;
        isPeng = hostPlayer.isPeng(oppoPlayer.getLastOutType());
        isGang = hostPlayer.isGang(oppoPlayer.getLastOutType());
        isHu = hostPlayer.isHupai(oppoPlayer.getLastOutType());
        bool setMenuItemHelper[] = {isChi, isPeng, isGang, isHu};

        if (isHu || isChi || isGang || isPeng) {
            menuEnable = true;

            auto menu = this->getChildByTag(MenuItemTag::MENU);

            for (int i = 0; i <= HU - CHI; ++i) {
                auto item = dynamic_cast<MenuItem *>(menu->getChildByTag(MenuItemTag(CHI + i)));
                item->setVisible(setMenuItemHelper[i]);
                item->setEnabled(setMenuItemHelper[i]);
            }

            auto guo = dynamic_cast<MenuItem *>(menu->getChildByTag(MenuItemTag::GUO));
            guo->setVisible(true);
            guo->setEnabled(true);

        } else {
            //摸牌
            auto newMaJiang = allMaJiang.consume();
            hostPlayer.mopai(newMaJiang);
            tryHuPai(&hostPlayer);
        }
        hostPlayer.sort();
        hostPlayer.display();
    }
}

void MaJiangScene::peng(Ref *ref) {
    auto majiang = oppoPlayer.popLastOutMaJiang();
    hostPlayer.peng(majiang);

    hostPlayer.display();
    oppoPlayer.display();
    disableAllChoice();
}

void MaJiangScene::gang(Ref *ref) {
    auto majiang = oppoPlayer.popLastOutMaJiang();
    hostPlayer.gang(majiang);

    hostPlayer.display();
    oppoPlayer.display();
    disableAllChoice();
}

void MaJiangScene::selectToChi(int chiPosition) {
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

void MaJiangScene::chi(Ref *ref) {
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

void MaJiangScene::hu(Ref *ref) {
    tryHuPai(&hostPlayer, &oppoPlayer);
    disableAllChoice();
}

void MaJiangScene::guo(Ref *ref) {
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

void MaJiangScene::onMouseUp(EventMouse *event) {

    if (!isMyTurn || menuEnable) {
        return;
    }
    auto location = event->getLocationInView();

    chupaiProcess(location);
}

bool MaJiangScene::tryHuPai(Player *player1, Player *player2) {
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

void MaJiangScene::showHuPai() {
    oppoPlayer.sort();
    hostPlayer.sort();
    oppoPlayer.displayAll();
    oppoPlayer.display();
    hostPlayer.display();

    auto hule = Sprite::create("hule.png");
    hule->setAnchorPoint(Vec2::ZERO);
    this->addChild(hule);
    Director::getInstance()->getEventDispatcher()->removeAllEventListeners();
    Director::getInstance()->pause();
}

void MaJiangScene::chiLeft(Ref *ref) {
    auto mj = oppoPlayer.popLastOutMaJiang();
    hostPlayer.chi(mj, 0b100);

    hostPlayer.display();
    oppoPlayer.display();
    disableAllChoice();
}

void MaJiangScene::disableAllChoice() {
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

void MaJiangScene::chiMiddle(Ref *ref) {
    auto mj = oppoPlayer.popLastOutMaJiang();
    hostPlayer.chi(mj, 0b010);

    hostPlayer.display();
    oppoPlayer.display();
    disableAllChoice();
}

void MaJiangScene::chiRight(Ref *ref) {
    auto mj = oppoPlayer.popLastOutMaJiang();
    hostPlayer.chi(mj, 0b001);

    hostPlayer.display();
    oppoPlayer.display();
    disableAllChoice();
}

void MaJiangScene::onEnter() {
    Node::onEnter();
    auto mouseListener = EventListenerMouse::create();
    mouseListener->onMouseUp = CC_CALLBACK_1(MaJiangScene::onMouseUp, this);

    Director::getInstance()->getEventDispatcher()->addEventListenerWithFixedPriority(mouseListener, 1);
}

void MaJiangScene::onExit() {
    Node::onExit();
    Director::getInstance()->getEventDispatcher()->removeAllEventListeners();
}












