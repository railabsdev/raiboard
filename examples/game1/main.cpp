#include "game.cpp"

int main() {
    Game game;
    game.init();
    game.start();

    while (true) {
        game.update();
        game.render();
        sleep_ms(16);
    }
    
    return 0;
}