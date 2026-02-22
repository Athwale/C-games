#pragma once

// Structure for colored play area elements:
// int pos_x
// int pos_y
// char shape
// short color_pair
typedef struct element {
    unsigned int pos_x;
    unsigned int pos_y;
    char shape;
    short color_pair;
} ELEMENT;

static void set_current_color(WINDOW *window, short color);
void draw_game_screen(int x_length, int y_length, const ELEMENT area[x_length][y_length], const char score[]);
void end_game_screen();
int draw_menu(int count, char **items, int selected);
void draw_end_screen(int score);
short add_color(short foreground, short background);
void set_border_color(short color_pair_number);
void set_score_color(short color_pair_number);
void start();
void end();