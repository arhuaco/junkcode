{
  Update: 2010-03-07
  Let's save this old source code I improved in 1996.
}

{
nelsoneci@gmail.com
}

{
I copied this source from an old book, I only adapted it for a newer pascal
version and did the graphic interface.
It was originally written for an Apple computer using Apple-Pascal.
}

{
Este cÛdigo lo copiÈ de un libro de los 70's:
Pascal Avanzado, TÈcnicas de programaciÛn
AdaptÈ el codigo a una versiÛn m·s nueva de pascal e hice la parte gr·fica.
Originalmente fue escrito para un computador Apple usando Apple-pascal.
}



{ nelson castillo Agosto 8 / 1996 }

{$s+}
program revesi;
 { El juego de revesi }
uses crt, dos,Graph;

type
 contents = (LIGHT,DARK,EMPTY,BORDER) ;
 player  = LIGHT..DARK ;
 squarenum = 0..99;
 charset = set of char;
const
 MAXMOVES = 60;
 MAXCRTCOL = 80;
 STX =120 ;
 STY =30 ;
 ANCHO =50;
 type
  lnk80 = string [80] ;
  movelist = record
          nmoves : 0..MAXMOVES;
          move   : array [ 1..MAXMOVES ] of squarenum
  end;

  board = record
      sq:        array [ squarenum ] of contents;
      ndiscs:    array [ player ]    of integer;
      possible:  movelist;
  end;

 direction = ( NORTH, NORTHEAST, EAST, SOUTHEAST,
               SOUTH, SOUTHWEST, WEST, NORTHWEST);

  var
   accept, fwdkey, backey, ch : char;
   delta :  array [ direction ] of integer;
   sqord :  array [ squarenum ] of integer;
   sqcolor: array [ contents  ] of integer;
   sqchar:  array [ contents  ] of char;
   corner, poison1, good1 : array [ 1..4 ] of squarenum;
   poison2, good2 : array [ 1..4 , 1..2 ]  of squarenum;
   edge  : array [1..4,1..4] of squarenum ;
   xmarg,ymarg : integer;

   {m¢dulos}

{**********************************************************************}
procedure IniciaSistemaGrafico;

var
 grDriver: Integer;
 grMode: Integer;
 ErrCode: Integer;
begin {IniciaSistemaGrafico}
 grDriver := Detect;
 InitGraph(grDriver, grMode,' ');
 ErrCode := GraphResult;
 if ErrCode = grOk then
 begin  { Do graphics }
 end
 else begin
   clrscr;
   Writeln('Graphics error:', GraphErrorMsg(ErrCode));
   halt
 end
end;



 {**************}
   procedure initrev;
   {Inicializa las variables globales de revesi}
   procedure initrev1 ;
   {inicializa las variables globales:parte 1}
    var
    i , j , sv : integer ;
   begin { initrev1 }
     accept := chr(13) ;
     backey := '-' ;
     fwdkey := '+' ;
     xmarg := ( MAXCRTCOL -32 ) div 2 ;
     ymarg := 5 ;
     sqchar [DARK]  := 'B' ;
     sqchar [LIGHT] := 'W' ;
     sqchar [EMPTY] := ' ' ;
     sqchar [BORDER]:= '*' ;
     sqcolor[DARK]  := 8;
     sqcolor[LIGHT] := 7;
     sqcolor[EMPTY] := 0;
     sqcolor[BORDER]:= 4;
     sqord[11]:=1; sqord[12]:=7;sqord[13]:=2;sqord[14]:=2;
                   sqord[22]:=8;sqord[23]:=6;sqord[24]:=5;
                                sqord[33]:=3;sqord[34]:=4;
                                             sqord[44]:=0;
     for j := 1 to 4 do
         for i:= j to 4 do begin
             sv := sqord[ 10*j +i ];
                sqord[ 10 * i + j ] := sv;
                sqord[ 10 * (9 - i) + j ] := sv;
                sqord[ 10 * (9 - j) + i ] := sv;
                sqord[ 10 * j + 9 - i ]   := sv;
                sqord[ 10 * i + 9 - j ]   := sv;
                sqord[ 10 * (9 - i) + 9 - j ]:= sv;
                sqord[ 10 * (9 - j) + 9 - i ]:= sv;
         end;
     delta [ NORTH ]     := -10;
     delta [ NORTHEAST ] :=  -9;
     delta [ EAST ]      :=   1;
     delta [ SOUTHEAST ] :=  11;
     delta [ SOUTH ]     :=  10;
     delta [ SOUTHWEST ] :=   9;
     delta [ WEST ]      :=  -1;
     delta [ NORTHWEST ] := -11;
 end;

 procedure initrev2;
 {Inicializa las variables globales: parte 2}
 var i: integer;
 begin {initev2}

 corner[1]     :=11 ;   poison2[1,1] :=12;  good2[1,1]:=13 ;
 poison2[1,2]  :=21 ;   poison1[1]    :=22;
 good2[1,2]    :=31 ;                       good1[1]  :=33 ;

 corner[2]     :=18 ;   poison2[2,1] :=17;  good2[2,1]:=16 ;
 poison2[2,2]  :=28 ;   poison1[2]    :=27;
 good2[2,2]    :=38 ;                       good1[2]  :=36 ;

 corner[3]     :=81 ;   poison2[3,1] :=82;  good2[3,1]:=83 ;
 poison2[3,2]  :=71 ;   poison1[3]   :=72;
 good2[3,2]    :=61 ;                       good1[3]  :=63 ;

 corner[4]     :=88 ;   poison2[4,1] :=87;  good2[4,1]:=86 ;
 poison2[4,2]  :=78 ;   poison1[4]   :=77;
 good2[4,2]    :=68 ;                       good1[4]  :=66 ;

 for i:=1 to 4 do begin
     edge[1,i]:=12+i;
     edge[2,i]:=28+10*i;
     edge[3,i]:=21+10*i;
     edge[4,i]:=82+i;
   end;
