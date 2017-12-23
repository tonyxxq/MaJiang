#include "MaJiangScene.h"
#include <algorithm>

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

    MenuItemImage *peng = MenuItemImage::create("peng.png", "peng.png", "peng.png",
                                                CC_CALLBACK_1(MaJiangScene::peng, this));
    peng->setAnchorPoint(Vec2::ZERO);
    peng->setPosition(Vec2(0, 0));
    peng->setVisible(false);
    peng->setEnabled(false);
    peng->setTag(MenuItemTag::PENG);

    MenuItemImage *gang = MenuItemImage::create("gang.png", "gang.png", "gang.png",
                                                CC_CALLBACK_1(MaJiangScene::gang, this));
    gang->setAnchorPoint(Vec2::ZERO);
    gang->setPosition(Vec2(45, 0));
    gang->setVisible(false);
    gang->setEnabled(false);
    gang->setTag(MenuItemTag::GANG);

    MenuItemImage *hu = MenuItemImage::create("hu.png", "hu.png", "hu.png",
                                              CC_CALLBACK_1(MaJiangScene::hu, this));
    hu->setAnchorPoint(Vec2::ZERO);
    hu->setPosition(Vec2(90, 0));
    hu->setVisible(false);
    hu->setEnabled(false);
    hu->setTag(MenuItemTag::HU);

    MenuItemImage *chi = MenuItemImage::create("chi.png", "chi.png", "chi.png",
                                               CC_CALLBACK_1(MaJiangScene::chi, this));
    chi->setAnchorPoint(Vec2::ZERO);
    chi->setPosition(Vec2(135, 0));
    chi->setVisible(false);
    chi->setEnabled(false);
    chi->setTag(MenuItemTag::CHI);

    MenuItemImage *guo = MenuItemImage::create("guo.png", "guo.png", "guo.png",
                                               CC_CALLBACK_1(MaJiangScene::guo, this));
    guo->setAnchorPoint(Vec2::ZERO);
    guo->setPosition(Vec2(180, 0));
    guo->setVisible(false);
    guo->setEnabled(false);
    guo->setTag(MenuItemTag::GUO);

    auto menu = Menu::create(peng, gang, hu, chi, guo, NULL);
    menu->setPosition(Vec2::ZERO);
    menu->setTag(MenuItemTag::MENU);
    this->addChild(menu);
    return true;
}

