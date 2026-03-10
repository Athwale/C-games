#include <ncurses.h>
#include "display.h"

#define SIZE 10
#define BACKGROUND '.'
#define PLAYER '0'

ELEMENT *player_location = nullptr;

int main() {
    bool moved = false;

    start();
    const short border_color = add_color(COLOR_RED, -1);
    const short field_color = add_color(COLOR_GREEN,-1);
    const short player_color = add_color(COLOR_YELLOW,-1);
    const short score_color = add_color(COLOR_GREEN,-1);
    const short score_border_color = add_color(COLOR_GREEN,-1);
    set_border_color(border_color);
    set_score_color(score_color);
    set_score_border_color(score_border_color);

    ELEMENT field[SIZE][SIZE];
    init_grid(SIZE, SIZE, field, BACKGROUND, field_color);

    char *items[2];
    items[0] = "Start";
    items[1] = "Quit";

    int choice = draw_menu(2, items, 0);
    if (choice == 1) {
        end();
        return 0;
    }

    // place player.
    player_location = &field[0][0];
    player_location->shape = PLAYER;
    player_location->color_pair = player_color;
    set_player_character(PLAYER);

    // todo add movement to test grid connections.
    // todo use ncurses for processing keypresses?
    // todo move player functions should be in library

    draw_game_screen(SIZE, SIZE, 5, SIZE, field, "None", false);
    player_location = process_player_move(SIZE, SIZE, field);

    end();
    return 0;
}