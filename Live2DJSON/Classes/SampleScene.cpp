/**
 *
 *  You can modify and use this source freely
 *  only for the development of application related Live2D.
 *
 *  (c) Live2D Inc. All rights reserved.
 */
#include "SampleScene.h"
#include "LAppView.h"
#include "LAppLive2DManager.h"
#include "LAppDefine.h"
#include "Live2DFramework.h"
USING_NS_CC;

Scene* SampleScene::createScene()
{
    // 'scene' is an autorelease object
    auto scene = Scene::create();
    
    // 'layer' is an autorelease object
    auto layer = SampleScene::create();

    // add layer as a child to scene
    scene->addChild(layer);

    // return the scene
    return scene;
}

// on "init" you need to initialize your instance
bool SampleScene::init()
{
    //////////////////////////////
    // 1. super init first
    if ( !Layer::init() )
    {
        return false;
    }
	
    
    Size visibleSize = Director::getInstance()->getVisibleSize();
    Point origin = Director::getInstance()->getVisibleOrigin();
	
    /////////////////////////////
    // 2. add a menu item with "X" image, which is clicked to quit the program
    //    you may modify it.
	
    // add a "close" icon to exit the progress. it's an autorelease object
    auto closeItem = MenuItemImage::create(
                                           "CloseNormal.png",
                                           "CloseSelected.png",
                                           CC_CALLBACK_1(SampleScene::menuCloseCallback, this));
    
	closeItem->setPosition(Point(origin.x + visibleSize.width - closeItem->getContentSize().width/2 ,
								 origin.y + closeItem->getContentSize().height/2));
	
    // create menu, it's an autorelease object
    auto menu = Menu::create(closeItem, NULL);
    menu->setPosition(Point::ZERO);
    this->addChild(menu, 1);
	
    /////////////////////////////
    // 3. add your codes below...

    // add a label shows "Hello World"
    // create and initialize a label
    

    Sprite* pSprite = Sprite::create("back_class_normal.png");

    // position the sprite on the center of the screen
    pSprite->setPosition(Point(visibleSize.width/2 + origin.x, visibleSize.height/2 + origin.y));
	pSprite->setScale(2);

    // add the sprite as a child to this layer
    this->addChild(pSprite, 0);
    
	
	
	auto *pChangeItem = MenuItemImage::create(
                                           "icon_gear.png",
                                           "icon_gear.png",
                                           CC_CALLBACK_1(SampleScene::menuChangeCallback, this));
    
	pChangeItem->setPosition(Point(origin.x + visibleSize.width - pChangeItem->getContentSize().width/2 ,
                                getContentSize().height - pChangeItem->getContentSize().height/2));
	
    // create menu, it's an autorelease object
    Menu* pChangeMenu = Menu::create(pChangeItem, NULL);
    pChangeMenu->setPosition(Point::ZERO);
    this->addChild(pChangeMenu, 1);
	
	

    // add "Live2DModel"
	auto* pLive2DSprite = LAppView::createDrawNode();
	//pLive2DSprite->init();

	this->addChild(pLive2DSprite);

    return true;
}


void SampleScene::menuCloseCallback(Ref* pSender)
{
    Director::getInstance()->end();

#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    exit(0);
#endif
}


void SampleScene::menuChangeCallback(Ref* pSender)
{
    LAppLive2DManager::getInstance()->nextScene();
	
}
