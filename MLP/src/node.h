#ifndef NODE_H_INCLUDED
#define NODE_H_INCLUDED
#include "node.h"
#include "input.h"
#include <vector>
using namespace std;

class Node{
    public:
        Node(Input *input);
        ~Node();
    // private:
        Input *in;
        vector<pair<int , int> > forbidden ; //lista de arcos proibidos
        vector< vector<int> > subtour; //conjunto de subtours da solucao gerada pelo algoritmo hungaro
        double lowerBound; // lower bound produzido pelo no (ou custo total da solucao do algoritmo hungaro)
        int chosen; //subtour escolhido dado o criterio de selecao 
        bool bound; //variavel que diz se o no nao deve gerar filhos
};

#endif //NODE_H_INCLUDED