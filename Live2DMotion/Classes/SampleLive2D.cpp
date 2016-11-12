#include "SampleLive2D.h"
#include "base/CCDirector.h"
#include "util/UtSystem.h"

#include "graphics/DrawProfileCocos2D.h"
#include "platform/CCFileUtils.h"

using namespace live2d;

USING_NS_CC;
const char* TEXTURES[] = {
	"haru_02.1024/texture_00.png",
	"haru_02.1024/texture_01.png",
	"haru_02.1024/texture_02.png",
	NULL
};

const char* MOTION[] = {
	"motions/idle_01.mtn",
	"motions/idle_02.mtn",
	"motions/haru_m_05.mtn",
	"motions/tapBody_06.mtn",
	"motions/tapBody_09.mtn",
	NULL
};

SampleLive2D::SampleLive2D()
	: live2DModel(nullptr)
{
}

SampleLive2D::~SampleLive2D()
{
	delete live2DModel;
	// テクスチャを解放
	Director::getInstance()->getTextureCache()->removeTextureForKey(std::string(TEXTURES[0]));
	Director::getInstance()->getTextureCache()->removeTextureForKey(std::string(TEXTURES[1]));
	Director::getInstance()->getTextureCache()->removeTextureForKey(std::string(TEXTURES[2]));

}

bool SampleLive2D::init()
{
	srand((unsigned int)time(NULL));
	auto listener = EventListenerTouchOneByOne::create();
	listener->setSwallowTouches(true);
	listener->onTouchBegan = CC_CALLBACK_2(SampleLive2D::onTouchBegan, this);
	listener->onTouchMoved = CC_CALLBACK_2(SampleLive2D::onTouchMoved, this);
	listener->onTouchEnded = CC_CALLBACK_2(SampleLive2D::onTouchEnded, this);
	this->getEventDispatcher()->addEventListenerWithSceneGraphPriority(listener, this);

	if (DrawNode::init())
	{
		if (live2DModel != nullptr)
		{
			delete live2DModel;
			live2DModel = nullptr;
		}

		const char* MODEL = "haru_02.moc";

		unsigned char* buf;
		ssize_t bufSize;
		buf = FileUtils::getInstance()->getFileData(MODEL, "rb", &bufSize);

		live2DModel = Live2DModelOpenGL::loadModel(buf, bufSize);
		free(buf);

		auto chacheMgr = Director::getInstance()->getTextureCache();
		for (int i = 0; TEXTURES[i] != NULL; i++)
		{
			Texture2D* texture = chacheMgr->addImage(TEXTURES[i]);

			Texture2D::TexParams texParams = { GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE };
			texture->setTexParameters(texParams);
			texture->generateMipmap();

			int glTexNo = texture->getName();

			live2DModel->setTexture(i, glTexNo);// テクスチャとモデルを結びつける
		}

		live2DModel->setPartsOpacity("PARTS_01_ARM_L_A_002", 0.0);
		live2DModel->setPartsOpacity("PARTS_01_ARM_R_A_002", 0.0);

		unsigned char* motionbuf;
		ssize_t motion_Size;

		for (int i = 0; MOTION[i] != NULL; i++)
		{
			motionbuf = FileUtils::getInstance()->getFileData(MOTION[i], "rb", &motion_Size);
			//ベクトルに追加
			motions.push_back(Live2DMotion::loadMotion(motionbuf, (int)motion_Size));
		}

		motionNum = motions.size();
		
		motionManager = new MotionQueueManager();
		motionManager->startMotion(motions[0], false);
		
		// Live2Dモデルはローカル座標で左上を原点にして(CanvasWidth、CanvasWidth)
		// のサイズになるため、以下のようにして調整してください。
		float w = Director::getInstance()->getWinSize().width;
		float h = Director::getInstance()->getWinSize().height;
		float scx = 2.0 / live2DModel->getCanvasWidth();
		float scy = -2.0 / live2DModel->getCanvasWidth() * (w / h);
		float x = -1;
		float y = 1;
		matrixInit(matrix);
		translate(x, y);
		scale(scx, scy);

		live2DModel->setMatrix(matrix);// 位置を設定

		live2DModel->setPremultipliedAlpha(true);
	}

	return true;
}

void SampleLive2D::draw(cocos2d::Renderer *renderer, const cocos2d::Mat4 &transform, uint32_t flags)
{
	DrawNode::draw(renderer, transform, flags);


	_customCommand.init(_globalZOrder);
	_customCommand.func = CC_CALLBACK_0(SampleLive2D::onDraw, this, transform, flags);
	renderer->addCommand(&_customCommand);
}

