#include "input.h"
#include "solution.h"
#include "construction.h"
#include "neighborhood.h"
#include "perturbation.h"
#include "localsearch.h"
#include "statistics.h"
#include <cstring>
#include <fstream>
#include <iostream>
#include <ctime>
using namespace std;

#include "hungarian.h"

int main(int argc, char** argv) {
    int seed = time(0);
    // seed = 1582915991; 
    srand(seed);

    Input in(argc, argv);
    in.problemSet(1);
    Solution sol(&in);
    LocalSearch ls(&in);

    // GILSRVND parameters
    int Imax = 50;
    int Iils = (in.dimensionGet() >= 150) ? in.dimensionGet()/2 : in.dimensionGet();
    vector<double> R;
    R.push_back(0.00);
    for(int i=1; i <= 25; i++)
        R.push_back(R[i-1]+0.01);
    // GILSRVND parameters

    for(int i=1; i <=1; i++ ){
        seed++;
        srand(seed);
        clock_t beginC = clock();
        sol = ls.GILSRVND(Imax,Iils,R);
        clock_t endC = clock();
        double elapsed_secs = double(endC - beginC) / CLOCKS_PER_SEC;
        printf("%.*s", int(strlen(argv[1]))-14, argv[1] + 10);
        cout<<","<<elapsed_secs<<",";

        if(in.problemGet() == 0)
            cout<<sol.costValueTSP;
        else{
            sol.computeCostValueMLP();
            cout<<sol.costValueMLP;
        }

        cout<<","<<seed<<endl;
        // cout<<st<<endl;
    }

    return 0;
}
