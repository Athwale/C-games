#include <ncurses.h>
#include <stdlib.h>
#include <unistd.h>

#define SIZE 20

bool initialized = false;
WINDOW *win; //Main screen, whole terminal
WINDOW *sub; //Child window


bool draw_field(const int x_length, const int y_length, const char area[x_length][y_length]) {
    if (!initialized) {
        fprintf(stderr,"start() must be called first");
        exit(EXIT_FAILURE);
    }

    // Win is the parent. Height, width, x, y
    // Todo calculate and put in the middle of the screen.
    // X vertical, Y horizontal
    sub = subwin(win, x_length + 2, y_length + 4, 0, 0);
    // Add border.
    box(sub, 0, 0);

    for (int i = 0; i < x_length; i++) {
        for (int j = 0; j < y_length; j++) {
            mvwaddch(sub, i+1, j+2, area[i][j]);
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
    noecho();
    // Enable arrow keys.
    keypad(win, TRUE);
    initialized = true;
}

void end() {
    delwin(sub);
    delwin(win);
    endwin();
    refresh();
}

int main(void) {
    // Test functionality.
    start();

    char arr[SIZE][SIZE] = {};
    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            arr[i][j] = '#';
        }
    }

    draw_field(SIZE, SIZE, arr);
    sleep(2);

    end();
    return EXIT_SUCCESS;
}