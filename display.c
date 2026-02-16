#include <ncurses.h>
#include <stdlib.h>
#include <unistd.h>

#define SIZE 20

#define DEFAULT_BORDER_COLOR 1
#define DEFAULT_SCORE_COLOR 2

bool initialized = false;
WINDOW *win; // Main screen, whole terminal
WINDOW *play_area; // Playing field
WINDOW *menu; // Menu window

short registered_colors = 0;
short score_color = 0;
short border_color = 0;

// Structure for colored play area elements, each able to hold a color previously initialized with add_color();
typedef struct element {
    unsigned int pos_x;
    unsigned int pos_y;
    char shape;
    short color_pair;
} ELEMENT;

// todo document all functions.
// todo some stuff can be static.
// todo add configurable menu screen.
// todo add configurable end screen.

static void set_current_color(WINDOW *window, const short color) {
    if (has_colors()) {
        wcolor_set(window, color, NULL);
    }
}

void draw_screen(const int x_length, const int y_length, const ELEMENT area[x_length][y_length], const char score[]) {
    if (!initialized) {
        fprintf(stderr,"start() must be called first");
        exit(EXIT_FAILURE);
    }
    // COLS/LINES are set now.
    // X vertical, Y horizontal.
    const int pos_x = (LINES / 2) - (x_length / 2);
    const int pos_y = (COLS / 2) - (y_length / 2);
    // Win is the parent. Height, width, x, y
    play_area = subwin(win, x_length + 2, y_length + 4, pos_x, pos_y);

    // Add border with color.
    set_current_color(play_area, border_color);
    box(play_area, 0, 0);
    set_current_color(play_area, DEFAULT_BORDER_COLOR);

    // These coordinates are relative to the new window.
    for (int i = 0; i < x_length; i++) {
        for (int j = 0; j < y_length; j++) {
            set_current_color(play_area, area[i][j].color_pair);
            mvwaddch(play_area, i+1, j+2, area[i][j].shape);
        }
    }

    set_current_color(win, score_color);
    mvaddstr(pos_x + x_length + 2, pos_y, score);
    set_current_color(play_area, DEFAULT_SCORE_COLOR);

    refresh();
}

int draw_menu(const int count, char **items, int selected) {
    if (!initialized) {
        fprintf(stderr,"start() must be called first");
        exit(EXIT_FAILURE);
    }

    // todo center the window, set width to longest string
    // Win is the parent. Height, width, x, y
    menu = subwin(win, count + 2, 20, 10, 10);
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
    return -1;
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

void start() {
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
    initialized = true;
}

void end() {
    delwin(play_area);
    delwin(win);
    endwin();
    refresh();
}

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

    char *menu_items[3];
    menu_items[0] = "test1";
    menu_items[1] = "test2";
    menu_items[2] = "test3";

    // Use -1 for terminal background.
    set_border_color(add_color(COLOR_RED, -1));
    set_score_color(add_color(COLOR_GREEN, -1));

    int result = draw_menu(3, menu_items, 0);
    printf("%d", result);
    draw_screen(SIZE, SIZE, arr, "SCORE LINE");
    sleep(1);

    end();
    return EXIT_SUCCESS;
}