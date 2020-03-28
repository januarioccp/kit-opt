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
    //seed = 1585356357; //1585058652
    srand(seed);
    // cout<<seed<<endl;

    Input in(argc, argv);
    in.problemSet(1);

    Solution sol(&in);

    LocalSearch ls(&in);

    // BEGIN GILSRVND parameters
    int Imax = 10;
    int Iils = min(in.dimensionGet(),unsigned(100));
    vector<double> R;
    R.push_back(0.00);
    for(int i=1; i <= 25; i++)
        R.push_back(R[i-1]+0.01);
    // END GILSRVND parameters

    for(int i=1; i <=10; i++ ){
        seed++;
        srand(seed);
        clock_t beginC = clock();
        
        ls.GILSRVND(Imax,Iils,R,sol);
        
        clock_t endC = clock();
        double elapsed_secs = double(endC - beginC) / CLOCKS_PER_SEC;
        printf("%.*s", int(strlen(argv[1]))-14, argv[1] + 10);
        cout<<","<<elapsed_secs<<",";
        cout<<fixed<<sol.costValueMLP;
        cout<<","<<seed<<endl;
    }

    // cout<<ls<<endl;

    // cout<<sol<<endl;

    sol.deleteMe();
    return 0;
}
