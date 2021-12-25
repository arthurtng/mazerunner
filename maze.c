#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>

#define GRID 21
#define MINSIZE 4
#define BUFFSIZE (GRID + 1)


struct maze{
   char map[GRID][GRID];
   int height;
   int width;
   int start_y;
   int start_x;
};
typedef struct maze maze;

// Test function
void test(void);

// Reads maze from file
void readfile(char* filename, maze* c);

// Converts dimensions to ints
bool readsize(maze* c, char buffer[]);

// Finds start of maze in upper left corner
bool findstart(maze* c);

// Recursively explores for solution
bool explore(char maze[GRID][GRID], int y, int x, int width, int height);

// Prints out the maze
void printmaze(maze* c);

// Exits program with warning
void on_error(const char* s);


int main(int argc, char* argv[])
{
   test(); 
   
   if (argc != 2){
     on_error("Incorrect usage. Please input .txt file with maze in it.");
   }
   
   maze* c = (maze*)calloc(1, sizeof(maze));
   if (c == NULL) {
      on_error("Cannot allocate memory.");
   }
   
   readfile(argv[1], c);
   if(!findstart(c)){
      on_error("Starting point must be in the upper left quadrant.");
   }
   
   if (!explore(c->map, c->start_y, c->start_x, c->width, c->height)){
      printf("The way is shut.\n");
      free(c);
      return 1;
   }
   
   printf("There is a way.\n");
   printmaze(c);
   free(c);      
   
   return 0;
}

void test(void)
{
   // Initialise maze for testing
   maze* testmaze = (maze*)calloc(1, sizeof(maze));
   
   // 10x10 maze with solution
   char test[GRID][GRID] = {
      {'#', '#', '#', '#', '#', '#', '#', '#', '#', '#'},
      {' ', ' ', '#', ' ', ' ', ' ', ' ', ' ', ' ', '#'},
      {'#', ' ', '#', ' ', '#', ' ', '#', '#', ' ', '#'},
      {'#', ' ', '#', ' ', '#', '#', '#', '#', ' ', '#'},
      {'#', ' ', '#', ' ', ' ', ' ', ' ', '#', ' ', '#'},
      {'#', ' ', '#', ' ', '#', '#', '#', '#', ' ', '#'},
      {'#', ' ', '#', ' ', ' ', ' ', ' ', '#', ' ', '#'},
      {'#', ' ', '#', '#', '#', '#', ' ', '#', ' ', '#'},
      {'#', ' ', ' ', ' ', ' ', ' ', ' ', '#', ' ', ' '},
      {'#', '#', '#', '#', '#', '#', '#', '#', '#', '#'}
   };
   for (int i=0; i<10; i++){
      strcpy(testmaze->map[i], test[i]);
   }
   
   char* testbuffer = "10 10";
   assert(readsize(testmaze, testbuffer));
   assert(testmaze->width==10);
   assert(testmaze->height==10);
   
   assert(findstart(testmaze));
   assert(testmaze->start_y==1);
   assert(testmaze->start_x==0);
     
   assert(explore(testmaze->map, testmaze->start_y, testmaze->start_x, testmaze->width, testmaze->height));
      
   //4x4 maze with no solution
   char test2[GRID][GRID] = {
      {'#', '#', '#', '#'},
      {' ', ' ', '#', '#'},
      {'#', '#', '#', '#'},
      {'#', '#', '#', '#'}
   };
   for (int i=0; i<4; i++){
      strcpy(testmaze->map[i], test2[i]);
   }
         
   char* testbuffer2 = "4 4";
   assert(readsize(testmaze, testbuffer2));
   assert(testmaze->width==4);
   assert(testmaze->height==4);

   assert(findstart(testmaze));
   assert(testmaze->start_y==1);
   assert(testmaze->start_x==0);
     
   assert(!explore(testmaze->map, testmaze->start_y, testmaze->start_x, testmaze->width, testmaze->height));
   
   //5x5 maze with invalid starting point  
   char test3[GRID][GRID] = {
      {'#', '#', '#', '#', '#'},
      {'#', ' ', ' ', ' ', ' '},
      {'#', ' ', '#', '#', '#'},
      {' ', ' ', '#', '#', '#'},
      {'#', '#', '#', '#', '#'}
   };
   for (int i=0; i<4; i++){
      strcpy(testmaze->map[i], test3[i]);
   }
         
   char* testbuffer3 = "5 5";
   assert(readsize(testmaze, testbuffer3));
   assert(testmaze->width==5);
   assert(testmaze->height==5);

   assert(!findstart(testmaze));   
   
   // Invalid dimensions
   char* testbuffer4 = "21 21";
   assert(!readsize(testmaze, testbuffer4));
   char* testbuffer5 = "3 3";
   assert(!readsize(testmaze, testbuffer5));
      
   free(testmaze);
   
}

bool findstart(maze* c)
{
   for (int j=0; j<c->height/2; j++){
      if (c->map[j][0] == ' '){
         c->start_y = j;
         c->start_x = 0;
         return true;
      }   
   }
   
   for (int i=0; i<c->width/2; i++){
      if (c->map[0][i] == ' '){
        c->start_y = 0;
        c->start_x = i;
        return true;
      }   
   }
   return false;
}

bool explore(char maze[GRID][GRID], int y, int x, int width, int height)
{  
   if (maze[y][x] == '#' || maze[y][x] == '.'){
      return false;      
   }   
     
   maze[y][x] = '.';
   
   if ((y == height-1 || x == width-1) && maze[y][x] != '#'){
      return true;
   }
   
   if (y + 1 < height){
      if (explore(maze, y+1, x, width, height)){
         return true;
      }
   }
   
   if (x + 1 < width){
      if (explore(maze, y, x+1, width, height)){
         return true;
      }
   }
   
   if (y - 1 >= 0){
      if (explore(maze, y-1, x, width, height)){
         return true;
      }
   }
   
   if (x - 1 >= 0){
      if (explore(maze, y, x-1, width, height)){
         return true;
      }
   } 
      
   return false;

}

void readfile(char* filename, maze* c)
{
   FILE* fp = fopen(filename, "r");
   if(!fp){
      on_error("Cannot read from file.");      
   }
   
   char buffer[BUFFSIZE];

   // Reads first line for dimensions
   if(!fgets(buffer, BUFFSIZE, fp)){
      on_error("Cannot read from file.");
   }
   
   if(!readsize(c, buffer)){
      on_error("Invalid dimensions. Maze must be at least 4x4 and no larger than 20x20.");
   }
   
   int i = 0;
   while(fgets(c->map[i], BUFFSIZE, fp)){
      i++;
   }
   
   fclose(fp);
   
}

bool readsize(maze* c, char buffer[])
{
   char h[BUFFSIZE], w[BUFFSIZE];

   sscanf(buffer, "%s %s", h, w);   
   c->height = atoi(h);
   c->width = atoi(w);
   
   if (c->height >= GRID || c->height < MINSIZE || c->width >= GRID || c->width < MINSIZE){
      return false;
   }
   return true;
}

void printmaze(maze* c)
{
   for (int j=0; j<c->height; j++){
      for (int i=0; i<c->width; i++){
         printf("%c", c->map[j][i]);
      }
      printf("\n");
   }
}

void on_error(const char* s)
{
   fprintf(stderr, "%s\n", s);
   exit(EXIT_FAILURE);
}
