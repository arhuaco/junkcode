 #include<stdio.h>
 #include<stdlib.h>
 #include<string.h>
 #include<ctype.h>

 char *NombrePrograma = "Trosky";

// Usar la siguiente declaracion
// cuando no se encuentre disponible
// el tipo de dato bool
// typedef int bool; 

 #define true  1
 #define false 0


 struct Numero{
	int Valor;
	Numero *Liga;
 };


 struct Simbolo{
        char Nombre[34];
	bool Terminal;
	bool Anulable;
	Numero *Primeros, *Siguientes;
	Simbolo *Liga;

	void InsertarPrimero(int p);
	void InsertarSiguiente(int s);
 };


 void Simbolo::InsertarPrimero(int p){
	Numero *Nuevo= new Numero;
	Nuevo->Valor= p;
	Nuevo->Liga= Primeros;
	Primeros= Nuevo;
 }

 void Simbolo::InsertarSiguiente(int s){
	Numero *Nuevo= new Numero;
	Nuevo->Valor= s;
	Nuevo->Liga= Siguientes;
	Siguientes= Nuevo;
 }


/*************************************
				CLASE Produccion
*************************************/                                

 class Produccion{
	  Numero *Inicio;
	  Numero *C_Seleccion;
	  Numero *Primeros;
	  Produccion *Liga;
	  int Cuantos;
	  bool Anulable;

	public:
	  Produccion(){
		 Inicio= C_Seleccion= Primeros= 0;
		 Liga= 0;
		 Cuantos= 0;
		 Anulable= false;
	  }
	  void Insertar(int Id);
	  void InsertarPrimero(int p);
	  void InsertarCS(int p);
	  void SetLiga(Produccion *l){ Liga= l; }
	  void SetAnulable(bool a){ Anulable= a; }


	  int GetCuantos(){ return Cuantos; }
	  Produccion *GetLiga(){ return Liga; }
	  Numero *GetInicio(){ return Inicio; }
	  Numero *GetCS(){ return C_Seleccion; }

	  int operator [](int Indice);
 };

 int Produccion::operator [](int Indice){
	Numero *p= Inicio;
	for(int i= 0; i<Indice; ++i)
	  p= p->Liga;
	return p->Valor;
 }

 void Produccion::Insertar(int Id){
	Numero *Nuevo= new Numero;
	Nuevo->Valor= Id;
	Nuevo->Liga= 0;
	Numero *p= 0, *q= Inicio;
	while(q){
	  p= q;
	  q= q->Liga;
	}
	if(!p)
	  Inicio= Nuevo;
	else
	  p->Liga= Nuevo;
	++Cuantos;
 }

 void Produccion::InsertarPrimero(int p){
	Numero *Nuevo= new Numero;
	Nuevo->Valor= p;
	Nuevo->Liga= Primeros;
	Primeros= Nuevo;
 }

 void Produccion::InsertarCS(int p){
   //se busca para no insertarlo 2 veces...
   Numero *Actual= C_Seleccion;
   while(Actual){
     if(Actual->Valor==p)
       return;
     Actual= Actual->Liga;
   }

   Numero *Nuevo= new Numero;
   Nuevo->Valor= p;
   Nuevo->Liga= C_Seleccion;
   C_Seleccion= Nuevo;
 }



