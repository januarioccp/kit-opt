#include "input.h"
#include "solution.h"
#include "construction.h"
#include "neighborhood.h"
#include "perturbation.h"
#include "localsearch.h"
#include <fstream>
#include <iostream>
#include <ctime>
using namespace std;

int main(int argc, char** argv) {
    int seed = time(0);
    srand(seed);
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

    clock_t beginC = clock();
    // std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
    sol = ls.GILSRVND(Imax,Iils,R);
    // std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
    clock_t endC = clock();

    // std::cout << "Time difference = " << std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count() << "[µs]" << std::endl;
    // std::cout << "Time difference = " << std::chrono::duration_cast<std::chrono::nanoseconds> (end - begin).count() << "[ns]" << std::endl;
    double elapsed_secs = double(endC - beginC) / CLOCKS_PER_SEC;

    cout<<"{";
    printf("%.*s", strlen(argv[1])-14, argv[1] + 10);
    cout<<","<<elapsed_secs<<","<<sol.costValue<<","<<seed<<"}"<<endl;

    return 0;
}
