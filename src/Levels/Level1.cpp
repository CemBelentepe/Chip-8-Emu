#include "Level1.hpp"
#include "../Emulator/Chip8.hpp"

Level1::Level1()
{
    addGameObject<Chip8>("games/INVADERS");
}

Level1::~Level1()
{
}
