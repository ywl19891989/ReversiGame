#include "GameScene.h"

USING_NS_CC;

const int GameLayer::FLAG_WHITE = 1;
const int GameLayer::FLAG_BLACK = 0;
const int GameLayer::FLAG_NONE = -1;

CCScene* GameLayer::scene() {

	CCScene *scene = CCScene::create();
	GameLayer *layer = GameLayer::create();
	scene->addChild(layer);

	return scene;
}

GameLayer::GameLayer() :
	_whiteTex(NULL), _blackTex(NULL), _whiteCount(0), _blackCount(0) {

}

GameLayer::~GameLayer() {
	CC_SAFE_RELEASE(_whiteTex);
	CC_SAFE_RELEASE(_blackTex);
}

bool GameLayer::init() {
	if (!CCLayer::init()) {
		return false;
	}

	CCSprite* bg = CCSprite::create("Images/Reversi.png");
	addChild(bg, 1);
	bg->setAnchorPoint(CCPointZero);

	_midArea = CCLayerColor::create(ccc4(255, 0, 0, 0), 600, 600);
	addChild(_midArea, 2);
	_midArea->setAnchorPoint(CCPointZero);
	_midArea->setPosition(ccp(88, 271));

	CCMenu* menu = CCMenu::create();
	_midArea->addChild(menu, 10);
	menu->setAnchorPoint(ccp(0, 0));
	menu->setPosition(ccp(0, 0));

	_gameOver = CCMenuItem::create();
	_gameOver->setTarget(this, menu_selector(GameLayer::startNewGame));
	menu->addChild(_gameOver);
	_gameOver->setAnchorPoint(ccp(0.5, 0.5));
	_gameOver->setPosition(ccp(300, 300));
	_gameOver->setContentSize(getContentSize());
	CCSprite* overImg = CCSprite::create("Images/GameOver.png");
	_gameOver->addChild(overImg);
	CCSize layerSize = getContentSize();
	overImg->setPosition(ccp(layerSize.width / 2, layerSize.height / 2));
	_gameOver->setVisible(false);
	_gameOver->setEnabled(false);

	float gridSize = 75;

	CCTextureCache* textureCache = CCTextureCache::sharedTextureCache();

	_whiteTex = textureCache->addImage("Images/ReversiWhitePiece.png");
	_blackTex = textureCache->addImage("Images/ReversiBlackPiece.png");
	_whiteTex->retain();
	_blackTex->retain();

	for (int i = 0; i < NUM_ROW; i++) {
		for (int j = 0; j < NUM_COL; j++) {
			CCMenuItem* item = CCMenuItem::create();
			item->setContentSize(CCSizeMake(gridSize, gridSize));
			menu->addChild(item);
			item->setTarget(this, menu_selector(GameLayer::onClickGrid));
			item->setTag(i * NUM_COL + j);
			item->setAnchorPoint(ccp(0, 0));
			item->setPosition(ccp(j * gridSize, i * gridSize));

			_gridSprites[i][j] = CCSprite::createWithTexture(_whiteTex);
			_midArea->addChild(_gridSprites[i][j], 2);
			_gridSprites[i][j]->setVisible(false);
			_gridSprites[i][j]->setAnchorPoint(item->getAnchorPoint());
			_gridSprites[i][j]->setPosition(item->getPosition());

			_grids[i][j] = FLAG_NONE;

			if ((i == 3 && j == 3) || (i == 4 && j == 4)) {
				_grids[i][j] = FLAG_BLACK;
				_blackCount++;
				_gridSprites[i][j]->setTexture(_blackTex);
				_gridSprites[i][j]->setVisible(true);
			} else if ((i == 3 && j == 4) || (i == 4 && j == 3)) {
				_grids[i][j] = FLAG_WHITE;
				_whiteCount++;
				_gridSprites[i][j]->setTexture(_whiteTex);
				_gridSprites[i][j]->setVisible(true);
			}
		}
	}

	_curFlag = FLAG_BLACK;
	_curFlagSprite = CCSprite::createWithTexture(_blackTex);
	_midArea->addChild(_curFlagSprite, 1);
	_curFlagSprite->setAnchorPoint(ccp(0, 0));
	_curFlagSprite->setPosition(ccp(7 * 75, 75 * 8 + 25));

	setKeypadEnabled(true);

	scheduleUpdate();

	printf("GameLayer::init whiteCount %d, blackCount %d\n", _whiteCount,
			_blackCount);

	return true;
}

void GameLayer::startNewGame(cocos2d::CCNode* node) {
	CCDirector::sharedDirector()->replaceScene(scene());
}

void GameLayer::update(float dt) {

}

