#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>

#include <SDL2/SDL.h>

#include "../headers/input.h"

int main(int argc, char *argv[]) {
    gb_init_input();

    uint8_t done = 0;

    static uint8_t old_state = 0;

    initMainRenderer("Input Tests");
    while (!done) {
        gb_update_input();

        GbInputState thrust = gb_get_input_state(GB_THRUST);

        if (thrust.state != old_state) {
            printf("Thrust state is: %d\n", thrust.state);
            old_state = thrust.state;
        }
        done = gb_get_input_state(GB_QUIT_GAME).state & GB_INPUT_PRESSED;
    }

    gb_teardown_input();
    return 0;
}
