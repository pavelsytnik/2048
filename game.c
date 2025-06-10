#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <threads.h>
#include <time.h>

#include <conio.h>

#define SIZE 4

void add_random(uint8_t grid[SIZE][SIZE])
{
    static bool initialized = false;
    uint8_t list[SIZE * SIZE][2];
    uint16_t len = 0;
    uint8_t r, n;
    uint8_t x, y;

    if (!initialized) {
        srand(time(NULL));
        initialized = true;
    }

    for (x = 0; x < SIZE; ++x) {
        for (y = 0; y < SIZE; ++y) {
            if (grid[x][y] == 0) {
                list[len][0] = x;
                list[len][1] = y;
                ++len;
            }
        }
    }

    if (len > 0) {
        r = rand() % len;
        x = list[r][0];
        y = list[r][1];
        n = (rand() % 10) < 9 ? 1 : 2;
        grid[x][y] = n;
    }
}

uint16_t count_empty(uint8_t grid[SIZE][SIZE])
{
    uint8_t x, y;
    uint16_t count = 0;

    for (x = 0; x < SIZE; x++) {
        for (y = 0; y < SIZE; y++) {
            if (grid[x][y] == 0) {
                count++;
            }
        }
    }

    return count;
}

bool find_pairs(uint8_t grid[SIZE][SIZE])
{
    uint8_t x, y;

    // Horizontal pairs
    for (x = 0; x < SIZE - 1; x++) {
        for (y = 0; y < SIZE; y++) {
            if (grid[x][y] == grid[x + 1][y]) {
                return true;
            }
        }
    }

    // Vertical pairs
    for (y = 0; y < SIZE - 1; y++) {
        for (x = 0; x < SIZE; x++) {
            if (grid[x][y] == grid[x][y + 1]) {
                return true;
            }
        }
    }

    return false;
}

bool can_move(uint8_t grid[SIZE][SIZE])
{
    if (count_empty(grid) > 0) {
        return true;
    }
    if (find_pairs(grid)) {
        return true;
    }
    return false;
}

void init_grid(uint8_t grid[SIZE][SIZE])
{
    for (int x = 0; x < SIZE; ++x) {
        for (int y = 0; y < SIZE; ++y) {
            grid[x][y] = 0;
        }
    }

    add_random(grid);
    add_random(grid);
}

uint8_t digit_count(uint32_t number)
{
    uint8_t count = 0;
    do {
        number /= 10;
        count++;
    } while (number);
    return count;
}

void draw_grid(uint8_t grid[SIZE][SIZE], uint32_t score)
{
    for (int y = 0; y < SIZE; ++y) {
        for (int x = 0; x < SIZE; ++x) {
            if (grid[x][y] != 0) {
                uint32_t value = 1 << grid[x][y];
                uint8_t t = 7 - digit_count(value);
                printf("%*s%d%*s", t - t / 2, "", value, t / 2, "");
            } else {
                printf("   .   ");
            }
        }
        printf("\n\n\n");
    }
    printf("Score: %d\n", score);
}

uint8_t *find_target(
    const uint8_t *curr,
    const uint8_t *stop,
    int8_t step
) {
    if (curr == stop || *curr == 0) return curr;

    for (const uint8_t *it = curr + step; ; it += step) {
        if (*it != 0) {
            if (*it == *curr) return it; // Merge
            return it - step; // Just move
        }
        if (it == stop) return it; // Don't move further
    }
    return NULL; // Fail
}

bool move_left(uint8_t grid[SIZE][SIZE], uint32_t *score)
{
    bool success = false;

    for (int y = 0; y < SIZE; ++y) {
        const uint8_t *stop = &grid[0][y];

        for (int x = 0; x < SIZE; ++x) {
            uint8_t *curr = &grid[x][y];

            if (*curr != 0) {
                uint8_t *target = find_target(curr, stop, -SIZE);
                if (target == curr) continue;

                if (*target == *curr) {
                    (*target)++;
                    *score += 1 << *target;
                    stop = target - SIZE;
                } else if (*target == 0) {
                    *target = *curr;
                }

                *curr = 0;
                success = true;
            }
        }
    }

    return success;
}

bool move_right(uint8_t grid[SIZE][SIZE], uint32_t *score)
{
    bool success = false;

    for (int y = 0; y < SIZE; ++y) {
        const uint8_t *stop = &grid[SIZE - 1][y];

        for (int x = SIZE - 1; x >= 0; --x) {
            uint8_t *curr = &grid[x][y];

            if (*curr != 0) {
                uint8_t *target = find_target(curr, stop, +SIZE);
                if (target == curr) continue;

                if (*target == *curr) {
                    (*target)++;
                    *score += 1 << *target;
                    stop = target - SIZE;
                } else if (*target == 0) {
                    *target = *curr;
                }

                *curr = 0;
                success = true;
            }
        }
    }

    return success;
}

bool move_up(uint8_t grid[SIZE][SIZE], uint32_t *score)
{
    bool success = false;

    for (int x = 0; x < SIZE; ++x) {
        const uint8_t *stop = &grid[x][0];

        for (int y = 0; y < SIZE; ++y) {
            uint8_t *curr = &grid[x][y];

            if (*curr != 0) {
                uint8_t *target = find_target(curr, stop, -1);
                if (target == curr) continue;

                if (*target == *curr) {
                    (*target)++;
                    *score += 1 << *target;
                    stop = target + 1;
                } else if (*target == 0) {
                    *target = *curr;
                }

                *curr = 0;
                success = true;
            }
        }
    }

    return success;
}

bool move_down(uint8_t grid[SIZE][SIZE], uint32_t *score)
{
    bool success = false;

    for (int x = 0; x < SIZE; ++x) {
        const uint8_t *stop = &grid[x][SIZE - 1];

        for (int y = SIZE - 1; y >= 0; --y) {
            uint8_t *curr = &grid[x][y];

            if (*curr != 0) {
                uint8_t *target = find_target(curr, stop, +1);
                if (target == curr) continue;

                if (*target == *curr) {
                    (*target)++;
                    *score += 1 << *target;
                    stop = target - 1;
                } else if (*target == 0) {
                    *target = *curr;
                }

                *curr = 0;
                success = true;
            }
        }
    }

    return success;
}

int main(void)
{
    uint8_t grid[SIZE][SIZE];
    uint32_t score = 0;

    init_grid(grid);
    draw_grid(grid, score);

    while (true) {
        int c = getch();
        bool success = false;
        switch ((char)c) {
        case 'a':
        case 75:
            success = move_left(grid, &score);
            break;
        case 'd':
        case 77:
            success = move_right(grid, &score);
            break;
        case 's':
        case 80:
            success = move_down(grid, &score);
            break;
        case 'w':
        case 72:
            success = move_up(grid, &score);
            break;
        case 'q':
            goto end;
        default:
            continue;
        }

        if (success) {
            system("cls");
            draw_grid(grid, score);

            thrd_sleep(&(struct timespec) { .tv_nsec = 150000000 }, NULL);
            add_random(grid);

            system("cls");
            draw_grid(grid, score);

            if (!can_move(grid)) {
                printf("GAME OVER!\n");
                break;
            }
        }
    }
    end:

    //printf("You lose\n");

    return EXIT_SUCCESS;
}
