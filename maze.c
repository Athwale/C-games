#include <ncurses.h>
#include <stdlib.h>

#include "display.h"

#define SIZE 10
#define BACKGROUND '.'
#define PLAYER '0'

ELEMENT *player_location = nullptr;
WINDOW *game_area= nullptr;

int main() {
    bool moved = false;
    int keyboard_input = '\0';

    //start();
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

    for (int x = 0; x < SIZE; x++) {
        for (int y = 0; y < SIZE; y++) {
            printf("ID: %lu X: %d Y: %d ", field[x][y].id, field[x][y].pos_x, field[x][y].pos_y);
            if (field[x][y].top != nullptr) {
                printf("T:%lu ", field[x][y].top->id);
            } else {
                printf("T:N ");
            }
            // todo only goes to x=8, y=0, init is broken there somewhere
            if (field[x][y].top_left != nullptr) {
                printf("TL:%lu ", field[x][y].top_left->id);
            } else {
                printf("TL:N ");
            }
            if (field[x][y].left != nullptr) {
                printf("L:%lu ", field[x][y].left->id);
            } else {
                printf("L:N ");
            }
            if (field[x][y].bottom_left != nullptr) {
                printf("BL:%lu ", field[x][y].bottom_left->id);
            } else {
                printf("BL:N ");
            }
            if (field[x][y].bottom != nullptr) {
                printf("B:%lu ", field[x][y].bottom->id);
            } else {
                printf("B:N ");
            }
            if (field[x][y].bottom_right != nullptr) {
                printf("BR:%lu ", field[x][y].bottom_right->id);
            } else {
                printf("BR:N ");
            }
            if (field[x][y].right != nullptr) {
                printf("R:%lu ", field[x][y].right->id);
            } else {
                printf("R:N ");
            }
            if (field[x][y].top_right != nullptr) {
                printf("TR:%lu ", field[x][y].top_right->id);
            } else {
                printf("TR:N ");
            }
        }
        puts("\n");
    }

    //end();
    char *items[2];
    items[0] = "Start";
    items[1] = "Quit";

    int choice = draw_menu(2, items, 0);
    if (choice == 1) {
        end();
        return 0;
    }

    // place player.
    player_location = &field[5][5];
    player_location->shape = PLAYER;
    player_location->color_pair = player_color;
    set_player_character(PLAYER);

    // todo add movement to test grid connections.
    // todo use ncurses for processing keypresses?
    // todo move player functions should be in library

    draw_game_screen(SIZE, SIZE, 5, SIZE, field, "None", false);
    // todo changes on resize, call after each draw screen?
    game_area = get_play_area_window();
    if (game_area == nullptr) {
        end();
        fprintf(stderr,"Game not initialized.\n");
        exit(EXIT_FAILURE);
    }

    // todo seg fault in last two lines.
    bool run = true;
    while (run) {
        keyboard_input = wgetch(game_area);
        switch (keyboard_input) {
            case 'w':
                player_location = move_player_up(SIZE, SIZE, field);
                break;
            case 's':
                player_location = move_player_down(SIZE, SIZE, field);
                break;
            case 'a':
                player_location = move_player_left(SIZE, SIZE, field);
                break;
            case 'd':
                player_location = move_player_right(SIZE, SIZE, field);
                break;
            case 'q':
                run = false;
                break;
            default:
                break;
        }

        draw_game_screen(SIZE, SIZE, 5, SIZE, field, "None", false);
    }

    end();
    return 0;
}