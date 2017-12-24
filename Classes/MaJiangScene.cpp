#include "MaJiangScene.h"

USING_NS_CC;

//extern const char* PHYSICSCONTACT_EVENT_NAME;

Scene *MaJiangScene::createScene() {

    // 'scene' is an autorelease object
    auto scene = Scene::create();

    // 'layer' is an autorelease object
    auto layer = MaJiangScene::create();

    // add layer as a child to scene
    scene->addChild(layer);
    // return the scene
    return scene;

}

// on "init" you need to initialize your instance
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
            allMaJiang.pushBack(majiang);
        }
    }

    allMaJiang.shuffle();

    std::for_each(allMaJiang.cbegin(), allMaJiang.cend(), [this](MaJiang *maJiang) {
        maJiang->setAnchorPoint(Vec2::ZERO);
        maJiang->setVisible(false);
        this->addChild(maJiang);
    });


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

    //初始化"吃碰杠胡过"菜单
    int fontSize = 60;
    TTFConfig ttfConfig("fonts/STLITI.ttf", fontSize, GlyphCollection::DYNAMIC);
    ttfConfig.bold = true;
    float labelYPosition = PLAYER1_MAJIANGS_POSITTION + (*allMaJiang.begin())->getContentSize().height + 20;
    auto labelXPosition = visibleSize.width / 2;
    typedef void (MaJiangScene::*callBackFuncPointer)(cocos2d::Ref *);
    callBackFuncPointer labelCallBackFunc[] = {&MaJiangScene::chi, &MaJiangScene::peng, &MaJiangScene::gang,
                                               &MaJiangScene::hu,
                                               &MaJiangScene::guo};
    std::string labels[] = {"吃", "碰", "杠", "胡", "过"};
    MenuItemLabel *menuItemLabels[5];
    fontSize += 20;
    for (int i = 0; i <= GUO - CHI; ++i) {
        auto label = Label::createWithTTF(ttfConfig, labels[i]);
        label->setColor(Color3B::RED);
        auto labelMenuItem = MenuItemLabel::create(label, std::bind(labelCallBackFunc[i], this, std::placeholders::_1));

        labelMenuItem->setAnchorPoint(Vec2::ZERO);
        labelMenuItem->setPosition(Vec2(labelXPosition, labelYPosition));
        labelMenuItem->setVisible(false);
        labelMenuItem->setEnabled(false);
        labelMenuItem->setTag(MenuItemTag(CHI + i));
        labelXPosition += fontSize;

        menuItemLabels[i] = labelMenuItem;
    }

    auto menu = Menu::create(menuItemLabels[0], menuItemLabels[1], menuItemLabels[2], menuItemLabels[3],
                             menuItemLabels[4], NULL);
    menu->setPosition(Vec2::ZERO);
    menu->setTag(MenuItemTag::MENU);
    this->addChild(menu);

    return true;
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

void MaJiangScene::chi(Ref *ref) {
    auto mj = oppoPlayer.popLastOutMaJiang();
    hostPlayer.chi(mj);

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


void MaJiangScene::disableAllChoice() {
    menuEnable = false;
    auto menu = dynamic_cast<Menu *>(this->getChildByTag(MenuItemTag::MENU));
    MenuItem *item = nullptr;
    for (int i = MenuItemTag::CHI; i <= MenuItemTag::GUO; ++i) {
        item = dynamic_cast<MenuItem *>(menu->getChildByTag(MenuItemTag(i)));
        if (item != nullptr) {
            item->setVisible(false);
            item->setEnabled(false);
        }
    }
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

void MaJiangScene::onMouseUp(EventMouse *event) {

    if (!isMyTurn || menuEnable) {
        return;
    }
    auto location = event->getLocationInView();

    if (hostPlayer.chupai(location)) {//我先出牌
        //AI摸牌，出牌
        if (!tryHuPai(&oppoPlayer, &hostPlayer)) {
            if (oppoPlayer.isGang(hostPlayer.getLastOutType()) || oppoPlayer.isPeng(hostPlayer.getLastOutType())) {
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

        //判断是否可以胡, 碰，杠
        bool isGang, isPeng, isChi, isHu;
        isChi = hostPlayer.isChi(oppoPlayer.getLastOutType());
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

bool MaJiangScene::tryHuPai(Player *player1, Player *player2) {
    if (player2 != nullptr) {
        if (player1->isHupai(player2->getLastOutType())) {
            auto mj = player2->popLastOutMaJiang();
            mj->setScale(1);
            player1->hupai(mj);
//            if (typeid(player1) == typeid(AIOppoPlayer)) {
//                dynamic_cast<AIOppoPlayer *>(player1)->displayAll();
//            }
            showHuPai();
            return true;
        }
    } else {
        if (player1->isHupai()) {
            auto mj = player1->popLastPlayerMaJiang();
            player1->hupai(mj);
            showHuPai();
            return true;
        }
    }
    return false;
}











