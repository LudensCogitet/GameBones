#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>

#include <SDL2/SDL.h>

#include "../headers/input.h"

int main(int argc, char *argv[]) {
    gb_init_input();

    uint8_t done = 0;

    initMainRenderer("Input Tests");
    while (!done) {
        gb_update_input();

        if (gb_check_input_state(GB_INPUT_BE_AWESOME, GB_INPUT_PRESSED)) {
            printf("You are be's awesome at this moment!");
        } else {
            printf(
                "Thrust state is: %d\nRotate left state is: %d\nRotate right state is: %d\nBreak state is: %d\nSelect state is: %d\n",
                gb_get_input_state(GB_INPUT_THRUST).state,
                gb_get_input_state(GB_INPUT_ROTATE_LEFT).state,
                gb_get_input_state(GB_INPUT_ROTATE_RIGHT).state,
                gb_get_input_state(GB_INPUT_BREAK).state,
                gb_get_input_state(GB_INPUT_SELECT).state
            );
        }

        done = gb_check_input_state(GB_INPUT_QUIT_GAME, GB_INPUT_JUST_PRESSED | GB_INPUT_SHIFT);

        SDL_Delay(0);

    }

    gb_teardown_input();
    return 0;
}
