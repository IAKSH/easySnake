#pragma once
#include <functional>
#include <easyx.h>
#include <array>

namespace easysnake
{
	static const int MAP_SIZE_X = 50;
	static const int MAP_SIZE_Y = 50;
	constexpr int WINDOW_SIZE_W = MAP_SIZE_X * 10;
	constexpr int WINDOW_SIZE_H = MAP_SIZE_Y * 10;

	static int score = 0;
	static int respawnTime = 0;
	static int loadCount = 0;

	// window shake effect's value
	static int shakeDistance = 2;
	static RECT rectBuffer;
	static HWND window = NULL, windowOld = NULL;
	static int windowX, windowY, windowWidth, windowHight;
	static int shakeCount = 0;

	// useless , but I'm too lazy to delete this
	enum MapMark {
		Apple = -2,Border,Empty,SnakeHead,SnakeBody,SnakeTail
	};

	class GamePlay {
	public:
		GamePlay(int fps,const char* title);
		~GamePlay();
		bool gameShouldBeClosed = false;
		void draw() noexcept;
		void computing() noexcept;
		void input() noexcept;
		void sync() noexcept;
	private:
		const float delay;
		using Map = std::array<std::array<int, MAP_SIZE_Y>, MAP_SIZE_X>;
		Map map;
		//int map[MAP_SIZE_Y][MAP_SIZE_X];
		int snakeMoveDirection = 2;
		int applePosX, applePosY;
		int backgroundEffectCount = 0;
		int appleExplodeCircleEffectPosX;
		int appleExplodeCircleEffectPosY;
		int appleExplodeCircleEffectRadius;
		bool appleExplodeCircleEffectActived = false;
		void foreachMarkDo(std::function<void(int, int, int&)>) noexcept;
	};
}