end;
begin { initrev }
      initrev1;
      initrev2;
end;

  Procedure DimCursor ( Dim : Char ) ;
    Var
        Regs    : Registers ;
        I       : Integer   ;
        Tvc     : Boolean   ;
    Begin
           {Tvc := CrtColor ;}
           Dim := UpCase(Dim) ;
           Case Tvc Of
       True  : I := 0 ;
       False : I := 6
            End;
       Regs.AH := $01 ;
       Case Dim Of
    'S' : Begin
               Regs.CH := $20 ; Regs.CL := $20
          End;
    'G' : Begin
               Regs.CH := $0 ; Regs.CL := $7+I
          End;
    'P' : Begin
               Regs.CH := $6+I ; Regs.CL := $7+I
          End
              End ;
           Intr ( $10, Regs ) ;
    End ;








{**********************************************************************}
{ Nelson Start }
procedure GCenter ( texto : lnk80; yorg: integer );
 var x:integer;
 begin{ center }
    x:=textheight(texto);
    outtextxy(320-x*16,yorg,texto);
 end;

procedure Center ( texto : lnk80; yorg: integer );
 begin{ center }
    gotoxy(41 - Length(texto) div 2, yorg );
    write(texto);
 end;

procedure eraseline ( linea : integer );
 begin{eraseline}
 end;

function getkey ( var ch: char; valid:charset; shiftlock:boolean): char ;
{obtiene una tecla pulsada sin eco}
 var
  ok: boolean;
 begin{getkey}
              repeat
                ch := readkey;
                if shiftlock and ( ch in ['a'..'z']) then
                   ch := chr( ord(ch) - 32 );
                ok := ch in valid;
                if not ok then begin
                   sound(100);
                   delay(50);
                   nosound;
                end
              until ok;
              getkey := ch;
end;



{FIN Nelson}
{**********************************************************************}

procedure dispgrid;
{ presenta la red del tablero }

procedure square(x,y:integer);
 begin{square}
   rectangle(x+1,y+1,x+49,y+49);
   rectangle(x+2,y+2,x+49,y+49)
 end;
var
   i,j:integer;

   begin {dispgrid}
   setcolor(1);
   for i:=0 to 7 do
     for j:=0 to 7 do
       square(STX+50*i,sty+50*j);
   rectangle(STX-2,STY-2,STX+ANCHO*8+2,sty+ANCHO*8+2);
   line(STX+1,STY+ANCHO*8+3,STX+ANCHO*8+3,STY+ANCHO*8+3);
   line(STX+ANCHO*8+3,STY+1,STX+ANCHO*8+3,STY+ANCHO*8+2);

end;


{************************************************************************}

