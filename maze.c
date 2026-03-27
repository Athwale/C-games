#include <ncurses.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#include "display.h"

#define SIZE 10
#define WALL '#'
#define EMPTY ' '
#define PLAYER '0'

ELEMENT *player_location = nullptr;
WINDOW *game_area= nullptr;

int count_empty_neighbors(const ELEMENT *block) {
    int empty = 0;
    if (block->top != nullptr) {
        if (block->top->shape == EMPTY) {
            empty++;
        }
    }
    if (block->left != nullptr) {
        if (block->left->shape == EMPTY) {
            empty++;
        }
    }
    if (block->right != nullptr) {
        if (block->right->shape == EMPTY) {
            empty++;
        }
    }
    if (block->bottom != nullptr) {
        if (block->bottom->shape == EMPTY) {
            empty++;
        }
    }
    return empty;
}

ELEMENT* get_neighbor(const ELEMENT *block, const short direction) {
    switch (direction) {
        case 0:
            if (block->top != nullptr) {
                return block->top;
            }
        case 1:
            if (block->left != nullptr) {
                return block->left;
            }
            break;
        case 2:
            if (block->right != nullptr) {
                return block->right;
            }
            break;
        case 3:
            if (block->bottom != nullptr) {
                return block->bottom;
            }
            break;
        default:
            return nullptr;
    }
    return nullptr;
}

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

    // Init rand function.
    srand(time(nullptr));
    int start_pos_x = rand() % SIZE;
    int start_pos_y = rand() % SIZE;

    // Set starting position.
    field[start_pos_x][start_pos_y].shape = EMPTY;
    // List of removed blocks. // todo free the array when done.
    int counter = 0;
    ELEMENT *ptr = calloc(1, sizeof(ELEMENT));
    ptr[counter] = field[start_pos_x][start_pos_y];
    ELEMENT *current_position = &field[start_pos_x][start_pos_y];

    draw_game_screen(SIZE, SIZE, 5, SIZE, field, "None", false);

    while (true) {
        short random_direction = 0;
        random_direction = rand() % 4;
        ELEMENT *next = get_neighbor(current_position, random_direction);

        if (next == nullptr) {
            continue;
        }
        if (count_empty_neighbors(next) > 1) {
            // Skip removing this block.
            continue;
        } if (next != nullptr) {
            if (next->shape != EMPTY) {
                next->shape = EMPTY;

                // Save the removed block into list.
                counter++;
                ptr = realloc(ptr, (counter+1)  * sizeof(ELEMENT));
                if (ptr == NULL) {
                    printf("Memory Reallocation Failed");
                    exit(1);
                }
                // Save the dereferenced copy.
                ptr[counter] = *next;

                // Move to the new position.
                current_position = next;
            }
        } else {
            // advance to next position.
            puts("next position");
        }
        usleep(20000);
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