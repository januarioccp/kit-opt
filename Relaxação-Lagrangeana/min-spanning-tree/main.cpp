#include "Kruskal.h"
#include "data.h"
#include <iostream>
#include <vector>
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
vector<vector<double>> cost;
vector<pair<int, int>> aresta;
vector<int> mapeamento;
pair<int, int> lowestCostEdges;
pair<int, int> lowestCostValue;
int N; // = input->getDimension()-1;

void calcularSolucao(Node &no);

int main(int argc, char **argv)
{
    input = new Data(argc, argv[1]);
    input->readData();
    N = input->getDimension() - 1;

    cost.resize(N);
    for (int i = 0; i < N; i++)
        for (int j = 0; j < N; j++)
            // Here we use i+1 and j+1 to avoid consider vertex 0 (zero)
            cost[i].push_back(input->getDistance(i + 1, j + 1));

    Kruskal k(cost);

    lowestCostValue.first = lowestCostValue.second = 99999998;
    lowestCostEdges.first = lowestCostEdges.second = 0;
    for (int i = 1; i < input->getDimension(); i++)
    {
        if (lowestCostValue.first > input->getDistance(0, i))
        {
            lowestCostEdges.second = lowestCostEdges.first;
            lowestCostValue.second = lowestCostValue.first;

            lowestCostValue.first = input->getDistance(0, i);
            lowestCostEdges.first = i;
        }
        else if (lowestCostValue.second > input->getDistance(0, i)){
            lowestCostValue.second = input->getDistance(0, i);
            lowestCostEdges.second = i;
        }
    }

    cout << k.MST(N) + lowestCostValue.first + lowestCostValue.second << endl;

    aresta = k.getEdges();
    for (int i = 0; i < aresta.size(); i++)
    {
        cout << aresta[i].first << "-" << aresta[i].second;
        cout << endl;
    }

    return 0;
}

// void calcularSolucao(Node &no)
// {
//

// 	for (int i = 0; i < N; i++)
//         for (int j = 0; j < N; j++)
//             cost[i][j] = input->getDistance(i, j);

//     for (int i = 0; i < no.arcos_proibidos.size(); i++){
// 		cost[no.arcos_proibidos[i].first][no.arcos_proibidos[i].second] = 99999998;
// 	}

//     Kruskal k(cost);

// 	no.subtour.resize(0);

// 	mapeamento.resize(N);
// 	int next, prev;
// 	fill(mapeamento.begin(), mapeamento.end(), -1);

// 	for (int i = 0; i < N; i++)
// 	{
// 		if (mapeamento[i] == -1)
// 		{
// 			prev = i;
// 			no.subtour.push_back(std::vector<int>());
// 			mapeamento[prev] = no.subtour.size() - 1;
// 			no.subtour[mapeamento[prev]].push_back(prev);
// 			do
// 			{
// 				for (int j = 0; j < N; j++)
// 				{
// 					next = j;
// 					if (p.assignment[prev][j])
// 						break;
// 				}
// 				mapeamento[next] = mapeamento[prev];
// 				no.subtour[mapeamento[next]].push_back(next);
// 				prev = next;
// 			} while (next != i);
// 			if (no.subtour.size() > 1)
// 				no.podar = false;
// 			else
// 				no.podar = true;
// 		}
// 	}

// 	computeDistance(no);
// 	hungarian_free(&p);

// }