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