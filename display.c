#include <ncurses.h>
#include <stdlib.h>
#include <unistd.h>

#define SIZE 20

#define BORDER_COLOR 1
#define SCORE_COLOR 2

bool initialized = false;
WINDOW *win; //Main screen, whole terminal
WINDOW *sub; //Child window

short registered_colors = 0;
short score_color = 0;
short border_color = 0;

// Structure for colored play area elements, each able to hold a color previously initialized with add_color();
typedef struct element {
    unsigned int pos_x;
    unsigned int pos_y;
    short color_pair;
} ELEMENT;

// todo document all functions.

bool draw_screen(const int x_length, const int y_length, const char area[x_length][y_length]) {
    if (!initialized) {
        fprintf(stderr,"start() must be called first");
        exit(EXIT_FAILURE);
    }
    // COLS/LINES are set now.
    // X vertical, Y horizontal.
    const int pos_y = (COLS / 2) - (y_length / 2);
    const int pos_x = (LINES / 2) - (x_length / 2);
    // Win is the parent. Height, width, x, y
    sub = subwin(win, x_length + 2, y_length + 4, pos_x, pos_y);

    // Add border.
    if (has_colors()) {
        wcolor_set(sub, BORDER_COLOR, NULL);
    }
    box(sub, 0, 0);

    for (int i = 0; i < x_length; i++) {
        for (int j = 0; j < y_length; j++) {
            // todo add color to each element of the field individually using a two dimensional array of structs and typedef.
            mvwaddch(sub, i+1, j+2, area[i][j]);
        }
    }
    refresh();
    return true;
}

short add_color(const short foreground, const short background) {
    // todo test 257
    if (has_colors()) {
        registered_colors++;
        if (registered_colors <= COLOR_PAIRS) {
            init_pair(registered_colors, foreground, background);
            return  registered_colors;
        }
    }
    return -1;
}

void set_border_color(const short color_pair_number) {
    // todo return error when use without adding a color first.
    if (has_colors()) {
        border_color = color_pair_number;
    }
}

void set_score_color(const short color_pair_number) {
    if (has_colors()) {
        score_color = color_pair_number;
    }
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
    start_color();
    if (has_colors()) {
        // Set some defaults.
        use_default_colors();
        init_pair(BORDER_COLOR, COLOR_WHITE, -1);
        init_pair(SCORE_COLOR, COLOR_WHITE, -1);
    }
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

    // -1 uses the terminal default color.
    set_border_color(COLOR_RED, -1);
    draw_screen(SIZE, SIZE, arr);
    sleep(2);

    end();
    return EXIT_SUCCESS;
}