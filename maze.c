#include <ncurses.h>
#include "display.h"

#define SIZE 10
#define BACKGROUND '#'

int main() {

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
    // Init the playing field.
    int count = 0;
    for (int x = 0; x < SIZE; x++) {
        for (int y = 0; y < SIZE; y++) {
            field[x][y].id = count;
            field[x][y].pos_x = x;
            field[x][y].pos_y = y;
            field[x][y].shape = BACKGROUND;
            field[x][y].color_pair = field_color;
            field[x][y].top = nullptr;
            field[x][y].left = nullptr;
            field[x][y].bottom = nullptr;
            field[x][y].right = nullptr;
            field[x][y].top_left = nullptr;
            field[x][y].top_right = nullptr;
            field[x][y].bottom_left = nullptr;
            field[x][y].bottom_right = nullptr;
            count++;
        }
    }

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
    for (int x = 0; x < SIZE; x++) {
        for (int y = 0; y < SIZE; y++) {
            // Fill adjacent block pointers.
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
            } else if (field[x][y].pos_x == 0 && field[x][y].pos_y <= SIZE - 2 && field[x][y].pos_y > 0) {
                // Top row except corners.
                field[x][y].top = nullptr;
                field[x][y].left = &field[x][y-1];
                field[x][y].bottom = &field[x+1][y];
                field[x][y].right = &field[x][y+1];
                field[x][y].top_left = nullptr;
                field[x][y].top_right = nullptr;
                field[x][y].bottom_left = &field[x+1][y-1];
                field[x][y].bottom_right = &field[x+1][y+1];
            } else if (field[x][y].pos_x == 0 && field[x][y].pos_y == SIZE - 1) {
                // Top right corner.
                field[x][y].top = nullptr;
                field[x][y].left = &field[x][y-1];
                field[x][y].bottom = &field[x+1][y];
                field[x][y].right = nullptr;
                field[x][y].top_left = nullptr;
                field[x][y].top_right = nullptr;
                field[x][y].bottom_left = &field[x+1][y-1];
                field[x][y].bottom_right = nullptr;
            } else if (field[x][y].pos_y == 0 && field[x][y].pos_x < SIZE - 2 && field[x][y].pos_x > 0) {
                // Left edge except corners.
                field[x][y].top = &field[x-1][y];
                field[x][y].left = nullptr;
                field[x][y].bottom = &field[x+1][y];
                field[x][y].right = &field[x][y+1];
                field[x][y].top_left = nullptr;
                field[x][y].top_right = &field[x-1][y+1];
                field[x][y].bottom_left = nullptr;
                field[x][y].bottom_right = &field[x+1][y+1];
            } else if (field[x][y].pos_x > 0 && field[x][y].pos_x < SIZE - 1 && field[x][y].pos_y > 0 && field[x][y].pos_y < SIZE - 1) {
                // Inside except edges.
                field[x][y].top = &field[x-1][y];
                field[x][y].left = &field[x][y-1];
                field[x][y].bottom = &field[x+1][y];
                field[x][y].right = &field[x][y+1];
                field[x][y].top_left = &field[x-1][y-1];
                field[x][y].top_right = &field[x-1][y+1];
                field[x][y].bottom_left = &field[x+1][y-1];
                field[x][y].bottom_right = &field[x+1][y+1];
            } else if (field[x][y].pos_y == SIZE - 1 && field[x][y].pos_x < SIZE - 2 && field[x][y].pos_x > 0) {
                // Right edge except corners.
                field[x][y].top = &field[x-1][y];
                field[x][y].left = &field[x][y-1];
                field[x][y].bottom = &field[x+1][y];
                field[x][y].right = nullptr;
                field[x][y].top_left = &field[x-1][y-1];
                field[x][y].top_right = nullptr;
                field[x][y].bottom_left = &field[x+1][y-1];
                field[x][y].bottom_right = nullptr;
            } else if (field[x][y].pos_x == SIZE - 1 && field[x][y].pos_y == 0 ) {
                // Bottom left corner.
                field[x][y].top = &field[x-1][y];
                field[x][y].left = nullptr;
                field[x][y].bottom = nullptr;
                field[x][y].right = &field[x][y+1];
                field[x][y].top_left = nullptr;
                field[x][y].top_right = &field[x-1][y+1];
                field[x][y].bottom_left = nullptr;
                field[x][y].bottom_right = nullptr;
            } else if (field[x][y].pos_x == SIZE - 1 && field[x][y].pos_y <= SIZE - 2 && field[x][y].pos_y > 1 ) {
                // Bottom row except corners.
                field[x][y].top = &field[x-1][y];
                field[x][y].left = &field[x][y-1];
                field[x][y].bottom = nullptr;
                field[x][y].right = &field[x][y+1];
                field[x][y].top_left = &field[x][y-1];
                field[x][y].top_right = &field[x-1][y+1];
                field[x][y].bottom_left = nullptr;
                field[x][y].bottom_right = nullptr;
            } else if (field[x][y].pos_x == SIZE - 1 && field[x][y].pos_y == SIZE - 1 ) {
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
        }
    }

    // todo make the grid init a function in the display library that you give your array to to init.

    char *items[2];
    items[0] = "Start";
    items[1] = "Quit";

    int choice = draw_menu(2, items, 0);
    if (choice == 1) {
        end();
        return 0;
    }

    end();
    //draw_game_screen(SIZE, SIZE, 5, SIZE, field, "None", false);

    for (int x = 0; x < SIZE; x++) {
        for (int y = 0; y < SIZE; y++) {
            printf("i:%3lu, x: %d, y: %d ", field[x][y].id, field[x][y].pos_x, field[x][y].pos_y);
        }
        puts("\n");
    }

    getch();

    end();
    return 0;
}