procedure playgame ;
{realiza un juego}
var
 mainboard     : board ;
 list          : movelist ;
 gameover,moved: boolean ;
 computer, human, currentplayer : player ;
 lookahead     : integer ;
 k             : squarenum ;

 {desde aqu°}

   {***********}
 procedure dispsquare( k:squarenum ; c: contents );
 {presenta los contenidos de los cuadros en la pantalla}
 procedure dispdisco(x,y:integer;disccolor:integer);

 const
  discsize = 15;
 var
  colorborder:integer;
  i : integer;
 begin{dispdisco}
   if (disccolor = 7) then
     colorborder:=2
   else
     colorborder:=7;
  if c = EMPTY then
     colorborder:=0;

   setcolor(colorborder);
   circle(x,y,discsize);
   circle(x,y,discsize-1);
   i:=discsize-2;
   setcolor(disccolor);
   while not i<=0 do begin
    circle(x,y,i);
    dec(i);
  end;
end;



 begin{dispsquare}

                  dispdisco(STX + 50*(k mod 10 -1) + 26,
                         STY + 50*(k div 10 -1) + 26 ,sqcolor[c]) ;

    {              textcolor(sqcolor[c]);
                  write(sqchar[c]);
                  gotoxy(1,1);}
 end;

   {***********}

 procedure setsquare ( k : squarenum; c : contents );
 {pone piezas en los cuadros}
 begin{setsquare}
      mainboard.sq[k] := c;
      dispsquare(k,c);
 end;

  {*********}
 function other ( pl:player ) : player;
 {devuelve el color del otro jugador}
 begin{other}
    if pl = LIGHT then
       other := DARK
    else
       other := LIGHT;
 end;


 {***************}
procedure dispscore;
{presenta la puntuaci¢n actual}
var
 s: string;
begin {dispscore}
 setcolor(0);
 outtextxy(195,10,'€€');
 outtextxy(505,10,'€€');
 setcolor(7);
 with mainboard do begin
    Str(ndiscs[computer]:2,s);
    outtextxy(195,10,s);
    Str(ndiscs[human]:2,s);
    outtextxy(505,10,s)
  end
end;


function flankling ( k:squarenum; dir:direction;var bd:board;pl:player):boolean;
{verifica si el jugador flanquea al oponente desde un cuadro y una condiciones dadas}

var
  ok:boolean;
  opponent:player;
  del:integer;
begin{flanking}
 ok:=FALSE;
 opponent:=other(pl);
 del := delta[dir];
 k := k+del;
 with bd do
   if sq[k] = opponent then begin
      repeat
        k:= k + del;
      until sq[k] <> opponent;
      ok:=(sq[k]=pl)
 end;
 flankling := ok;
end;






function makelist ( var legal :movelist ; pl:player ; var bd:board):integer;
{obtiene una lista de movimientos permitidos,
devuelve el numero de movimientos permitidos}

var
 i: integer;
 function legalmove ( k:squarenum ; var bd:board ; pl:player):boolean;
 {comprueba si un movimiento est† permitido}
  var
    ok : boolean;
    dir : direction ;
  begin{legalmove}
    dir := NORTH;
    ok := flankling (k, dir, bd, pl);
    while (dir <> NORTHWEST) and (not ok)   do begin
        dir := succ(dir);
        ok := flankling (k, dir, bd, pl);
    end;
    legalmove := ok ;
end;

begin{makelist}
   legal.nmoves := 0;
   with bd.possible do
     for i:= 1 to nmoves do
       if legalmove(move[i],bd,pl) then begin
         legal.nmoves := legal.nmoves+1;
         legal.move[legal.nmoves]:=move[i];
       end;
   makelist := legal.nmoves;
