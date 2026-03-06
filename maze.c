#include <ncurses.h>
#include "display.h"

#define SIZE 20
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
    int count = 0;
    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            field[i][j].id = count;
            field[i][j].pos_x = 0;
            field[i][j].pos_y = 0;
            field[i][j].shape = BACKGROUND;
            field[i][j].color_pair = field_color;
            count++;
        }
    }

    char *items[2];
    items[0] = "Start";
    items[1] = "Quit";

    int choice = draw_menu(2, items, 0);
    if (choice == 1) {
        end();
        return 0;
    }

    // todo run the maze. Create the maze inside the field.
    draw_game_screen(SIZE, SIZE, 5, SIZE, field, "None", false);

    // for (int i = 0; i < SIZE; i++) {
    //     for (int j = 0; j < SIZE; j++) {
    //         printf("%3lu ", field[i][j].id);
    //     }
    //     puts("\n");
    // }

    getch();

    end();
    return 0;
}