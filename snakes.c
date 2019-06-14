/*
 * snake:  This is a demonstration program to investigate the viability
 *         of a curses-based assignment.
 *
 * Author: Dr. Phillip Nico
 *         Department of Computer Science
 *         California Polytechnic State University
 *         One Grand Avenue.
 *         San Luis Obispo, CA  93407  USA
 *
 * Email:  pnico@csc.calpoly.edu
 *
 * Revision History:
 *         $Log: snakes.c,v $
 *         Revision 1.2  2013-04-10 11:22:49-07  pnico
 *         fix myriad 64-bit incompatabilities
 *
 *         Revision 1.1  2013-04-02 15:47:33-07  pnico
 *         Initial revision
 *
 *         Revision 1.2  2004-04-13 12:32:18-07  pnico
 *         checkpointing with listener
 *
 *         Revision 1.1  2004-04-13 09:53:55-07  pnico
 *         Initial revision
 *
 *         Revision 1.1  2004-04-13 09:52:46-07  pnico
 *         Initial revision
 *
 */

#include "snakes.h"
#include "types.h"
#include "kmalloc.h"
#include "vga_console.h"
#include "asm.h"
#include "proc.h"

#define SN_LENGTH 10
#define SN_BODY_CHAR '*'
#define SN_FOOD_CHAR 3
#define SN_FOOD_COLOR 7

#define MAXSNAKES  100


static void erase_snake(snake s);
static void draw_snake(snake s);
static void move_snake(snake sn);
static void move_hungry_snake(snake sn);
static int obstructed(sn_point p);
static void delay();

static int deltaX[]={-1,0,1,-1,1,-1,0,1};
static int deltaY[]={-1,-1,-1,0,0,1,1,1};

static char head[]={'<','^','>','<','>','<','v','>'};

static char colors[] = { VGA_BLACK, VGA_BLUE, VGA_RED,
VGA_GREEN, VGA_MAGENTA, VGA_CYAN, VGA_YELLOW, VGA_WHITE, VGA_BLACK };

snake allsnakes=NULL;

static int rows;                /* global for the obvious reasons */
static int cols;

static int endsnake = 0;            /* a flag, that when set causes the
                                 * currently running snake to exit
                                 */


#define limit(x,min,max) ((x)<(min)?(min):(((x)>(max))?(max):x))
#define INT_MAX 0x7fffffffffffffffU

// Random functions, since we don't have any
static unsigned int seed = 1;
static void srand (int newseed) {
    seed = (unsigned)newseed & 0x7fffffffffffffffU;
}

int rand (void) {
    seed = (seed * 6364136223846793005U + 1442695040888963407U)
                 & 0x7fffffffffffffffU;
    return (int)seed;
}

// Delay functions
unsigned long rdtsc(void)
{
   unsigned long tsc;
   asm volatile("rdtsc" : "=a" (tsc));
   return tsc;
}

snake new_snake(int y, int x, int len, int dir, int color) {
  /* if parts of a snake would be off the screen, it starts
   * "coiled" (with parts stacked on top of each other.
   */
  snake new;
  int i;

  /* get snake on the screen */
  y = limit(y,0,rows-1);
  x = limit(x,0,cols-1);

  /* now get the snake */
  if ( !(new = (snake) kmalloc(sizeof(struct snake_st))) ) {
    return NULL;
  }

  new->dir = dir;
  new->len = len;
  new->color = color;
  new->body = (sn_point*)kmalloc(new->len * sizeof(sn_point));
  if ( !new->body ) {
    kfree(new);
    return NULL;
  }

  for(i=0;i<new->len;i++) {
    new->body[i].x = x;
    new->body[i].y = y;
    x = x - deltaX[new->dir];   /* work backwards, so head is at loc */
    y = y - deltaY[new->dir];
    y = limit(y,0,rows-1);
    x = limit(x,0,cols-1);
  }

  new->others = allsnakes;
  allsnakes = new;

  return new;
}

void free_snake(snake s){
  /* deallocate the given snake.  First remove it from allsnakes,
   * then free its bits
   */
  snake sp;
  /* remove s from the community of snakes */
  if ( s == allsnakes )
    allsnakes = allsnakes->others;
  else {
    for(sp=allsnakes; s != sp->others; sp = sp->others)
      ;
    if ( sp ) {
      sp->others = sp->others->others;
    }
  }
  /* now free the parts of s */
  kfree(s->body);
  kfree(s);
}

sn_point food={-1,-1};           /* the location of the food */

void place_food(){
  do {
    food.x = rand() % cols;
    food.y = rand() % rows;
  } while (obstructed(food));
}

