
void Gramatica::Sangria(int n,FILE *stream){
  for(int i=0;i<2*n;++i){
	 fprintf(stream," ");
  }
}

void Gramatica::GeneraBloqueInicial(FILE *stream){
  /*comentario inicial*/
  Sangria(3,stream);fprintf(stream,"/*********************************************\n");
  Sangria(3,stream);fprintf(stream," Reconocedor descendente generado por %s.\n",NombrePrograma);
  Sangria(3,stream);fprintf(stream," Trosky - Copyright (c) 1999.\n",NombrePrograma);
  Sangria(3,stream);fprintf(stream," Nelson Castillo <nelsoneci@hotmail.com>\n");
  Sangria(3,stream);fprintf(stream," Eduardo Daza <edazc@hotmail.com>\n");
  Sangria(3,stream);fprintf(stream," Universidad del Magdalena\n");
  Sangria(3,stream);fprintf(stream," **********************************************/\n\n");

  /*archivos de cabecera*/
  Sangria(1,stream);fprintf(stream,"#include<stdio.h>\n");
  Sangria(1,stream);fprintf(stream,"#include<stdlib.h>\n\n");

  /*variables globales*/
  Sangria(1,stream);fprintf(stream,"int Error, Avance;\n\n");

  /*TAD REGSTRO*/
  Sangria(1,stream);fprintf(stream,"typedef struct REG{\n");
  Sangria(2,stream);fprintf(stream,"int Dato;\n");
  Sangria(2,stream);fprintf(stream,"struct REG *Liga;\n");
  Sangria(1,stream);fprintf(stream,"}REGISTRO;\n\n");
  Sangria(1,stream);fprintf(stream,"REGISTRO *Pila= 0;\n\n");

  /*funcion apilar*/
  Sangria(1,stream);fprintf(stream,"void Apilar(int Dato){\n");
  Sangria(2,stream);fprintf(stream,"REGISTRO *Aux;\n");
  Sangria(2,stream);fprintf(stream,"Aux= (REGISTRO*)malloc(sizeof(REGISTRO));\n");
  Sangria(2,stream);fprintf(stream,"if(!Aux){\n");
  Sangria(3,stream);fprintf(stream,"printf(\"\\nMEMORIA AGOTADA!\");\n");
  Sangria(3,stream);fprintf(stream,"exit(1);\n");
  Sangria(2,stream);fprintf(stream,"}\n");
  Sangria(2,stream);fprintf(stream,"Aux->Dato= Dato;\n");
  Sangria(2,stream);fprintf(stream,"Aux->Liga= Pila;\n");
  Sangria(2,stream);fprintf(stream,"Pila= Aux;\n");
  Sangria(1,stream);fprintf(stream,"}\n\n");

  /*funcion desapilar*/
  Sangria(1,stream);fprintf(stream,"void Desapilar(void){\n");
  Sangria(2,stream);fprintf(stream,"REGISTRO *Aux= Pila;\n");
  Sangria(2,stream);fprintf(stream,"Pila= Pila->Liga;\n");
  Sangria(2,stream);fprintf(stream,"free(Aux);\n");
  Sangria(1,stream);fprintf(stream,"}\n\n");

  /*funcion hayerror*/
  Sangria(1,stream);fprintf(stream,"void HayError(void){\n");
  Sangria(2,stream);fprintf(stream,"Error= 1;\n");
  Sangria(2,stream);fprintf(stream,"Avance= 0;\n");
  Sangria(2,stream);fprintf(stream,"while(Pila){\n");
  Sangria(3,stream);fprintf(stream,"Desapilar();\n");
  Sangria(2,stream);fprintf(stream,"}\n");
  Sangria(1,stream);fprintf(stream,"}\n\n");
}

void Gramatica::GeneraInicioDemaquinaDePila(FILE *stream){
  Sangria(1,stream);fprintf(stream,"void MaquinaDePila(char *Cad){\n");
  Sangria(2,stream);fprintf(stream,"char x;\n");
  Sangria(2,stream);fprintf(stream,"x= *Cad;\n");
  Sangria(2,stream);fprintf(stream,"Apilar(%d); /*Pila vacia*/\n",nSimbolos);
  Sangria(2,stream);fprintf(stream,"Apilar(0); /*Simbolo inicial de la GIC*/\n\n");

  Sangria(2,stream);fprintf(stream,"while(!Error && Pila){\n");
  Sangria(3,stream);fprintf(stream,"switch(Pila->Dato){ /* Ultimo simbolo en pila */\n\n");
  Sangria(4,stream);fprintf(stream,"case %d: /* pila vacia */\n",nSimbolos);
  Sangria(5,stream);fprintf(stream,"Desapilar();\n");
  Sangria(5,stream);fprintf(stream,"Avance= 0;\n");
  Sangria(5,stream);fprintf(stream,"break;\n\n");
}

