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
    for (int i = 0; i < pai_ptr[0] - 2; ++i) {
        auto majiang = MaJiang::create(MaJiangType(pai_ptr[i + 2]));
        majiang->setAnchorPoint(Vec2::ZERO);
        majiang->setVisible(false);
        this->addChild(majiang);
        allMaJiang.pushBack(majiang);
    }
    delete[] pai_ptr;

    bool isHost = socketConnector.getIsHost();
    int numHost = 14;
    int numOppo = 1;
    if (isHost) {
        isMyTurn = true;
        for (int k = 0; k < numHost; ++k) {
            hostPlayer.mopai(allMaJiang.consume());
        }
        for (int i = 0; i < numOppo; ++i) {
            oppoPlayer.mopai(allMaJiang.consume());
        }
    } else {
        for (int k = 0; k < numHost; ++k) {
            oppoPlayer.mopai(allMaJiang.consume());
        }
        for (int i = 0; i < numOppo; ++i) {
            hostPlayer.mopai(allMaJiang.consume());
        }
    }

    oppoPlayer.sort();
    hostPlayer.sort();

    hostPlayer.resetColor();
//    oppoPlayer.resetColor();

    hostPlayer.display();
    oppoPlayer.display();

    initMenu();

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
    if (hostPlayer.chupai(location)) {
        int frame[] = {3, MaJiangCommand::CHUPAI, hostPlayer.getLastOutType()};
        socketConnector.putFrame(frame);

        hostPlayer.display();
        isMyTurn = false;
    }
}

void MaJiangMultiPlayerScene::doReadOppo() {
    while (true) {
        if (!isMyTurn) {
            int *frame = socketConnector.getFrame();
            if (frame[1] == MaJiangCommand::HU) {
                if (frame[0] == 2) {//对方是自摸
                    tryHuPai(&oppoPlayer, nullptr);
                } else if (frame[0] == 3) { // 对方是被点炮
                    tryHuPai(&oppoPlayer, &hostPlayer);
                }
            } else if (frame[1] == MaJiangCommand::MOPAI) {
                oppoPlayer.mopai(allMaJiang.consume());
                oppoPlayer.sort();
                oppoPlayer.display();
            } else if (frame[1] >= MaJiangCommand::CHUPAI && frame[1] <= MaJiangCommand::GANG) {
                switch (frame[1]) {
                    case MaJiangCommand::CHUPAI:
                        oppoPlayer.chupai(MaJiangType(frame[2]));
                        break;
                    case MaJiangCommand::CHI:
                        oppoPlayer.chi(hostPlayer.popLastOutMaJiang(), frame[2]);
                        break;
                    case MaJiangCommand::PENG:
                        oppoPlayer.peng(hostPlayer.popLastOutMaJiang());
                        break;
                    case MaJiangCommand::GANG:
                        oppoPlayer.gang(hostPlayer.popLastOutMaJiang());
                        break;
                    default:
                        break;
                }
                if (frame[1] != MaJiangCommand::CHUPAI) {//如果对面不是出牌,那么不能检测是否可以吃碰杠
                    oppoPlayer.sort();
                    oppoPlayer.display();
                    hostPlayer.sort();
                    hostPlayer.display();

                    delete[] frame;
                    continue;
                }
                //如果是吃碰杠还需要出一张牌,否则摸牌
                if (frame[1] >= CHI && frame[1] <= GANG) {
                    delete[] frame;
                    frame = socketConnector.getFrame();
                    assert(frame[1] == MaJiangCommand::CHUPAI);
                    oppoPlayer.chupai(MaJiangType(frame[2]));
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

                    int mopaiFrame[] = {2, MaJiangCommand::MOPAI};
                    socketConnector.putFrame(mopaiFrame);

                    tryHuPai(&hostPlayer, nullptr, true);
                }
                oppoPlayer.sort();
                oppoPlayer.display();
                hostPlayer.sort();
                hostPlayer.display();
                isMyTurn = true;
            }
            delete[] frame;
        } else {
            usleep(100000);
        }
    }
}

void MaJiangMultiPlayerScene::peng(Ref *ref) {
    auto majiang = oppoPlayer.popLastOutMaJiang();
    hostPlayer.peng(majiang);

    int frame[] = {2, MaJiangCommand::PENG};
    socketConnector.putFrame(frame);

    hostPlayer.display();
    oppoPlayer.display();

    disableAllChoice();
}

void MaJiangMultiPlayerScene::gang(Ref *ref) {
    auto majiang = oppoPlayer.popLastOutMaJiang();
    hostPlayer.gang(majiang);

    int frame[] = {2, MaJiangCommand::GANG};
    socketConnector.putFrame(frame);

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

        int chiFrame[] = {3, MaJiangCommand::CHI, chiPosition};
        socketConnector.putFrame(chiFrame);
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
    tryHuPai(&hostPlayer, &oppoPlayer, true);
    disableAllChoice();
}

void MaJiangMultiPlayerScene::guo(Ref *ref) {
    //摸牌
    auto newMaJiang = allMaJiang.consume();
    hostPlayer.mopai(newMaJiang);

    int frame[] = {2, MaJiangCommand::MOPAI};
    socketConnector.putFrame(frame);

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

bool MaJiangMultiPlayerScene::tryHuPai(Player *player1, Player *player2, bool isFirst) {
    if (player2 != nullptr) {
        if (player1->isHupai(player2->getLastOutType())) {//被点炮
            auto mj = player2->popLastOutMaJiang();
            mj->setScale(1);
            player1->hupai(mj);

            if (isFirst) {//如果通过对面发来胡牌的消息,那就不需要再将消息发回给对面
                int huFrame[] = {3, MaJiangCommand::HU, mj->maJiangType};
                socketConnector.putFrame(huFrame);
            }

            showHuPai();
            return true;
        }
    } else {
        if (player1->isHupai()) {//自摸
            auto mj = player1->popLastPlayerMaJiang();
            player1->hupai(mj);

            if (isFirst) {
                int huFrame[] = {2, MaJiangCommand::HU};
                socketConnector.putFrame(huFrame);
            }

            showHuPai();
            return true;
        }
    }
    return false;
}

void MaJiangMultiPlayerScene::showHuPai() {
    oppoPlayer.sort();
    hostPlayer.sort();
    oppoPlayer.displayAll();
    hostPlayer.displayAll();

    //bug??
//    auto hule = Sprite::create("hule.png");
//    hule->setAnchorPoint(Vec2::ZERO);
//    this->addChild(hule);

    Director::getInstance()->getEventDispatcher()->removeAllEventListeners();
    Director::getInstance()->pause();
}

void MaJiangMultiPlayerScene::chiLeft(Ref *ref) {
    auto mj = oppoPlayer.popLastOutMaJiang();
    hostPlayer.chi(mj, 0b100);

    int frame[] = {3, MaJiangCommand::CHI, 0b100};
    socketConnector.putFrame(frame);

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

    int frame[] = {3, MaJiangCommand::CHI, 0b010};
    socketConnector.putFrame(frame);

    hostPlayer.display();
    oppoPlayer.display();
    disableAllChoice();
}

void MaJiangMultiPlayerScene::chiRight(Ref *ref) {
    auto mj = oppoPlayer.popLastOutMaJiang();
    hostPlayer.chi(mj, 0b001);

    int frame[] = {3, MaJiangCommand::CHI, 0b001};
    socketConnector.putFrame(frame);

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
    readOppoThread = new std::thread(CC_CALLBACK_0(MaJiangMultiPlayerScene::doReadOppo, this));
    readOppoThread->detach();
}











