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
using namespace std;
list<Node> arvore;
vector<pair<int, int> > solucaoEdges;
Data *input;
int N;
double UB = INT_MAX;

double BFS(double UB_plus);
double DFS(double UB_plus);
double bestBound(double UB_plus);

int main(int argc, char **argv)
{
	int seed = time(0);
	srand(seed);

	Input in(argc, argv);
	input = new Data(argc, argv[1]);
	input->readData();
	in.problemSet(0);
	Solution sol(&in);
	LocalSearch ls(&in);

	int Imax = 50;
	int Iils = (in.dimensionGet() >= 150) ? in.dimensionGet() / 2 : in.dimensionGet();
	vector<double> R;
	R.push_back(0.00);
	for (int i = 1; i <= 25; i++)
		R.push_back(R[i - 1] + 0.01);

	sol = ls.GILSRVND(Imax, Iils, R);

	cout << "{"<<flush;
	printf("%.*s", int(strlen(argv[1])) - 14, argv[1] + 10);
	cout << ","<<flush;
	clock_t beginC = clock();
	cout << bestBound(sol.costValueTSP + 1);
	clock_t endC = clock();
	cout << ",";
	cout << double(endC - beginC) / CLOCKS_PER_SEC << ","<<flush;
	beginC = clock();
	cout << DFS(sol.costValueTSP + 1);
	endC = clock();
	cout << ",";
	cout << double(endC - beginC) / CLOCKS_PER_SEC << "," << flush;
	beginC = clock();
	cout << bestBound(sol.costValueTSP + 1);
	// cout<<BFS();
	endC = clock();
	cout << ",";
	cout << double(endC - beginC) / CLOCKS_PER_SEC;
	cout << "}" << endl;

	return 0;
}

double bestBound(double UB_plus)
{
	UB = UB_plus;

	Node raiz(input->getDimension());

	raiz.calculateLB(input, UB);

	list<Node> tree;

	tree.push_back(raiz);

	auto node = tree.begin();

	while (tree.empty() == false)
	{
		if (node->pruning)
		{
			tree.erase(node);
			continue;
		}
		
		// Usa a estratégia do menor bound
		double menorLB = INF;
		for (auto it = tree.begin(); it != tree.end(); ++it)
		{
			if (menorLB > it->LB)
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
			// Qualquer solução com LB maior que a solução de menor lower bound sera removido
			for (auto it = tree.begin(); it != tree.end(); it++)
				if (it->LB > node->currentNodeCost)
					it->pruning = true;

			// Se a solução viável, guarde-a
			if (node->currentNodeCost < UB)
			{
				solucaoEdges = node->arestas;
				UB = node->currentNodeCost;
			}
			tree.erase(node);
			continue;
		}

		if (node->pruning)
		{
			tree.erase(node);
			continue;
		}

		//Escolha o vértice de maior grau
		int noEscolhido = max_element(node->degree.begin() + 0, node->degree.end()) - node->degree.begin();

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
	return UB;
}

double DFS(double UB_plus)
{
	UB = UB_plus;

	Node raiz(input->getDimension());

	raiz.DFS = true;
	raiz.calculateLB(input, UB);

	stack<Node> tree;

	tree.push(raiz);

	Node node(input->getDimension());

	while (tree.empty() == false)
	{
		node = tree.top();
		// cout<<node.LB<<endl;
		if (node.LB > UB){
			tree.pop();
			continue;
		}

		// Check again if the tree is alredy empty since you were using the top method previously
		if (tree.empty())
			break;

		if (node.isFeasible)
		{
			// Se a solução é viável, guarde-a
			if (node.currentNodeCost < UB)
			{
				solucaoEdges = node.arestas;
				UB = node.currentNodeCost;
			}
			tree.pop();
			continue;
		}
		
		if(node.indexStar < node.star.size()){
			Node n(input->getDimension());
			n.arcosProibidos = node.arcosProibidos;
			n.arcosProibidos.push_back(node.star[node.indexStar]);
			n.lambda = node.lambda;
			n.DFS = true;
			n.calculateLB(input, UB);
			node.indexStar = node.indexStar + 1;
			tree.pop();
			if(node.indexStar < node.star.size())
				tree.push(node);
			tree.push(n);
		}else{
			cout<<node.LB<<endl;
			tree.pop();
		}
	}
	return UB;
}

double BFS(double UB_plus)
{
	UB = UB_plus;

	Node raiz(input->getDimension());

	raiz.calculateLB(input, UB);

	list<Node> tree;

	tree.push_back(raiz);

	auto node = tree.begin();

	cout<<UB<<endl;
	exit(0);
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
			// cout << "UB=" << node->currentNodeCost << endl;

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

		if (node->pruning)
		{
			tree.erase(node);
			continue;
		}

		//Escolha o vértice de maior grau
		int noEscolhido = max_element(node->degree.begin() + 0, node->degree.end()) - node->degree.begin();

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
	return UB;
}