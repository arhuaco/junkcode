/* Este archivo contiene las funciones que obtienen las
	diferentes matrices de relaciones entre los símbolos de
	la gramática que se utilizarán para hallar los CS.

	Relacion1: COMIENZA_DIRECTAMENTE_CON
	Relacion2: COMIENZA_CON
	Relacion3: ES_SEGUIDO_DIRECTAMENTE_POR (no se utiliza)
	Relacion4: ES_FIN_DIRECTO_DE
	Relacion5: ES_FIN_DE
	Relacion6: ES_SEGUIDO_POR
*/


 int **ObtenerMatrizNula(int Filas, int Columnas){
	int **Matriz= new int* [Filas];
	for(int i= 0; i<Filas; ++i){
	  Matriz[i]= new int[Columnas];
	  for(int j= 0; j<Columnas; ++j)
		 Matriz[i][j]= 0;
	}
	return Matriz;
 }


 void Gramatica::ObtenerR1(){
	Relacion1= ObtenerMatrizNula(nSimbolos, nSimbolos);

	Produccion *p= Producciones;
	while(p){
	  int Indice1= p->GetInicio()->Valor;
	  for(int i= 1; i<p->GetCuantos(); ++i){
		 int Indice2= (*p)[i];
		 Relacion1[Indice1][Indice2]= 1;
		 if((*this)(Indice2)->Anulable==false)
			break;
	  }
	  p= p->GetLiga();
	}
 }



 void Gramatica::ObtenerR2(){
	Relacion2= Relacion1;  //Se parte de la matriz anterior

	//Diagonal principal
	for(int i= 0; i<nSimbolos; ++i)
	  Relacion1[i][i]= (Relacion1[i][i])?1:2;

	bool HuboCambio= true;
	while(HuboCambio){
	  HuboCambio= false;
	  for(int i= 0; i<nSimbolos; ++i){ //avanzar de fila en fila
		 for(int j= 0; j<nSimbolos; ++j){//avanzar de col en col
			if(Relacion1[i][j]){ //traerse la fila j
			  for(int k= 0; k<nSimbolos; ++k)
				 if(Relacion1[j][k] && !Relacion1[i][k]){
					  Relacion1[i][k]= 2;
					  HuboCambio= true;
				 }
			}
		 }//for2
	  }//for1
	}//while
 }


 void Gramatica::ObtenerR3(){

	Relacion3= ObtenerMatrizNula(nSimbolos, nSimbolos);

	Produccion *p= Producciones;
	while(p){
	  Numero *n1= p->GetInicio()->Liga;
	  while(n1){
		 int Indice1= n1->Valor;
		 Numero *n2= n1->Liga;
		 while(n2){
			int Indice2= n2->Valor;
			Relacion3[Indice1][Indice2]= 1;
			if((*this)(Indice2)->Anulable==false)
			  break;
			n2= n2->Liga;
		 }
		 n1= n1->Liga;
	  }
	  p= p->GetLiga();
	}
 }




 void Gramatica::ObtenerR4(){

	Relacion4= ObtenerMatrizNula(nSimbolos, nSimbolos);

	Produccion *p= Producciones;
	while(p){
	  int Indice2= p->GetInicio()->Valor;
	  for(int i= p->GetCuantos()-1; i!=0; --i){
		 int Indice1= (*p)[i];
		 Relacion4[Indice1][Indice2]= 1;
		 if((*this)(Indice1)->Anulable==false)
			break;
	  }
	  p= p->GetLiga();
	}
 }



 void Gramatica::ObtenerR5(){
	Relacion5= Relacion4;  //Se parte de la matriz anterior

	//Diagonal principal
	for(int i= 0; i<nSimbolos; ++i)
	  Relacion5[i][i]= (Relacion5[i][i])?1:2;

	bool HuboCambio= true;
	while(HuboCambio){
	  HuboCambio= false;
	  for(int i= 0; i<nSimbolos; ++i){ //avanzar de col en col
		 for(int j= 0; j<nSimbolos; ++j){//avanzar de fila en fila
			if(Relacion5[j][i]){ //traerse la columna j
			  for(int k= 0; k<nSimbolos; ++k)
				 if(Relacion5[k][j] && !Relacion5[k][i]){
					  Relacion5[k][i]= 2;
					  HuboCambio= true;
				 }
			}
		 }//for2
	  }//for1
	}//while
 }




 void Gramatica::ObtenerR6(){

	Relacion6= ObtenerMatrizNula(nSimbolos, nSimbolos+1);

	Produccion *p= Producciones;
	while(p){
	  Numero *n1= p->GetInicio()->Liga;
	  while(n1){
		 int Indice1= n1->Valor;
		 Numero *n2= n1->Liga;
		 while(n2){
			int Indice2= n2->Valor;
			for(int A= 0; A<nSimbolos; ++A){
			  if(Relacion5[A][Indice1]){
				 for(int B= 0; B<nSimbolos; ++B)
					if(Relacion2[Indice2][B])
					  Relacion6[A][B]= 1;
			  }
			}
			if((*this)(Indice2)->Anulable==false)
           break;
			n2= n2->Liga;
		 }
		 n1= n1->Liga;
	  }
	  p= p->GetLiga();
	}

	//Agregar la marca de fin de secuencia

	for(int i= 0; i<nSimbolos; ++i)
     Relacion6[i][nSimbolos]= (Relacion5[i][0])?1:0;

 }