void draw_food(){
  VGA_display_attr_char(food.x, food.y,SN_FOOD_CHAR, VGA_WHITE, VGA_BLACK);
}

int onfood(snake s) {
  /* is the snake's head on the food */
  return ((s->body[0].x == food.x)&&(s->body[0].y == food.y));
}

void run_hungry_snake(void *arg){
  snake *s = (snake*) arg;
  /* run a snake until it eats enough food.  We keep track of how
   * much each snake has eaten by its color.  Snakes get brighter
   * and brighter until they turn white then wink out.
   */
  if ( (food.x == -1) || (food.y == -1)) {
    place_food();
    draw_food();
  }

  for (;;) {
    delay();
    erase_snake(*s);
    move_hungry_snake(*s);
    draw_snake(*s);
    /* if the new head is on the food, count it as eaten and
     * replace the food
     */
    if (onfood(*s)) {
      place_food();             /* place new snake food */
      draw_food();              /* draw it */

      (*s)->color++;           /* use the color as the counter */
      erase_snake(*s);
      /* now check to see if we're "full" */
      if ( (*s)->color > MAX_VISIBLE_SNAKE ) {
        free_snake(*s);
        kexit();
      }
      draw_snake(*s);
    }
    /* now check to see if we've been signaled to exit */
    if ( endsnake ) {
      endsnake=0;               /* clear the flag */
      erase_snake(*s);
      free_snake(*s);
      kexit();
    }
    yield();                /* yield to the next snake */
  }
}



void run_snake(void *arg){
  snake *s = (snake*) arg;
  /* run a snake until the endsnake flag is set. */
  for (;;) {
    delay();
    erase_snake(*s);
    move_snake(*s);
    draw_snake(*s);
    if ( endsnake ) {
      endsnake=0;               /* clear the flag */
      erase_snake(*s);
      free_snake(*s);
      kexit();
    }
    yield();                /* yield to the next snake */
  }
}

void draw_all_snakes(){
  /* draw all the snakes */
  snake s;
  for(s=allsnakes;s;s=s->others)
    draw_snake(s);
}

static int obstructed(sn_point p) {
  int  i;
  snake s;

  int ret=0;
  if ( p.x >= cols || p.x < 0 || p.y >= rows || p.y < 0 ) {
    /* check the walls */
    ret = 1;
  } else for(s=allsnakes; s && !ret ; s=s->others) { /* look for snakes */
      for(i=0;i<s->len;i++)
        if ( (p.x == s->body[i].x ) && (p.y == s->body[i].y ) ) {
          ret = 1;
          break;
        }
  }

  return ret;
}


static void move_snake(snake s){
  int i;
  sn_point newhead;
  int deadends[NUMDIRS];
  int alldead;

  for(i=0;i<NUMDIRS;i++) {
    deadends[i]=0;
  }
  alldead = 0;

  newhead.x = s->body[0].x + deltaX[s->dir];
  newhead.y = s->body[0].y + deltaY[s->dir];

  while (obstructed(newhead) && ! alldead) {
    deadends[s->dir] = 1;
    s->dir = rand() % NUMDIRS;
    if (s->dir >= NUMDIRS)
       s->dir = NUMDIRS - 1;
    // s->dir = (int) ((1.0 * rand()/INT_MAX) * NUMDIRS);
    newhead.x = s->body[0].x + deltaX[s->dir];
    newhead.y = s->body[0].y + deltaY[s->dir];
    alldead = 1;
    for(i=0;i<NUMDIRS;i++)
      if ( deadends[i] == 0 )
        alldead = 0;
  }

  if ( alldead ) {              /* we're stuck */
    newhead = s->body[0];
  }

  for(i=s->len-1; i; i--){
    s->body[i].x = s->body[i-1].x;
    s->body[i].y = s->body[i-1].y;
  }
  s->body[0].x = newhead.x;
  s->body[0].y = newhead.y;

}

static direction delta2dir[3][3] = { {NW, N, NE},
                                     { W, N,  E},
                                     {SW, S,  SE} };

static direction food_direction(int x,int y) {
  /* return the direction of the food (well, as close as this system can)
   * get.
   */
  int deltaX,deltaY;

  if ( food.x > x )
    deltaX = 1;
  else if ( food.x < x )
    deltaX = -1;
  else
    deltaX = 0;

  if ( food.y > y )
    deltaY = 1;
  else if ( food.y < y )
    deltaY = -1;
  else
    deltaY = 0;

  return delta2dir[deltaY+1][deltaX+1];
}

