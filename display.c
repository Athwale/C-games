#include <ncurses.h>
#include <stdlib.h>
#include <unistd.h>

bool initialized = false;
WINDOW *win;

bool draw_field(const int x_length, const int y_length, const char area[x_length][y_length]) {
    if (!initialized) {
        fprintf(stderr,"start() must be called first");
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < x_length; i++) {
        for (int j = 0; j < y_length; j++) {
            mvaddch(i, j, area[i][j]);
        }
    }

    refresh();
    return true;
}

void start() {
    // Init ncurses.

    if ((win = initscr()) == NULL ) {
        fprintf(stderr, "Error initialising ncurses.\n");
        exit(EXIT_FAILURE);
    }
    cbreak();
    noecho();
    initialized = true;
}

void end() {
    delwin(win);
    endwin();
    refresh();
}

int main(void) {
    // Test functionality.
    start();

    char arr[10][10] = {};
    for (int i = 0; i < 10; i++) {
        for (int j = 0; j < 10; j++) {
            arr[i][j] = '#';
        }
    }

    draw_field(10, 10, arr);
    sleep(3);

    end();
    return EXIT_SUCCESS;
}