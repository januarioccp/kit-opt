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
    //srand(1582915991);

    Input in(argc, argv);
    Solution sol(&in);
    LocalSearch ls(&in);
    
    // GILSRVND parameters
    int Imax = 10;
    int Iils = min(100,in.dimension);
    vector<double> R;
    R.push_back(0.00);
    for(int i=1; i <= 25; i++)
        R.push_back(R[i-1]+0.01);
    // GILSRVND parameters

    
    for(int i=1; i <=10; i++ ){
        seed++;
        srand(seed);
        clock_t beginC = clock();
        sol = ls.GILSRVND(Imax,Iils,R);
        clock_t endC = clock();
        double elapsed_secs = double(endC - beginC) / CLOCKS_PER_SEC;
        printf("%.*s", strlen(argv[1])-14, argv[1] + 10);
        cout<<","<<elapsed_secs<<","<<sol.costValue<<","<<seed<<endl;
    }
    

    

    return 0;
}