static void move_hungry_snake(snake s){
  int i;
  sn_point newhead;
  int deadends[NUMDIRS];
  int alldead;

  for(i=0;i<NUMDIRS;i++) {
    deadends[i]=0;
  }
  alldead = 0;

  s->dir = food_direction(s->body[0].x,s->body[0].y);

  newhead.x = s->body[0].x + deltaX[s->dir];
  newhead.y = s->body[0].y + deltaY[s->dir];

  while (obstructed(newhead) && ! alldead) {
    deadends[s->dir] = 1;
    s->dir = rand() % NUMDIRS;
    if (s->dir >= NUMDIRS)
       s->dir = NUMDIRS - 1;
    // s->dir = (int) ((1.0 * rand()/INT_MAX) * NUMDIRS);
    newhead.x = s->body[0].x + deltaX[s->dir];
    newhead.y = s->body[0].y + deltaY[s->dir];
    alldead = 1;
    for(i=0;i<NUMDIRS;i++)
      if ( deadends[i] == 0 )
        alldead = 0;
  }

  if ( alldead ) {              /* we're stuck */
    newhead = s->body[0];
  }

  for(i=s->len-1; i; i--){
    s->body[i].x = s->body[i-1].x;
    s->body[i].y = s->body[i-1].y;
  }
  s->body[0].x = newhead.x;
  s->body[0].y = newhead.y;

}

static void draw_snake(snake s){
  int i;

  VGA_display_attr_char(s->body[0].x,s->body[0].y,head[s->dir],
                        colors[s->color], VGA_BLACK);
  for(i=1;i<s->len;i++)
    VGA_display_attr_char(s->body[i].x, s->body[i].y, SN_BODY_CHAR,
        colors[s->color], VGA_BLACK);
}

static void erase_snake(snake s){
  int i;
  for(i=0;i<s->len;i++)
    VGA_display_attr_char(s->body[i].x, s->body[i].y, ' ',
           VGA_BLACK, VGA_BLACK);
}

void kill_snake(){
  /* set the flag so that the next time run_snake goes around it's
   * killed
   */
  endsnake=1;
}


static int snake_delay=10;     /* default 50 msec */
extern unsigned int get_snake_delay() {
  /* return whatever the current delay is */
  return snake_delay;
}

extern void set_snake_delay(unsigned int msec) {
  /* set the snake delay between moves to the given number
   * of milliseconds
   */
  snake_delay=msec;
}

static int noop_func(int i)
{
   return i + 10;
}

static void delay() {
   for (int i = 0; i < 0xFFFFF; i++)
      noop_func(10);
#if 0
  /*
   * Sleep for the number of milliseconds specified by
   * snake_delay
   */
  struct timespec tv;
  tv.tv_sec  = (snake_delay * 1000)/1000000000;
  tv.tv_nsec = (snake_delay * 1000000)%1000000000;
  if ( (-1 == nanosleep(&tv,NULL)) && errno != EINTR ) {
    perror("nanosleep");
  }
#endif
}

snake snakeFromLWpid(int lw_pid){
  /* return a pointer to the snake with the given lw_pid,
   * or NULL if it doesn't exist
   */
  snake s;
  for(s=allsnakes;s;s=s->others)
    if ( s->pid == curr_proc->pid )
      break;
  return s;
}

void setup_snakes(int hungry)
{
   int i,cnt;
   static snake s[MAXSNAKES];
   struct Process *snake;

   // Don't use this seed for anything meaningful
   srand(curr_proc->pid);
   rows = VGA_row_count();
   cols = VGA_col_count();

   /* Initialize Snakes */                                     
   cnt = 0;                                                    
   /* snake new_snake(int y, int x, int len, int dir, int color) ;*/          
                                                              
   s[cnt++] = new_snake( 8,30,10, E,1);/* each starts a different color */   
   s[cnt++] = new_snake(10,30,10, E,2);                        
   s[cnt++] = new_snake(12,30,10, E,3);                        
   s[cnt++] = new_snake( 8,50,10, W,4);                        
   s[cnt++] = new_snake(10,50,10, W,5);                        
   s[cnt++] = new_snake(12,50,10, W,6);                        
   s[cnt++] = new_snake( 4,40,10, S,7);                        

   /* Draw each snake */                                       
   VGA_clear();
   draw_all_snakes();                                          

   /* turn each snake loose as an individual LWP */            
   for(i=0; i<cnt; i++) {                                        
      snake = PROC_create_kthread(hungry ? run_hungry_snake : run_snake, &s[i]);    
      if (snake)
         s[i]->pid = snake->pid;
   }                                                           

}
