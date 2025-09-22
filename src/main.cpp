
#include "../include/Game.hpp"

int main()
{
    Game game;

    while (game.getWindowIsOpen())
    {
        game.gameUpdate();
        game.gameRender();
    }
}
