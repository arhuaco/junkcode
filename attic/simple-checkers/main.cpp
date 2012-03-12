/*
 *
 *
 *   Simple checkers game. Version 1.0 April 1999
 *
 *
*/

 #include <stdlib.h>
 #include <ctype.h>
 #include <string.h>
 #include <stdio.h>

 const int MAX_DEPTH= 40;
 const int MAX_MOVE_LENGTH= 25;
 const int MAX_MOVES_PER_POSITION= 25;

 const int INFINITY= 10000;

 const int WKING =0, BKING=1;
 const int WMAN  =2, BMAN=3;
 const int EMPTY=4,  BORDER=5;

 #include "player.cpp"

 #include "moves.cpp"

 TMoveGenerator TMoveGenerator;

 #include "eval.cpp"

 #include "search.cpp"
 TSearcher TSearcher;

 #include "game.cpp"

main(){
 printf("\n\n\nC++ Simple checkers game. Version 1.0 April 1999 - ");
 printf("by Nelson Castillo\n\n");
 printf("nelsoneci@gmail.com\n");
 printf("please see the file README for further information.\n\n\n");

 TGame Game;
 Game.Options();
 Game.Play();
 printf("\n\n * Hasta la vista, baby *\n\n");
 return 0;
}
