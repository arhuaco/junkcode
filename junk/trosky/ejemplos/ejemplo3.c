      /*********************************************
       Reconocedor descendente generado por Trosky.
       Trosky - Copyright (c) 1999.
       Nelson Castillo <nelsoneci@hotmail.com>
       Eduardo Daza <edazc@hotmail.com>
       Universidad del Magdalena
       **********************************************/

  #include<stdio.h>
  #include<stdlib.h>

  int Error, Avance;

  typedef struct REG{
    int Dato;
    struct REG *Liga;
  }REGISTRO;

  REGISTRO *Pila= 0;

  void Apilar(int Dato){
    REGISTRO *Aux;
    Aux= (REGISTRO*)malloc(sizeof(REGISTRO));
    if(!Aux){
      printf("\nMEMORIA AGOTADA!");
      exit(1);
    }
    Aux->Dato= Dato;
    Aux->Liga= Pila;
    Pila= Aux;
  }

  void Desapilar(void){
    REGISTRO *Aux= Pila;
    Pila= Pila->Liga;
    free(Aux);
  }

  void HayError(void){
    Error= 1;
    Avance= 0;
    while(Pila){
      Desapilar();
    }
  }

  void Produccion_1(void){
    Desapilar();
    Apilar(3); /* c */
    Apilar(2); /* <c> */
    Apilar(1); /* <b> */
    Avance= 0;
  }

  void Produccion_2(void){
    Desapilar();
    Apilar(1); /* <b> */
    Apilar(5); /* <d> */
    Avance= 1;
  }

  void Produccion_3(void){
    Desapilar();
    Avance= 0;
  }

  void Produccion_4(void){
    Desapilar();
    Apilar(7); /* <e> */
    Apilar(5); /* <d> */
    Apilar(2); /* <c> */
    Avance= 1;
  }

  void Produccion_5(void){
    Desapilar();
    Apilar(1); /* <b> */
    Apilar(8); /* a */
    Apilar(5); /* <d> */
    Avance= 0;
  }

  void Produccion_6(void){
    Desapilar();
    Apilar(8); /* a */
    Avance= 1;
  }

  void Produccion_7(void){
    Desapilar();
    Avance= 0;
  }

  void Produccion_8(void){
    Desapilar();
    Apilar(5); /* <d> */
    Avance= 1;
  }

  void Produccion_9(void){
    Desapilar();
    Apilar(10); /* f */
    Apilar(0); /* <a> */
    Avance= 1;
  }

  void Produccion_10(void){
    Desapilar();
    Avance= 1;
  }

  void MaquinaDePila(char *Cad){
    char x;
    x= *Cad;
    Apilar(11); /*Pila vacia*/
    Apilar(0); /*Simbolo inicial de la GIC*/

    while(!Error && Pila){
      switch(Pila->Dato){ /* Ultimo simbolo en pila */

        case 11: /* pila vacia */
          Desapilar();
          Avance= 0;
          break;

        case 0:  /* <a> */
          switch(x){
            case 'c':
            case 'a':
            case 'd':
            case 'b':
              Produccion_1();
              break;
            case 'e':
              Produccion_2();
              break;
            default: HayError();
          }
          break;

        case 1:  /* <b> */
          switch(x){
            case 'c':
            case 'e':
            case 'a':
            case 'd':
            case 'f':
            case '\0':
              Produccion_3();
              break;
            case 'b':
              Produccion_4();
              break;
            default: HayError();
          }
          break;

        case 2:  /* <c> */
          switch(x){
            case 'a':
            case 'd':
              Produccion_5();
              break;
            case 'c':
              Produccion_6();
              break;
            default: HayError();
          }
          break;

        case 3:  /* c */
          if(x=='c'){
            Avance= 1;
            Desapilar();
          }else{
            HayError();
          }
          break;

        case 5:  /* <d> */
          switch(x){
            case 'c':
            case 'e':
            case 'b':
            case 'a':
            case 'f':
            case '\0':
              Produccion_7();
              break;
            case 'd':
              Produccion_8();
              break;
            default: HayError();
          }
          break;

        case 7:  /* <e> */
          switch(x){
            case 'e':
              Produccion_9();
              break;
            case 'c':
              Produccion_10();
              break;
            default: HayError();
          }
          break;

        case 8:  /* a */
          if(x=='a'){
            Avance= 1;
            Desapilar();
          }else{
            HayError();
          }
          break;

        case 10:  /* f */
          if(x=='f'){
            Avance= 1;
            Desapilar();
          }else{
            HayError();
          }
          break;

      }/*switch(Pila->Dato)*/

      if(Avance){
        x= *(++Cad);
      }
    }/*ciclo for*/
    if(Pila || x){
      HayError();
    }
  }/*Maquina de Pila*/

  main(){
    char Aux[512];
    printf("\nIngrese una secuencia nula para terminar.\n\n");
    do{
      printf(">");
      gets(Aux);
      Error= 0;
      MaquinaDePila(Aux);
      printf("%s\n\n", Error?" SECUENCIA NO VALIDA": " CORRECTO");
    }while(*Aux);
    return 0;
  }