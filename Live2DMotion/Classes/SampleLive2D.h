#ifndef __sampleCocos2dx__SampleLive2D__
#define __sampleCocos2dx__SampleLive2D__

#include "cocos2d.h"
#include "2d/CCSprite.h"
#include <vector>
#include "Live2DModelOpenGL.h"
#include "motion\Live2DMotion.h"
#include "motion\MotionQueueManager.h"

class SampleLive2D :public cocos2d::DrawNode {

	live2d::Live2DModelOpenGL* live2DModel;
	std::vector<live2d::Live2DMotion *> motions;
	live2d::MotionQueueManager* motionManager;
	int motionNum;
	float matrix[16];
public:
	SampleLive2D();
	virtual ~SampleLive2D();

	virtual bool init();
	virtual void draw(cocos2d::Renderer *renderer, const cocos2d::Mat4 &transform, uint32_t flags) override;
	void onDraw(const cocos2d::Mat4 &transform, uint32_t flags);
	
	bool onTouchBegan(cocos2d::Touch *touch, cocos2d::Event *event);
	void onTouchMoved(cocos2d::Touch *touch, cocos2d::Event *event);
	void onTouchEnded(cocos2d::Touch *touch, cocos2d::Event *event);

	bool tapCheck(char *area, float x, float y);

	void matrixInit(float *m);
	void translate(float x, float y);
	void scale(float x, float y);
	float transformX(float src);
	float transformY(float src);
	float invertTransformX(float src);
	float invertTransformY(float src);
	
	static SampleLive2D* createDrawNode();
protected:
	cocos2d::CustomCommand _customCommand;
};

#endif