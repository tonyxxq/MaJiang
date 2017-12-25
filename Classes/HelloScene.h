#ifndef __HELLO_SCENE_H__
#define __HELLO_SCENE_H__

#include "cocos2d.h"

class HelloScene : public cocos2d::Scene {
public:
    virtual bool init() override;

    static cocos2d::Scene *scene();

    void PVEMenuCallBack(Ref *ref);

    void PVPMenuCallBack(Ref *ref);

    CREATE_FUNC(HelloScene);

    void exitGameCallBack(Ref *ref);
};

#endif // __HELLOWORLD_SCENE_H__
