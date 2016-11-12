/**
 *
 *  You can modify and use this source freely
 *  only for the development of application related Live2D.
 *
 *  (c) Live2D Inc. All rights reserved.
 */
#include "AppDelegate.h"
#include "SampleScene.h"
#include "LAppLive2DManager.h"

using namespace live2d;

USING_NS_CC;

AppDelegate::AppDelegate() {
}

AppDelegate::~AppDelegate() 
{
#ifdef L2D_TARGET_ANDROID_ES2
    Director::getInstance()->getEventDispatcher()->removeEventListener(_recreatedEventlistener);
#endif
	LAppLive2DManager* Live2DMgr=LAppLive2DManager::getInstance();
	Live2DMgr->releaseInstance();
}

bool AppDelegate::applicationDidFinishLaunching() {
	// initialize director
    auto director = Director::getInstance();
    auto glview = director->getOpenGLView();
    if(!glview) {
        glview = GLViewImpl::create("Live2D SampleApp1");
		glview->setFrameSize(640,960);
        director->setOpenGLView(glview);
    }
	
	
    // turn on display FPS
    director->setDisplayStats(true);
	
    // set FPS. the default value is 1.0/60 if you don't call this
    director->setAnimationInterval(1.0 / 60);

	
	//Live2D
	LAppLive2DManager* Live2DMgr=LAppLive2DManager::getInstance();
	
	#ifdef L2D_TARGET_ANDROID_ES2
        _recreatedEventlistener = EventListenerCustom::create(EVENT_RENDERER_RECREATED, 
	        [this](EventCustom*) 
	        {
	        	LAppLive2DManager::getInstance()->reinit();
	        });
        director->getEventDispatcher()->addEventListenerWithFixedPriority(_recreatedEventlistener, -1);
	#endif

    // create a scene. it's an autorelease object
    auto scene = SampleScene::createScene();
	
    // run
    director->runWithScene(scene);

    return true;
}

// This function will be called when the app is inactive. When comes a phone call,it's be invoked too
void AppDelegate::applicationDidEnterBackground() {
    Director::getInstance()->stopAnimation();
	
    // if you use SimpleAudioEngine, it must be pause
    // SimpleAudioEngine::sharedEngine()->pauseBackgroundMusic();
}

// this function will be called when the app is active again
void AppDelegate::applicationWillEnterForeground() {
    Director::getInstance()->startAnimation();

    // if you use SimpleAudioEngine, it must resume here
    // SimpleAudioEngine::sharedEngine()->resumeBackgroundMusic();
}
