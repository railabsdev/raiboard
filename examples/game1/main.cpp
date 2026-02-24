#include <stdio.h>
#include "pico/stdlib.h"
#include "Screen.hpp"
#include "CoinGame.hpp"

int main() {
    stdio_init_all();
    sleep_ms(1000);
    
    Screen screen;
    CoinGame game(screen);
    game.run();
    
    return 0;
}
