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
        void bridgePerturbation(Solution* s);
        vector<pair<int,int> > makeBridges(const Solution* s,int bridge);

    //private:
    Input* in;
    friend ostream & operator << (ostream &out, const Perturbation &c);
    
};

#endif //PERTURBATION_H_INCLUDED