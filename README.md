#Assignment 4 - The Game of Life

Our program runs the Game of Life simulation, given a starting universe. The Game of Life is a simulation where a potentially infinite, 2-D universe begins with a grid of cells, which are either alive or dead. Over the course of multiple generations (iterations), cells come to life or die in accordance with the following rules:
	1) Any live cell with two or three live neighbors survives.
	2) Any dead cell with exactly three live neighbors becomes a live cell.
	3) All other cells die or remain dead.

The cells to be live for the first generation are determined by the input file, which specifies the number of rows and columns the 2-D universe must contain, as well as the initial placement of live cells in that universe. It should be further noted that the user can specify whether the universe is flat or toroidal.

The following are the command-line options that the user has.

-t: The universe is toroidal
-s: Silent mode. Do not display the evolution over generations
-n <generations>: Number of generations
-i <input file>: Input file for initial state. Default is stdin
-o <output file>: Output file for the final state. Default is stdout


## Building

Run the following command to build the `life` command.

```
$ make all
```

## Running

Use the following command to run the program for 20 generations using the input file unix.txt in the 'lists' directory. Neither the file nor the directory is uploaded to my GitLab repository. It is assumed that the user has both.

```
$ make tst
```

To run the program using your own input file and preferred number of generations, the following is the usage. Invoking the -t option sets the universe to being toroidal, while invoking the -s option silences the ncurses display.

```
$ ./life -[ts] -n <generation> [-i <input>] [-o <output>]
```

