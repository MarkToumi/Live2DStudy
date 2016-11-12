/**
 *
 *  You can modify and use this source freely
 *  only for the development of application related Live2D.
 *
 *  (c) Live2D Inc. All rights reserved.
 */
#include "LAppView.h"

//Live2D Application
#include "LAppLive2DManager.h"
#include "LAppDefine.h"


using namespace live2d;
using namespace live2d::framework;

USING_NS_CC;

LAppView::LAppView():DrawNode()
{
	log("hello");
}

//bool LAppView::init()
//{
//    if ( !Sprite::init() )
//    {
//        return false;
//    }
//	
//    // イベントリスナー作成
//    auto listener = EventListenerTouchAllAtOnce::create();
//	
//    // タッチメソッド設定
//    listener->onTouchesBegan = CC_CALLBACK_2(LAppView::onTouchesBegan, this);
//    listener->onTouchesMoved = CC_CALLBACK_2(LAppView::onTouchesMoved, this);
//    listener->onTouchesEnded = CC_CALLBACK_2(LAppView::onTouchesEnded, this);
//   
//	
//    // 優先度100でディスパッチャーに登録
//    this->getEventDispatcher()->addEventListenerWithFixedPriority(listener, 100);
//	
//    return true;
//}


void LAppView::onEnter()
{
	DrawNode::onEnter();
	
	
	// タッチ関係のイベント管理
	touchMgr=new TouchManager();
	
	// デバイス座標からスクリーン座標に変換するための
	deviceToScreen=new L2DMatrix44();
	
	// 画面の表示の拡大縮小や移動の変換を行う行列
	viewMatrix=new L2DViewMatrix();

	Size size=Director::getInstance()->getWinSize();
	
	float width=size.width;
	float height=size.height;
	float ratio=(float)height/width;
	float left = VIEW_LOGICAL_LEFT;
	float right = VIEW_LOGICAL_RIGHT;
	float bottom = -ratio;
	float top = ratio;
	
	viewMatrix->setScreenRect(left,right,bottom,top);// デバイスに対応する画面の範囲。 Xの左端, Xの右端, Yの下端, Yの上端
	
	float screenW=abs(left-right);
	deviceToScreen->multTranslate(-width/2.0f,-height/2.0f );
	deviceToScreen->multScale( screenW/width , -screenW/width );
	
	// 表示範囲の設定
	viewMatrix->setMaxScale( VIEW_MAX_SCALE );// 限界拡大率
	viewMatrix->setMinScale( VIEW_MIN_SCALE );// 限界縮小率
	
	// 表示できる最大範囲
	viewMatrix->setMaxScreenRect(
								 VIEW_LOGICAL_MAX_LEFT,
								 VIEW_LOGICAL_MAX_RIGHT,
								 VIEW_LOGICAL_MAX_BOTTOM,
								 VIEW_LOGICAL_MAX_TOP
								 );

	// イベントリスナー作成
    auto listener = EventListenerTouchAllAtOnce::create();
	
    // タッチメソッド設定
    listener->onTouchesBegan = CC_CALLBACK_2(LAppView::onTouchesBegan, this);
    listener->onTouchesMoved = CC_CALLBACK_2(LAppView::onTouchesMoved, this);
    listener->onTouchesEnded = CC_CALLBACK_2(LAppView::onTouchesEnded, this);
   
	
    // 優先度100でディスパッチャーに登録
    this->getEventDispatcher()->addEventListenerWithFixedPriority(listener, 100);
	
}


void LAppView::onExit()
{
	DrawNode::onExit();
	
	delete touchMgr;
	delete deviceToScreen;
	delete viewMatrix;
	
}


void LAppView::draw(cocos2d::Renderer *renderer, const cocos2d::Mat4 &transform, uint32_t flags)
{
	DrawNode::draw(renderer, transform, flags);
	
	
	_customCommand.init(_globalZOrder);
	_customCommand.func = CC_CALLBACK_0(LAppView::onDraw, this, transform, flags);
	renderer->addCommand(&_customCommand);
}


