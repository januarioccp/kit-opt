#ifndef NODE_H_INCLUDED
#define NODE_H_INCLUDED
#include "node.h"
#include "data.h"
#include "input.h"
#include <vector>
using namespace std;

class Node
{
  public:
    Node(int dimension);
    std::vector<std::pair<int, int> > arcosProibidos;
    std::vector<std::pair<int, int> > arestas;
    std::vector<double> lambda;
    std::vector<int> g;
    std::vector<int> degree;
    int noEscolhido;
    double LB;
    double currentNodeCost;
    bool isFeasible;
    bool pruning;
    void calculateLB(Data* input, const double &);
};

#endif //NODE_H_INCLUDED