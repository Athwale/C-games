#define _XOPEN_SOURCE_EXTENDED
#include <ncursesw/ncurses.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <locale.h>

#include "display.h"

// 54 max.
#define SIZE 54
#define SCORE_BOX_HEIGHT 3

ELEMENT *player_location = nullptr;
ELEMENT *finish_location = nullptr;
WINDOW *game_area= nullptr;

wchar_t wall = L'\u2588';
wchar_t empty = ' ';
wchar_t player = L'\u263A';
wchar_t finish = L'\u0298';

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

bool compare_cchar(const cchar_t c1, const wchar_t c2) {
    wchar_t wbuf[CCHARW_MAX] = {};
    attr_t attrs;
    short color_pair;

    if (getcchar(&c1, wbuf, &attrs, &color_pair, nullptr) == ERR) {
        return false;
    }

    // Compare the first wide character in the buffer to c2
    return wbuf[0] == c2;
}

int count_empty_neighbors(const ELEMENT *block) {
    int n_of_empty = 0;
    if (block->top != nullptr) {
        if (compare_cchar(block->top->shape, empty)) {
            n_of_empty++;
        }
    }
    if (block->left != nullptr) {
        if (compare_cchar(block->left->shape, empty)) {
            n_of_empty++;
        }
    }
    if (block->right != nullptr) {
        if (compare_cchar(block->right->shape, empty)) {
            n_of_empty++;
        }
    }
    if (block->bottom != nullptr) {
        if (compare_cchar(block->bottom->shape, empty)) {
            n_of_empty++;
        }
    }
    return n_of_empty;
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
    init_grid(SIZE, SIZE, field, wall, field_color);

    // Init rand function.
    srand(time(nullptr));
    int start_pos_x = rand() % SIZE;
    int start_pos_y = rand() % SIZE;

    // Set starting position.
    setcchar(&field[start_pos_x][start_pos_y].shape, &wall, 0, 0, nullptr);

    int counter = 0;
    int starting_position = 0;
    int stop_counter = (SIZE * SIZE) * 3;

    // List of removed blocks.
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

    draw_game_screen(SIZE, SIZE, SCORE_BOX_HEIGHT, SIZE, field, "Building", 1,
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
            if (compare_cchar(next->shape, empty) == false) {
                setcchar(&next->shape, &empty, 0, 0, nullptr);

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
        usleep(300);
        draw_game_screen(SIZE, SIZE, SCORE_BOX_HEIGHT, SIZE, field, "Building", 1,
            false);
        stop_counter--;
    }
    free(ptr);
    ptr = nullptr;

    // todo handle no path found.

    // Search if the finish is reachable from the start.
    // todo size must be the amount of free spaces but would have to be dynamic.
    ELEMENT *nodes[SIZE * SIZE] = {};
    nodes[0] = &field[0][0];
    // This the number of empty spaces.
    const int top = counter;
    counter = 1;
    int discovered = 0;
    constexpr wchar_t marker = '+';

    for (int j =0; j<top; j++) {
        discovered = 0;
        for (int i = 0; i < counter; i++) {
            if (nodes[i] != nullptr && compare_cchar(nodes[i]->shape, marker) == false && compare_cchar(nodes[i]->shape, wall) == false) {
                setcchar(&nodes[i]->shape, &marker, 0, 0, nullptr);
                nodes[i]->color_pair = search_color;
                // Save all empty neighbors to nodes.
                if (nodes[i]->top != nullptr && compare_cchar(nodes[i]->top->shape, marker) == false) {
                    if (insert_node(SIZE * SIZE, nodes, nodes[i]->top)) {
                        discovered++;
                    }
                }
                if (nodes[i]->left != nullptr && compare_cchar(nodes[i]->left->shape, marker) == false) {
                    if (insert_node(SIZE * SIZE, nodes, nodes[i]->left)) {
                        discovered++;
                    }
                }
                if (nodes[i]->right != nullptr && compare_cchar(nodes[i]->right->shape, marker) == false) {
                    if (insert_node(SIZE * SIZE, nodes, nodes[i]->right)) {
                        discovered++;
                    }
                }
                if (nodes[i]->bottom != nullptr && compare_cchar(nodes[i]->bottom->shape, marker) == false) {
                    if (insert_node(SIZE * SIZE, nodes, nodes[i]->bottom)) {
                        discovered++;
                    }
                }
                usleep(1000);
                draw_game_screen(SIZE, SIZE, SCORE_BOX_HEIGHT, SIZE, field,
                    "Checking", 1, false);
            }
        }
        counter += discovered;
    }

    // Clear the markers.
    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            if (compare_cchar(field[i][j].shape, marker)) {
                setcchar(&field[i][j].shape, &empty, 0, 0, nullptr);
            }
        }
    }

    // Place player once ready.
    player_location = &field[0][0];
    setcchar(&player_location->shape, &player, 0, 0, nullptr);
    player_location->color_pair = player_color;
    set_player_character(player);

    // Place finish.
    finish_location = &field[SIZE-1][SIZE-1];
    setcchar(&finish_location->shape, &finish, 0, 0, nullptr);
    finish_location->color_pair = finish_color;

    draw_game_screen(SIZE, SIZE, SCORE_BOX_HEIGHT, SIZE, field,
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
                if (player_location->top != nullptr && compare_cchar(player_location->top->shape, wall)) {
                    break;
                }
                player_location = move_player_up(SIZE, SIZE, field);
                break;

            case 's':
                if (player_location->bottom != nullptr && compare_cchar(player_location->bottom->shape, wall)) {
                    break;
                }
                player_location = move_player_down(SIZE, SIZE, field);
                break;

            case 'a':
                if (player_location->left != nullptr && compare_cchar(player_location->left->shape, wall)) {
                    break;
                }
                player_location = move_player_left(SIZE, SIZE, field);
                break;

            case 'd':
                if (player_location->right != nullptr && compare_cchar(player_location->right->shape, wall)) {
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
        // todo step counter is not working.
        draw_game_screen(SIZE, SIZE, 5, SIZE, field, "Steps", steps, false);
    }

    if (win) {
        draw_end_screen(1);
    }

    end();
    return 0;
}