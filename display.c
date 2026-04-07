#define _XOPEN_SOURCE_EXTENDED
#include <ncursesw/ncurses.h>
#include <stdlib.h>
#include <string.h>
#include <locale.h>
#include "display.h"

#define DEFAULT_BORDER_COLOR 1
#define DEFAULT_SCORE_COLOR 2

bool initialized = false;
WINDOW *win; // Main screen, whole terminal
WINDOW *game_window; // Playing field
WINDOW *score_window; // Score field
WINDOW *menu; // Menu window
WINDOW *endscreen; // Menu window

short registered_colors = 0;
short score_color = 0;
short score_border_color = 0;
short border_color = 0;
int lines = 0;
int cols = 0;
wchar_t w_player_character;
wchar_t w_background_character;
cchar_t c_player_character;
bool moved = false;

// todo document all functions.
// todo some stuff can be static.

bool player_moved() {
    return moved;
}

void set_player_character(const wchar_t character) {
    w_player_character = character;
    setcchar(&c_player_character, &w_player_character, 0, 0, nullptr);
}

void init_grid(const int x_length, const int y_length, ELEMENT field[x_length][y_length], const wchar_t background_char, const short color) {
    // X increases downwards, Y increases to the right.
    // Field IDs.
    // i:  0, x: 0, y: 0 i:  1, x: 0, y: 1 i:  2, x: 0, y: 2 i:  3, x: 0, y: 3 i:  4, x: 0, y: 4 i:  5, x: 0, y: 5 i:  6, x: 0, y: 6 i:  7, x: 0, y: 7 i:  8, x: 0, y: 8 i:  9, x: 0, y: 9
    // i: 10, x: 1, y: 0 i: 11, x: 1, y: 1 i: 12, x: 1, y: 2 i: 13, x: 1, y: 3 i: 14, x: 1, y: 4 i: 15, x: 1, y: 5 i: 16, x: 1, y: 6 i: 17, x: 1, y: 7 i: 18, x: 1, y: 8 i: 19, x: 1, y: 9
    // i: 20, x: 2, y: 0 i: 21, x: 2, y: 1 i: 22, x: 2, y: 2 i: 23, x: 2, y: 3 i: 24, x: 2, y: 4 i: 25, x: 2, y: 5 i: 26, x: 2, y: 6 i: 27, x: 2, y: 7 i: 28, x: 2, y: 8 i: 29, x: 2, y: 9
    // i: 30, x: 3, y: 0 i: 31, x: 3, y: 1 i: 32, x: 3, y: 2 i: 33, x: 3, y: 3 i: 34, x: 3, y: 4 i: 35, x: 3, y: 5 i: 36, x: 3, y: 6 i: 37, x: 3, y: 7 i: 38, x: 3, y: 8 i: 39, x: 3, y: 9
    // i: 40, x: 4, y: 0 i: 41, x: 4, y: 1 i: 42, x: 4, y: 2 i: 43, x: 4, y: 3 i: 44, x: 4, y: 4 i: 45, x: 4, y: 5 i: 46, x: 4, y: 6 i: 47, x: 4, y: 7 i: 48, x: 4, y: 8 i: 49, x: 4, y: 9
    // i: 50, x: 5, y: 0 i: 51, x: 5, y: 1 i: 52, x: 5, y: 2 i: 53, x: 5, y: 3 i: 54, x: 5, y: 4 i: 55, x: 5, y: 5 i: 56, x: 5, y: 6 i: 57, x: 5, y: 7 i: 58, x: 5, y: 8 i: 59, x: 5, y: 9
    // i: 60, x: 6, y: 0 i: 61, x: 6, y: 1 i: 62, x: 6, y: 2 i: 63, x: 6, y: 3 i: 64, x: 6, y: 4 i: 65, x: 6, y: 5 i: 66, x: 6, y: 6 i: 67, x: 6, y: 7 i: 68, x: 6, y: 8 i: 69, x: 6, y: 9
    // i: 70, x: 7, y: 0 i: 71, x: 7, y: 1 i: 72, x: 7, y: 2 i: 73, x: 7, y: 3 i: 74, x: 7, y: 4 i: 75, x: 7, y: 5 i: 76, x: 7, y: 6 i: 77, x: 7, y: 7 i: 78, x: 7, y: 8 i: 79, x: 7, y: 9
    // i: 80, x: 8, y: 0 i: 81, x: 8, y: 1 i: 82, x: 8, y: 2 i: 83, x: 8, y: 3 i: 84, x: 8, y: 4 i: 85, x: 8, y: 5 i: 86, x: 8, y: 6 i: 87, x: 8, y: 7 i: 88, x: 8, y: 8 i: 89, x: 8, y: 9
    // i: 90, x: 9, y: 0 i: 91, x: 9, y: 1 i: 92, x: 9, y: 2 i: 93, x: 9, y: 3 i: 94, x: 9, y: 4 i: 95, x: 9, y: 5 i: 96, x: 9, y: 6 i: 97, x: 9, y: 7 i: 98, x: 9, y: 8 i: 99, x: 9, y: 9
    // Going left to right then next row.
    // Modifies the ELEMENT array in place.
    int count = 0;
    w_background_character = background_char;
    for (int x = 0; x < x_length; x++) {
        for (int y = 0; y < y_length; y++) {
            // Fill adjacent block pointers.
            field[x][y].id = count;
            field[x][y].pos_x = x;
            field[x][y].pos_y = y;
            setcchar(&field[x][y].shape, &w_background_character, 0, color, nullptr);
            field[x][y].color_pair = color;

            if (field[x][y].pos_x == 0 && field[x][y].pos_y == 0) {
                // Top left top corner.
                field[x][y].top = nullptr;
                field[x][y].left = nullptr;
                field[x][y].bottom = &field[x+1][y];
                field[x][y].right = &field[x][y+1];
                field[x][y].top_left = nullptr;
                field[x][y].top_right = nullptr;
                field[x][y].bottom_left = nullptr;
                field[x][y].bottom_right = &field[x+1][y+1];
            } else if (field[x][y].pos_x == 0 && field[x][y].pos_y <= y_length - 2 && field[x][y].pos_y > 0) {
                // Top row except corners.
                field[x][y].top = nullptr;
                field[x][y].left = &field[x][y-1];
                field[x][y].bottom = &field[x+1][y];
                field[x][y].right = &field[x][y+1];
                field[x][y].top_left = nullptr;
                field[x][y].top_right = nullptr;
                field[x][y].bottom_left = &field[x+1][y-1];
                field[x][y].bottom_right = &field[x+1][y+1];
            } else if (field[x][y].pos_x == 0 && field[x][y].pos_y == y_length - 1) {
                // Top right corner.
                field[x][y].top = nullptr;
                field[x][y].left = &field[x][y-1];
                field[x][y].bottom = &field[x+1][y];
                field[x][y].right = nullptr;
                field[x][y].top_left = nullptr;
                field[x][y].top_right = nullptr;
                field[x][y].bottom_left = &field[x+1][y-1];
                field[x][y].bottom_right = nullptr;
            } else if (field[x][y].pos_y == 0 && field[x][y].pos_x <= x_length - 2 && field[x][y].pos_x > 0) {
                // Left edge except corners.
                field[x][y].top = &field[x-1][y];
                field[x][y].left = nullptr;
                field[x][y].bottom = &field[x+1][y];
                field[x][y].right = &field[x][y+1];
                field[x][y].top_left = nullptr;
                field[x][y].top_right = &field[x-1][y+1];
                field[x][y].bottom_left = nullptr;
                field[x][y].bottom_right = &field[x+1][y+1];
            } else if (field[x][y].pos_x > 0 && field[x][y].pos_x < x_length - 1 && field[x][y].pos_y > 0 && field[x][y].pos_y < y_length - 1) {
                // Inside except edges.
                field[x][y].top = &field[x-1][y];
                field[x][y].left = &field[x][y-1];
                field[x][y].bottom = &field[x+1][y];
                field[x][y].right = &field[x][y+1];
                field[x][y].top_left = &field[x-1][y-1];
                field[x][y].top_right = &field[x-1][y+1];
                field[x][y].bottom_left = &field[x+1][y-1];
                field[x][y].bottom_right = &field[x+1][y+1];
            } else if (field[x][y].pos_y == y_length - 1 && field[x][y].pos_x <= x_length - 2 && field[x][y].pos_x > 0) {
                // Right edge except corners.
                field[x][y].top = &field[x-1][y];
                field[x][y].left = &field[x][y-1];
                field[x][y].bottom = &field[x+1][y];
                field[x][y].right = nullptr;
                field[x][y].top_left = &field[x-1][y-1];
                field[x][y].top_right = nullptr;
                field[x][y].bottom_left = &field[x+1][y-1];
                field[x][y].bottom_right = nullptr;
            } else if (field[x][y].pos_x == x_length - 1 && field[x][y].pos_y == 0 ) {
                // Bottom left corner.
                field[x][y].top = &field[x-1][y];
                field[x][y].left = nullptr;
                field[x][y].bottom = nullptr;
                field[x][y].right = &field[x][y+1];
                field[x][y].top_left = nullptr;
                field[x][y].top_right = &field[x-1][y+1];
                field[x][y].bottom_left = nullptr;
                field[x][y].bottom_right = nullptr;
            } else if (field[x][y].pos_x == x_length - 1 && field[x][y].pos_y <= y_length - 2 && field[x][y].pos_y > 0 ) {
                // Bottom row except corners.
                field[x][y].top = &field[x-1][y];
                field[x][y].left = &field[x][y-1];
                field[x][y].bottom = nullptr;
                field[x][y].right = &field[x][y+1];
                field[x][y].top_left = &field[x][y-1];
                field[x][y].top_right = &field[x-1][y+1];
                field[x][y].bottom_left = nullptr;
                field[x][y].bottom_right = nullptr;
            } else if (field[x][y].pos_x == x_length - 1 && field[x][y].pos_y == y_length - 1 ) {
                // Bottom right corner.
                field[x][y].top = &field[x-1][y];
                field[x][y].left = &field[x][y-1];
                field[x][y].bottom = nullptr;
                field[x][y].right = nullptr;
                field[x][y].top_left = &field[x][y-1];
                field[x][y].top_right = nullptr;
                field[x][y].bottom_left = nullptr;
                field[x][y].bottom_right = nullptr;
            }
            count++;
        }
    }
}