end;


  procedure initgame;
 {inicializa las variables del juego}

   var
     i, j : integer ;
     ch   : char ;
 begin{initgame}
        { eraseline ( ymarg +17 ) ;}
         with mainboard do begin
              for i:=0 to 9 do begin
                  sq[i]      := BORDER ;
                  sq[i+90]   := BORDER ;
                  sq[10*i]   := BORDER ;
                  sq[10*i+9] := BORDER ;
              end;
              ndiscs[LIGHT] := 2;
              ndiscs[DARK]  := 2;
              with possible do begin
                    nmoves := 12;
                    move[ 1]:= 33 ;
                    move[ 2]:= 34 ;
                    move[ 3]:= 35 ;
                    move[ 4]:= 36 ;
                    move[ 5]:= 43 ;
                    move[ 6]:= 46 ;
                    move[ 7]:= 53 ;
                    move[ 8]:= 56 ;
                    move[ 9]:= 63 ;
                    move[10]:= 64 ;
                    move[11]:= 65 ;
                    move[12]:= 66
              end
         end;
         for i:= 1 to 8 do
             for j:= 1 to 8 do
                 setsquare( 10*i + j , EMPTY );
         setsquare ( 44 , LIGHT );
         setsquare ( 55 , LIGHT );
         setsquare ( 45 , DARK  );
         setsquare ( 54 , DARK  );
         setcolor(15);
         outtextxy(190,ymarg+450,'Do you want white or black? (W/B):');
         case getkey ( ch , ['W','B'], TRUE ) of
              'W' : begin
                  human := LIGHT;
                  gotoxy(xmarg,ymarg+17);write('Computer is black')
                  end;
              'B' : begin
                  human := DARK;
                  gotoxy(xmarg,ymarg+17);write('Computer is white')
         end;
         end;
         setcolor(0);
         outtextxy(190,ymarg+450,'Do you want white or black? (W/B):');
         setcolor(15);
         outtextxy(185,ymarg+450,'Enter lookahead for computer (1-6):');
         lookahead := ord(getkey(ch,['1'..'6'],FALSE)) - 48;
         setcolor(0);
         outtextxy(185,ymarg+450,'Enter lookahead for computer (1-6):');
         setcolor(0);
         outtextxy(20,ymarg+450,'Lookahead:'+'€');
         setcolor(7);
         outtextxy(20,ymarg+450,'Lookahead:'+ch);
         textcolor(9);
         outtextxy(121,10,'Computer:');
         outtextxy(456,10,'Human:');
         computer := other ( human );
 end;
 {/****************/}

procedure addmove ( k:squarenum ; var list:movelist);
{a§ade un movimiento a la lista a menos de que ya estÇ presente}
 var
   i:integer;
begin{addmove}
 with list do begin
  move[nmoves+1] := k;
  i := 1 ;
  while move[i] <> k do
    inc(i);
  if i = nmoves+1 then
     inc(nmoves)
  end
end;


procedure delmove ( k:squarenum ; var list:movelist);
{suprime un movimiento de la lista}
 var
   i:integer;
begin{delmove}
 with list do begin
  move[nmoves+1] := k;
  i := 1 ;
  while move[i] <> k do
    inc(i);
   if i < nmoves+1 then begin
      while i <= nmoves -1 do begin
        move[i] := move[i+1];
        inc(i);
      end;
      dec(nmoves) ;
   end
  end
end;










{**************}

function getmove(var list:movelist ; pl:player ) : squarenum ;
{obtiene el movimiento del jugador actual}

function gethuman( var list:movelist ) : squarenum ;
{ obtiene el movimiento del humano }
var
  i   :integer ;
  ch  :char ;
  ok  :boolean;
begin {gethuman}

   i:=1;
   with list do begin
       repeat
           dispsquare ( move[i] , BORDER );
           while keypressed do
              readkey;
           ok:=FALSE;
           repeat
             ch:=readkey;
             if (ch= chr(0)) or (ch=accept) then
               if ch= chr(0) then
                 ch:=readkey;
               if ch=chr(72) then begin
                  ch:=backey;
                  ok:=TRUE
               end
               else if ch=chr(80) then begin
                  ch:=fwdkey;
                  ok:=true
               end
               else if not (ch=accept) then begin
                 sound(115);
                 delay(200);
                 nosound
               end
               else
                  ok:=TRUE;
           until ok;
           dispsquare ( move[i] , EMPTY );
           if ch = backey then begin
              dec(i);
              if i<1 then
                 i:=nmoves
           end
           else if ch=fwdkey then begin
             inc(i);
             if i>nmoves then
                i:=1;
           end
      until ch=accept;
      gethuman:=move[i];
   end;
end;


function getcomputer(var list:movelist) : squarenum;
{obtiene el movimiento de la computadora}
var
  max  :integer;
  best :squarenum;

procedure trymove(trysq:squarenum; pl:player ; var bd:board );
{hace un movimiento,autualiza el tablero}
var
  dir      : direction;
  k1       :squarenum;
  opp :player;
  del      :integer;
begin{tryemove}
   opp := other(pl);
   with bd do begin
       sq[trysq]:=pl;
       inc(ndiscs[pl]);
        delmove(trysq,possible);
        for dir:= NORTH to NORTHWEST do begin
           del := delta[dir];
           if flankling(trysq,dir,bd,pl) then begin
            k1:= trysq + del ;
              repeat
                sq[k1]:=pl;
                inc(ndiscs[pl]);
                dec(ndiscs[opp]);
                k1:=k1+del;
              until sq[k1]=pl
           end
           else if sq[trysq+del]=EMPTY then
               addmove(trysq+del,possible)
           end
      end
