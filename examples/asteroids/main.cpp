#include "../../drivers/cpp_wrappers/Display.hpp"
#include "AsteroidsGame.hpp"

int main() {
    Display display;
    // Initialize display - replace pins and spi instance with your board setup
    display.init(-1, -1, -1, -1, -1, -1, nullptr, 25000000);

    // Initialize touch wrapper
    Touch touch;
    touch.init();

    AsteroidsGame game(display, &touch);
    game.run();

    return 0;
}