void SampleLive2D::onDraw(const cocos2d::Mat4 &transform, uint32_t flags)
{
	kmGLPushMatrix();
	kmGLLoadMatrix(&transform);

	/*
	// モデルのパラメータを変更。動作確認用です。
	double t = (UtSystem::getUserTimeMSec() / 1000.0) * 2 * M_PI;// 1秒ごとに2π(1周期)増える
	double cycle = 3.0;// パラメータが一周する時間(秒)
	double value = sin(t / cycle);// -1から1の間を周期ごとに変化する
	live2DModel->setParamFloat("PARAM_ANGLE_X", (float)(30 * value));// PARAM_ANGLE_Xのパラメータが[cycle]秒ごとに-30から30まで変化する
	*/
	if (motionManager->isFinished())
	{
		int rn = rand() % 2;
		motionManager->startMotion(motions[rn], false);
	}
	motionManager->updateParam(live2DModel);
	// Live2Dの描画前と描画後に以下の関数を呼んでください
	// live2d::DrawProfileCocos2D::preDraw() ;
	// live2d::DrawProfileCocos2D::postDraw() ;
	// これはOpenGLの状態をもとに戻すための処理です。
	// これを行わない場合、Cocos2DかLive2Dどちらかで状態の不整合が起こります。
	live2d::DrawProfileCocos2D::preDraw();

	live2DModel->update();
	live2DModel->draw();

	live2d::DrawProfileCocos2D::postDraw();

	kmGLPopMatrix();
}

SampleLive2D* SampleLive2D::createDrawNode()
{
	SampleLive2D *ret = new SampleLive2D();
	if (ret && ret->init())
	{
		ret->autorelease();
		return ret;
	}
	else
	{
		CC_SAFE_DELETE(ret);
		return nullptr;
	}
}

bool SampleLive2D::onTouchBegan(cocos2d::Touch *touch, cocos2d::Event *event)
{
	return true;
}

void SampleLive2D::onTouchMoved(cocos2d::Touch *touch, cocos2d::Event *event)
{
}

void SampleLive2D::onTouchEnded(cocos2d::Touch *touch, cocos2d::Event *event)
{
	Point p = touch->getLocationInView();
	float x = p.x;
	float y = p.y;
	if (tapCheck("D_REF.BODY", x, y)){
		int sn = motionNum - 2;
		int rn = rand() % sn + 2;
		motionManager->startMotion(motions[rn], false);
		motionManager->updateParam(live2DModel);
	}
	else if (tapCheck("D_REF.HEAD", x, y))
		return;
}

bool SampleLive2D::tapCheck(char *area, float ox, float oy)
{
	int index = live2DModel->getDrawDataIndex(area);
	int pointCount = 0;
	float *points = live2DModel->getTransformedPoints(index, &pointCount);
	float left = live2DModel->getCanvasWidth();
	float right = 0;
	float top = live2DModel->getCanvasHeight();
	float bottom = 0;
	for (int i = 0; i < pointCount; i++)
	{
		float px = points[i * DEF::VERTEX_STEP + DEF::VERTEX_OFFSET];
		float py = points[i * DEF::VERTEX_STEP + DEF::VERTEX_OFFSET + 1];
		if (px < left)left = (px / 2) *1.7;
		if (px > right)right = (px / 2) * 1.57;
		if (py < top)top = (py / 2) *1.5;
		if (py > bottom)bottom = (py / 2) * 1.5;
	}
	float x = ox * 2;
	float y = oy * 2;
	return (left <= x && x <= right && top <= y && y <= bottom);
}

void SampleLive2D::matrixInit(float *m)
{
	for (int i = 0; i < 16; i++) m[i] = ((i % 5) == 0) ? 1.0f : 0.0f;
}

void SampleLive2D::translate(float x, float y)
{
	matrix[12] = x;
	matrix[13] = y;
}

void SampleLive2D::scale(float x, float y)
{
	matrix[0] = x;
	matrix[5] = y;
}

float SampleLive2D::transformX(float src)
{
	return matrix[0] * src + matrix[12];
}

float SampleLive2D::transformY(float src)
{
	return matrix[5] * src + matrix[13];
}

float SampleLive2D::invertTransformX(float src)
{
	return (src - matrix[12]) / matrix[0];
}

float SampleLive2D::invertTransformY(float src)
{
	return (src - matrix[13]) / matrix[5];
}