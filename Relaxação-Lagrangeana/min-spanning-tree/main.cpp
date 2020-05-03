
#include "Lagrange.h"
#include "data.h"
#include <iostream>
#include <vector>
#include <ctime>
using namespace std;

typedef struct node
{
    vector<pair<int, int>> arcos_proibidos; //lista de arcos proibidos
    vector<vector<int>> subtour;            //conjunto de subtours da solucao gerada pelo algoritmo hungaro
    double lower_bound;                     // lower bound produzido pelo no (ou custo total da solucao do algoritmo hungaro)
    int escolhido;                          //subtour escolhido dado o criterio de selecao
    int index;                              //indice dentro do subtour escolhido dado o criterio de selecao
    bool podar;                             //variavel que diz se o no deve gerar filhos
} Node;

Data *input;

vector<int> mapeamento;
void calcularSolucao(Node &no);

int main(int argc, char **argv)
{
    input = new Data(argc, argv[1]);
    input->readData();

    Lagrange lagrange(input);

    // LB = lagrange.lowerBound();

    cout << __LINE__ << " " << fixed << lagrange.lowerBound() << endl;

    return 0;
}