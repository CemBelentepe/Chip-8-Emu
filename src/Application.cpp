#include "Engine/Game.hpp"
#include "Engine/Level.hpp"

#include "Levels/Level1.hpp"

int main()
{
	Game::getInstance(800, 600, "My Game!");
	Game::attachLevel(new Level1());
	Game::run();
	Game::free();
	return 0;
}