end;

function eval( var bd:board ; pl:player ):integer;
{evalua una situaci¢n en el tablero}
const
  K1 = 1;       {factor peso para la ventaja en discon}
  K2 = 3;       {factor peso para la movilidad}
  K3 = 200;     {puntuaci¢n para el poseedor de la esquina}
  K4 = -100;    {penalizaci¢n para los cuadros poison1 }
  K5 = 50;      {puntuaci¢n para los cuadros good1}
  K6 = -25;     {penalizaci¢n para los cuadros  poison2 }
  K7 = 15;      {puntuaci¢n para los cuadros good1 }
  K8 = 10;      {puntuaci¢n portener discos en las ar°stas}
  K9 = 20;      {puntuaci¢n por ocupar una ar°sta }

var
 list      : movelist;
 i,j,score : integer;
 c         : contents;
 sideset   : set of contents;

begin{eval}
  with bd do
    if ndiscs[human]=0 then {humano destruido}
       eval:=MAXINT
    else if ndiscs[computer]=0 then {computadora destruida}
       eval:=-MAXINT
    else begin
      score := K1*(ndiscs[computer]-ndiscs[human]);
      if pl=computer then
         inc(score,K2*makelist(list,pl,bd))
      else
         dec(score,K2*makelist(list,pl,bd));
      for i:=1 to 4 do begin
        c:=sq[corner[i]];
        if c= computer then
          inc(score,K3)
        else if c=human then
          dec(score,K3)
        else begin {corner empty , check poison squares }
          c:=sq[poison1[i]];
          if c=computer then
            inc(score,K4)
          else if c=human then
            dec(score,K4)
          else begin
            c:=sq[good1[i]];
            if c=computer then
               inc(score,K5)
            else if c=human then
               dec(score,K5)
          end;
          for j:= 1 to 2 do begin
           c:=sq[poison2[i,j]];
           if c=computer then
             inc(score,K6)
           else if c=human then
             dec(score,K6)
           else begin
             c:=sq[good2[i,j]];
             if c=computer then
               inc(score,K7)
             else if c=human then
               dec(score,K7)
             end
           end
          end
        end;
        for i:=1 to 4 do begin
          sideset :=[];
          for j:=1 to 4 do
          sideset:=sideset+[sq[edge[i,j]]] ;
          if sideset = [computer] then
            inc(score,K9)
          else if sideset = [computer,EMPTY] then
            inc(score,K8)
          else if sideset = [human,EMPTY] then
            dec(score,K8)
          else if sideset = [human] then
            dec(score,K9)
        end;
      eval:=score
   end
end;

procedure sortlist(var list:movelist);
{ordena la lista para poner los movimientos buenos primero}

var
 i, j, jg, gap, k : integer;

 begin{sortlist}
   with list do begin
        gap := nmoves div 2;
        while gap>0 do begin
              for i:=gap+1 to nmoves do begin
                j:= i-gap;
                while j>0 do begin
                   jg := j+gap;
                      if sqord[move[j]] <= sqord[move[jg]] then
                          j:=0
                      else begin
                         k := move[j];
                         move[j] := move[jg];
                         move[jg] := k;
                      end;
                      dec(j,gap)
                   end
                end;
                gap := gap div 2
              end
        end
   end;



function findmin (look:integer; var list:movelist ; var bd:board ;
                 cutoff:integer; var bestmove:squarenum):integer;forward;

function findmax (look:integer; var list:movelist ; var bd:board ;
                 cutoff:integer; var bestmove:squarenum):integer;
{obtiene el mejor movimiento de la computadora}
var
   newlist:movelist;
   newbd  :board;
   i, maxscore, score, nm:integer;
   junk   :squarenum;
begin{findmax}
 sortlist(list);
 with list do
    if nmoves>0 then begin
       maxscore := -MAXINT;
       i:=1;
       repeat
             newbd := bd;
             trymove(move[i], computer, newbd );
             if look <= 1 then
                score := eval(newbd,human)
             else begin
                nm := makelist(newlist,human,newbd);
                score := findmin( look-1,newlist,newbd,maxscore,junk)
             end;
             if score > maxscore then begin
                maxscore := score;
                bestmove := move[i];
             end;
             inc(i);
       until (i>nmoves) or (maxscore>=cutoff)
    end
    else begin{no legal move}
       if look<=1 then
          maxscore := eval(bd,human)
       else begin
          nm :=makelist(newlist,human,bd);
          maxscore := findmin ( look-1,newlist,bd,-MAXINT,junk)
       end
    end;
 findmax := maxscore;
