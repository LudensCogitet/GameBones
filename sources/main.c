#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>

#include <SDL2/SDL.h>

#include "../headers/input.h"

int main(int argc, char *argv[]) {
    gb_init_input();
    gb_init_main_renderer("GFX tests");

    uint8_t done = 0;
    while (!done) {
        gb_update_input();
        SDL_Delay(0);
    }

    gb_teardown_input();
    return 0;
}
