#include <stdio.h>

#define VACIO -1

class Cubo {

  int C[51][51];
  int N;

  public:

    /* Función que inicializa el problema */
    Cubo(int N);

    /* Resuelve el problema */
    void resuelve();

    void imprime();
};

Cubo::Cubo(int _N)
{
  N = _N;

  for (int i = 0; i < N; ++i)
    for (int j = 0; j < N; ++j)
      C[i][j] = VACIO;
}

void Cubo::imprime()
{
  for (int i = 0; i < N; ++i)
  {
    for (int j = 0; j < N; ++j)
      printf("%4d", C[i][j]);
    printf("\n");
  }
}

void Cubo::resuelve()
{
  int col = N / 2;
  int fil = 0;
  int sigue = 1;
  while (sigue <= N * N)
  {
    C[fil][col] = sigue;
    int nueva_col = (col + 1) % N;
    int nueva_fil = (N + fil - 1) % N;
    if (C[nueva_fil][nueva_col] != VACIO)
    {
      nueva_fil = (fil + 1) % N;
      nueva_col = col;
    }
    fil = nueva_fil;
    col = nueva_col;
    sigue ++;
  }
}

int main(int argc, char *argv[])
{
  Cubo c(3);
  c.resuelve();
  c.imprime();
  return 0;
}
