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
    c.constructiveProcedure(&sol,0.1);
    Neighborhood n(&in);
    sol.computeCostValue();
    cout<<sol<<endl;
    n.firstSwap(&sol);
    cout<<sol<<endl;
    n.firstSwap(&sol);
    cout<<sol<<endl;
    return 0;
}
