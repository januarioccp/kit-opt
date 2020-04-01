#ifndef NEIGHBORHOOD_H_INCLUDED
#define NEIGHBORHOOD_H_INCLUDED
#include "solution.h"
#include "colormod.h"
#include <chrono>
using namespace std;

class Neighborhood{
    public:
        // Neighborhood(Input *input, Statistics *st);
        Neighborhood(Input *input);
        vector<string> NeighborhoodList(){return NL;};
        void improove(Solution* s,string choosenNeighborhood);
        /**
         * SWAP
        */
        void Swap(Solution* s);
        int swapDeltaEvaluation(Solution* s,int i,int j);
        void swapMove(Solution* s,int i,int j, int delta);

        /**
         * 2-opt
        */
        void bestTwoOpt(Solution* s);
        int twoOptDeltaEvaluation(Solution* s,int arci,int arcj);
        void twoOptMove(Solution* s, int arci,int arcj, int delta);

        /**
         * reInsertion
        */
        void bestReInsertion(Solution* s, int size = 1 );
        int reInsertionDeltaEvaluation(Solution* s, int arc, int size, int shift);
        void reInsertionMove(Solution* s, int arc, int size, int shift, int delta);
        

    //private:
    int delta;
    int origin_best;
    int destination_best;
    int delta_best;
    int t,c,w;
    int a1,a2,b1,b2,c1,c2,d1,d2,e1,e2;
    int ca,cb,cc,cd,ta,tb,tc,wa,wb,wc;
    chrono::time_point<chrono::system_clock> temp1, temp2;
    vector<double> tempo;
    vector<int> contador;
    Input* in;
    // Statistics* st;
    vector<string> NL;
    // vector<int> reInsertionAuxVector;
    friend ostream & operator << (ostream &out, const Neighborhood &c);
};

#endif //NEIGHBORHOOD_H_INCLUDED