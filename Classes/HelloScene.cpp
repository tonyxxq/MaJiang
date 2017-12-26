#include "HelloScene.h"
#include "MaJiangScene.h"
#include "MaJiangMultiPlayerScene.h"

USING_NS_CC;


Scene *HelloScene::scene() {
    return HelloScene::create();
}

bool HelloScene::init() {
    if (!Scene::init()) {
        return false;
    }

    auto visibleSize = Director::getInstance()->getVisibleSize();
    auto origin = Director::getInstance()->getVisibleOrigin();

    auto background = LayerColor::create(Color4B(153, 204, 102, 255));
    this->addChild(background);

    int fontSize = 70;
    TTFConfig ttfConfig("fonts/STLITI.ttf", fontSize, GlyphCollection::DYNAMIC);
    ttfConfig.bold = true;

    std::string labels[] = {"单人游戏", "双人对战", "退出游戏"};
    typedef void(HelloScene::*callPointer)(Ref *);
    callPointer callBacks[] = {&HelloScene::PVEMenuCallBack, &HelloScene::PVPMenuCallBack,
                               &HelloScene::exitGameCallBack};
    auto menu = Menu::create(NULL);
    for (int i = 0; i < 3; ++i) {
        auto label = Label::createWithTTF(ttfConfig, labels[i], TextHAlignment::CENTER);
        label->setColor(Color3B(51, 51, 51));
        auto item = MenuItemLabel::create(label, std::bind(callBacks[i], this, std::placeholders::_1));
        menu->addChild(item);
    }

    menu->alignItemsVerticallyWithPadding(10);

    this->addChild(menu);

    return true;
}

void HelloScene::PVEMenuCallBack(Ref *ref) {
    auto scene = MaJiangScene::createScene();
    auto reScene = TransitionMoveInT::create(0.5f, scene);
    Director::getInstance()->replaceScene(reScene);
}

void HelloScene::PVPMenuCallBack(Ref *ref) {
    auto scene = MaJiangMultiPlayerScene::createScene();
    auto reScene = TransitionMoveInB::create(0.5f, scene);
    Director::getInstance()->replaceScene(reScene);
}

void HelloScene::exitGameCallBack(Ref *ref) {
    Director::getInstance()->end();
}