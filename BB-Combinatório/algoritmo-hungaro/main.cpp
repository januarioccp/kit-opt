#include <iostream>
#include <vector>
#include <algorithm>
#include <list>
#include <iomanip>
using namespace std;

#include "data.h"
#include "hungarian.h"

typedef struct node
{
	vector<pair<int, int> > arcos_proibidos; //lista de arcos proibidos
	vector<vector<int> > subtour;			//conjunto de subtours da solucao gerada pelo algoritmo hungaro
	double lower_bound;						// lower bound produzido pelo no (ou custo total da solucao do algoritmo hungaro)
	int escolhido;							//subtour escolhido dado o criterio de selecao
	bool podar;								//variavel que diz se o no deve gerar filhos
} Node;

list<Node> arvore;
Data *input;
Node raiz;
double **cost;
int N;
int mode = HUNGARIAN_MODE_MINIMIZE_COST;

/** 
 * calcular solucao usando algoritmo hungaro e matriz de distancia inicial e 
 * preencher os atributos da estrutura Node de acordo 
 * */
void calcularSolucao(Node &no, hungarian_problem_t &p);
int computeDistance(Node &no);
int escolherSubtour(Node &no);
void printArvore(list<Node> &arvore);
void printNode(Node &no);

int main(int argc, char **argv)
{

	input = new Data(argc, argv[1]);
	input->readData();

	cost = new double *[input->getDimension()];
	for (int i = 0; i < input->getDimension(); i++)
	{
		cost[i] = new double[input->getDimension()];
		for (int j = 0; j < input->getDimension(); j++)
		{
			cost[i][j] = input->getDistance(i, j);
		}
	}

	N = input->getDimension();

	hungarian_problem_t p;

	hungarian_init(&p, cost, input->getDimension(), input->getDimension(), mode); // Carregando o problema
	// hungarian_solve(&p);

	calcularSolucao(raiz, p);
	arvore.push_back(raiz); //inserir novos nos na arvore

	for (int j = 0; j < arvore.size(); j++)
	{
		Node nutella = arvore.front();
		arvore.pop_front();
		escolherSubtour(nutella);
		if (!nutella.podar)
			for (int i = 0; i < nutella.subtour[nutella.escolhido].size() - 1; i++)
			{ // iterar por todos os arcos do subtour escolhido
				Node n;
				n.arcos_proibidos = nutella.arcos_proibidos;
				pair<int, int> arco_proibido;
				arco_proibido.first = nutella.subtour[nutella.escolhido][i];
				arco_proibido.second = nutella.subtour[nutella.escolhido][i + 1];
				n.arcos_proibidos.push_back(arco_proibido);
				calcularSolucao(n, p);
				arvore.push_back(n); //inserir novos nos na arvore
			}
		printArvore(arvore);
	}

	hungarian_free(&p);
	for (int i = 0; i < input->getDimension(); i++)
		delete[] cost[i];
	delete[] cost;
	delete input;

	return 0;

	arvore.push_back(raiz);
}

void calcularSolucao(Node &raiz, hungarian_problem_t &p)
{
	hungarian_init(&p, cost, input->getDimension(), input->getDimension(), mode);
	for (int i = 0; i < raiz.arcos_proibidos.size(); i++)
		p.cost[raiz.arcos_proibidos[i].first][raiz.arcos_proibidos[i].second] = INT_MAX;
	hungarian_solve(&p);

	raiz.subtour.resize(0);
	vector<int> mapeamento;
	mapeamento.resize(N);
	int next, prev;
	fill(mapeamento.begin(), mapeamento.end(), -1);

	for (int i = 0; i < N; i++)
	{
		if (mapeamento[i] == -1)
		{
			prev = i;
			raiz.subtour.push_back(std::vector<int>());
			mapeamento[prev] = raiz.subtour.size() - 1;
			raiz.subtour[mapeamento[prev]].push_back(prev);
			do
			{
				for (int j = 0; j < N; j++)
				{
					next = j;
					if (p.assignment[prev][j])
						break;
				}
				mapeamento[next] = mapeamento[prev];
				raiz.subtour[mapeamento[next]].push_back(next);
				prev = next;
			} while (next != i);
			if (raiz.subtour.size() > 1)
				raiz.podar = false;
			else
				raiz.podar = true;
		}
	}

	computeDistance(raiz);
}

int computeDistance(Node &raiz)
{
	int custo = 0;

	for (int i = 0; i < raiz.subtour.size(); i++)
	{
		for (int j = 0; j < raiz.subtour[i].size() - 1; j++)
		{
			custo += cost[raiz.subtour[i][j]][raiz.subtour[i][j + 1]];
		}
	}
	raiz.lower_bound = custo;
	return custo;
}

int escolherSubtour(Node &no)
{
	int size = INT_MAX / 2;
	for (int i = 0; i < raiz.subtour.size(); i++)
	{
		if (raiz.subtour[i].size() < size)
		{
			size = raiz.subtour[i].size();
			raiz.escolhido = i;
		}
	}
	return raiz.escolhido;
}

void printArvore(list<Node> &arvore)
{
	for (list<Node>::iterator it = arvore.begin(); it != arvore.end(); ++it)
	{
		printNode(*it);
	}
}

void printNode(Node &no)
{
	for (int i = 0; i < raiz.subtour.size(); i++)
	{
		cout << "{";
		for (int j = 0; j < raiz.subtour[i].size(); j++)
		{
			cout << raiz.subtour[i][j];
			if (j < raiz.subtour[i].size() - 1)
				cout << ",";
		}
		cout << "}";
	}
	cout << " - " << raiz.lower_bound << endl;
}