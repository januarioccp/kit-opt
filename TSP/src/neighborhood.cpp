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
                if(delta < 0){
                    swapMove(s,i,j,delta);
                    s->computeCostValue();
                    stuck = false;
                }
            }
    }
}

int Neighborhood::swapDeltaEvaluate(Solution* s,int i,int j){
    
    if(i > j)
        swap(i,j);

    int delta = 0;
    int last = s->location.size()-2;
    if( i == 0){
        if(j==1){ //It doesn't work for non simmetric instances
            return
            in->matrizAdj[s->location[last]][s->location[j]] +
            in->matrizAdj[s->location[i]][s->location[j+1]] -
            in->matrizAdj[s->location[last]][s->location[i]] -
            in->matrizAdj[s->location[j]][s->location[j+1]];
        } if(j==last){
            return in->matrizAdj[s->location[last]][s->location[i+1]] +
            in->matrizAdj[s->location[last-1]][s->location[i]] -
            in->matrizAdj[s->location[j]][s->location[j-1]] -
            in->matrizAdj[s->location[i]][s->location[i+1]];
        } else{
            return in->matrizAdj[s->location[last]][s->location[j]]
               + in->matrizAdj[s->location[j]][s->location[i+1]]
               + in->matrizAdj[s->location[j-1]][s->location[i]]
               + in->matrizAdj[s->location[i]][s->location[j+1]]
               - in->matrizAdj[s->location[last]][s->location[i]]
               - in->matrizAdj[s->location[i]][s->location[i+1]]
               - in->matrizAdj[s->location[j-1]][s->location[j]]
               - in->matrizAdj[s->location[j]][s->location[j+1]];
        }
    }
    else if(i+1==j){ //It doesn't work for non simmetric instances
        return
        in->matrizAdj[s->location[j]][s->location[i-1]] +
        in->matrizAdj[s->location[i]][s->location[j+1]] -
        in->matrizAdj[s->location[i]][s->location[i-1]] -
        in->matrizAdj[s->location[j]][s->location[j+1]];
    }
    else {// Exactly like in "O kit"
        return in->matrizAdj[s->location[i]][s->location[j-1]] +
                    in->matrizAdj[s->location[i]][s->location[j+1]] +
                    in->matrizAdj[s->location[j]][s->location[i-1]] +
                    in->matrizAdj[s->location[j]][s->location[i+1]] -
                    in->matrizAdj[s->location[i]][s->location[i-1]] -
                    in->matrizAdj[s->location[i]][s->location[i+1]] -
                    in->matrizAdj[s->location[j]][s->location[j-1]] -
                    in->matrizAdj[s->location[j]][s->location[j+1]];
    }
    
    //return delta;
}

void Neighborhood::swapMove(Solution* s,int a,int b,int delta){
    if(a > b)
        swap(a,b);

    s->costValue+=delta;

    swap(s->location[a],s->location[b]);

    if(a==0)
        s->location[s->location.size()-1] = s->location[a];
    
}

