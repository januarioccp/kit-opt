#include "neighborhood.h"
#include "solution.h"
#include <iostream>
#include <cstdlib>
#include <fstream>
#include <cmath>
#include <iomanip>
#include <set>
#include <algorithm> 
using namespace std;

Neighborhood::Neighborhood( Input* input)
{
    this->in = input;
}

void Neighborhood::firstSwap(Solution* s){
    bool stuck = false;
    int delta = 0;
    while(!stuck){
        stuck = true;
        for(int i=0; i < s->location.size()-1; i++)
            for(int j=i+1; j < s->location.size()-1; j++){
                delta = swapDeltaEvaluate(s,i,j);
                if(delta < 0 && j>i+1){
                    cout<<i<<" "<<j<<endl;
                    cout<<s->location[i]<<" "<<s->location[j]<<endl;
                    cout<<*s<<endl;
                    swapMove(s,i,j,delta);
                    cout<<*s<<endl;
                    s->computeCostValue();
                    cout<<*s<<endl;
                    //exit(0);
                    stuck = false;
                }
            }
    }
    exit(0);
}

int Neighborhood::swapDeltaEvaluate(Solution* s,int i,int j){
    // cij−1 + cij+1 + cji−1 + cji+1 − cii−1 − cii+1 − cjj−1 − cjj+1 
    int delta = 0;
    int last = s->location.size()-2;
    if( i == 0){
        delta  =    in->matrizAdj[s->location[i]][s->location[j-1]] +
                    in->matrizAdj[s->location[i]][s->location[j+1]] +
                    in->matrizAdj[s->location[j]][s->location[last]] +
                    in->matrizAdj[s->location[j]][s->location[i+1]] -
                    in->matrizAdj[s->location[i]][s->location[last]] -
                    in->matrizAdj[s->location[i]][s->location[i+1]] -
                    in->matrizAdj[s->location[j]][s->location[j-1]] -
                    in->matrizAdj[s->location[j]][s->location[j+1]];
        
    }
    else
        delta  =    in->matrizAdj[s->location[i]][s->location[j-1]] +
                    in->matrizAdj[s->location[i]][s->location[j+1]] +
                    in->matrizAdj[s->location[j]][s->location[i-1]] +
                    in->matrizAdj[s->location[j]][s->location[i+1]] -
                    in->matrizAdj[s->location[i]][s->location[i-1]] -
                    in->matrizAdj[s->location[i]][s->location[i+1]] -
                    in->matrizAdj[s->location[j]][s->location[j-1]] -
                    in->matrizAdj[s->location[j]][s->location[j+1]];
    
    return delta;
}

void Neighborhood::swapMove(Solution* s,int a,int b,int delta){
    if(a > b)
        swap(a,b);

    s->costValue+=delta;

    swap(s->location[a],s->location[b]);

    if(a==0)
        s->location[s->location.size()-1] = s->location[a];
    
}

