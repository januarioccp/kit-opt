#ifndef STATISTICS_H_INCLUDED
#define STATISTICS_H_INCLUDED
#include "colormod.h"
#include <iostream>
#include <vector>
using namespace std;

class 
Statistics{
    public:
        Statistics(int samples);
        void on(){collectStatistics = true;};
        void off(){collectStatistics = false;};
        void insert(clock_t beginC, clock_t endC, int index);

    // private:
    vector<clock_t> myClock;
    bool collectStatistics;
    friend ostream & operator << (ostream &out, const Statistics &s);

    // 0 = bestSwap
};

#endif //STATISTICS_H_INCLUDED