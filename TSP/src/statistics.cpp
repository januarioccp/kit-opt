#include "statistics.h"
#include <iostream>
#include <cstdlib>
#include <fstream>
#include <cmath>
#include <iomanip>
#include <set>
#include <algorithm> 
using namespace std;

Statistics::Statistics(int samples)
{
    on();
    myClock.resize(samples);

}

void Statistics::insert(clock_t beginC, clock_t endC, int index){
    if(collectStatistics){
        myClock[index] += endC - beginC;        
    }
}

ostream & operator << (ostream &out, const Statistics &c){
    Color::Modifier red(Color::FG_RED);
    Color::Modifier green(Color::GREEN);
    Color::Modifier blue(Color::FG_BLUE);
    Color::Modifier deff(Color::FG_DEFAULT);


    out<<"bestSwap\t"<<c.myClock[0]/CLOCKS_PER_SEC;
    
    return out;
}