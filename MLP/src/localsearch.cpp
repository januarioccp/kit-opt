#include "neighborhood.h"
#include "localsearch.h"
#include "solution.h"
#include "colormod.h"
#include "construction.h"
#include "perturbation.h"
#include <iostream>
#include <cstdlib>
#include <fstream>
#include <cmath>
#include <iomanip>
#include <set>
#include <vector>
#include <algorithm> 
using namespace std;

LocalSearch::LocalSearch( Input* input)
{
    this->in = input;
    s =  new Solution(in);
    s_star =  new Solution(in);
    s_line =  new Solution(in);
    s_rvnd =  new Solution(in);
    c = new Construction(s);
    p = new Perturbation(in);
    n = new Neighborhood(in);
    tempo.resize(10);
}

LocalSearch::~LocalSearch()
{
    delete s;
    delete s_star;
    delete s_line;
    delete s_rvnd;
    delete c;
    delete p;
    delete n;
}

void LocalSearch::reset(){
    s_star->reset();
    s_line->reset();
}

// GILS-RVND from EJOR2012-Marcos
Solution LocalSearch::GILSRVND(int Imax, int Iils, vector<double> R){
    Color::Modifier red(Color::FG_RED);
    Color::Modifier deff(Color::FG_DEFAULT);
    reset();

    for(int i = 1; i <= Imax; i++){
        alpha = randomValue(R);        
        c->constructiveProcedure(s,alpha);
        s_line->copy(s);
        iterILS = 0;
        while(iterILS < Iils){
            RVND(s);
            if(f(s) < f(s_line)){
                s_line->copy(s);
                iterILS = 0;
            }//end_if
            s->copy(s_line);
            p->bridgePerturbation(s,4);
            iterILS = iterILS + 1;
        }//end_while
        if(f(s_line) < f(s_star)){
            s_star->copy(s_line);
        }
    }
    return (*s_star);
}

double LocalSearch::f(Solution* s){
        return s->costValueMLP;
}

double LocalSearch::randomValue(vector<double> R){
    return R[rand()%R.size()];
}

string LocalSearch::randomNeighborhood(){
    return NL[rand()%NL.size()];
}

void LocalSearch::deleteNeighborhood(string choosenNeighborhood){
    auto it = find(NL.begin(), NL.end(), choosenNeighborhood);
    if(it != NL.end())
        NL.erase(it);
}

void LocalSearch::RVND(Solution* s){
    (*s_rvnd) = (*s);
    NL = n->NeighborhoodList();
    string choosenNeighborhood;
    while(NL.size() != 0){
        choosenNeighborhood = randomNeighborhood();
        n->improove(s_rvnd,choosenNeighborhood);
        if(f(s_rvnd) < f(s)){
            (*s) = (*s_rvnd);
        }
        else
            deleteNeighborhood(choosenNeighborhood);
    }
}
