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

LocalSearch::LocalSearch(Input *input)
{

    this->in = input;
    s = new Solution(in);
    s_line = new Solution(in);
    s_rvnd = new Solution(in);
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

ostream &operator<<(ostream &out, LocalSearch &ls)
{
    // out << setw(14) << "RND:\t" << ls.tempo[0] / 1000000.0 << endl;

    // out << setw(14) << "Constructive:\t" << ls.tempo[1] / 1000000.0 << endl;

    out << setw(14) << "Swap:\t" << ls.tempo[2] / 1000000.0 << endl;

    out << setw(14) << "2-opt:\t" << ls.tempo[3] / 1000000.0 << endl;

    out << setw(14) << "re-insertion:\t" << ls.tempo[4] / 1000000.0 << endl;

    out << setw(14) << "or-opt2:\t" << ls.tempo[5] / 1000000.0 << endl;

    out << setw(14) << "or-opt3:\t" << ls.tempo[6] / 1000000.0 << endl;

    out << setw(14) << "Perturbation:\t" << ls.tempo[7] / 1000000.0 << endl;

    return out;
}

// GILS-RVND from EJOR2012-Marcos
void LocalSearch::GILSRVND(int &Imax, int &Iils, vector<double> &R, Solution &s_star)
{
    std::chrono::time_point<std::chrono::system_clock> temp1, temp2;
    s_star.costValueMLP = INT_MAX;
    s_line->costValueMLP = INT_MAX;

    for (int i = 0; i < Imax; i++)
    {
        alpha = R[rand() % 26];

        c->constructiveProcedure(s, alpha);
        s_line->copy(s);

        iterILS = 0;
        while (iterILS < Iils)
        {

            RVND(s);

            if (f(s) < f(s_line))
            {
                s_line->copy(s);
                iterILS = 0;
            } //end_if
            s->copy(s_line);
            temp1 = std::chrono::system_clock::now();
            p->bridgePerturbation(s);
            temp2 = std::chrono::system_clock::now();
            tempo[7] += std::chrono::duration_cast<std::chrono::microseconds>(temp2 - temp1).count();
            iterILS = iterILS + 1;
        } //end_while
        if (f(s_line) < f(&s_star))
        {
            s_star.copy(s_line);
        }
    }

    cout << *n << endl;
}

double LocalSearch::f(Solution *s)
{
    return s->costValueMLP;
}

double LocalSearch::randomValue(vector<double> &R)
{
    return R[rand() % R.size()];
}

void LocalSearch::deleteNeighborhood(int &choosenNeighborhood)
{
    auto it = find(NL.begin(), NL.end(), choosenNeighborhood);
    if (it != NL.end())
        NL.erase(it);
}

void LocalSearch::RVND(Solution *s)
{
    std::chrono::time_point<std::chrono::system_clock> temp1, temp2;
    (*s_rvnd) = (*s);
    NL = n->NeighborhoodList();
    int choosenNeighborhood;
    while (!NL.empty())
    {
        choosenNeighborhood = rand() % NL.size();

        switch (NL[choosenNeighborhood])
        {
        case 0:
            n->Swap(s_rvnd);
            break;
        case 1:
            n->bestTwoOpt(s_rvnd);
            break;
        case 2:
            n->bestReInsertion(s_rvnd, 1);
            break;
        case 3:
            n->bestReInsertion(s_rvnd, 2);
            break;
        case 4:
            n->bestReInsertion(s_rvnd, 3);
            break;
        default:
            break;
        }

        if (f(s_rvnd) < f(s))
        {
            (*s) = (*s_rvnd);
            NL = n->NeighborhoodList();
        }
        else{
            NL.erase(NL.begin() + choosenNeighborhood);
        }
    }
}
