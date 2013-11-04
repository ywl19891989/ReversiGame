#ifndef __HELLOWORLD_SCENE_H__
#define __HELLOWORLD_SCENE_H__

#include "cocos2d.h"

class GameLayer: public cocos2d::CCLayer {
public:

	CREATE_FUNC( GameLayer);
	virtual bool init();

	GameLayer();
	~GameLayer();

	static cocos2d::CCScene* scene();

	void onClickGrid(cocos2d::CCNode* node);

	virtual void keyBackClicked();

	bool canSetFlag(int row, int col, int flag);

	void update(float dt);

	void startNewGame(cocos2d::CCNode* node);

private:

	void setGrid(int i, int j, int val);

	int getMax(int & v1, int & v2);
	int getMin(int & v1, int & v2);

	static const int NUM_ROW = 8;
	static const int NUM_COL = 8;

	static const int FLAG_WHITE;
	static const int FLAG_BLACK;
	static const int FLAG_NONE;

	cocos2d::CCLayerColor* _midArea;
	cocos2d::CCSprite *_gridSprites[NUM_ROW][NUM_COL], *_curFlagSprite;
	cocos2d::CCTexture2D *_whiteTex, *_blackTex;
	int _grids[NUM_ROW][NUM_COL];
	int _curFlag;
	int _whiteCount, _blackCount;
	cocos2d::CCMenuItem* _gameOver;
};

#endif // __HELLOWORLD_SCENE_H__