void LAppView::onDraw(const cocos2d::Mat4 &transform, uint32_t flags)
{
	kmGLPushMatrix();
    kmGLLoadMatrix(&transform);
	
	LAppLive2DManager* Live2DMgr=LAppLive2DManager::getInstance();
	Live2DMgr->onUpdate();
	
	
	kmGLPopMatrix();
}


void LAppView::onTouchesBegan(const std::vector<Touch*>& touches, Event *event) {
    // タッチ開始
	size_t touchNum = touches.size() ;
		
	if( touchNum == 1 )
	{
		Point pt =  touches[0]->getLocationInView();
        if(LAppDefine::DEBUG_TOUCH_LOG)log( "touchesBegan x:%.0f y:%.0f",pt.x,pt.y);
        touchMgr->touchesBegan(pt.x,pt.y);
	}
}

void LAppView::onTouchesMoved(const std::vector<Touch*>& touches, Event *event) {
    // タッチ中
	size_t touchNum = touches.size() ;
	
	float screenX=this->transformScreenX(touchMgr->getX());
	float screenY=this->transformScreenY(touchMgr->getY());
	float viewX=this->transformViewX(touchMgr->getX());
	float viewY=this->transformViewY(touchMgr->getY());
	
	if( touchNum == 1 )
	{
		Point pt = touches[0]->getLocationInView();
		
		if(LAppDefine::DEBUG_TOUCH_LOG)log( "touchesMoved device{x:%.0f y:%.0f} screen{x:%.2f y:%.2f} view{x:%.2f y:%.2f}",pt.x,pt.y,screenX,screenY,viewX,viewY);
        touchMgr->touchesMoved(pt.x,pt.y);
	}
	LAppLive2DManager* live2DMgr=LAppLive2DManager::getInstance();
	live2DMgr->onDrag(viewX, viewY);
}

void LAppView::onTouchesEnded(const std::vector<cocos2d::Touch*>& touches, cocos2d::Event *event) {
    // タッチ終了
    LAppLive2DManager* live2DMgr=LAppLive2DManager::getInstance();
	live2DMgr->onDrag(0, 0);
    
    if( touches.size() == 1 )
    {
        // シングルタップ
        float x = deviceToScreen->transformX( touchMgr->getX() );// 論理座標変換した座標を取得。
        float y = deviceToScreen->transformY( touchMgr->getY() );// 論理座標変換した座標を取得。
        if (LAppDefine::DEBUG_LOG) log( "touchesEnded x:%.2f y:%.2f",x,y);
		live2DMgr->onTap(x,y);
    }
}


void LAppView::updateViewMatrix(float dx ,float dy ,float cx ,float cy ,float scale)
{
	LAppLive2DManager* live2DMgr=LAppLive2DManager::getInstance();
	
	// 拡大縮小
	viewMatrix->adjustScale(cx, cy, scale);
	
	// 移動
	viewMatrix->adjustTranslate(dx, dy) ;
	
	live2DMgr->setViewMatrix(viewMatrix);
}


float LAppView::transformViewX(float deviceX)
{
	float screenX = deviceToScreen->transformX( deviceX );// 論理座標変換した座標を取得。
	return  viewMatrix->invertTransformX(screenX);// 拡大、縮小、移動後の値。
}


float LAppView::transformViewY(float deviceY)
{
	float screenY = deviceToScreen->transformY( deviceY );// 論理座標変換した座標を取得。
	return  viewMatrix->invertTransformY(screenY);// 拡大、縮小、移動後の値。
}


float LAppView::transformScreenX(float deviceX)
{
	return  deviceToScreen->transformX( deviceX );
}


float LAppView::transformScreenY(float deviceY)
{
	return  deviceToScreen->transformY( deviceY );
}

LAppView* LAppView::createDrawNode()
{
	LAppView *ret = new (std::nothrow) LAppView();
	if (ret && ret->init())
	{
		ret->autorelease();
		return ret;
	}
	CC_SAFE_DELETE(ret);
	return nullptr;

}