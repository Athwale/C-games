#include <ncurses.h>
#include "display.h"
#include <stdlib.h>
#include <unistd.h>

#define SIZE 20

int main(void) {
    // Test functionality.
    start();

    const short element_color = add_color(COLOR_YELLOW, -1);
    ELEMENT arr[SIZE][SIZE] = {};
    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            arr[i][j].pos_x = i;
            arr[i][j].pos_y = j;
            arr[i][j].shape = '$';
            arr[i][j].color_pair = element_color;
        }
    }

    char *menu_items[5];
    menu_items[0] = "test1";
    menu_items[1] = "test2";
    menu_items[2] = "test3";
    menu_items[3] = "test4";
    menu_items[4] = "test5 test ehwifgweujlkhgwerluwjeyhverwrwvebruolrewyvbe";

    // Use -1 for terminal background.
    set_border_color(add_color(COLOR_RED, -1));
    set_score_color(add_color(COLOR_GREEN, -1));

    int result = draw_menu(5, menu_items, 0);
    printf(" %d ", result);
    draw_game_screen(SIZE, SIZE, arr, "SCORE LINE");
    sleep(1);
    end_game_screen();
    draw_end_screen(1000000000);

    end();
    return EXIT_SUCCESS;
}
