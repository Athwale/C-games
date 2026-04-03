#include <ncurses.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <wchar.h>
#include <locale.h>

#include "display.h"

// 54 max.
#define SIZE 30
#define WALL L'\u2588'
#define EMPTY ' '
#define PLAYER '0'
#define END 'O'

ELEMENT *player_location = nullptr;
ELEMENT *finish_location = nullptr;
WINDOW *game_area= nullptr;

bool insert_node(const int length, ELEMENT *nodes[length], ELEMENT *node) {
    int i = 0;
    for (; i < length; i++) {
        if (nodes[i] == node) {
            return false;
        }
        if (nodes[i] == nullptr) {
            nodes[i] = node;
            return true;
        }
    }
    return false;
}

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
    // Needed for wchar.
    setlocale(LC_ALL, "");
    int keyboard_input = '\0';
    int steps = 0;

    start();
    const short border_color = add_color(COLOR_RED, -1);
    const short field_color = add_color(COLOR_GREEN,-1);
    const short player_color = add_color(COLOR_YELLOW,-1);
    const short score_color = add_color(COLOR_GREEN,-1);
    const short score_border_color = add_color(COLOR_GREEN,-1);
    const short finish_color = add_color(COLOR_CYAN,-1);
    const short search_color = add_color(COLOR_RED,-1);

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
    int counter = 0;
    int starting_position = 0;
    int stop_counter = (SIZE * SIZE) * 3;

    // List of removed blocks. // todo free the array when done.
    ELEMENT *ptr = calloc(1, sizeof(ELEMENT));
    ptr[counter] = field[start_pos_x][start_pos_y];
    const ELEMENT *current_position = &ptr[starting_position];

    char *items[2];
    items[0] = "Start";
    items[1] = "Quit";

    const int choice = draw_menu(2, items, 0);
    if (choice == 1) {
        end();
        return 0;
    }

    draw_game_screen(SIZE, SIZE, 5, SIZE, field, "Building", 1,
        false);

    while (stop_counter >= 0) {
        short random_direction = 0;
        random_direction = rand() % 4;
        ELEMENT *next = get_neighbor(current_position, random_direction);

        if (next == nullptr) {
            // Edge of the level.
            starting_position++;
            if (starting_position > counter) {
                current_position = &ptr[0];
                starting_position = 0;
            } else {
                // Advance to next position when current path is drilled.
                current_position = &ptr[starting_position];
            }
            continue;
        }
        if (count_empty_neighbors(next) > 1) {
            // Block has blank spaces on other sides.
            starting_position++;
            if (starting_position > counter) {
                current_position = &ptr[0];
                starting_position = 0;
            } else {
                current_position = &ptr[starting_position];
            }
            continue;
        } if (next != nullptr) {
            if (next->shape != EMPTY) {
                next->shape = EMPTY;

                // Save the removed block into list.
                counter++;
                ELEMENT *temp = realloc(ptr, (counter + 1) * sizeof(ELEMENT));
                if (temp == NULL) {
                    // Clean up original memory before exiting
                    free(ptr);
                    fprintf(stderr, "Memory Reallocation Failed");
                    exit(1);
                }
                ptr = temp;
                // Save the dereferenced copy.
                ptr[counter] = *next;

                // Move to the new position.
                current_position = next;
            }
        }
        usleep(1000);
        draw_game_screen(SIZE, SIZE, 5, SIZE, field, "Building", 1,
            false);
        stop_counter--;
    }
    free(ptr);
    ptr = nullptr;

    // Search if the finish is reachable from the start.
    // todo size must be the amount of free spaces but would have to be dynamic.
    // todo check if we have reached the end.
    ELEMENT *nodes[SIZE * SIZE] = {};
    nodes[0] = &field[0][0];
    // This the number of empty spaces.
    const int top = counter;
    counter = 1;
    int discovered = 0;

    for (int j =0; j<top; j++) {
        discovered = 0;
        for (int i = 0; i < counter; i++) {
            if (nodes[i] != nullptr && nodes[i]->shape != '+' && nodes[i]->shape != WALL) {
                nodes[i]->shape = '+';
                nodes[i]->color_pair = search_color;
                // Save all empty neighbors to nodes.
                if (nodes[i]->top != nullptr && nodes[i]->top->shape != '+') {
                    if (insert_node(SIZE * SIZE, nodes, nodes[i]->top)) {
                        discovered++;
                    }
                }
                if (nodes[i]->left != nullptr && nodes[i]->left->shape != '+') {
                    if (insert_node(SIZE * SIZE, nodes, nodes[i]->left)) {
                        discovered++;
                    }
                }
                if (nodes[i]->right != nullptr && nodes[i]->right->shape != '+') {
                    if (insert_node(SIZE * SIZE, nodes, nodes[i]->right)) {
                        discovered++;
                    }
                }
                if (nodes[i]->bottom != nullptr && nodes[i]->bottom->shape != '+') {
                    if (insert_node(SIZE * SIZE, nodes, nodes[i]->bottom)) {
                        discovered++;
                    }
                }
                usleep(1000);
                draw_game_screen(SIZE, SIZE, 5, SIZE, field,
                    "Checking", 1, false);
            }
        }
        counter += discovered;
    }
    // todo place finish and breath search if accessible.
    // todo add score

    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            if (field[i][j].shape == '+') {
                field[i][j].shape = EMPTY;
            }
        }
    }

    // Place player once ready.
    player_location = &field[0][0];
    player_location->shape = PLAYER;
    player_location->color_pair = player_color;
    set_player_character(PLAYER);

    // Place finish.
    finish_location = &field[SIZE-1][SIZE-1];
    finish_location->shape = END;
    finish_location->color_pair = finish_color;

    draw_game_screen(SIZE, SIZE, 5, SIZE, field,
        "Ready.\nWSAD to move, Q to quit.\nSteps: 0", 0, false);
    // todo changes on resize, call after each draw screen?
    game_area = get_play_area_window();
    if (game_area == nullptr) {
        end();
        fprintf(stderr,"Game not initialized.\n");
        exit(EXIT_FAILURE);
    }

    bool run = true;
    bool win = false;
    while (run) {
        keyboard_input = wgetch(game_area);
        switch (keyboard_input) {
            case 'w':
                if (player_location->top != nullptr && player_location->top->shape == WALL) {
                    break;
                }
                player_location = move_player_up(SIZE, SIZE, field);
                break;

            case 's':
                if (player_location->bottom != nullptr && player_location->bottom->shape == WALL) {
                    break;
                }
                player_location = move_player_down(SIZE, SIZE, field);
                break;

            case 'a':
                if (player_location->left != nullptr && player_location->left->shape == WALL) {
                    break;
                }
                player_location = move_player_left(SIZE, SIZE, field);
                break;

            case 'd':
                if (player_location->right != nullptr && player_location->right->shape == WALL) {
                    break;
                }
                player_location = move_player_right(SIZE, SIZE, field);
                break;

            case 'q':
                run = false;
                break;

            default:
                break;
        }
        if (player_location->pos_x == finish_location->pos_x && player_location->pos_y == finish_location->pos_y) {
            run = false;
            win = true;
        }
        if (player_moved()) {
            steps++;
        }
        draw_game_screen(SIZE, SIZE, 5, SIZE, field, "Steps", steps, false);
        // todo edges are not accessible. The screen is 2 chars wider.
        mvwaddch(game_area, 1, 1, 'T');
    }

    if (win) {
        draw_end_screen(1);
    }

    end();
    return 0;
}