/************************
     CLASE Gamatica
************************/

 class Gramatica{
	  Produccion *Producciones, *UltimaProd;
	  Simbolo *Simbolos;
	  int nProducciones, nSimbolos;

	  int **Relacion1;
	  int **Relacion2;  //La misma matriz de Relacion1.
	  int **Relacion3;  //Relacion3 no se está utilizando.
	  int **Relacion4;
	  int **Relacion5;  //La misma matriz de Relacion4.
	  int **Relacion6;

	public:

	  Gramatica(){  Producciones= 0;
						 Simbolos= 0;
						 nProducciones= 0;
						 nSimbolos= 0;
					 }

	  void Sangria(int n,FILE *stream);
	  void GeneraBloqueInicial(FILE *stream);
	  void GeneraInicioDemaquinaDePila(FILE *stream);
	  void GeneraReduccionesPorProduccion(FILE *stream);
	  void GeneraRestoDeMaquinaDePila(FILE *stream);
	  void GenCod(FILE *stream);
          bool LeerDeArchivo(FILE *p);
          bool Analiza(FILE *p, int Linea, Produccion **Prod);
          int NProducciones(){ return nProducciones; }

	  int BuscarSimbolo(char *Cad, bool Terminal);
	  int BuscarSimbolo(char c);
	  int CrearSimbolo(char *Cad, bool Terminal);
	  int CrearSimbolo(char c);

	  bool EsLL1();

	  Produccion *operator [] (int Indice);
	  Simbolo    *operator () (int Indice);

	  void ObtenerR1();
	  void ObtenerR2();
	  void ObtenerR3();
	  void ObtenerR4();
	  void ObtenerR5();
	  void ObtenerR6();

	  void Anulables();
	  void PrimerosSimb();
	  void SiguientesSimb();
	  void CSProd();

 };

 #include "relacion.h"
 #include "gencod.h"

 Produccion *Gramatica::operator [] (int Indice){
	Produccion *Actual= Producciones;
	for(int i= 0; i<Indice; ++i)
	  Actual= Actual->GetLiga();
	return Actual;
 }


 Simbolo *Gramatica::operator () (int Indice){
	Simbolo *Actual= Simbolos;
	for(int i= 0; i<Indice; ++i)
	  Actual= Actual->Liga;
	return Actual;
 }

 int
 Gramatica::BuscarSimbolo(char *Cad, bool Terminal= false){
	Simbolo *p= Simbolos;
	for(int i= 0; p; ++i){
	  if((!strcmp(Cad, p->Nombre)) && p->Terminal==Terminal)
		 return i;
	  p= p->Liga;
	}

	return -1;
 }


 int Gramatica::BuscarSimbolo(char c){
	char Cad[2]={0, 0};
	Cad[0]= c;
	return BuscarSimbolo(Cad, true);
 }


 int
 Gramatica::CrearSimbolo(char *Cad, bool Terminal= false){
	Simbolo *p= 0, *q= Simbolos;
	int Pos;
	for(Pos= 0; q; ++Pos){
	  p= q;
	  q= q->Liga;
	}

	Simbolo *Nuevo= new Simbolo;
	strcpy(Nuevo->Nombre, Cad);
	Nuevo->Terminal= Terminal;
	Nuevo->Anulable= false;
	Nuevo->Primeros= Nuevo->Siguientes= 0;
	Nuevo->Liga= 0;
	if(!p)
	  Simbolos= Nuevo;
	else
	  p->Liga= Nuevo;
   ++nSimbolos;
	return Pos;
 }


 int Gramatica::CrearSimbolo(char c){
	char Cad[2]={0, 0};
	Cad[0]= c;
	return CrearSimbolo(Cad, true);
 }



 // Define que simbolos son anulables en la gramatica

 void Gramatica::Anulables(){


	/* Primero se determinan los simbolos anulables */
	bool HuboCambio= true;
	while(HuboCambio){
	  HuboCambio= false;
	  Produccion *ActualProd= Producciones;
	  while(ActualProd){
		 Numero *ActualNum= ActualProd->GetInicio();
		 if(((*this)(ActualNum->Valor))->Anulable==true){
			ActualProd= ActualProd->GetLiga();
			continue;
		 }

		 //Se comienza desde el primero del lado derecho...
		 ActualNum= ActualNum->Liga;
		 for( ; ActualNum; ActualNum= ActualNum->Liga){
			if( ((*this)(ActualNum->Valor)->Anulable)!=true )
			  break;
		 }


		 if(!ActualNum){//Todo el lado derecho es anulable?
                        //El simbolo del lado izquierdo tambien es anulable
			(*this)(ActualProd->GetInicio()->Valor)->Anulable= true;
			HuboCambio= true;
		 }

		 ActualProd= ActualProd->GetLiga();
	  }
	}

	/* Ahora a determinar producciones anulables */
	Produccion *ActualProd= Producciones;
	while(ActualProd){
	  Numero *n= ActualProd->GetInicio();
	  while(n){
		 if((*this)(n->Valor)->Anulable==false)
			break;
    	 n= n->Liga;
	  }
	  if(n==0)
		 ActualProd->SetAnulable(true);
	  ActualProd= ActualProd->GetLiga();
	}
 }

 void Gramatica::PrimerosSimb(){
	for(int i= 0; i<nSimbolos; ++i){
	  for(int j= 0; j< nSimbolos; ++j)
		 if(Relacion2[i][j])
			(*this)(i)->InsertarPrimero(j);
	}
 }


 void Gramatica::SiguientesSimb(){
	for(int i= 0; i<nSimbolos; ++i){
	  for(int j= 0; j<=nSimbolos; ++j)
		 if(Relacion6[i][j])
			(*this)(i)->InsertarSiguiente(j);
	}
 }


 void Gramatica::CSProd(){
	Produccion *p= Producciones;
	while(p){
	  Numero *n= p->GetInicio()->Liga;;
	  //se agregan los primeros hasta encontrar un no anulable
	  while(n){
		 Numero *n2= (*this)(n->Valor)->Primeros;
		 while(n2){
			if((*this)(n2->Valor)->Terminal==true)
			  p->InsertarCS(n2->Valor);
			n2= n2->Liga;
		 }
		 if((*this)(n->Valor)->Anulable==false)
			break;
		 n= n->Liga;
	  }

	  //si no se encontro un no anulable se agregan tambien
	  //los siguientes del simbolo de la izquierda

	  if(n==0){
		 int LIzq= p->GetInicio()->Valor;
		 Numero *n2= (*this)(LIzq)->Siguientes;
		 while(n2){
			if(n2->Valor==nSimbolos){
			  p->InsertarCS(n2->Valor);
			}
         else if((*this)(n2->Valor)->Terminal==true)
			  p->InsertarCS(n2->Valor);
			n2= n2->Liga;
		 }
	  }

	  //se avanza a la siguiente produccion
	  p= p->GetLiga();
	}
 }


 bool Gramatica::EsLL1(){
	int cProd0, cProd1;

	Produccion *p= Producciones;
	cProd0= 0;
	while(p){
	  ++cProd0;
	  Produccion *q= p->GetLiga();
	  cProd1= cProd0;
	  while(q){
       ++cProd1;
		 if(q==p || (q->GetInicio()->Valor!=p->GetInicio()->Valor)){
			q= q->GetLiga();
			continue;
		 }

		 for(Numero *n1= p->GetCS(); n1; n1= n1->Liga)
			for(Numero *n2= q->GetCS(); n2; n2= n2->Liga)
			  if(n1->Valor==n2->Valor){
				 printf("Las producciones %d y %d tienen el mismo lado izquierdo (<%s>) y\n", cProd0, cProd1,(*this)(q->GetInicio()->Valor)->Nombre);
				 printf("sus conjuntos de seleccion no son disyuntos\n");
				 return false;
			  }
		 q= q->GetLiga();
	  }
	  p= p->GetLiga();
	}
	return true;
 }


 #include "entrada.h"


 bool Gramatica::LeerDeArchivo(FILE *p){
   Produccion *Nueva;
   bool Error= false;
   int Linea=1;
   while(!feof(p)&&!Error){
     Nueva=0;
     Error= !Analiza(p,Linea++,&Nueva);
     if(Nueva){
       Nueva->SetLiga(0);
       if(!Producciones)
         Producciones= Nueva;
       else
         UltimaProd->SetLiga(Nueva);
       UltimaProd= Nueva;
       ++nProducciones;
     }
   }
   return Error;
 }


 main( int argc, char *argv[]){
   if(argc!=3 && argc!=2){
     printf(" uso: %s fuente [destino]\n",NombrePrograma);
     exit(1);
   }

   FILE *p;
   p= fopen(argv[1],"r" );

   if(!p){
     printf("No se pudo leer el archivo '%s'\n",argv[1]);
     exit(1);
   }

   Gramatica G;
   bool Error= G.LeerDeArchivo(p);
   fclose(p);

   if(Error){
     exit(1);
   }

   if(G.NProducciones()==0){
     printf("El archivo '%s' no contiene producciones\n",argv[1]);
     exit(1);
   }

   G.Anulables();
   G.ObtenerR1();
   G.ObtenerR2();
   G.ObtenerR4();
   G.ObtenerR5();
   G.ObtenerR6();

   G.PrimerosSimb();
   G.SiguientesSimb();

   G.CSProd();  //Codigos de seleccion

   if(!G.EsLL1()){
     printf("\nError: La gram tica no es LL(1)\n");
     exit(1);
   }

   if(argc==3){
     p= fopen(argv[2],"w" );
     if(!p){
       printf("No se pudo escribir el archivo '%s'\n",argv[2]);
       exit(1);
      }
     G.GenCod(p);
     fclose(p);
   }
   else{
     G.GenCod(stdout);
   }
   return 0;
 }
