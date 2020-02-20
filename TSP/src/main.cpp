#include "input.h"
#include "solution.h"
#include "construction.h"
#include "neighborhood.h"
#include <fstream>
#include <iostream>
using namespace std;

int main(int argc, char** argv) {
    srand(time(0));
    //srand(0);
    double alpha = 0.1;
    Input in(argc, argv);
    Solution sol(&in);
    Construction c(&sol);
    c.constructiveProcedure(&sol,0.05);
    Neighborhood n(&in);
    sol.computeCostValue();

    int f;
    cout<<sol<<endl;
    do{
        f = sol.costValue;
        n.firstSwap(&sol);
        n.firstTwoOpt(&sol);
        n.bestSwap(&sol);
        n.bestTwoOpt(&sol);
        cout<<sol<<endl;
    }while(f > sol.costValue);
    
    return 0;
}
