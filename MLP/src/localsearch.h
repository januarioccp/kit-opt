#ifndef LOCALSEARCH_H_INCLUDED
#define LOCALSEARCH_H_INCLUDED
#include "solution.h"
#include "colormod.h"
#include "construction.h"
#include "perturbation.h"
#include "neighborhood.h"
#include <vector>
#include <string>

class LocalSearch{
    public:
        LocalSearch(Input *input);
        ~LocalSearch();
        void GILSRVND(int &Imax, int &Iils, vector<double> &R, Solution& s_star);
        void RVND(Solution* s);
        double randomValue(vector<double> &R);
        string randomNeighborhood();
        void deleteNeighborhood(int &choosenNeighborhood);
        double f(Solution* s);
        friend ostream & operator << (ostream &out, LocalSearch &c);
    private:
        vector<double> tempo;
        vector<int> NL;
        int iterILS;
        double alpha;
        Solution* s;
        Solution* s_line;
        Solution* s_rvnd;
        Construction* c;
        Perturbation* p;
        Neighborhood* n;
        Input* in;
};

#endif //LOCALSEARCH_H_INCLUDED