void debug_print_grid(const int x_length, const int y_length, ELEMENT field[x_length][y_length]) {
    for (int x = 0; x < x_length; x++) {
        for (int y = 0; y < y_length; y++) {
            printf("ID: %lu X: %d Y: %d ", field[x][y].id, field[x][y].pos_x, field[x][y].pos_y);
            if (field[x][y].top != nullptr) {
                printf("T:%lu ", field[x][y].top->id);
            } else {
                printf("T:N ");
            }
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
}


static void set_current_color(WINDOW *window, const short color) {
    if (has_colors()) {
        wcolor_set(window, color, NULL);
    }
}

static void terminal_too_small() {
    end();
    fprintf(stderr,"Terminal window is too small.\n");
    exit(EXIT_FAILURE);
}

WINDOW *get_play_area_window()
{
    // todo changes on terminal resize!
    return game_window;
}

static ELEMENT* find_player(const int x_length, const int y_length, ELEMENT field[x_length][y_length]) {
    wchar_t wbuf[CCHARW_MAX] = {};
    attr_t attrs;
    short color_pair;

    getcchar(&c_player_character, wbuf, &attrs, &color_pair, nullptr);
    const wchar_t player_ch = wbuf[0];
    if (player_ch == '\0') {
        end();
        fprintf(stderr,"Player character is not set.\n");
        exit(EXIT_FAILURE);
    }

    // Find the block where the player is.
    ELEMENT *player = nullptr;
    for (int x = 0; x < x_length; x++) {
        for (int y = 0; y < y_length; y++) {
            getcchar(&field[x][y].shape, wbuf, &attrs, &color_pair, nullptr);
            const wchar_t shape_ch = wbuf[0];
            if (shape_ch == player_ch) {
                player = &field[x][y];
            }
        }
    }
    if (player == nullptr) {
        return nullptr;
    }
    return player;
}

// Changes the field in place ready to be redrawn in next step and returns the new player position.
// If no movement is made, original player position is returned.
ELEMENT* move_player_up(const int x_length, const int y_length, ELEMENT field[x_length][y_length]) {
    ELEMENT *player = find_player(x_length, y_length, field);
    moved = false;
    if (player->top != nullptr) {
        // Switch positions.
        const cchar_t shape = player->shape;
        const short color_pair = player->color_pair;

        player->shape = player->top->shape;
        player->color_pair = player->top->color_pair;

        player->top->shape = shape;
        player->top->color_pair = color_pair;
        moved = true;
        return player->top;
    }
    return player;
}

ELEMENT* move_player_down(const int x_length, const int y_length, ELEMENT field[x_length][y_length]) {
    ELEMENT *player = find_player(x_length, y_length, field);
    moved = false;
    if (player->bottom != nullptr) {
        // Switch positions.
        const cchar_t shape = player->shape;
        const short color_pair = player->color_pair;

        player->shape = player->bottom->shape;
        player->color_pair = player->bottom->color_pair;

        player->bottom->shape = shape;
        player->bottom->color_pair = color_pair;
        moved = true;
        return player->bottom;
    }
    return player;
}

ELEMENT* move_player_left(const int x_length, const int y_length, ELEMENT field[x_length][y_length]) {
    ELEMENT *player = find_player(x_length, y_length, field);
    moved = false;
    if (player->left != nullptr) {
        // Switch positions.
        const cchar_t shape = player->shape;
        const short color_pair = player->color_pair;

        player->shape = player->left->shape;
        player->color_pair = player->left->color_pair;

        player->left->shape = shape;
        player->left->color_pair = color_pair;
        moved = true;
        return player->left;
    }
    return player;
}

ELEMENT* move_player_right(const int x_length, const int y_length, ELEMENT field[x_length][y_length]) {
    ELEMENT *player = find_player(x_length, y_length, field);
    moved = false;
    if (player->right != nullptr) {
        // Switch positions.
        const cchar_t shape = player->shape;
        const short color_pair = player->color_pair;

        player->shape = player->right->shape;
        player->color_pair = player->right->color_pair;

        player->right->shape = shape;
        player->right->color_pair = color_pair;
        moved = true;
        return player->right;
    }
    return player;
}

// x_length - vertical
// y_length - horizontal
void draw_game_screen(const int game_x_length, const int game_y_length, const int score_x_length,
    const int score_y_length, const ELEMENT area[game_x_length][game_y_length], char score[], const int value, bool id) {
    if (!initialized) {
        fprintf(stderr,"start() must be called first\n");
        exit(EXIT_FAILURE);
    }

    // COLS/LINES are set now.
    // X vertical, Y horizontal.
    const int pos_x = (LINES / 2) - (game_x_length / 2);
    const int pos_y = (COLS / 2) - (game_y_length / 2);

    if (LINES < game_x_length + score_x_length + 8) {
        terminal_too_small();
    }

    if (COLS < game_y_length + score_y_length + 4) {
        terminal_too_small();
    }

    if (cols != COLS || lines != LINES) {
        cols = COLS;
        lines = LINES;
        wclear(game_window);
        wclear(score_window);
        delwin(game_window);
        delwin(score_window);
        game_window = nullptr;
    }

    // Win is the parent. Height, width, x, y +2 for border.
    if (game_window == nullptr) {
        // X are lines
        // Y are columns
        game_window = subwin(win, game_x_length + 2, game_y_length + 2, pos_x, pos_y);
        score_window = subwin(win, score_x_length + 2, score_y_length + 2, pos_x + game_x_length + 2, pos_y);
        // Add border with color.
        set_current_color(game_window, border_color);
        box(game_window, 0, 0);
        set_current_color(game_window, DEFAULT_BORDER_COLOR);

        set_current_color(score_window, score_border_color);
        box(score_window, 0, 0);
        set_current_color(score_window, DEFAULT_BORDER_COLOR);
    }

    // These coordinates are relative to the new window.
    for (int i = 0; i < game_x_length; i++) {
        for (int j = 0; j < game_y_length; j++) {
            set_current_color(game_window, area[i][j].color_pair);
            if (id) {
                mvwprintw(game_window, i+1, j+2, "%lu ", area[i][j].id);
            } else {
                mvwadd_wch(game_window, i+1, j+1, &area[i][j].shape);
            }
        }
    }

    // Blank the score box without wclear which flickers the screen.
    char blank[score_y_length] = {};
    for (int i = 0; i < score_y_length; i++) {
        blank[i] = ' ';
    }
    blank[score_y_length-1] = '\0';
    int line = 1;
    for (int i = 0; i < score_x_length; i++) {
        mvwprintw(score_window, line, 2, "%s", blank);
    }

    char *part;
    // strsep is destructive, it needs a read write memory not a static string.
    char *str = strdup(score);
    line = 1;
    set_current_color(score_window, score_color);
    while ((part = strsep(&str,"\n")) != NULL) {
        mvwprintw(score_window, line, 2, "%s", part);
        line++;
    }
    set_current_color(score_window, DEFAULT_SCORE_COLOR);

    touchwin(win);
    wrefresh(win);
    wrefresh(game_window);
    wrefresh(score_window);
    refresh();
}

int draw_menu(const int count, char **items, int selected) {
    if (!initialized) {
        fprintf(stderr,"start() must be called first\n");
        exit(EXIT_FAILURE);
    }

    unsigned long max_len = 0;
    for (int i = 0; i < count; i++) {
        if (max_len < strlen(items[i])) {
            max_len = strlen(items[i]);
        }
    }

    const int pos_x = (LINES / 2) - (count / 2);
    const int pos_y = (COLS / 2) - ((int)max_len / 2);
    // Win is the parent. Height, width, x, y
    menu = subwin(win, count + 2, (int)max_len + 2, pos_x, pos_y);
    keypad(menu, TRUE);

    // Add border with color.
    set_current_color(menu, border_color);
    box(menu, 0, 0);
    set_current_color(menu, DEFAULT_BORDER_COLOR);

    int ch = '\0';
    bool confirmed = false;
    do {
        if (ch == KEY_UP) {
            selected--;
            if (selected <= 0) {
                selected = 0;
            }
        } else if (ch == KEY_DOWN) {
            selected++;
            if (selected >= count - 1) {
                selected = count - 1;
            }
        } else if (ch == KEY_ENTER || ch == 10) {
            confirmed = true;
        }

        // These coordinates are relative to the new window.
        for (int i = 0; i < count; i++) {
            if (i == selected) {
                wattron(menu, A_STANDOUT);
            }
            mvwaddstr(menu, i+1, 1, items[i]);
            wattroff(menu, A_STANDOUT);
        }
        wrefresh(menu);

        if (confirmed) {
            wclear(menu);
            delwin(menu);
            return selected;
        }

    } while((ch = wgetch(menu)) != 'q');
    wclear(menu);
    delwin(menu);
    return -1;
}

void draw_end_screen(const int score) {
    if (!initialized) {
        fprintf(stderr,"start() must be called first\n");
        exit(EXIT_FAILURE);
    }

    wclear(game_window);
    wclear(score_window);

    char message[50] = {};
    snprintf(message, 50, "Final score: %d", score);
    // Vertical position:
    const int pos_x = LINES / 2 - 2;
    // Horizontal position:
    const int pos_y = COLS / 2 - (int)strlen(message) / 2;
    // Win is the parent. Height, width, x, y
    endscreen = subwin(win, 4, (int)strlen(message) + 2, pos_x, pos_y);
    box(endscreen, 0, 0);
    keypad(endscreen, TRUE);

    mvwaddstr(endscreen, 1, 1, message);
    mvwaddstr(endscreen, 2, 1, "Press any key");

    wrefresh(game_window);
    wrefresh(score_window);
    wrefresh(endscreen);
    refresh();
    wgetch(endscreen);

    wclear(endscreen);
    delwin(endscreen);
}

short add_color(const short foreground, const short background) {
    // todo test 257
    if (has_colors()) {
        registered_colors++;
        if (registered_colors <= COLOR_PAIRS) {
            init_pair(registered_colors, foreground, background);
            return registered_colors;
        }
    }
    return -1;
}

void set_border_color(const short color_pair_number) {
    if (has_colors()) {
        if (registered_colors == 0) {
            fprintf(stderr, "No colors have been added.\n");
            exit(EXIT_FAILURE);
        }
        border_color = color_pair_number;
    }
}

void set_score_color(const short color_pair_number) {
    if (has_colors()) {
        if (registered_colors == 0) {
            fprintf(stderr, "No colors have been added.\n");
            exit(EXIT_FAILURE);
        }
        score_color = color_pair_number;
    }
}

void set_score_border_color(const short color_pair_number) {
    if (has_colors()) {
        if (registered_colors == 0) {
            fprintf(stderr, "No colors have been added.\n");
            exit(EXIT_FAILURE);
        }
        score_border_color = color_pair_number;
    }
}

void start() {
    setlocale(LC_ALL, "");
    set_player_character('0');
    setcchar(&c_player_character, &w_player_character, 0, 0, nullptr);
    // Init ncurses.
    if ((win = initscr()) == NULL ) {
        fprintf(stderr, "Error initialising ncurses.\n");
        exit(EXIT_FAILURE);
    }
    noecho();
    // Enable arrow keys.
    curs_set(0);
    start_color();
    if (has_colors()) {
        // Set some defaults.
        use_default_colors();
        init_pair(DEFAULT_BORDER_COLOR, COLOR_WHITE, -1);
        registered_colors++;
        init_pair(DEFAULT_SCORE_COLOR, COLOR_WHITE, -1);
        registered_colors++;
    }
    cols = COLS;
    lines = LINES;
    initialized = true;
}

void end() {
    wclear(game_window);
    wclear(score_window);
    wclear(win);

    delwin(game_window);
    delwin(score_window);
    delwin(win);
    endwin();
    refresh();
}