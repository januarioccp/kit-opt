#include "input.h"
#include "solution.h"
#include "construction.h"
#include "neighborhood.h"
#include "perturbation.h"
#include "localsearch.h"
#include <cstring>
#include <fstream>
#include <iostream>
#include <ctime>
using namespace std;

int main(int argc, char** argv) {
    int seed = time(0);
    //seed = 1584647643; 
    srand(seed);

    Input in(argc, argv);
    in.problemSet(1);

    int * format = new int[8];
    delete [] format;
    //Solution sol(&in);
    // LocalSearch ls(&in);

    // // BEGIN GILSRVND parameters
    // int Imax = 50;
    // int Iils = (in.dimensionGet() >= 150) ? in.dimensionGet()/2 : in.dimensionGet();
    // vector<double> R;
    // R.push_back(0.00);
    // for(int i=1; i <= 25; i++)
    //     R.push_back(R[i-1]+0.01);
    // // END GILSRVND parameters

    // for(int i=1; i <=1; i++ ){
    //     seed++;
    //     srand(seed);
    //     clock_t beginC = clock();
    //     sol = ls.GILSRVND(Imax,Iils,R);
    //     clock_t endC = clock();
    //     double elapsed_secs = double(endC - beginC) / CLOCKS_PER_SEC;
    //     printf("%.*s", int(strlen(argv[1]))-14, argv[1] + 10);
    //     cout<<","<<elapsed_secs<<",";
    //     cout<<sol.costValueMLP;
    //     cout<<","<<seed<<endl;
    // }

    return 0;
}
