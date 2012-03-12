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
    Apilar(1); /* 1 */
    Apilar(0); /* <s> */
    Avance= 1;
  }

  void Produccion_2(void){
    Desapilar();
    Apilar(2); /* 0 */
    Apilar(0); /* <s> */
    Avance= 1;
  }

  void Produccion_3(void){
    Desapilar();
    Avance= 1;
  }

  void MaquinaDePila(char *Cad){
    char x;
    x= *Cad;
    Apilar(4); /*Pila vacia*/
    Apilar(0); /*Simbolo inicial de la GIC*/

    while(!Error && Pila){
      switch(Pila->Dato){ /* Ultimo simbolo en pila */

        case 4: /* pila vacia */
          Desapilar();
          Avance= 0;
          break;

        case 0:  /* <s> */
          switch(x){
            case '1':
              Produccion_1();
              break;
            case '0':
              Produccion_2();
              break;
            case 'm':
              Produccion_3();
              break;
            default: HayError();
          }
          break;

        case 1:  /* 1 */
          if(x=='1'){
            Avance= 1;
            Desapilar();
          }else{
            HayError();
          }
          break;

        case 2:  /* 0 */
          if(x=='0'){
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