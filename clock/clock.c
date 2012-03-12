#include<fonts.h>

#include <curses.h>
#include <unistd.h>
#include <time.h>
#include <stdlib.h>

#define RIGHT 0
#define LEFT (!RIGHT)

#define MINUTES 10

long right_time = 10 * 60 * MINUTES;
long left_time  = 10 * 60 * MINUTES;
int  turn = LEFT;
int  paused = 1;

/* screen coordinates -- not used, yet */
int begy, begx, maxy, maxx;

void show_times (WINDOW *w);
void print_row (char *s);

int clock_exit(void)
{
  endwin();
  return 0;
}

int
main(int argc, char *argv[])
{
  WINDOW *w;
  struct timespec delay;

  delay.tv_sec = 0;
  delay.tv_nsec = 1000000000.0 / 10.0;        /* 1/10 second */

  /* initialize ncurses data structures and clear the screen */ 
  w = initscr();

  /* make characters typed by  the user  immediately  available to the program.*/
  cbreak();

  /* let's not show keys read by getch */
  noecho();

  /* don't translate RETURN to newline */
  nonl();

  /* don't flush output on interrupts */
  intrflush(stdscr, FALSE);

  /* treat the function keys specially */
  keypad(stdscr, TRUE);

  /* we want getch to be a non-blocking call */
  nodelay(w, true);

  /* get our window coordinates */
  getbegyx(w, begy, begx); /* TODO: Always [0,0] ? */ 
  getmaxyx(w, maxy, maxx);

  while(1)
  {
    int key = 0;
    while((key = getch()) == ERR)
    {

       /* TODO : update only when needed */
      clear();
      show_times(w);

      nanosleep(&delay, NULL);

      if (!paused)
      {
        if (turn == LEFT)
          left_time  --;
        else
          right_time --;
      }
    }

    if (key == 13)  /* ENTER */
    {
      turn = LEFT;
      paused = 0;
    }
    else if (key == ' ')
    {
      turn = RIGHT;
      paused = 0;
    }
    else if (key == 'p' || key == 'P')
      paused = 1;
    else if (key == 27)
      return clock_exit(); /* exit the program */

    wrefresh(w); /* TODO: refresh when needed */
  }

  return clock_exit(); /* never reached */
}

int show_time(int side, long time)
{
  static char buff[256];
  int min = time / 600;
  int sec = (time % 600) / 10;

  snprintf(buff, 255, "%s%3d:%2d%s",
		      side == LEFT  && turn == LEFT  ? "->" : "  ", 
                      min, sec,
		      side == RIGHT && turn == RIGHT ? "<-" : "");

  buff[255] = '\0';

  print_row(buff);

  return 1; /* TODO:fix. Always refresh */
}

void
show_times (WINDOW *w)
{
  int refresh;
  static char *progress =  "-\\|/-\\|/";
  static int prog_count = 0;

  refresh = show_time(LEFT, left_time);

  printw("\n\n");

  refresh += show_time(RIGHT, right_time);

  printw("\n\n");

  if (paused)
    printw("**** PAUSED ****");
  else
  {
    printw("%c", progress[prog_count]);
    if (0 == progress[++prog_count])
      prog_count = 0;
  }

  if(refresh)
   wrefresh(w);
}

void print_char(char *c, char buff[7][256], int *the_col)
{
  int row, col, src_col;

  for(row = 0; row < 7; row ++)
    for(col = *the_col, src_col = 0; src_col < 6; col++, src_col++)
      buff[row][col] = c[row * 6 + src_col] ? '#' : ' ';

  *the_col += 6;
}


#define TABLE_SIZE 128

void init_table(char* t[TABLE_SIZE])
{
  int i;

  for (i = 0; i < TABLE_SIZE; ++i)
    t[i] = 0;

  t['0'] = get_0();
  t['1'] = get_1();
  t['2'] = get_2();
  t['3'] = get_3();
  t['4'] = get_4();
  t['5'] = get_5();
  t['6'] = get_6();
  t['7'] = get_7();
  t['8'] = get_8();
  t['9'] = get_9();
  t['>'] = get_gt();
  t['<'] = get_lt();
  t['-'] = get_dash();
  t[' '] = get_sp();
  t[':'] = get_dots();
}

void print_row (char *s)
{
  static char buff[7][256];
  static char *table[TABLE_SIZE];
  static int table_ready = 0;

  int col=0;
  int i;

  if (!table_ready)
  {
    init_table(table);
    table_ready = 1;
  }

  for (; *s; s++)
    if(table[(int)*s])
      print_char(table[(int)*s], buff, &col);

  for (i = 0; i < 7; ++i)
  {
    buff[i][col] = '\0';
    printw(buff[i]);
    printw("\n");
  }
}
