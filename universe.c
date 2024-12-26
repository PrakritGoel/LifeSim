#include "universe.h"

#include <stdlib.h>

// rows and cols specify the dimensions of our finite universe.
// A value of false indicates that a cell is dead, while true means it's alive.
// A true value for toroidal means our universe is toroidal, but flat otherwise.
struct Universe {
    uint32_t rows;
    uint32_t cols;
    bool **grid;
    bool toroidal;
};

// Initialize the universe by allocating memory for the universe, and all
// the rows and columns, and setting the toroidal boolean variable.
//
// Input Parameters:
// rows: uint32_t: Number of rows
// cols: uint32_t: Number of columns
// toroidal: Boolean: Specifies the shape of the universe
// Returns: Pointer to the universe
Universe *uv_create(uint32_t rows, uint32_t cols, bool toroidal) {
    Universe *u = (Universe *) calloc(1, sizeof(Universe));

    u->rows = rows;
    u->cols = cols;
    u->grid = (bool **) calloc(rows, sizeof(bool *));

    for (uint32_t r = 0; r < rows; r++) {
        u->grid[r] = (bool *) calloc(cols, sizeof(bool));
    }

    toroidal = false;
    return (u);
}

// The destructor method to create. Frees all the memory allocated.
//
// Input Parameters:
// u: Pointer to the universe
// Returns: void
void uv_delete(Universe *u) {
    for (uint32_t r = 0; r < u->rows; r++) {
        free(u->grid[r]);
    }

    free(u->grid);
    free(u);
    return;
}

// Return the number of rows in the universe.
//
// Input Parameters:
// u: Universe *: Universe
// Returns: Number of rows
uint32_t uv_rows(Universe *u) {
    return u->rows;
}

// Return the number of cols in the universe.
//
// Input Parameters:
// u: Universe *: Universe
// Returns: Number of cols
uint32_t uv_cols(Universe *u) {
    return u->cols;
}

// Mark the cell as live.
//
// Input Parameters:
// u: Universe *: Universe
// r: uint32_t: Row number
// c: uint32_t: Column number
// Returns: void
void uv_live_cell(Universe *u, uint32_t r, uint32_t c) {
    if (r >= 0 && r < u->rows && c >= 0 && c < u->cols) {
        u->grid[r][c] = true;
    }

    return;
}

// Mark the cell as dead.
//
// Input Parameters:
// u: Universe *: Universe
// r: uint32_t: Row number
// c: uint32_t: Column number
// Returns: void
void uv_dead_cell(Universe *u, uint32_t r, uint32_t c) {
    if (r >= 0 && r < u->rows && c >= 0 && c < u->cols) {
        u->grid[r][c] = false;
    }

    return;
}

// Return the status of the cell. If the input is out of bounds,
// return false
//
// Input Parameters:
// u: Universe *: Universe
// r: uint32_t: Row number
// c: uint32_t: Column number
// Returns: true if the cell is live, false otherwise
bool uv_get_cell(Universe *u, uint32_t r, uint32_t c) {
    if (r >= 0 && r < u->rows && c >= 0 && c < u->cols) {
        return (u->grid[r][c]);
    }

    return (false);
}

// Populate the universe based on the information in the input file.
// The first row of the file that contains number of rows and
// columns has already been read, and the universe has already been
// created. The remaining rows of the file contain row and column
// indices of one live cell per line.
//
// Input Parameters:
// u: Universe *: Universe to populate
// infile: FILE *: File containing information on live cells
// Returns: false if an index is out of bound, true otherwise.
bool uv_populate(Universe *u, FILE *infile) {
    uint32_t num_rows, num_cols;
    int ret_val = 0;
    uint32_t r, c;

    num_rows = uv_rows(u);
    num_cols = uv_cols(u);

    // Read the rest of the file to identify live cells.
    while (ret_val != EOF) {
        ret_val = fscanf(infile, "%d", &r);
        ret_val = fscanf(infile, "%d", &c);

        if (r < 0 || r >= num_rows || c < 0 || c >= num_cols) {
            return (false);
        }
        uv_live_cell(u, r, c);
    }

    return (true);
}

// Count and returns the number of live neighbors.
//
// Input Parameters:
// u: Universe *: Universe
// r: uint32_t: Row index
// c: uint32_t: Column index
// Returns: uint32_t: Number of live neighbors
uint32_t uv_census(Universe *u, uint32_t r, uint32_t c) {
    uint32_t num_live_neighbors = 0;
    uint32_t row_low, row_high, col_low, col_high;

    // When r/c are 0 or u->row, the cell lies on the boundary of the
    // grid. If the universe is toroidal, we need to wrap to find all
    // the neighbors. If the universe is flat however, we need to ignore
    // the neighbors that will fall off the grid. In other cases, we
    // want to consider the 8 neighbors around the cell.
    // Moreover, since we are using uint32_t, we can't compare it to -1.
    // Further if we subtract 1 from 0, we will get 0xffffffff. So, setting
    // the range for determining neighbors here. Toroidal universes are
    // handled as a special case when determining neighbors.

    row_low = r - 1;
    row_high = r + 1;
    col_low = c - 1;
    col_high = c + 1;

    if (r == 0) {
        row_low = 0;
    }
    if (c == 0) {
        col_low = 0;
    }
    if (r == u->rows - 1) {
        row_high = u->rows - 1;
    }
    if (c == u->cols - 1) {
        col_high = u->cols - 1;
    }

    for (uint32_t i = row_low; i <= row_high; i++) {
        for (uint32_t j = col_low; j <= col_high; j++) {
            // Ignore the current cell
            if (i == r && j == c) {
                continue;
            }

            if (uv_get_cell(u, i, j) == true) {
                num_live_neighbors++;
            }
        }
    }

    if (r == 0 && u->toroidal == true) {
        if (uv_get_cell(u, u->rows - 1, c) == true) {
            num_live_neighbors++;
        }
    }
    if (r == u->rows - 1 && u->toroidal == true) {
        if (uv_get_cell(u, 0, c) == true) {
            num_live_neighbors++;
        }
    }

    // If we repeat the above process for columns, the four corner
    // elements will be counted twice. Therefore, we add checks for them.
    if (c == 0 && r != 0 && r != u->rows - 1 && u->toroidal == true) {
        if (uv_get_cell(u, r, u->cols - 1) == true) {
            num_live_neighbors++;
        }
    }
    if (c == u->cols - 1 && r != 0 && r != u->rows - 1 && u->toroidal == true) {
        if (uv_get_cell(u, r, 0) == true) {
            num_live_neighbors++;
        }
    }

    return num_live_neighbors;
}

// Print the universe. Print an "o" for a live cell, and a
// "." for a dead cell.
//
// Input Parameters:
// u: Universe *: Universe to print
// outfile: FILE *: File to print the output to
// Returns: void
void uv_print(Universe *u, FILE *outfile) {
    for (uint32_t r = 0; r < u->rows; r++) {
        for (uint32_t c = 0; c < u->cols; c++) {
            if (uv_get_cell(u, r, c)) {
                fprintf(outfile, "o");
            } else {
                fprintf(outfile, ".");
            }
        }

        fprintf(outfile, "\n");
    }

    return;
}