end;


function findmin ;{ (look:integer; var list:movelist ; var bd:board ;
                 cutoff:integer; var bestmove:squarenum):integer;}
{obtiene el mejor movimiento de la computadora}
var
   newlist:movelist;
   newbd  :board;
   i, minscore, score, nm:integer;
   junk   :squarenum;
begin{findmin}
 sortlist(list);
 with list do
    if nmoves>0 then begin
       minscore := MAXINT;
       i:=1;
       repeat
             newbd := bd;
             trymove(move[i], human, newbd );
             if look <= 1 then
                score := eval(newbd,computer)
             else begin
                nm := makelist(newlist,computer,newbd);
                score := findmax( look-1,newlist,newbd,minscore,junk)
             end;
             if score < minscore then begin
                minscore := score;
                bestmove := move[i];
             end;
             inc(i);
       until (i>nmoves) or (minscore<=cutoff)
    end
    else begin{no legal move}
       if look<=1 then
          minscore := eval(bd,computer)
       else begin
          nm :=makelist(newlist,computer,bd);
          minscore := findmax ( look-1,newlist,bd,MAXINT,junk)
       end
    end;
 findmin := minscore;
end;



begin{getcomputer}
 if list.nmoves =1 then { only one legal move }
     getcomputer := list.move[1]
 else begin
     max := findmax(lookahead, list, mainboard, MAXINT, best);
     getcomputer := best;
     end
end;





begin{getmove}
 if pl=computer then
   getmove := getcomputer(list)
 else
   getmove := gethuman(list)
end;

procedure declarewinner ;
{ QuiÇn gan¢ ? }

var
 diff  :integer;
 s:string;

begin{declarewinner}
 with mainboard do
      diff := ndiscs[computer]-ndiscs[human];
      setcolor(15);
 if diff > 0 then begin
  str(diff,s);
  outtextxy(275,ymarg+450, 'I won by '+s)
 end
 else if diff<0 then begin
  str(-diff,s);
  outtextxy(275,ymarg+450,'You won by '+s)
 end
 else
  outtextxy(275,ymarg+450,'We have tied !')
end;

procedure makemove (k:squarenum ; pl:player );
{hace un movimiento sobre la pantalla}
var
  dir      : direction;
  k1       :squarenum;
  opponent :player;
  del      :integer;
begin{makemove}
   setsquare(k,pl);
   opponent := other(pl);
   with mainboard do begin
        inc(ndiscs[pl]);
        delmove(k,possible);
        for dir:= NORTH to NORTHWEST do begin
           del := delta[dir];
           if flankling(k,dir,mainboard,pl) then begin
            k1:= k + del ;
              repeat
                setsquare(k1,pl);
                delay(270);
                inc(ndiscs[pl]);
                dec(ndiscs[opponent]);
                k1:=k1+del;
              until sq[k1]=pl
           end
           else if sq[k+del]=EMPTY then
               addmove(k+del,possible)
           end

       end
 end;
 { hasta aqu°}

 var
  i:integer;

 begin {playgame}
       initgame;
       currentplayer := DARK ;
       gameover      := FALSE;
       moved         := TRUE ;

       repeat
             dispscore;
             if makelist(list,currentplayer,mainboard) >0 then begin
                moved := TRUE ;
                k := getmove(list,currentplayer);
        {nelson}if currentplayer =computer then begin
                   sound(115);
                   delay(200);
                   nosound
                end;
                makemove(k,currentplayer);
             end
             else if moved then
                moved := FALSE
             else { Neither player able to move}
                gameover := TRUE;
             currentplayer := other(currentplayer)
       until gameover;
       declarewinner;
end;{playgame}

begin { main }
IniciaSistemaGrafico;
setbkcolor(0);
initrev;
SetTextStyle(DefaultFont, HorizDir, 1);
cleardevice;
{DimCursor('P');}
dispgrid;
repeat
   playgame;
   setcolor(15);
   outtextxy(500,470,'play again (Y/N):');
   ch:=getkey(ch,['Y','N'],TRUE);
   setcolor(0);
   outtextxy(500,470,'play again (Y/N):');
   outtextxy(275,ymarg+450, '€€€€€€€€€€€€€€€€')
until ch='N';
CloseGraph;
textbackground(0);
textcolor(7);
clrscr;
write('Nelson 1996');
end. { fin main }
