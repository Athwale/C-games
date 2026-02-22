#include <stdio.h>
#include <ncurses.h>
#include <math.h>
#include <time.h>
#include <stdlib.h>
#include <poll.h>
#include <string.h>
#include <unistd.h>
#include "display.h"

// todo check sqrt and add if needed.
// todo malloc for some array free + set to null , calloc sets allocated space to 0;

// Compiling: clear; cc snake.c -o snake -lncurses; ./snake

#define SIZE 40
#define HEAD_POS 20
#define WIN_LENGTH 15
#define START_LENGTH 10
#define SPEED 100

// block types:
#define EMPTY '.'
#define BODY '0'
#define FOOD '#'

// clear; cc snake.c -o snake -lncurses; ./snake
//  y y y
// x
// x
// x

bool print_field(char arr[SIZE][SIZE], int length, bool ate);
int process_move(ELEMENT snake[], char direction, int length, int food_x, int food_y);
void update_field(char arr[SIZE][SIZE], ELEMENT snake[], int length, int food_x, int food_y);
char get_input(char current);

bool print_field(char arr[SIZE][SIZE], int length, bool ate) {
    int body_parts = 0;

    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            mvaddch(i, j, arr[i][j]);
            if (arr[i][j] == BODY)
                body_parts++;
        }
    }
    addstr("\nMovement, WSAD, Q-Quit, P-Pause");
    char score[40];
    snprintf(score, sizeof(score), "\nScore: %d\n", length);
    addstr(score);
    refresh();

    if (length > body_parts) {
        // We bit ourselves because there are less body parts than the length of the snake.
        // If the snake eats the new part is added on the next move, so we have to ignore it.
        if (ate) {
            return false;
        }
        return true;
    }
    // Normal step, nothing happened, move on.
    return false;
}

int process_move(ELEMENT snake[], char direction, int length, int food_x, int food_y) {
    // Returns: 1 - no food, consumed, 2 - food consumed, 3 - death.

    // Update the snake array with new coordinates.
    // Take first element, back up coords, move it, put original coord to next element.
    unsigned int prev_x = 0;
    unsigned int prev_y = 0;

    for (int i = 0; i < length; i++) {
        if (i == 0) {
            prev_x = snake[i].pos_x;
            prev_y = snake[i].pos_y;
            switch (direction) {
                case 'w':
                    snake[i].pos_x -= 1;
                    if (snake[0].pos_x == -1) {
                        return 3;
                    }
                    break;
                case 's':
                    snake[i].pos_x += 1;
                    if (snake[0].pos_x == SIZE) {
                        return 3;
                    }
                    break;
                case 'a':
                    snake[i].pos_y -= 1;
                    if (snake[0].pos_y == -1) {
                        return 3;
                    }
                    break;
                case 'd':
                    snake[i].pos_y += 1;
                    if (snake[0].pos_y == SIZE) {
                        return 3;
                    }
                    break;
                default:
                    break;
            }
        } else {
            unsigned int current_x = snake[i].pos_x;
            unsigned int current_y = snake[i].pos_y;
            snake[i].pos_x = prev_x;
            snake[i].pos_y = prev_y;
            prev_x = current_x;
            prev_y = current_y;
        }
    }

    char pos[20];
    snprintf(pos, sizeof(pos), "\nHead pos: x:%d y:%d\n", snake[0].pos_x, snake[0].pos_y);
    addstr(pos);

    // Check if food was consumed
    if (snake[0].pos_x == food_x && snake[0].pos_y == food_y) {
        return 2;
    }
    return 1;
}

void update_field(char arr[SIZE][SIZE], ELEMENT snake[], int length, int food_x, int food_y) {
    // Zero the whole field.
    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            arr[i][j] = EMPTY;
        }
    }

    // Place food.
    arr[food_x][food_y] = FOOD;

    // Draw snake into the field.
    for (int i = 0; i < length; i++) {
        arr[snake[i].pos_x][snake[i].pos_y] = snake[i].shape;
    }
}

