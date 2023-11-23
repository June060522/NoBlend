#pragma once
class Game_Scene;
class Object;
class StageManager
{
	SINGLE(StageManager);
public:
	void Init(const int& curStage, Game_Scene* _curGameScene);
public:
	const char(*GetMap())[BOARD_Y] { return m_board; }
private:
	const void LoadMap(const int& curStage);
	void RenderBoard();
	Object* MakeTile(const int& _xIdx, const int& _yIdx, const TILE_TYPE& _type);
private:
	char m_board[BOARD_Y][BOARD_X];
	Game_Scene* m_curGameScene;
	Vec2 vResolution;
};

