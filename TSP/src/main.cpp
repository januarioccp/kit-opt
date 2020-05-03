#include "Lagrange.h"
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
#include <list>
#include <iomanip>
#include <stack>
#include <climits>
#include <queue>
using namespace std;
#include "data.h"
#include "hungarian.h"

typedef struct node
{
	vector<pair<int, int>> arcos_proibidos; //lista de arcos proibidos
	vector<vector<int>> subtour;			//conjunto de subtours da solucao gerada pelo algoritmo hungaro
	vector<pair<int, int>> arestas;			//conjunto de arestas da solução gerada pelo Kruskal
	vector<pair<int, int>> estrela;			//conjunto de arestas da solução gerada pelo Kruskal
	vector<int> degree;						// graus dos vertices na solução gerada pelo Kruskal
	double lower_bound;						// lower bound produzido pelo no (ou custo total da solucao do algoritmo hungaro)
	int escolhido;							//subtour escolhido dado o criterio de selecao
	int index;								//indice dentro do subtour escolhido dado o criterio de selecao
	bool podar;								//variavel que diz se o no deve gerar filhos
	vector<double> lambda;
} Node;

ostream & operator << (ostream &out, const Node &c){
	out <<"Lower bound: "<< c.lower_bound<<endl;
	out <<"Arestas: ";
	for(int i =0; i < c.arestas.size(); i++)
		out<<"\t("<<c.arestas[i].first<<","<<c.arestas[i].second<<") ";
	out<<endl;
	out <<"Grau: \t";
	for(int i =0; i < c.degree.size(); i++)
		out<<"\t("<<i<<"="<<c.degree[i]<<") ";
	out<<endl;
	out <<"Estrela: ";
	for(int i =0; i < c.estrela.size(); i++)
		out<<"\t("<<c.estrela[i].first<<","<<c.estrela[i].second<<") ";
	out<<endl;
	return out;
}

struct CompareNode
{
	bool operator()(Node const &p1, Node const &p2)
	{
		// return "true" if "p1" is ordered
		// before "p2", for example:
		if (p1.lower_bound < p2.lower_bound)
			return true;
		else
		{

			if ((p1.subtour[p1.escolhido].size() - p1.escolhido) < (p2.subtour[p2.escolhido].size() - p2.escolhido))
				return true;
			else
			{
				// cout<<"Empate"<<endl;
				return (p1.subtour.size() < p2.subtour.size());
			}
		}
	}
};

// Function for binary_predicate
bool compare(Node &a, Node &b);

list<Node> arvore;
stack<Node> arvoreDFS;
vector<int> mapeamento;
priority_queue<Node, vector<Node>, CompareNode> arvoreBest;

Data *input;
double **cost;
int N;
int mode = HUNGARIAN_MODE_MINIMIZE_COST;
int LB = INT_MAX;
int UB = INT_MAX;

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

	UB = sol.costValueTSP;

	Node bb;
	bb.lambda.resize(input->getDimension());
	// largura(bb);
	profundidade(bb);
	// melhorbound(bb);

	cout<<bb<<endl;

	return 0;

	for (int i = 0; i < input->getDimension(); i++)
		delete[] cost[i];
	delete[] cost;
	delete input;

	// cout<<"Done"<<endl;
	return 0;
}
void calcularSolucaoLagrange(Node &no)
{
	Lagrange lagrange(input);

	for (int i = 0; i < no.arcos_proibidos.size(); i++)
	{
		lagrange.cost[no.arcos_proibidos[i].first][no.arcos_proibidos[i].second] = 99999998;
	}

	lagrange.lambda = no.lambda;

	no.lower_bound = lagrange.lowerBound(UB);

	no.arestas = lagrange.aresta;
	no.degree = lagrange.degree;

	no.podar = true;
	for (int i = 0; i < no.degree.size(); i++)
		if (no.degree[i] > 2 || no.degree[i] < 2){
			no.podar = false;
			break;
		}

	// computeDistance(no);
}

