/*
 * labyrinth.h
 *
 *  Created on: Nov 1, 2018
 *      Author: brian
 */

#ifndef LABYRINTH_H_
#define LABYRINTH_H_

//This is 1 square in the labyrinth. 1 Square contains 4 cells.
//A cell can contain 1 hole or nothing. The marble can move over any cell
#define BLOCK_SIZE 20

#define NUM_ROWS 4
#define NUM_COLS 4

#define CELL_SIZE BLOCK_SIZE / 2
#define HALF_CELL BLOCK_SIZE / 4
#define NUM_CELLS NUM_ROWS * NUM_COLS * 4  //4 CELLS PER BLOCK

//Max number of walls
#define MAX_H_WALLS (NUM_ROWS + 1) * NUM_COLS
#define MAX_V_WALLS (NUM_COLS + 1) * NUM_ROWS

//Number of walls per row/column in labyrinth
#define WALLS_PER_AXIS 4

//Number of rows/columns for walls
#define NUM_WALL_AXI 5

//Labyrinth boundaries
#define LAB_MIN_X 24
#define LAB_MAX_X LAB_MIN_X + BLOCK_SIZE * WALLS_PER_AXIS
#define LAB_MIN_Y 24
#define LAB_MAX_Y LAB_MIN_Y + BLOCK_SIZE * WALLS_PER_AXIS

#define HOLE_RADIUS 2 //Hole size
#define MARBLE_RADIUS 2 //Marble Size
#define HOLE_MARGIN 3 //Margin for marble "falling" into hole
#define TARGET_MARGIN 3

//Directions the marble can move
typedef enum {Up, Right, Down, Left, Stable} direction_t;

typedef struct
{
    direction_t direction; //different directions marble can move
    int valid; //Exists in game and on screen if true
    int x;
    int y;
} marble_t;

typedef struct
{
    int valid;
    int x;
    int y;
} hole_t;

typedef struct
{
    int valid;
    int x1;
    int x2;
    int y;
} h_wall_t;

typedef struct
{
    int valid;
    int x;
    int y1;
    int y2;
} v_wall_t;

typedef struct
{
    int valid;
    int x_cell_index;
    int y_cell_index;
} target_cell_t;

typedef struct
{
    h_wall_t h_walls[MAX_H_WALLS];
    v_wall_t v_walls[MAX_V_WALLS];
    hole_t holes[NUM_CELLS];   //64 possible hole cells in labyrinth
    target_cell_t targetCell;
} labyrinth_t;



#endif /* LABYRINTH_H_ */


#ifndef true
#define true 1
#endif

#ifndef false
#define false 0
#endif