void MaJiangScene::onEnter() {
    Node::onEnter();
    auto mouseListener = EventListenerMouse::create();
    mouseListener->onMouseUp = [this](EventMouse *event) {
        if (!isMyTurn || menuEnable) {
            return;
        }
        auto location = event->getLocationInView();

        if (hostPlayer.chupai(location)) {//我先出牌
            //AI摸牌，出牌
            if (oppoPlayer.isHupai(hostPlayer.getLastOutType())) {
                auto mj = hostPlayer.popLastOutMaJiang();
                mj->setScale(1);
                oppoPlayer.hupai(mj);
                oppoPlayer.displayAll();
                hostPlayer.display();

                auto hule = Sprite::create("hule.png");
                hule->setAnchorPoint(Vec2::ZERO);
                this->addChild(hule);
                Director::getInstance()->getEventDispatcher()->removeAllEventListeners();
            } else {
                if (oppoPlayer.isGang(hostPlayer.getLastOutType()) || oppoPlayer.isPeng(hostPlayer.getLastOutType())) {
                    auto mj = hostPlayer.popLastOutMaJiang();
                    oppoPlayer.chupai(mj);
                    oppoPlayer.sort();
                    oppoPlayer.display();
                } else {
                    oppoPlayer.mopai(allMaJiang.consume());
                    oppoPlayer.sort();
                    oppoPlayer.display();

                    if (oppoPlayer.isHupai()) {
                        oppoPlayer.displayAll();
                        auto hule = Sprite::create("hule.png");
                        hule->setAnchorPoint(Vec2::ZERO);
                        this->addChild(hule);
                        Director::getInstance()->getEventDispatcher()->removeAllEventListeners();
                    } else {
                        oppoPlayer.chupai();
                        oppoPlayer.sort();
                        oppoPlayer.display();
                    }
                }
            }

            //判断是否可以胡, 碰，杠
            bool isGang, isPeng, isChi, isHu;
            isHu = hostPlayer.isHupai(oppoPlayer.getLastOutType());
            isChi = hostPlayer.isChi(oppoPlayer.getLastOutType());
            isGang = hostPlayer.isGang(oppoPlayer.getLastOutType());
            isPeng = hostPlayer.isPeng(oppoPlayer.getLastOutType());

            if (isHu || isChi || isGang || isPeng) {
                menuEnable = true;
                hostPlayer.sort();
                hostPlayer.display();
                auto menu = dynamic_cast<Menu *>(this->getChildByTag(MenuItemTag::MENU));


                auto hu = dynamic_cast<MenuItemImage *>(menu->getChildByTag(MenuItemTag::HU));
                hu->setVisible(isHu);
                hu->setEnabled(isHu);

                auto gang = dynamic_cast<MenuItemImage *>(menu->getChildByTag(MenuItemTag::GANG));
                gang->setVisible(isGang);
                gang->setEnabled(isGang);

                auto peng = dynamic_cast<MenuItemImage *>(menu->getChildByTag(MenuItemTag::PENG));
                peng->setVisible(isPeng);
                peng->setEnabled(isPeng);

                auto chi = dynamic_cast<MenuItemImage *>(menu->getChildByTag(MenuItemTag::CHI));
                chi->setVisible(isChi);
                chi->setEnabled(isChi);

                auto guo = dynamic_cast<MenuItemImage *>(menu->getChildByTag(MenuItemTag::GUO));
                guo->setVisible(true);
                guo->setEnabled(true);

            } else {
                //摸牌
                auto newMaJiang = allMaJiang.consume();
                hostPlayer.mopai(newMaJiang);
                hostPlayer.sort();
                hostPlayer.display();
                if (hostPlayer.isHupai()) {//判断自摸
                    auto hule = Sprite::create("hule.png");
                    hule->setAnchorPoint(Vec2::ZERO);
                    this->addChild(hule);
                    Director::getInstance()->getEventDispatcher()->removeAllEventListeners();
                }
            }
        }
    };

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
    auto mj = oppoPlayer.popLastOutMaJiang();

    mj->setScale(1);
    hostPlayer.hupai(mj);
    hostPlayer.display();
    oppoPlayer.display();

    auto hule = Sprite::create("hule.png");
    hule->setAnchorPoint(Vec2::ZERO);
    this->addChild(hule);
    Director::getInstance()->getEventDispatcher()->removeAllEventListeners();

    disableAllChoice();
}

void MaJiangScene::guo(Ref *ref) {
    //摸牌
    auto newMaJiang = allMaJiang.consume();
    hostPlayer.mopai(newMaJiang);
    hostPlayer.sort();
    hostPlayer.display();
    if (hostPlayer.isHupai()) {
        auto hule = Sprite::create("hule.png");
        hule->setAnchorPoint(Vec2::ZERO);
        this->addChild(hule);
    }
    disableAllChoice();
}


void MaJiangScene::disableAllChoice() {
    menuEnable = false;
    auto menu = dynamic_cast<Menu *>(this->getChildByTag(MenuItemTag::MENU));
    MenuItemImage *item = nullptr;
    for (int i = MenuItemTag::PENG; i <= MenuItemTag::GUO; ++i) {
        item = dynamic_cast<MenuItemImage *>(menu->getChildByTag(MenuItemTag(i)));
        if (item != nullptr) {
            item->setVisible(false);
            item->setEnabled(false);
        }
    }
}











