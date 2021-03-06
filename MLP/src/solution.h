#ifndef SOLUTION_H_INCLUDED
#define SOLUTION_H_INCLUDED
#include "input.h"
#include "colormod.h"
#include <vector>
#include <iostream>
#include <iomanip>
#include <set>
#include <string>
using namespace std;

class Solution{
    // Create get/set functions
    public:
        Solution();
        Solution(Input *in);
        Solution(Solution &s);
        ~Solution();
        void deleteMe();
        void computeCostValueMLP();
        void reset();
        //int T(int begin, int end);
        int C(int begin, int end);

        int W(int begin, int end);
        int T_recursive(int begin, int end);

        void updateSwap(int a, int b);
        void update2opt(int a, int b);

    //private:
        int*** reOpt;

        double costValueMLP;
        
        int size;
        int dimension;

        vector<int> location;
        Input* in;

        int contador;

    friend ostream & operator << (ostream &out, const Solution &c);

    Solution& operator=(const Solution& other); // copy assignment

    double t_(unsigned i, unsigned j);

    void copy(const Solution *s);
    void printCost();
    void printDuration();

};
#endif //SOLUTION_H_INCLUDED