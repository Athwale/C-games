#pragma once

// Structure for colored play area elements:
// int pos_x
// int pos_y
// char shape
// short color_pair
typedef struct element {
    unsigned long int id;
    unsigned int pos_x;
    unsigned int pos_y;
    char shape;
    short color_pair;
    struct element *top;
    struct element *bottom;
    struct element *left;
    struct element *right;
    struct element *top_left;
    struct element *top_right;
    struct element *bottom_left;
    struct element *bottom_right;
} ELEMENT;

static void set_current_color(WINDOW *window, short color);
void draw_game_screen(int game_x_length, int game_y_length, int score_x_length, int score_y_length, const ELEMENT area[game_x_length][game_y_length], const char score[], bool id);
int draw_menu(int count, char **items, int selected);
void draw_end_screen(int score);
short add_color(short foreground, short background);
void set_border_color(short color_pair_number);
void set_score_color(short color_pair_number);
void set_score_border_color(short color_pair_number);
void start();
void end();
void terminal_too_small();