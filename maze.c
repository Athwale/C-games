#include <ncurses.h>
#include <stdlib.h>
#include <time.h>

#include "display.h"

#define SIZE 40
#define WALL '#'
#define EMPTY ' ';
#define PLAYER '0'

ELEMENT *player_location = nullptr;
WINDOW *game_area= nullptr;

int main() {
    bool moved = false;
    int keyboard_input = '\0';

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
    init_grid(SIZE, SIZE, field, WALL, field_color);

    // todo generate a maze in the field. Pick random position and use it as a starting point.
    //   Select random direction and see if it can be removed.
    //   Go through all free positions and remove blocks until nothing can be removed.
    // Init rand function.
    srand(time(nullptr));
    int start_pos_x = rand() % SIZE;
    int start_pos_y = rand() % SIZE;

    // Save the removed blocks into a dynamic array.
    field[start_pos_x][start_pos_y].shape = EMPTY;
    int removed_blocks = 1;
    // Array of removed blocks.
    // todo free the array when done.
    ELEMENT *ptr = calloc(1, sizeof(ELEMENT));
    ptr[0] = field[start_pos_x][start_pos_y];

    bool block_removed = true;
    while (block_removed) {
        int random_direction = 0;
        for (int i = 0; i < removed_blocks; i++) {
            random_direction = rand() % 4;
            switch (random_direction) {
                case 0:
                    // up
                    // todo check blok removability.
                    // todo separate to a function?
                    // todo ignore blocks we have already seen
                    if (ptr[i].top != nullptr) {
                        ptr[i].top->shape = EMPTY;
                        block_removed = true;
                        removed_blocks++;
                        ptr = realloc(ptr, removed_blocks * sizeof(ELEMENT));
                        if (ptr == NULL) {
                            printf("Memory Reallocation Failed");
                            exit(1);
                        }
                        // value copy. We save the whole elements, not just their addresses.
                        ptr[i+1] = *ptr[i].top;
                    }
                    break;
                case 1:
                    // down
                    if (ptr[i].bottom != nullptr) {
                        ptr[i].bottom->shape = EMPTY;
                        block_removed = true;
                        removed_blocks++;
                        ptr = realloc(ptr, removed_blocks * sizeof(ELEMENT));
                        if (ptr == NULL) {
                            printf("Memory Reallocation Failed");
                            exit(1);
                        }
                        ptr[i+1] = *ptr[i].bottom;
                    }
                    break;
                case 2:
                    // left
                    if (ptr[i].left != nullptr) {
                        ptr[i].left->shape = EMPTY;
                        block_removed = true;
                        removed_blocks++;
                        ptr = realloc(ptr, removed_blocks * sizeof(ELEMENT));
                        if (ptr == NULL) {
                            printf("Memory Reallocation Failed");
                            exit(1);
                        }
                        ptr[i+1] = *ptr[i].left;
                    }
                    break;
                case 3:
                    // right
                    if (ptr[i].right != nullptr) {
                        ptr[i].right->shape = EMPTY;
                        block_removed = true;
                        removed_blocks++;
                        ptr = realloc(ptr, removed_blocks * sizeof(ELEMENT));
                        if (ptr == NULL) {
                            printf("Memory Reallocation Failed");
                            exit(1);
                        }
                        ptr[i+1] = *ptr[i].right;
                    }
                    break;
                default:
                    break;
            }
        }
        draw_game_screen(SIZE, SIZE, 5, SIZE, field, "None", false);
    }

    char *items[2];
    items[0] = "Start";
    items[1] = "Quit";

    const int choice = draw_menu(2, items, 0);
    if (choice == 1) {
        end();
        return 0;
    }

    // todo place player once ready.
    //player_location = &field[5][5];
    //player_location->shape = PLAYER;
    //player_location->color_pair = player_color;
    //set_player_character(PLAYER);

    draw_game_screen(SIZE, SIZE, 5, SIZE, field, "None", false);
    // todo changes on resize, call after each draw screen?
    game_area = get_play_area_window();
    if (game_area == nullptr) {
        end();
        fprintf(stderr,"Game not initialized.\n");
        exit(EXIT_FAILURE);
    }

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
        moved = player_moved();
        mvprintw(0, 0, "%d ", moved);

        draw_game_screen(SIZE, SIZE, 5, SIZE, field, "None", false);
    }

    end();
    return 0;
}