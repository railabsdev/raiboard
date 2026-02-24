#include "../../drivers/cpp_wrappers/Display.hpp"
#include "AsteroidsGame.hpp"

#include "game.cpp"

int main() {
    Game1 game;
    game.init();
    game.start();

    while (true) {
        game.update();
        game.render();
        sleep_ms(16);
    }
    
    return 0;
}