void calcularSolucao(Node &no)
{
	hungarian_problem_t p;

	hungarian_init(&p, cost, input->getDimension(), input->getDimension(), mode);

	for (int i = 0; i < no.arcos_proibidos.size(); i++)
	{
		p.cost[no.arcos_proibidos[i].first][no.arcos_proibidos[i].second] = 99999998;
	}

	//cout<<no.lower_bound<<endl;
	// for(int i=0; i < input->getDimension(); i++){
	// 	for(int j=0; j < input->getDimension(); j++){
	// 		cout<<setw(7)<<p.cost[i][j];
	// 	}
	// 	cout<<endl;
	// }
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
	hungarian_free(&p);
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
void escolherEstrela(Node &no)
{
	// Procurar o vértice de maior grau
	int mgrau = 0;
	int vertex_mgrau = 0;
	for (int i = 0; i < no.degree.size(); i++)
	{
		mgrau = max(mgrau, no.degree[i]);
		if (mgrau == no.degree[i])
			vertex_mgrau = i;
	}
	no.estrela.resize(0);
	for (int i = 0; i < no.arestas.size(); i++)
	{
		if (no.arestas[i].first == vertex_mgrau || no.arestas[i].second == vertex_mgrau)
			no.estrela.push_back(no.arestas[i]);
	}
	no.index = 0;
}

void escolherSubtour(Node &no)
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
	// return no.escolhido;
}

void printArvore(stack<Node> &tree)
{
	stack<Node> temp;
	while (!tree.empty())
	{
		temp.push(tree.top());
		printNode(tree.top());
		tree.pop();
	}

	while (!temp.empty())
	{
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
		{
			cout << "Prune -> ";
			printNode(*it);
			tree.erase(it);
		}
	}
	//tree.unique(compare);
}

