#ifndef LAGRANGE_TIAGO
#define LAGRANGE_TIAGO
#include "data.h"
#include "Kruskal.h"
#include <vector>
#include <iostream>
using namespace std;

class Lagrange{
    public:
        Lagrange(Data* in);
        int N;
        pair<int, int> lowestCostEdges;
        pair<int, int> lowestCostValue;
        double lowerBound(vector<int> &g,double &LB, double &UB);
        vector<vector<double> > cost;
        vector<vector<double> > cost2;
        vector<pair<int, int> > aresta;
        vector<double> lambda;	
        vector<int> degree;
        double isFeasible;
        bool pruning;
        double ek;
        friend ostream& operator <<(ostream& os, const Lagrange&);

};

#endif