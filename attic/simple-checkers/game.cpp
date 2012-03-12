/*
 * contents:
 *
 *  enum TPlayerType{HUMAN,COMPUTER}
 *  void GetHumanMove(int *Board, TPlayer Player, int *Move)
 *  void ShowBoard(int *Board);
 *  class TGame
 *
 *
*/


 char
 PtC(int Piece){
   switch(Piece){
     case WKING: return 'W';
     case BKING: return 'B';
     case WMAN:  return 'w';
     case BMAN:  return 'b';
     default:    return '-';
   }
 }

 void
 ShowBoard(int *Board){
   printf("    White\n");
   printf("  %c   %c   %c   %c     32  31  30  29\n",PtC(Board[10]),PtC(Board[11]),PtC(Board[12]),PtC(Board[13]));
   printf("%c   %c   %c   %c     28  27  26  25\n",PtC(Board[14]),PtC(Board[15]),PtC(Board[16]),PtC(Board[17]));
   printf("  %c   %c   %c   %c     24  23  22  21\n",PtC(Board[19]),PtC(Board[20]),PtC(Board[21]),PtC(Board[22]));
   printf("%c   %c   %c   %c     20  19  18  17\n",PtC(Board[23]),PtC(Board[24]),PtC(Board[25]),PtC(Board[26]));
   printf("  %c   %c   %c   %c     16  15  14  13\n",PtC(Board[28]),PtC(Board[29]),PtC(Board[30]),PtC(Board[31]));
   printf("%c   %c   %c   %c     12  11  10   9\n",PtC(Board[32]),PtC(Board[33]),PtC(Board[34]),PtC(Board[35]));
   printf("  %c   %c   %c   %c      8   7   6   5\n",PtC(Board[37]),PtC(Board[38]),PtC(Board[39]),PtC(Board[40]));
   printf("%c   %c   %c   %c     4    3   2   1\n",PtC(Board[41]),PtC(Board[42]),PtC(Board[43]),PtC(Board[44]));
   printf("    Black\n");
 }

 void
 GetHumanMove(int *Board, TPlayer Player, int *Move){
   TMoveList MoveList;
   char Aux[128];
   TMoveGenerator.MakeMoveList(Board,Player,&MoveList);
   for(int i=0;i<MoveList.NMoves;++i){
     TSearcher.MoveToStr(MoveList.List[i],Aux);
     printf("%d)%s  ",i+1,Aux);
     if(i&&(!(i%4)))
       printf("\n");
   }
   int Selection;
   do{
   printf("\n(0 exits) which move[%d..%d]?",1,MoveList.NMoves);
     scanf("%d",&Selection);      
  }while(Selection<0||Selection>MoveList.NMoves);
   if(!Selection)
     exit(0);
 TSearcher.CopyMove(Move, MoveList.List[Selection-1]);
 
}

 ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

 enum TPlayerType{HUMAN,COMPUTER};

 ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

 class TGame{
   int Board[55];
   TPlayer Turn;
   int NMove;
   char PlayerName[2][12];
   TPlayerType PlayerType[2];
   int SarchDepth[2];
 public:
   TGame();
   void Options();
   void Play();
 };

 TGame::TGame(){
  TMoveGenerator.InitBoard(Board);
   NMove=0;
   Turn= BLACK;
   strcpy(PlayerName[WHITE],"WHITE");
   strcpy(PlayerName[BLACK],"BLACK");
   PlayerType[BLACK]= HUMAN;
   PlayerType[WHITE]= COMPUTER;
   SarchDepth[BLACK]=SarchDepth[WHITE]= 5;
 }

 void TGame::Options(){
   TPlayer Player[2]={BLACK,WHITE};
   char Aux[64];
   printf("Game options\n\n");
   for( int i=0 ; i<2 ; ++i ){
     printf("%s Player=Human/Computer H/C?",PlayerName[Player[i]]);
     do{
       fgets(Aux,48,stdin); //used instead of: scanf("%s",Aux);
       *Aux= (char)toupper(*Aux);
     }while(*Aux!='H'&&*Aux!='C');
     PlayerType[Player[i]]= *Aux=='C'?COMPUTER:HUMAN;
     if(PlayerType[Player[i]]==COMPUTER){
       printf("Search depth(1-10)?");
       int Time;
       do{
         fgets(Aux,48,stdin); //used instead of scanf("%s",Aux);
         Time=atoi(Aux);
       }while(Time<1||Time>10);
       SarchDepth[Player[i]]= Time;
     }
   }
 }

 void
 TGame::Play(){
   bool EndOfGame= false;
   int Move[MAX_MOVE_LENGTH];
   char Aux[256];
   while(!EndOfGame){
     NMove++;
     ShowBoard(Board);
     printf("\n %d) %s to move\n", NMove, PlayerName[Turn]);
     if(PlayerType[Turn]==HUMAN){
       GetHumanMove(Board,Turn,Move);
       TSearcher.MoveToStr(Move,Aux);
       printf("\n%s plays %s\n",PlayerName[Turn],Aux);
     }
     else{
       TSearcher.SetSearchDepth(SarchDepth[Turn]);
       TSearcher.GetComputerMove(Board,Turn,Move);
       printf("\n%s\n", TSearcher.GetMoveInfoStr());
     }
     TMoveGenerator.MakeMove(Move,Board);
     if(TMoveGenerator.NoMovesLeft(Board,Other(Turn))){
       EndOfGame= true;
     }
     else{
       Turn= Other(Turn);
       printf("\n\nHit enter");
       fgets(Aux,128,stdin); // used instead of gets(Aux)
       printf("\n\n");
     }
   }
   printf("%s wins",PlayerName[Turn]);
 }