void prune(stack<Node> &tree, int LB)
{
	stack<Node> temp;
	while (!tree.empty())
	{
		if (tree.top().lower_bound < LB)
		{
			temp.push(tree.top());
		}
		else
			cout << "Prune!" << endl;
		tree.pop();
	}

	while (!temp.empty())
	{
		tree.push(temp.top());
		temp.pop();
	}
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

Node largura(Node &no)
{
	// calcularSolucao(no);
	calcularSolucaoLagrange(no);
	arvore.push_back(no); //inserir novos nos na arvore
	Node nutella;
	while (!arvore.empty())
	{
		cout<<arvore.size()<<endl;
		nutella = arvore.front();
		arvore.pop_front();
		if (nutella.podar)
		{
			cout<<nutella<<endl;
			continue;
		}
		
		// escolherSubtour(nutella);
		escolherEstrela(nutella);
		cout<<nutella<<endl;
		
		for (int i = 0; i < nutella.estrela.size() - 1; i++)
		{
			Node n;
			n.arcos_proibidos = nutella.arcos_proibidos;
			n.lambda = nutella.lambda;
			pair<int, int> arco_proibido;
			arco_proibido.first = nutella.estrela[i].first;
			arco_proibido.second = nutella.estrela[i].second;
			n.arcos_proibidos.push_back(arco_proibido);
			arco_proibido.first = nutella.estrela[i].second;
			arco_proibido.second = nutella.estrela[i].first;
			n.arcos_proibidos.push_back(arco_proibido);

			calcularSolucaoLagrange(n);
			arvore.push_back(n); //inserir novos nos na arvore
			// if (n.lower_bound < UB)
			// {
			// 	arvore.push_back(n); //inserir novos nos na arvore
			// 	// if (n.subtour.size() == 1)
			// 	// {
			// 	// 	printNode(n);
			// 	// 	UB = n.lower_bound;
			// 	// 	no = n;
			// 	// 	prune(arvore, UB);
			// 	// }
			// }
		}

		// for (int i = 0; i < nutella.subtour[nutella.escolhido].size() - 1; i++)
		// { // iterar por todos os arcos do subtour escolhido
		// 	Node n;
		// 	n.arcos_proibidos = nutella.arcos_proibidos;
		// 	pair<int, int> arco_proibido;
		// 	arco_proibido.first = nutella.subtour[nutella.escolhido][i];
		// 	arco_proibido.second = nutella.subtour[nutella.escolhido][i + 1];
		// 	n.arcos_proibidos.push_back(arco_proibido);
		// 	calcularSolucao(n);
		// 	if (n.lower_bound < UB)
		// 	{
		// 		arvore.push_back(n); //inserir novos nos na arvore
		// 		if (n.subtour.size() == 1)
		// 		{
		// 			printNode(n);
		// 			UB = n.lower_bound;
		// 			no = n;
		// 			prune(arvore, UB);
		// 		}
		// 	}
		// }
	}
	return no;
}

void profundidade(Node &no)
{
	calcularSolucaoLagrange(no);
	// escolherSubtour(no);
	escolherEstrela(no);
	
	arvoreDFS.push(no);

	Node nutella;
	Node n;
	pair<int, int> arco_proibido;
	while (!arvoreDFS.empty())
	{
		cout<<arvoreDFS.size()<<endl;
		// printArvore(arvoreDFS);
		// cout<<"Press enter: ";
		// cin.get();
		nutella = arvoreDFS.top();
		
		if (int(nutella.lower_bound) > UB)
		{
			arvoreDFS.pop();
			continue;
		}
		
		n.arcos_proibidos = nutella.arcos_proibidos;
		arco_proibido.first = nutella.estrela[nutella.index].first;
		arco_proibido.second = nutella.estrela[nutella.index].second;
		n.arcos_proibidos.push_back(arco_proibido);
		arco_proibido.first = nutella.estrela[nutella.index].second;
		arco_proibido.second = nutella.estrela[nutella.index].first;
		n.lambda = nutella.lambda;
		n.arcos_proibidos.push_back(arco_proibido);
		calcularSolucaoLagrange(n);
		escolherEstrela(n);
		
		// arco_proibido.first = nutella.subtour[nutella.escolhido][nutella.index];
		// arco_proibido.second = nutella.subtour[nutella.escolhido][nutella.index + 1];
		// n.arcos_proibidos.push_back(arco_proibido);
		// calcularSolucao(n);
		// escolherSubtour(n);

		nutella.index++;
		arvoreDFS.pop();

		if (nutella.index < nutella.estrela.size() - 1)
			arvoreDFS.push(nutella);

		if (int(n.lower_bound) > UB)
			continue;

		if (!n.podar)
		{
				arvoreDFS.push(n); //inserir novos nos na arvore
		}
		else
		{
			cout<<"-\n"<<n<<endl;
		}
	}
}

void melhorbound(Node &no)
{
	calcularSolucao(no);
	escolherSubtour(no);
	arvoreBest.push(no);

	Node nutella;
	Node n;
	pair<int, int> arco_proibido;
	while (!arvoreBest.empty())
	{
		cout << arvoreBest.size() << endl;
		nutella = arvoreBest.top();
		if (!(nutella.lower_bound < UB))
		{
			arvoreBest.pop();
			continue;
		}
		n.arcos_proibidos = nutella.arcos_proibidos;
		arco_proibido.first = nutella.subtour[nutella.escolhido][nutella.index];
		arco_proibido.second = nutella.subtour[nutella.escolhido][nutella.index + 1];
		n.arcos_proibidos.push_back(arco_proibido);
		calcularSolucao(n);
		escolherSubtour(n);

		nutella.index++;
		arvoreBest.pop();

		if (nutella.index < nutella.subtour[nutella.escolhido].size() - 1)
			arvoreBest.push(nutella);

		if (n.lower_bound < UB)
		{
			arvoreBest.push(n); //inserir novos nos na arvore
			if (n.subtour.size() == 1)
			{
				printNode(n);
				// cout<<"Press enter: ";
				// cin.get();
				UB = n.lower_bound;
				no = n;
				arvoreBest.pop();
				//prune(arvoreBest, UB);
			}
		}
	}
}
