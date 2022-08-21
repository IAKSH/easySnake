#include "GamePlay.hpp"
#include <Windows.h>
#include <mmsystem.h>
#include <conio.h>
#include <sstream>

using namespace easysnake;

GamePlay::GamePlay(int fps, const char* title) : delay(1.0f / fps)
{
	if (!loadCount)
	{
		initgraph(WINDOW_SIZE_W, WINDOW_SIZE_H + 20);
		loadCount++;
	}

	SetWindowText(GetHWnd(), L"Easy Snake");
	foreachMarkDo([](int i, int j, int& mark) {
		if (i == 0)
		{
			if (j == 0) mark = SnakeTail;
			else if (j == 1) mark = SnakeHead;
		}
		});

	int i, j;
	for (i = 0; i < MAP_SIZE_Y; i++)
	{
		map[i][0] = -1;
		map[i][MAP_SIZE_X - 1] = -1;
	}
	for (j = 0; j < MAP_SIZE_X; j++)
	{
		map[0][j] = -1;
		map[MAP_SIZE_Y - 1][j] = -1;
	}

	map[MAP_SIZE_Y / 2][MAP_SIZE_X / 2] = SnakeHead;

	for (i = 1; i <= 4; i++)
		map[MAP_SIZE_Y / 2][MAP_SIZE_X / 2 - i] = i + 1;

	snakeMoveDirection = 2;

	applePosX = rand() % (MAP_SIZE_Y - 5) + 2;
	applePosY = rand() % (MAP_SIZE_X - 5) + 2;
	map[applePosX][applePosY] = Apple;
}

GamePlay::~GamePlay()
{
	//closegraph();
}

void easysnake::GamePlay::foreachMarkDo(std::function<void(int, int, int&)> func) noexcept
{
	std::array<int, 50>* line;
	for (int i = 0; i < map.size(); i++)
	{
		line = &map.at(i);
		for (int j = 0; j < line->size(); j++)
		{
			func(i, j, line->at(j));
		}
	}
}

// C-Style array version
/*
void easysnake::GamePlay::foreachMarkDo(std::function<void(int, int, int&)> func) noexcept
{
	static int* lineHead;
	for (int i = 0; i < MAP_SIZE_Y; i++)
	{
		lineHead = &map[i][0];
		for (int j = 0; j < MAP_SIZE_X; j++)
		{
			func(i, j,lineHead[j]);
		}
	}
}
*/

void easysnake::GamePlay::input() noexcept
{
	if (_kbhit()) {
		int key = _getch();
		switch (key) {
		case 'w':snakeMoveDirection = 1; break;
		case 'd':snakeMoveDirection = 2; break;
		case 's':snakeMoveDirection = 3; break;
		case 'a':snakeMoveDirection = 4; break;
		default:break;
		}
	}
}

void easysnake::GamePlay::draw() noexcept
{
	BeginBatchDraw();
	cleardevice();

	// Draw States
	static std::wstringstream wss;
	wss.clear();
	wss.str(L"");
	wss << L"Score:" << std::to_wstring(score) << L"    respawn count:" << std::to_wstring(respawnTime);
	settextcolor(WHITE);
	outtextxy(0, MAP_SIZE_Y * 10 + 2, wss.str().c_str());

	// Draw Objects
	foreachMarkDo([](int i, int j, int& mark) {
		if (mark == -1) // wall
		{
			setfillcolor(WHITE);
			solidrectangle(10 * i, 10 * j, 10 * (i + 1), 10 * (j + 1));
		}
		else if (mark == 1) // snake's head
		{
			setfillcolor(BROWN);
			solidrectangle(10 * i, 10 * j, 10 * (i + 1), 10 * (j + 1));
		}
		else if (mark > 1) // snake's body and tail
		{
			setfillcolor(GREEN);
			solidrectangle(10 * i, 10 * j, 10 * (i + 1), 10 * (j + 1));
		}
		else if (mark == -2) // apple
		{
			setfillcolor(RED);
			solidrectangle(10 * i, 10 * j, 10 * (i + 1), 10 * (j + 1));
		}
	});

	// draw apple explode circle effect
	if (appleExplodeCircleEffectActived)
	{
		circle(appleExplodeCircleEffectPosX * 10 + 2, appleExplodeCircleEffectPosY * 10 + 2, appleExplodeCircleEffectRadius);
	}
	EndBatchDraw();
}

