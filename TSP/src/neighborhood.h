#ifndef NEIGHBORHOOD_H_INCLUDED
#define NEIGHBORHOOD_H_INCLUDED
#include "solution.h"

class Neighborhood{
    public:
        Neighborhood(Input *input);

        /**
         * SWAP
        */
        void bestSwap(Solution* s);
        void firstSwap(Solution* s);
        int swapDeltaEvaluation(Solution* s,int i,int j);
        void swapMove(Solution* s,int i,int j, int delta);

        /**
         * 2-opt
        */
        void firstTwoOpt(Solution* s);
        void bestTwoOpt(Solution* s);
        int twoOptDeltaEvaluation(Solution* s,int arci,int arcj);
        void twoOptMove(Solution* s, int arci,int arcj, int delta);

        /**
         * reInsertion
        */
        void reInsertion(Solution* s);
        

    //private:
    Input* in;
    friend ostream & operator << (ostream &out, const Neighborhood &c);
};

#endif //CONSTRUCTION_H_INCLUDED