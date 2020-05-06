#include "input.h"
#include "solution.h"
#include "construction.h"
#include "neighborhood.h"
#include "perturbation.h"
#include "localsearch.h"
#include <cstring>
#include <fstream>
#include <iostream>
#include <ctime>
#include <vector>
#include <algorithm>
#include <cmath>
#include <list>
#include <iomanip>
#include <stack>
#include <climits>
#include <queue>
#include "node.h"
#include "data.h"
#include "hungarian.h"
// #define INF std::numeric_limits<double>::infinity()
#define EPSILON std::numeric_limits<double>::epsilon()
using namespace std;
list<Node> arvore;
stack<Node> arvoreDFS;
vector<int> mapeamento;
// priority_queue<Node, vector<Node>, CompareNode> arvoreBest;
std::vector<std::pair<int, int>> solucaoEdges;
Data *input;
double **cost;
int N;
int mode = HUNGARIAN_MODE_MINIMIZE_COST;
double UB = INT_MAX;

/** 
 * calcular solucao usando algoritmo hungaro e matriz de distancia inicial e 
 * preencher os atributos da estrutura Node de acordo 
 * */
void calcularSolucao(Node &no);
void calcularSolucaoLagrange(Node &no);
int computeDistance(Node &no);
void escolherSubtour(Node &no);
void escolherEstrela(Node &no);
void printArvore(list<Node> &arvore);
void printArvore(stack<Node> &tree);
void printNode(Node &no);
void prune(list<Node> &tree, int UB);
void prune(stack<Node> &tree, int UB);
Node largura(Node &no);
void profundidade(Node &no);
void melhorbound(Node &no);

int main(int argc, char **argv)
{
	int seed = time(0);
	// seed = 1582915991;
	srand(seed);

	Input in(argc, argv);
	input = new Data(argc, argv[1]);
	input->readData();
	// Input in(argv);
	in.problemSet(0);
	Solution sol(&in);
	LocalSearch ls(&in);

	int Imax = 50;
	int Iils = (in.dimensionGet() >= 150) ? in.dimensionGet() / 2 : in.dimensionGet();
	// int Imax = 1;
	// int Iils = 0;
	vector<double> R;
	R.push_back(0.00);
	for (int i = 1; i <= 25; i++)
		R.push_back(R[i - 1] + 0.01);
	// GILSRVND parameters

	for (int i = 1; i <= 1; i++)
	{
		seed++;
		srand(seed);
		clock_t beginC = clock();
		sol = ls.GILSRVND(Imax, Iils, R);
		clock_t endC = clock();
		double elapsed_secs = double(endC - beginC) / CLOCKS_PER_SEC;
		printf("%.*s", int(strlen(argv[1])) - 14, argv[1] + 10);
		cout << "," << elapsed_secs << ",";
		if (in.problemGet() == 0)
			cout << sol.costValueTSP;
		cout << "," << seed << endl;
	}

	UB = sol.costValueTSP + 1;

	Node raiz(input->getDimension());

	raiz.calculateLB(input, UB);

	std::list<Node> tree;
	tree.push_back(raiz);

	auto node = tree.begin();

	while (tree.empty() == false)
	{
		// Usa a estratégia do menor bound
		double menorLB = INF;
		for (auto it = tree.begin(); it != tree.end(); ++it)
		{
			// Prune as soon as possible
			if (it->LB > UB)
				tree.erase(it);
			else if (menorLB > it->LB)
			{
				menorLB = it->LB;
				node = it;
			}
		}

		// Check again if the tree is alredy empty since you were using the erase method previously
		if (tree.empty())
			break;

		if (node->isFeasible)
		{
			cout << "UB=" << node->currentNodeCost << endl;

			// Qualquer vertice com LB maior que o vertice de menor lower bound sera removido
			for (auto it = tree.begin(); it != tree.end(); it++)
				if (it->LB > node->currentNodeCost)
					it->pruning = true;

			// Se a solução viável, guarde-a
			if (node->currentNodeCost < UB)
			{
				solucaoEdges = node->arestas;
				UB = node->currentNodeCost;
			}
		}
		else
			cout << "LB: " << node->LB << endl;

		if (node->pruning)
		{
			tree.erase(node);
			continue;
		}

		//Escolha o vértice de maior grau
		int noEscolhido = std::max_element(node->degree.begin() + 0, node->degree.end()) - node->degree.begin();

		for (auto &aresta : node->arestas)
		{
			if (aresta.first == noEscolhido || aresta.second == noEscolhido)
			{
				Node n(input->getDimension());
				n.arcosProibidos = node->arcosProibidos;
				n.arcosProibidos.push_back(aresta);
				n.lambda = node->lambda;
				n.calculateLB(input, UB);
				tree.push_back(n);
			}
		}
		tree.erase(node);
	}

	std::cout << "\n\n"
			  << UB << "\n";

	return 0;

	for (int i = 0; i < input->getDimension(); i++)
		delete[] cost[i];
	delete[] cost;
	// cout<<"Done"<<endl;
	return 0;
}