void easysnake::GamePlay::computing() noexcept
{
	static int oldTailPosY, oldTailPosX, oldHeadPosY, oldHeadPosX;
	static int newHeadPosY, newHeadPosX;
	static int i, j;
	int max = 0;

	// apple explode circle effect
	if (appleExplodeCircleEffectActived)
	{
		if (appleExplodeCircleEffectRadius < 50) appleExplodeCircleEffectRadius += 10;
		else appleExplodeCircleEffectActived = false;
	}

	// realize window shake effect
	window = GetForegroundWindow();
	if (true)
	{
		GetWindowRect(window, &rectBuffer);
		windowX = rectBuffer.left;
		windowY = rectBuffer.top;
		windowWidth = rectBuffer.right - windowX;
		windowHight = rectBuffer.bottom - windowY;
		//windowOld = window;
	}
	switch (shakeCount)
	{
	case 4:MoveWindow(window, windowX - shakeDistance, windowY, windowWidth, windowHight, TRUE); break;
	case 3:MoveWindow(window, windowX - shakeDistance, windowY - shakeDistance, windowWidth, windowHight, TRUE); break;
	case 2:MoveWindow(window, windowX, windowY - shakeDistance, windowWidth, windowHight, TRUE); break;
	case 1:MoveWindow(window, windowX, windowY, windowWidth, windowHight, TRUE); break;
	default:break;
	}
	if (shakeCount > 0) shakeCount--;

	// all marks ++
	// Apple = -2, Border, Empty, SnakeHead, SnakeBody, SnakeTail
	for (i = 1; i < MAP_SIZE_Y - 1; i++)
	{
		for (j = 1; j < MAP_SIZE_X - 1; j++)
		{
			if (map[i][j] > 0) map[i][j]++;
		}
	}

	for (i = 1; i < MAP_SIZE_Y - 1; i++)
	{
		for (j = 1; j < MAP_SIZE_X - 1; j++)
		{
			if (map[i][j] > 0)
			{
				if (max < map[i][j])
				{
					max = map[i][j];
					oldTailPosY = i;
					oldTailPosX = j;
				}
				if (map[i][j] == 2)
				{
					oldHeadPosY = i;
					oldHeadPosX = j;
				}
			}
		}
	}

	// move
	if (snakeMoveDirection == 1) // up
	{
		newHeadPosY = oldHeadPosY;
		newHeadPosX = oldHeadPosX - 1;
	}
	else if (snakeMoveDirection == 2) // right
	{
		newHeadPosY = oldHeadPosY + 1;
		newHeadPosX = oldHeadPosX;
	}
	else if (snakeMoveDirection == 3) // down
	{
		newHeadPosY = oldHeadPosY;
		newHeadPosX = oldHeadPosX + 1;
	}
	else if (snakeMoveDirection == 4) // left
	{
		newHeadPosY = oldHeadPosY - 1;
		newHeadPosX = oldHeadPosX;
	}

	// snake eat
	if (map[newHeadPosY][newHeadPosX] == Apple)
	{
		score++;

		// sound
		PlaySound(TEXT("eat_01.wav"), nullptr, SND_FILENAME | SND_ASYNC);

		// enable window shake effect
		shakeCount = 4;

		// enable apple explode circle effect
		appleExplodeCircleEffectPosX = applePosX;
		appleExplodeCircleEffectPosY = applePosY;
		appleExplodeCircleEffectRadius = 1;
		appleExplodeCircleEffectActived = true;

		map[applePosX][applePosY] = Empty;
		applePosX = rand() % (MAP_SIZE_Y - 5) + 2;
		applePosY = rand() % (MAP_SIZE_X - 5) + 2;
		map[applePosX][applePosY] = -2;
	}
	else map[oldTailPosY][oldTailPosX] = Empty;

	// snake die
	if (map[newHeadPosY][newHeadPosX] > 0 || map[newHeadPosY][newHeadPosX] == -1)
	{
		settextcolor(RED);
		outtextxy(200, MAP_SIZE_Y * 10 + 2,L"Game Over,respawn in 3 seconds...");
		for (int i = 0; i < 3; i++)
		{
			PlaySound(TEXT("score.wav"), nullptr, SND_FILENAME | SND_ASYNC);
			Sleep(1000); // may not be 1sec
		}
		PlaySound(NULL, NULL, 0);// it looks like this can prevent memory leak (by a little)
		respawnTime++;

		gameShouldBeClosed = true;
		//exit(0);
	}
	else map[newHeadPosY][newHeadPosX] = SnakeHead;
}

void easysnake::GamePlay::sync() noexcept
{
	Sleep(static_cast<DWORD>(delay * 1000));
}