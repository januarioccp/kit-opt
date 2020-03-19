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

#include "hungarian.h"

int main(int argc, char** argv) {
    int seed = time(0);
    // seed = 1582915991; 
    srand(seed);

    Input in(argc, argv);
    in.problemSet(0);
    Solution sol(&in);
    LocalSearch ls(&in);
    
	// double **cost = new double*[in.dimensionGet()];
	// for (unsigned i = 0; i < in.dimensionGet(); i++){
	// 	cost[i] = new double[in.dimensionGet()];
	// 	for (unsigned j = 0; j < in.dimensionGet(); j++){
	// 		cost[i][j] = in.distanceGet(i,j);
	// 	}
	// }

    // hungarian_problem_t p;
	// int mode = HUNGARIAN_MODE_MINIMIZE_COST;
	// hungarian_init(&p, cost, in.dimensionGet(), in.dimensionGet(), mode); // Carregando o problema

	// double obj_value = hungarian_solve(&p);

	// // cout << "Obj. value: " << obj_value << endl;
	// // cout << "Assignment" << endl;
	// // hungarian_print_assignment(&p);

	// hungarian_free(&p);

	// for (unsigned i = 0; i < in.dimensionGet(); i++) 
    //     delete [] cost[i];
	// delete [] cost;

    // GILSRVND parameters
    int Imax = 50;
    int Iils = (in.dimensionGet() >= 150) ? in.dimensionGet()/2 : in.dimensionGet();
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