void GameLayer::onClickGrid(CCNode* node) {
	int index = node->getTag();
	int i = index / NUM_COL;
	int j = index % NUM_COL;
	if (canSetFlag(i, j, _curFlag)) {

		setGrid(i, j, _curFlag);

		_curFlag = (_curFlag == FLAG_WHITE ? FLAG_BLACK : FLAG_WHITE);
		_curFlagSprite->setTexture(
				_curFlag == FLAG_WHITE ? _whiteTex : _blackTex);

		if (_whiteCount + _blackCount == 64) {
			_gameOver->setVisible(true);
			_gameOver->setEnabled(true);
		}
	}
}

void GameLayer::setGrid(int row, int col, int flag) {

	_grids[row][col] = flag;
	_gridSprites[row][col]->setVisible(true);

	if (flag == FLAG_WHITE) {
		_whiteCount++;
	} else if (flag == FLAG_BLACK) {
		_blackCount++;
	}

	CCTexture2D* tex = flag == FLAG_WHITE ? _whiteTex : _blackTex;
	_gridSprites[row][col]->setTexture(tex);

	int diffFlag = (flag == FLAG_WHITE ? FLAG_BLACK : FLAG_WHITE);

	int deltas[8][2] = { { 0, 1 }, { 0, -1 }, { 1, 0 }, { -1, 0 }, { 1, 1 }, {
			-1, -1 }, { -1, 1 }, { 1, -1 } };

	int i, j;

	for (int round = 0; round < 8; round++) {

		int firstRow = row + deltas[round][0];
		int firstCol = col + deltas[round][1];

		for (i = firstRow, j = firstCol; i < NUM_COL && j < NUM_COL && i >= 0
				&& j >= 0 && _grids[firstRow][firstCol] == diffFlag; i
				+= deltas[round][0], j += deltas[round][1]) {
			if (_grids[i][j] == FLAG_NONE) {
				break;
			} else if (_grids[i][j] == flag) {
				printf(
						"setGrid find diff and pairs curFlag %d, target %d, index %d, %d\n",
						flag, diffFlag, i, j);
				int deltaI = deltas[round][0];
				int deltaJ = deltas[round][1];
				int startI = firstRow;
				int startJ = firstCol;
				int endI = i;
				int endJ = j;

				int indexI, indexJ;

				printf("---- start set between (%d, %d) - (%d, %d) flag %d\n",
						startI, startJ, endI, endJ, flag);
				for (indexI = startI, indexJ = startJ; indexI != endI || indexJ
						!= endJ; indexI += deltaI, indexJ += deltaJ) {
					printf("setGrid find & set index %d, %d, flag %d\n",
							indexI, indexJ, flag);
					_grids[indexI][indexJ] = flag;
					_gridSprites[indexI][indexJ]->setTexture(tex);
				}
			}
		}
	}

}

int GameLayer::getMax(int & v1, int & v2) {
	return v1 >= v2 ? v1 : v2;
}

int GameLayer::getMin(int & v1, int & v2) {
	return v1 < v2 ? v1 : v2;
}

bool GameLayer::canSetFlag(int row, int col, int flag) {

	if (_grids[row][col] != FLAG_NONE) {
		return false;
	}

	if (_whiteCount == 0 && _blackCount == 0) {
		return true;
	}

	int i, j;

	int diffFlag = (flag == FLAG_WHITE ? FLAG_BLACK : FLAG_WHITE);

	int deltas[8][2] = { { 0, 1 }, { 0, -1 }, { 1, 0 }, { -1, 0 }, { 1, 1 }, {
			-1, -1 }, { -1, 1 }, { 1, -1 } };

	for (int round = 0; round < 8; round++) {

		int firstRow = row + deltas[round][0];
		int firstCol = col + deltas[round][1];

		for (i = firstRow, j = firstCol; i < NUM_COL && j < NUM_COL && i >= 0
				&& j >= 0 && _grids[firstRow][firstCol] == diffFlag; i
				+= deltas[round][0], j += deltas[round][1]) {
			if (_grids[i][j] == FLAG_NONE) {
				break;
			} else if (_grids[i][j] == flag) {
				printf(
						"find diff and pairs curFlag %d, target %d, index %d, %d\n",
						flag, diffFlag, i, j);
				return true;
			}
		}
	}

	printf("cannot find diff and pairs curFlag %d, target %d\n", flag, diffFlag);
	return false;
}

void GameLayer::keyBackClicked() {
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WINRT) || (CC_TARGET_PLATFORM == CC_PLATFORM_WP8)
	CCMessageBox(
			"You pressed the close button. Windows Store Apps do not implement a close button.",
			"Alert");
#else
	CCDirector::sharedDirector()->end();
#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
	exit(0);
#endif
#endif
}
