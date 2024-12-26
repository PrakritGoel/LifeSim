#include "universe.h"

#include <inttypes.h>
#include <ncurses.h>
#include <stdlib.h>
#include <unistd.h>

#define DELAY 50000

// If the silent mode is off, display the evolution by showing live cells
// of each generation for 50000 ms.
//
// Input Parameters:
// uA: Universe *: Universe to be displayed
// Returns: void
void show_evolution(Universe *uA) {
    clear();

    for (uint32_t r = 0; r < uv_rows(uA); r++) {
        for (uint32_t c = 0; c < uv_cols(uA); c++) {
            if (uv_get_cell(uA, r, c)) {
                mvprintw(r, c, "o");
            }
        }
    }

    usleep(DELAY);
    refresh();
    return;
}

// Play the game per the given rules for the desired number of generations.
// In each generation, live cells with 2 or 3 neighbors survive, dead cells
// with 3 neighbors come alive, while all others die.
// We start with two identical universes, A and B. We count the number
// of neighbors of each cell in universe A, and update universe B per the
// rules of the game based on that information. If the silent mode is off
// we display the evolution of the universe in each generation. At the end
// each generation, uA and uB are swapped.
//
// Input Parameters:
// num_generations: uint32_t: Number of generations
// uA: Universe *: Universe A
// uB: Universe *: Universe B
// silent: bool: Determines if evolution is to be displayed or not
// Returns: void
void play_game(uint32_t num_generations, Universe *uA, Universe *uB, bool silent) {
    Universe *temp;

    initscr();
    curs_set(FALSE);

    // Play for the given number of generations.
    for (uint32_t g = 0; g < num_generations; g++) {
        for (uint32_t i = 0; i < uv_rows(uA); i++) {
            for (uint32_t j = 0; j < uv_cols(uA); j++) {
                uint32_t live_neighbor_count = uv_census(uA, i, j);
                // Cells with 3 neighbors and live cells with 2 neighbors live.
                if ((live_neighbor_count == 3)
                    || (live_neighbor_count == 2 && uv_get_cell(uA, i, j))) {
                    uv_live_cell(uB, i, j);
                } else {
                    uv_dead_cell(uB, i, j);
                }
            }
        }
        if (silent == false) {
            show_evolution(uA);
        }
        temp = uA;
        uA = uB;
        uB = temp;
    }

    endwin();
    return;
}

// Usage Function
//
// Input Parameters:
// exec_name: char *: Name of the program
// Returns: void
void usage(char *exec_name) {
    printf("USAGE: %s [-ts] [-n <generation>] [-i <input>] [-o <output>]\n", exec_name);
    printf("Play the game of life given an initial state and the given number of generations\n");
    printf("-t: The universe is toroidal\n");
    printf("-s: Silent mode. Do not display the evolution over generations\n");
    printf("-n <generations>: Number of generations\n");
    printf("-i <input file>: Input file for initial state. Default is stdin\n");
    printf("-o <output file>: Output file for the final state. Default is stdout\n");
    return;
}

// Close open files and free the memory allocated
//
// Input Parameters:
// infile: char *: Name of the input file
// ifp: FILE *: File pointer to the input file opened for reading
// uA: Universe *: Pointer to universe A
// uB: Universe *: Pointer to universe B
// Returns: void
void cleanup(char *infile, FILE *ifp, Universe *uA, Universe *uB) {
    if (infile != NULL) {
        fclose(ifp);
    }

    // Free the allocated space
    uv_delete(uA);
    uv_delete(uB);
    return;
}

// Main Function
int main(int argc, char **argv) {
    int opt;
    char *infile = NULL;
    char *outfile = NULL;
    uint32_t num_generations = 100;
    uint32_t num_rows, num_cols;
    bool toroidal = false;
    FILE *ifp, *ofp;
    Universe *uA, *uB;
    bool silent = false;

    // Parse the input options. Store the sorting options in a set.
    while ((opt = getopt(argc, argv, "tsn:i:o:H")) != -1) {
        switch (opt) {
        case ('t'): toroidal = true; break;
        case ('s'): silent = true; break;
        case ('n'): num_generations = strtoul(optarg, NULL, 10); break;
        case ('i'): infile = optarg; break;
        case ('o'): outfile = optarg; break;
        default: usage(argv[0]); exit(EXIT_FAILURE);
        }
    }

    if (infile == NULL) {
        ifp = stdin;
    } else if ((ifp = fopen(infile, "r")) == NULL) {
        printf("The input file is invalid. Please provide a valid input file\n");
        exit(EXIT_FAILURE);
    }

    fscanf(ifp, "%ud", &num_rows);
    fscanf(ifp, "%ud", &num_cols);
    uA = uv_create(num_rows, num_cols, toroidal);
    uB = uv_create(num_rows, num_cols, toroidal);

    // If there is no incorrect entry in the input file...
    if (uv_populate(uA, ifp)) {
        play_game(num_generations, uA, uB, silent);

        if (outfile == NULL) {
            uv_print(uA, stdout);
        } else if ((ofp = fopen(outfile, "w")) == NULL) {
            printf("Unable to open %s for writing. Please check the path\n", outfile);
        } else {
            uv_print(uA, ofp);
            fclose(ofp);
        }
        cleanup(infile, ifp, uA, uB);
        return 0;
    } else {
        printf("The input file contains invalid entries. Exiting...\n");
        cleanup(infile, ifp, uA, uB);
        return 1;
    }
}
