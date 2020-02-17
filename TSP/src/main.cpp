#include "input.h"
#include "solution.h"
#include "construction.h"
#include <fstream>
#include <iostream>
using namespace std;

int main(int argc, char** argv) {
    double alpha = 0.1;
    Input in(argc, argv);
    Solution sol(&in);
    Construction c(&sol,alpha);
    c.calculaCustoInsercao(&sol);
    return 0;
}
