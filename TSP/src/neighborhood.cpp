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
                delta = swapDeltaEvaluation(s,i,j);
                if(delta < 0){
                    swapMove(s,i,j,delta);
                    stuck = false;
                }
            }
    }
}

void Neighborhood::bestSwap(Solution* s){
    bool stuck = false;
    int delta = 0;

    int i_best;
    int j_best;
    int delta_best;

    while(!stuck){
        stuck = true;
        delta_best = INT_MAX;
        for(int i=0; i < s->location.size()-1; i++)
            for(int j=i+1; j < s->location.size()-1; j++){
                delta = swapDeltaEvaluation(s,i,j);
                if(delta < 0 && delta < delta_best){
                    delta_best = delta;
                    i_best = i;
                    j_best = j;
                }
            }
        if(delta_best < 0){
            swapMove(s,i_best,j_best,delta_best);
            stuck = false;
        }
    }
}

int Neighborhood::swapDeltaEvaluation(Solution* s,int i,int j){
    
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

void Neighborhood::firstTwoOpt(Solution* s){
    bool stuck = false;
    int delta = 0;
    int last = s->location.size()-1;
    while(!stuck){
        stuck = true;
        for(int i=0; i < last; i++)
            for(int j=i+2; j < last; j++){
                if(!(i==0 && j == last-1)){
                    delta = twoOptDeltaEvaluation(s,i,j);
                    if(delta < 0){
                        twoOptMove(s,i,j,delta);
                        stuck = false;
                    }
                }
            }
    }
}

void Neighborhood::bestTwoOpt(Solution* s){
  bool stuck = false;
    int delta = 0;

    int i_best;
    int j_best;
    int delta_best;

    int last = s->location.size()-1;
    while(!stuck){
        stuck = true;
        delta_best = INT_MAX;
        for(int i=0; i < last; i++)
            for(int j=i+2; j < last; j++){
                if(!(i==0 && j == last-1)){
                    delta = twoOptDeltaEvaluation(s,i,j);
                    if(delta < 0 && delta < delta_best){
                        delta_best = delta;
                        i_best = i;
                        j_best = j;
                    }
                }
            }
        if(delta_best < 0){
            twoOptMove(s,i_best,j_best,delta_best);
            stuck = false;
        }
    }
}

int Neighborhood::twoOptDeltaEvaluation(Solution* s,int i,int j){
    if(i > j)
        swap(i,j);
    
    return
            in->matrizAdj[s->location[i]][s->location[j]] +
            in->matrizAdj[s->location[i+1]][s->location[j+1]] -
            in->matrizAdj[s->location[i]][s->location[i+1]] -
            in->matrizAdj[s->location[j]][s->location[j+1]];
}

void Neighborhood::twoOptMove(Solution* s,int i,int j, int delta){
    if(i > j)
        swap(i,j);
    reverse(s->location.begin()+i+1,s->location.begin()+j+1);
    s->costValue+=delta;
}
