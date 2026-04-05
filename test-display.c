#define _XOPEN_SOURCE_EXTENDED
#include <ncursesw/ncurses.h>
#include <stdlib.h>
#include <unistd.h>
#include "display.h"

#define SIZE 10
#define WALL L'\u2588'

int main(void) {
    // Test functionality.
    start();
    // todo colors do not work with the Unicode chars.
    const short field_color = add_color(COLOR_GREEN,-1);

    ELEMENT field[SIZE][SIZE];
    init_grid(SIZE, SIZE, field, WALL, field_color);

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
    draw_game_screen(SIZE, SIZE, 5, SIZE, field, "Test", 1,false);
    sleep(5);
    draw_end_screen(1000000000);

    end();
    return EXIT_SUCCESS;
}
