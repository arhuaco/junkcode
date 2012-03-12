 /*
   Este archivo contiene una mef para
   reconocer el archivo con el fuente
   de la gramática.
   Traduce aotomaticamente a la
   representacion interna
 */

 void ErrorMsg(int x, char * Texto, int Linea, int Columna){
   printf("Error en línea %d, columna %d. caracter (0x%02x):'%c'\n%s\n",
          Linea,Columna,x,x,Texto);
 }

 /* para nombres de no terminales */
 bool EsLetra2(int x){
   return x >= 'a' && x<='z' || x>='A' && x<='Z' || x=='_'
          || x >= '0' && x <='9';
 }

 /* para los terminales */
 bool EsLetra(int x){
   if( isspace(x) || x=='\n'|| x==EOF || !isprint(x)||x=='>'||x=='<')
     return false;
   return true;
 }

 bool Gramatica::Analiza(FILE *p, int Linea, Produccion **Prod){
   char NT[64];
   int NNT;
   int x;
   int Caso= 50;
   x= fgetc(p);
   bool Error= false;
   bool Acepte= false;
   int Columna = 1;
   while( !Error && !Acepte ){
     /* printf("Caso = %d, x=%d[%c]\n", Caso, x,x); */
     /* para depurar la M.E.F. */
     switch(Caso){
       case 50:
         if( x==' ' || x== '\t') Caso= 53;
         else if( x=='<') Caso = 54;
         else if( x=='*') Caso = 51;
         else if( x==EOF || x == '\n')  Acepte=1;
         else{
           ErrorMsg(x,"Se debe iniciar con '*' ó con '<'",Linea,Columna);
           Error = true;
         }
         break;

       case 51:
       case 52:
         if( x==EOF || x== '\n')  Acepte=1;
         break;

       case 53:
         if( x==' ' || x== '\t') Caso= 53;
         else if( x=='<') Caso = 54;
         else if( x=='*') Caso = 51;
         else if( x==EOF || x== '\n')  Acepte=1;
         else{
           ErrorMsg(x,"Se debe iniciar con '*' ó con '<'",Linea,Columna);
           Error = true;
         }
         break;

       case 54:
         if( EsLetra2(x)){
            Caso= 55;
            NNT=1;
            NT[0]=(char)x;
         }
         else{
           if(x=='\n' || x== EOF){
             ErrorMsg(x,"Se esperó el nombre del no-terminal",Linea,Columna);
           }
           else{
             ErrorMsg(x,"El caracter no es permitido en el nombre de un no-terminal",Linea,Columna);
           }
           Error = true;
         }
         break;

       case 55:
         if( x=='>'){

          NT[NNT]='\0';
          /* printf("\n<%s> -> ",NT);*/ /*reconocido un N.T. */
          *Prod= new Produccion;
          int Id= BuscarSimbolo(NT);
          if(Id==-1)
            Id= CrearSimbolo(NT);
          (*Prod)->Insertar(Id);

          Caso = 56;
         }
         else if( EsLetra2(x)){
            if(NNT>=32){           
              ErrorMsg(x,"Nombre de no-terminal muy largo. Maximo 32 caracteres",Linea,Columna);
              Error = true;
            }else{
             NT[NNT++]=(char)x;
            }
            Caso = 55;
         }
         else{
           ErrorMsg(x,"Se esperó '>'",Linea,Columna);
           Error = true;
         }
         break;

       case 56:
       case 57:
         if( x==' ' || x== '\t') Caso= 57;
         else if( x=='-') Caso = 58;
         else{
           ErrorMsg(x,"Se esperó '->'",Linea,Columna);
           Error = true;
         }
         break;

       case 58:
         if( x=='>') Caso = 59;
         else{
           ErrorMsg(x,"Se esperó '>'",Linea,Columna);
           Error = true;
         }
         break;

       case 59:
       case 510:
         if( x==' ' || x== '\t') Caso= 510;
         else if( x=='<') Caso = 512;
         else if( EsLetra(x)){
           /*printf("%c",char(x));*/ /* reconocido un terminal */
           int Id= BuscarSimbolo(char(x));
           if(Id==-1)
              Id= CrearSimbolo(char(x));
           (*Prod)->Insertar(Id);

           Caso = 511;
         }
         else if( x==EOF || x== '\n')  Acepte=1;
         else{
           ErrorMsg(x,"No se esperó el caracter",Linea,Columna);
           Error = true;
         }
         break;

       case 511:
         if( x==' ' || x== '\t') Caso= 515;
         else if( x=='<') Caso = 512;
         else if( EsLetra(x)){
           // printf("%c",char(x)); reconocido un terminal
           int Id= BuscarSimbolo(char(x));
           if(Id==-1)
              Id= CrearSimbolo(char(x));
           (*Prod)->Insertar(Id);
           Caso = 511;
         }
         else if( x==EOF || x== '\n')  Acepte=1;
         else{
           ErrorMsg(x,"No se esperó el caracter",Linea,Columna);
           Error = true;
         }
         break;

       case 512:
         if( EsLetra2(x) ){
           NNT=1;
           NT[0]=(char)x;
           Caso= 513;
         }
         else{
           if(x=='\n' || x== EOF){
             ErrorMsg(x,"Se esperó el nombre del no-terminal",Linea,Columna);
           }
           else{
             ErrorMsg(x,"El caracter no es permitido en el nombre de un no-terminal",Linea,Columna);
           }
           Error = true;
         }
         break;


       case 513:
         if( x=='>'){
           NT[NNT]='\0';
           // printf("<%s>",NT); reconocido un no terminal

           int Id= BuscarSimbolo(NT);
           if(Id==-1)
              Id= CrearSimbolo(NT);
            (*Prod)->Insertar(Id);

           Caso = 514;
         }
         else if( EsLetra2(x)){
           if(NNT>=32){           
              ErrorMsg(x,"Nombre de no-terminal muy largo. Maximo 32 caracteres",Linea,Columna);
              Error = true;
            }else{
              NT[NNT++]=(char)x;
            }
           Caso = 513;
         }
         else{
           if(x=='\n' || x== EOF){
             ErrorMsg(x,"Se esperó '>'",Linea,Columna);
           }
           else{
             ErrorMsg(x,"El caracter no es permitido en el nombre de un no-terminal",Linea,Columna);
           }
           Error = true;
         }
         break;

       case 514:
         if( x==' ' || x== '\t') Caso= 515;
         else if( x=='<') Caso= 512;
         else if(EsLetra(x)){
           //printf("%c",char(x)); reconocido un terminal
           int Id= BuscarSimbolo(char(x));
           if(Id==-1)
              Id= CrearSimbolo(char(x));
            (*Prod)->Insertar(Id);
            Caso = 511;
          }
         else if( x==EOF || x== '\n')  Acepte=1;
         else{
           ErrorMsg(x,"Se esperó un simbolo o un fin de línea",Linea,Columna);
           Error = true;
         }
         break;

       case 515:
         if( x==' ' || x== '\t') Caso= 515;
         else if( x==EOF || x== '\n')  Acepte=1;
         else{
           ErrorMsg(x,"Se esperó un fin de línea",Linea,Columna);
           Error = true;
         }
         break;
     }/*switch*/

     if(!Error && !Acepte){
       x= fgetc(p);
       Columna++;
     }

   }/* while( !Error && !Acepte ) */
   /*printf("\nAccept =%d", Acepte);*/
   /*printf("\nError  =%d", Error);*/
   if(Error&&(*Prod)){
     delete *Prod;
     *Prod=0 ;
   }
   return Acepte;
 }