char get_input(char current) {
    struct pollfd mypoll = {STDIN_FILENO, POLLIN|POLLPRI};
    char inputs[] = "wsadqp";
    char c;

    if (poll(&mypoll, 1, SPEED))    {
        scanf("%c", &c);
        for (int i = 0; i < strlen(inputs); i++) {
            if (c == inputs[i]) {
                return c;
            }
        }
    }
    return current;
}

bool write_results(char text[]) {
    FILE *p_file = fopen("./results.txt", "w");
    if (p_file == NULL) {
        puts("results.txt failed to open");
        return false;
    }
    fprintf(p_file, "%s", text);
    fclose(p_file);
    return true;
}

void end_game(bool win, int length) {
    endwin();
    if (!win) {
        puts("You died.");
    } else if (length == WIN_LENGTH) {
        puts("You win.");
    }

    printf("Snake length %d\n", length);

    char result[20];
    snprintf(result, sizeof(result), "Final score %d\n.", length);
    if (write_results(result)) {
        puts("Results written to results.txt");
    }
}

int main(void) {
    // Init rand function.
    srand(time(NULL));

    char field[SIZE][SIZE];
    ELEMENT snake[WIN_LENGTH];
    int length = START_LENGTH;
    int food_x = rand() % SIZE;
    int food_y = rand() % SIZE;

    // Init field.
    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            field[i][j] = EMPTY;
        }
    }

    // Init snake
    for (int i = 0; i < length; i++) {
        snake[i].shape = BODY;
        snake[i].pos_x = HEAD_POS;
        snake[i].pos_y = HEAD_POS + i;
    }

    char *menu[3] = {"START", "CREDITS", "QUIT"};
    int selection = 0;

    for (int i = 0; i < 3; i++) {
        printf("%d. %s\n", i, menu[i]);
    }
    printf("Select option: ");
    do {
        scanf("%d", &selection);
        if (strcmp("QUIT", menu[selection]) == 0) {
            return 0;
        }
        if (strcmp("CREDITS", menu[selection]) == 0) {
            puts("Ondrej Mejzlik");
            printf("Select option: ");
        }
    } while (strcmp("START", menu[selection]) != 0);
    puts("");

    // Init ncurses.
    initscr();
    cbreak();
    noecho();

    // Draw initial play area.
    update_field(field, snake, length, food_x, food_y);
    print_field(field, length, false);

    // Main loop.
    char last_direction = 's';
    char c = 'w';
    int result = 0;
    bool paused = true;

    while (c != 'q') {
        c = get_input(c);
        if (c == 'p') {
            paused = !paused;
            c = last_direction;
        }

        if (paused) {
            continue;
        }

        if (c == 'w' && last_direction == 's') {
            c = 's';
        }
        if (c == 's' && last_direction == 'w') {
            c = 'w';
        }
        if (c == 'a' && last_direction == 'd') {
            c = 'd';
        }
        if (c == 'd' && last_direction == 'a') {
            c = 'a';
        }
        last_direction = c;

        result = process_move(snake, c, length, food_x, food_y);
        // 1 - normal step, 2 - food consumed, 3 - death by hitting the edge.

        if (result == 2) {
            // Grow snake. The new block will be added to the correct place on the next move.
            snake[length].pos_x = snake[0].pos_x;
            snake[length].pos_y = snake[0].pos_y;
            snake[length].shape = BODY;
            length += 1;

            if (length == WIN_LENGTH) {
                break;
            }

            // New food location.
            food_x = rand() % SIZE;
            food_y = rand() % SIZE;
        }
        update_field(field, snake, length, food_x, food_y);
        if (print_field(field, length, ((result == 2) ? true : false ))) {
            // Returns true if the snake bit itself.
            result = 3;
        }

        if (result == 3) {
            // Die.
            break;
        }
    }

    if (result == 3) {
        end_game(false, length);
    } else {
        end_game(true, length);
    }

    return 0;
}