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
#include <chrono>
#include <climits>
using namespace std;

LocalSearch::LocalSearch(Input* input)
{
    
    this->in = input;
    s =  new Solution(in);
    s_line =  new Solution(in);
    s_rvnd =  new Solution(in);
    c = new Construction(s);
    p = new Perturbation(in);
    n = new Neighborhood(in);
    tempo.resize(8);
}

LocalSearch::~LocalSearch()
{
    s->deleteMe();
    delete s;
    
    s_line->deleteMe();
    delete s_line;

    s_rvnd->deleteMe();
    delete s_rvnd;
    delete c;
    delete p;
    delete n;
}

ostream & operator << (ostream &out, LocalSearch &ls){
    out << setw(14) <<
    "RND:\t" << 
    ls.tempo[0]/1000000.0 << endl;

    out << setw(14) <<
    "Constructive:\t" << 
    ls.tempo[1]/1000000.0 << endl;

    out << setw(14) <<
    "Swap:\t" << 
    ls.tempo[2]/1000000.0 << endl;

    out << setw(14) <<
    "2-opt:\t" << 
    ls.tempo[3]/1000000.0 << endl;

    out << setw(14) <<
    "re-insertion:\t" << 
    ls.tempo[4]/1000000.0 << endl;

    out << setw(14) <<
    "or-opt2:\t" << 
    ls.tempo[5]/1000000.0 << endl;

    out << setw(14) <<
    "or-opt3:\t" << 
    ls.tempo[6]/1000000.0 << endl;

    out << setw(14)<<
    "Perturbation:\t" << 
    ls.tempo[7]/1000000.0 << endl;

    return out;
}

// GILS-RVND from EJOR2012-Marcos
void LocalSearch::GILSRVND(int Imax, int Iils, vector<double> R, Solution& s_star){
    Color::Modifier red(Color::FG_RED);
    Color::Modifier deff(Color::FG_DEFAULT);
    std::chrono::time_point<std::chrono::system_clock> temp1, temp2;
    s_star.costValueMLP = INT_MAX;
    s_line->costValueMLP = INT_MAX;

    for(int i = 1; i <= Imax; i++){
        alpha = randomValue(R);      
        temp1 = std::chrono::system_clock::now();  
        c->constructiveProcedure(s,alpha);
        temp2 = std::chrono::system_clock::now();
            tempo[1] += std::chrono::duration_cast<std::chrono::microseconds>(temp2 - temp1).count();
        s_line->copy(s);
        iterILS = 0;
        while(iterILS < Iils){
            temp1 = std::chrono::system_clock::now();
            RVND(s);
            temp2 = std::chrono::system_clock::now();
            tempo[0] += std::chrono::duration_cast<std::chrono::microseconds>(temp2 - temp1).count();
            if(f(s) < f(s_line)){
                s_line->copy(s);
                iterILS = 0;
            }//end_if
            s->copy(s_line);
            temp1 = std::chrono::system_clock::now();
            p->bridgePerturbation(s);
            temp2 = std::chrono::system_clock::now();
            tempo[7] += std::chrono::duration_cast<std::chrono::microseconds>(temp2 - temp1).count();
            iterILS = iterILS + 1;
        }//end_while
        if(f(s_line) < f(&s_star)){
            s_star.copy(s_line);
        }
    }

    // cout<<*n<<endl;
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
    std::chrono::time_point<std::chrono::system_clock> temp1, temp2;
    (*s_rvnd) = (*s);
    NL = n->NeighborhoodList();
    string choosenNeighborhood;
    while(NL.size() != 0){
        choosenNeighborhood = randomNeighborhood();
        temp1 = std::chrono::system_clock::now();
        n->improove(s_rvnd,choosenNeighborhood);
        temp2 = std::chrono::system_clock::now();

        if(choosenNeighborhood == "bestSwap")
            tempo[2] += std::chrono::duration_cast<std::chrono::microseconds>(temp2 - temp1).count();
        else if(choosenNeighborhood == "bestTwoOpt")
            tempo[3] += std::chrono::duration_cast<std::chrono::microseconds>(temp2 - temp1).count();
        else if(choosenNeighborhood == "bestReInsertion-1")
            tempo[4] += std::chrono::duration_cast<std::chrono::microseconds>(temp2 - temp1).count();
        else if(choosenNeighborhood == "bestReInsertion-2")
            tempo[5] += std::chrono::duration_cast<std::chrono::microseconds>(temp2 - temp1).count();
        else if(choosenNeighborhood == "bestReInsertion-3")
            tempo[6] += std::chrono::duration_cast<std::chrono::microseconds>(temp2 - temp1).count();

        if(f(s_rvnd) < f(s)){
            (*s) = (*s_rvnd);
        }
        else
            deleteNeighborhood(choosenNeighborhood);
    }
}