void Gramatica::GeneraReduccionesPorProduccion(FILE *s){
  int i=0;
  for(Produccion *p= Producciones;p;p=p->GetLiga()){
	 bool Avanzar= false;
         Sangria(1,s);fprintf(s,"void Produccion_%d(void){\n",++i);
	 Sangria(2,s);fprintf(s,"Desapilar();\n");

	 Numero *Lado_d= p->GetInicio()->Liga;
	 if(Lado_d){
		int Hasta= (*this)(Lado_d->Valor)->Terminal?2:1;
		for(int j=p->GetCuantos()-1;j>=Hasta;--j){
		  if((*this)((*p)[j])->Terminal){
			 Sangria(2,s);fprintf(s,"Apilar(%d); /* %s */\n",(*p)[j], (*this)((*p)[j])->Nombre);
		  }
		  else{
			 Sangria(2,s);fprintf(s,"Apilar(%d); /* <%s> */\n",(*p)[j], (*this)((*p)[j])->Nombre);
		  }
		}
		if(Hasta==2){ // Primer simbolo del lado derecho es un terminal
		  Avanzar=true;
		}
	 }
	 Sangria(2,s);fprintf(s,"Avance= %d;\n",Avanzar?1:0);
	 Sangria(1,s);fprintf(s,"}\n\n");
  }
}


void Gramatica::GeneraRestoDeMaquinaDePila(FILE *s){
  for(int iSimb=0 ; iSimb < nSimbolos ; ++iSimb){
	 bool VaEnPila= false;
	 Simbolo *NodoSimb=(*this)(iSimb);
	 if( NodoSimb->Terminal ==false ){
	 VaEnPila= true;
	 }
	 else{
		for(Produccion *p= Producciones;p;p=p->GetLiga()){
		  for(int j=2;j<p->GetCuantos();++j){
			 if((*p)[j]==iSimb){
				VaEnPila= true;
			 }
		  }
		}
	 }
	 if(VaEnPila){

		Simbolo *SimbActual= (*this)(iSimb);
		if(SimbActual->Terminal){
		  Sangria(4,s);fprintf(s,"case %d:  /* %s */\n",iSimb, SimbActual->Nombre);
		}
		else{
		  Sangria(4,s);fprintf(s,"case %d:  /* <%s> */\n",iSimb, SimbActual->Nombre);
      }

		if(NodoSimb->Terminal==true){
		  Sangria(5,s);fprintf(s,"if(x=='%c'){\n",NodoSimb->Nombre[0]);
		  Sangria(6,s);fprintf(s,"Avance= 1;\n");
		  Sangria(6,s);fprintf(s,"Desapilar();\n");
		  Sangria(5,s);fprintf(s,"}else{\n");
		  Sangria(6,s);fprintf(s,"HayError();\n");
		  Sangria(5,s);fprintf(s,"}\n");

		}
		else{
		  int i=0;
		  Sangria(5,s);fprintf(s,"switch(x){\n");
		  for(Produccion *p= Producciones;p;p=p->GetLiga()){
			 if((*p)[0]==iSimb){
				for(Numero *n= p->GetCS();n;n=n->Liga){
				  if(n->Valor==nSimbolos){
				  Sangria(6,s);fprintf(s,"case '\\0':\n");
				  }
				  else{
					 Sangria(6,s);fprintf(s,"case '%c':\n",(*this)(n->Valor)->Nombre[0]);
				  }
				}
                                Sangria(7,s);fprintf(s,"Produccion_%d();\n",i+1);
				Sangria(7,s);fprintf(s,"break;\n");
			 }
			 ++i;
		  }
		  Sangria(6,s);fprintf(s,"default: HayError();\n");
		  Sangria(5,s);fprintf(s,"}\n");
		}
		Sangria(5,s);fprintf(s,"break;\n\n");
	 }
  }//primer for(;;)
  Sangria(3,s);fprintf(s,"}/*switch(Pila->Dato)*/\n\n");
  Sangria(3,s);fprintf(s,"if(Avance){\n");
  Sangria(4,s);fprintf(s,"x= *(++Cad);\n");
  Sangria(3,s);fprintf(s,"}\n");
  Sangria(2,s);fprintf(s,"}/*ciclo for*/\n");
  Sangria(2,s);fprintf(s,"if(Pila || x){\n");
  Sangria(3,s);fprintf(s,"HayError();\n");
  Sangria(2,s);fprintf(s,"}\n");
  Sangria(1,s);fprintf(s,"}/*Maquina de Pila*/\n\n");

  /*main*/

 Sangria(1,s);fprintf(s,"main(){\n");
 Sangria(2,s);fprintf(s,"char Aux[512];\n");
 Sangria(2,s);fprintf(s,"printf(\"\\nIngrese una secuencia nula para terminar.\\n\\n\");\n");
 Sangria(2,s);fprintf(s,"do{\n");
 Sangria(3,s);fprintf(s,"printf(\">\");\n");
 Sangria(3,s);fprintf(s,"gets(Aux);\n");
 Sangria(3,s);fprintf(s,"Error= 0;\n");
 Sangria(3,s);fprintf(s,"MaquinaDePila(Aux);\n");
 Sangria(3,s);fprintf(s,"printf(\"%%s\\n\\n\", Error?\" SECUENCIA NO VALIDA\": \" CORRECTO\");\n");
 Sangria(2,s);fprintf(s,"}while(*Aux);\n");
 // Sangria(2,s);fprintf(s,"getch();\n");
 Sangria(2,s);fprintf(s,"return 0;\n");
 Sangria(1,s);fprintf(s,"}");

}

void Gramatica::GenCod(FILE *stream){
  GeneraBloqueInicial(stream);
  GeneraReduccionesPorProduccion(stream);
  GeneraInicioDemaquinaDePila(stream);
  GeneraRestoDeMaquinaDePila(stream);
}
