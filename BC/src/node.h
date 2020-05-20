#ifndef NODE_H_INCLUDED
#define NODE_H_INCLUDED
#include "node.h"
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
    std::vector<int> penalizacao;
    std::vector<int> degree;
    double LB;
    bool isFeasible;
    bool pruning;
    void calculateLB(int dimension, std::vector<std::vector<double> > &, const double &);
};

#endif //NODE_H_INCLUDED