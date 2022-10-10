#include "Game.h"

int main() {
    Game *g = new Game(10, 24);
    g->game_loop();
    delete g;
    return 0;
}
