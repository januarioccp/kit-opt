#ifndef PERTURBATION_H_INCLUDED
#define PERTURBATION_H_INCLUDED
#include "solution.h"
#include "colormod.h"

class Perturbation{
    public:
        Perturbation(Input *input);
        ~Perturbation();

        /**
         * Double Bridge
        */
        void bridgePerturbation(Solution* s, int maxBridges);
        vector<pair<int,int> > makeBridges(const Solution* s,int bridge);

    //private:
    Input* in;
    Solution* localSolution;
    friend ostream & operator << (ostream &out, const Perturbation &c);
    
};

#endif //PERTURBATION_H_INCLUDED