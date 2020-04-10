#include <iostream>
#include <vector>
#include <algorithm>
#include <list>
#include <iomanip>
#include <stack>
#include <climits>
using namespace std;

#include "data.h"
#include "hungarian.h"

typedef struct node
{
	vector<pair<int, int> > arcos_proibidos; //lista de arcos proibidos
	vector<vector<int> > subtour;			//conjunto de subtours da solucao gerada pelo algoritmo hungaro
	double lower_bound;						// lower bound produzido pelo no (ou custo total da solucao do algoritmo hungaro)
	int escolhido;							//subtour escolhido dado o criterio de selecao
	int index;								//indice dentro do subtour escolhido dado o criterio de selecao
	bool podar;								//variavel que diz se o no deve gerar filhos
} Node;

// Function for binary_predicate
bool compare(Node &a, Node &b);

list<Node> arvore;
stack<Node> arvoreDFS;
vector<int> mapeamento;

Data *input;
double **cost;
int N;
int mode = HUNGARIAN_MODE_MINIMIZE_COST;
int LB = 0;
int UB = INT_MAX;

/** 
 * calcular solucao usando algoritmo hungaro e matriz de distancia inicial e 
 * preencher os atributos da estrutura Node de acordo 
 * */
void calcularSolucao(Node &no, hungarian_problem_t &p);
int computeDistance(Node &no);
int escolherSubtour(Node &no);
void printArvore(list<Node> &arvore);
void printArvore(stack<Node> &tree);
void printNode(Node &no);
void prune(list<Node> &tree, int UB);
void largura(Node &no, hungarian_problem_t &p);
void profundidade(Node &no, hungarian_problem_t &p);

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
	Node sol;
	// largura(sol, p);
	profundidade(sol, p);
	printNode(sol);

	hungarian_free(&p);
	for (int i = 0; i < input->getDimension(); i++)
		delete[] cost[i];
	delete[] cost;
	delete input;

	return 0;
}

void calcularSolucao(Node &no, hungarian_problem_t &p)
{
	hungarian_init(&p, cost, input->getDimension(), input->getDimension(), mode);
	for (int i = 0; i < no.arcos_proibidos.size(); i++)
		p.cost[no.arcos_proibidos[i].first][no.arcos_proibidos[i].second] = INT_MAX;
	hungarian_solve(&p);

	no.subtour.resize(0);
	
	mapeamento.resize(N);
	int next, prev;
	fill(mapeamento.begin(), mapeamento.end(), -1);

	for (int i = 0; i < N; i++)
	{
		if (mapeamento[i] == -1)
		{
			prev = i;
			no.subtour.push_back(std::vector<int>());
			mapeamento[prev] = no.subtour.size() - 1;
			no.subtour[mapeamento[prev]].push_back(prev);
			do
			{
				for (int j = 0; j < N; j++)
				{
					next = j;
					if (p.assignment[prev][j])
						break;
				}
				mapeamento[next] = mapeamento[prev];
				no.subtour[mapeamento[next]].push_back(next);
				prev = next;
			} while (next != i);
			if (no.subtour.size() > 1)
				no.podar = false;
			else
				no.podar = true;
		}
	}

	computeDistance(no);
	
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
	int size = INT_MAX;
	for (int i = 0; i < no.subtour.size(); i++)
	{
		if (no.subtour[i].size() < size)
		{
			size = no.subtour[i].size();
			no.escolhido = i;
		}
	}
	no.index = 0;
	return no.escolhido;
}

void printArvore(stack<Node> &tree)
{
	stack<Node> temp;
	while(!tree.empty()){
		temp.push(tree.top());
		printNode(tree.top());
		tree.pop();
	}

	while(!temp.empty()){
		tree.push(temp.top());
		temp.pop();
	}
	
} 

void printArvore(list<Node> &tree)
{
	for (list<Node>::iterator it = tree.begin(); it != tree.end(); ++it)
	{
		if (it->subtour.size() == 1)
			cout << "***";
		printNode(*it);
	}
}

void printNode(Node &no)
{
	for (int i = 0; i < no.subtour.size(); i++)
	{
		cout << "{";
		for (int j = 0; j < no.subtour[i].size(); j++)
		{
			cout << no.subtour[i][j];
			if (j < no.subtour[i].size() - 1)
				cout << ",";
		}
		cout << "}";
	}
	cout << " - " << no.lower_bound << endl;
}

void prune(list<Node> &tree, int UB)
{
	for (list<Node>::iterator it = tree.begin(); it != tree.end(); ++it)
	{
		if (it->lower_bound >= UB)
			tree.erase(it);
	}
	//tree.unique(compare);
}

bool compare(Node &a, Node &b)
{
	if (a.lower_bound != b.lower_bound)
		return false;
	else
	{
		if (a.subtour.size() != b.subtour.size())
			return false;
		for (int i = 0; i < a.subtour.size(); i++)
		{
			if (a.subtour[i].size() != b.subtour[i].size())
				return false;
			for (int j = 0; j < a.subtour[i].size(); j++)
			{
				if (a.subtour[i][j] != b.subtour[i][j])
					return false;
			}
		}

		return true;
	}
}

void largura(Node &no, hungarian_problem_t &p)
{
	calcularSolucao(no, p);
	arvore.push_back(no); //inserir novos nos na arvore
	Node nutella;
	while (!arvore.empty())
	{
		nutella = arvore.front();
		arvore.pop_front();
		if (nutella.lower_bound >= UB)
			continue;

		escolherSubtour(nutella);
		for (int i = 0; i < nutella.subtour[nutella.escolhido].size() - 1; i++)
		{ // iterar por todos os arcos do subtour escolhido
			Node n;
			n.arcos_proibidos = nutella.arcos_proibidos;
			pair<int, int> arco_proibido;
			arco_proibido.first = nutella.subtour[nutella.escolhido][i];
			arco_proibido.second = nutella.subtour[nutella.escolhido][i + 1];
			n.arcos_proibidos.push_back(arco_proibido);
			calcularSolucao(n, p);
			if (n.lower_bound < UB)
			{
				arvore.push_back(n); //inserir novos nos na arvore
				if (n.subtour.size() == 1)
				{
					UB = n.lower_bound;
					no = n;
					prune(arvore, UB);
				}
			}
		}
	}
}

void profundidade(Node &no, hungarian_problem_t &p)
{
	calcularSolucao(no, p);
	escolherSubtour(no);
	arvoreDFS.push(no);

	Node nutella;
	Node n;
	pair<int, int> arco_proibido;
	while (!arvoreDFS.empty())
	{
		// cout<<arvoreDFS.size()<<endl;
		nutella = arvoreDFS.top();
		n.arcos_proibidos = nutella.arcos_proibidos;
		arco_proibido.first = nutella.subtour[nutella.escolhido][nutella.index];
		arco_proibido.second = nutella.subtour[nutella.escolhido][nutella.index + 1];
		n.arcos_proibidos.push_back(arco_proibido);
		calcularSolucao(n, p);
		escolherSubtour(n);

		nutella.index++;
		arvoreDFS.pop();
		
		if (nutella.index < nutella.subtour[nutella.escolhido].size()-1)
			arvoreDFS.push(nutella);
		
		if (n.lower_bound < UB)
		{
			arvoreDFS.push(n); //inserir novos nos na arvore
			if (n.subtour.size() == 1)
			{
				// printNode(n);
				// cout<<"Press enter: ";
				// cin.get();
				UB = n.lower_bound;
				no = n;
				arvoreDFS.pop();
				// prune(arvore, UB);
			}
		}
	}
}