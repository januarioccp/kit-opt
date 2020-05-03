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
        double lowerBound(double UB = 3323);
        vector<vector<double> > cost;
        vector<vector<double> > cost2;
        vector<pair<int, int> > aresta;
        vector<double> lambda;	
        vector<int> g;
        vector<int> degree;
        double LB;
        bool prune;
        double ek;
        friend ostream& operator <<(ostream& os, const Lagrange&);

};

#endif