#ifndef SOLUTION_H_INCLUDED
#define SOLUTION_H_INCLUDED
#include "input.h"
#include "colormod.h"
#include <vector>
#include <iostream>
#include <iomanip>
#include <set>
using namespace std;
class Solution{
    // Create get/set functions
    public:
        Solution(Input *in);
        void computeCostValue();
        void reset();

    //private:
        double costValue;
        vector<int> location;
        Input* in;

    friend ostream & operator << (ostream &out, const Solution &c);

};
#endif //SOLUTION_H_INCLUDED