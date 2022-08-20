// 使用easyX与Windows API实现一个酷酷的贪吃蛇
// 本来要求好像用C，可easyX难道不是一个C++库吗？
// 我索性直接用C++好了
#include <iostream>
#include "GamePlay.hpp"

static bool gameShouldBeClosed{ false };

int main() noexcept
{
	while(true)
	{
		easysnake::GamePlay gp(15, "Easy Snake");
		while (!gp.gameShouldBeClosed)
		{
			gp.sync();
			gp.draw();
			gp.input();
			gp.computing();
		}
	};
}