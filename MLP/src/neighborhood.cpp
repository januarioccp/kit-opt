#include <climits>
#include "neighborhood.h"
#include "solution.h"
#include "colormod.h"
#include <iostream>
#include <cstdlib>
#include <fstream>
#include <cmath>
#include <iomanip>
#include <set>
#include <algorithm>
#include <climits>
using namespace std;

Neighborhood::Neighborhood(Input *input)
{
    this->in = input;

    NL.push_back("bestSwap");
    NL.push_back("bestTwoOpt");
    NL.push_back("bestReInsertion-1");
    NL.push_back("bestReInsertion-2");
    NL.push_back("bestReInsertion-3");
    // NL.push_back("bestReInsertion-4");

    // So far 4 subsequences is enough
    sigma.resize(in->dimensionGet());
    tempo.resize(10);
}

ostream &operator<<(ostream &out, const Neighborhood &nb)
{
    out << setw(20) << "re-insertion move:\t" << nb.tempo[0] / 1000000.0 << endl;

    out << setw(20) << "re-insertion delta:\t" << nb.tempo[1] / 1000000.0 << endl;

    // out << setw(20) << "re-insertion o<d:\t" << nb.tempo[2] / 1000000.0 << endl;

    // out << setw(20) << "re-insertion o>d:\t" << nb.tempo[3] / 1000000.0 << endl;

    return out;
}

void Neighborhood::improove(Solution *s, string choosenNeighborhood)
{
    if (choosenNeighborhood == "bestSwap")
        bestSwap(s);
    else if (choosenNeighborhood == "bestTwoOpt")
        bestTwoOpt(s);
    else if (choosenNeighborhood == "bestReInsertion-1")
        bestReInsertion(s, 1);
    else if (choosenNeighborhood == "bestReInsertion-2")
        bestReInsertion(s, 2);
    else if (choosenNeighborhood == "bestReInsertion-3")
        bestReInsertion(s, 3);
    // else if(choosenNeighborhood == "bestReInsertion-4")
    //     bestReInsertion(s,4);
}

void Neighborhood::firstSwap(Solution *s)
{
    double delta = 0;
    for (unsigned i = 0; i < s->location.size() - 1; i++)
        for (unsigned j = i + 1; j < s->location.size() - 1; j++)
        {
            delta = swapDeltaEvaluation(s, i, j);
            if (delta < 0)
            {
                swapMove(s, i, j, delta);
            }
        }
    //}
}

void Neighborhood::bestSwap(Solution *s)
{
    double delta = 0;
    int i_best;
    int j_best;
    double delta_best;

    delta_best = INT_MAX;
    for (unsigned i = 1; i < s->location.size() - 1; i++)
        for (unsigned j = i + 1; j < s->location.size() - 1; j++)
        {
            delta = swapDeltaEvaluation(s, i, j);
            if (delta < 0 && delta < delta_best)
            {
                delta_best = delta;
                i_best = i;
                j_best = j;
            }
        }
    if (delta_best < 0)
    {
        swapMove(s, i_best, j_best, delta_best);
    }
}

double Neighborhood::swapDeltaEvaluation(Solution *s, int i, int j)
{

    int t;
    int c;

    if (i == 0)
        return INT_MAX;

    if (i + 1 == j)
    { //consecutives = 3 operations
        a1 = 0;
        a2 = i - 1;

        sigma[1].first = j;
        sigma[1].second = j;

        sigma[2].first = i;
        sigma[2].second = i;

        sigma[3].first = j + 1;
        sigma[3].second = in->dimensionGet();

        t = s->T_recursive(a1, a2) +
            s->t_(a2, sigma[1].first) +
            s->T_recursive(sigma[1].first, sigma[1].second);

        c = s->C(a1, a2) +
            s->W(sigma[1].first, sigma[1].second) * (s->T_recursive(a1, a2) +
                                                     s->t_(a2, sigma[1].first)) +
            s->C(sigma[1].first, sigma[1].second);

        for (int sub = 1; sub < 3; sub++)
        {
            c = c +
                s->W(sigma[sub + 1].first, sigma[sub + 1].second) *
                    (t + s->t_(sigma[sub].second, sigma[sub + 1].first)) +
                s->cost[sigma[sub + 1].first][sigma[sub + 1].second];
            if (sub < 2)
                t = t +
                    in->dist[s->location[sigma[sub].second]][s->location[sigma[sub + 1].first]] +
                    s->duration[sigma[sub + 1].first][sigma[sub + 1].second];
        }

        return c - s->costValueMLP;
    }
    else
    {
        // nonconsecutives = 4 operations
        a1 = 0;
        a2 = i - 1;

        sigma[1].first = j;
        sigma[1].second = j;

        sigma[2].first = i + 1;
        sigma[2].second = j - 1;

        sigma[3].first = i;
        sigma[3].second = i;

        sigma[4].first = j + 1;
        sigma[4].second = in->dimensionGet();

        t = s->T_recursive(a1, a2) +
            s->t_(a2, sigma[1].first) +
            s->T_recursive(sigma[1].first, sigma[1].second);

        c = s->C(a1, a2) +
            s->W(sigma[1].first, sigma[1].second) * (s->T_recursive(a1, a2) +
                                                     s->t_(a2, sigma[1].first)) +
            s->C(sigma[1].first, sigma[1].second);

        for (int sub = 1; sub < 4; sub++)
        {
            c = c +
                s->W(sigma[sub + 1].first, sigma[sub + 1].second) *
                    (t + s->t_(sigma[sub].second, sigma[sub + 1].first)) +
                s->cost[sigma[sub + 1].first][sigma[sub + 1].second];
            if (sub < 3)
                t = t +
                    in->dist[s->location[sigma[sub].second]][s->location[sigma[sub + 1].first]] +
                    s->duration[sigma[sub + 1].first][sigma[sub + 1].second];
        }

        return c - s->costValueMLP;
    }
}

void Neighborhood::swapMove(Solution *s, int a, int b, double delta)
{
    // cout<<*in<<endl;

    if (a > b)
        swap(a, b);
    swap(s->location[a], s->location[b]);
    if (a == 0 || s->location[0] != 1)
    {
        cout << __FILE__ << ", line " << __LINE__ << endl;
        s->location[s->location.size() - 1] = s->location[a];
    }

    s->costValueMLP += delta;

    int v1, v2;
    v1 = s->costValueMLP;
    s->updateSwap(a, b);
    //s->computeCostValueMLP();
    v2 = s->costValueMLP;

    if (v1 != v2)
    {
        cout << __FILE__ << ", line " << __LINE__ << endl;
        exit(0);
    }
}

void Neighborhood::firstTwoOpt(Solution *s)
{

    double delta = 0;
    int last = s->location.size() - 1;
    //while(!stuck){
    for (int i = 0; i < last; i++)
        for (int j = i + 2; j < last; j++)
        {
            if (!(i == 0 && j == last - 1))
            {
                delta = twoOptDeltaEvaluation(s, i, j);
                if (delta < 0)
                {
                    twoOptMove(s, i, j, delta);
                }
            }
        }
    // }
}

void Neighborhood::bestTwoOpt(Solution *s)
{
    double delta = 0;
    int i_best;
    int j_best;
    double delta_best;

    int last = s->location.size() - 1;
    delta_best = INT_MAX;
    for (int i = 0; i < last; i++)
        for (int j = i + 2; j < last; j++)
        {
            if (!((i == 0 && j == last - 1) || (abs(i - j) < 2)))
            {
                delta = twoOptDeltaEvaluation(s, i, j);
                if (delta < 0 && delta < delta_best)
                {
                    delta_best = delta;
                    i_best = i;
                    j_best = j;
                }
            }
        }
    if (delta_best < 0)
    {
        twoOptMove(s, i_best, j_best, delta_best);
    }
}

double Neighborhood::twoOptDeltaEvaluation(Solution *s, int i, int j)
{
    if (i > j)
        swap(i, j);
    t = 0;
    c = 0;

    a1 = 0;
    a2 = i;

    sigma[1].first = j;
    sigma[1].second = i + 1;

    sigma[2].first = j + 1;
    sigma[2].second = in->dimensionGet();

    t = s->T_recursive(a1, a2) +
        s->t_(a2, sigma[1].first) +
        s->T_recursive(sigma[1].first, sigma[1].second);

    c = s->C(a1, a2) +
        s->W(sigma[1].first, sigma[1].second) * (s->T_recursive(a1, a2) +
                                                 s->t_(a2, sigma[1].first)) +
        s->C(sigma[1].first, sigma[1].second);

    int sub = 1;
    c = c +
        s->W(sigma[sub + 1].first, sigma[sub + 1].second) *
            (t + s->t_(sigma[sub].second, sigma[sub + 1].first)) +
        s->C(sigma[sub + 1].first, sigma[sub + 1].second);

    t = t +
        s->t_(sigma[sub].second, sigma[sub + 1].first) +
        s->T_recursive(sigma[sub + 1].first, sigma[sub + 1].second);

    return c - s->costValueMLP;
}

void Neighborhood::twoOptMove(Solution *s, int i, int j, double delta)
{
    if (i > j)
        swap(i, j);

    reverse(s->location.begin() + i + 1, s->location.begin() + j + 1);
    s->costValueMLP += delta;

    if (s->location[0] != 1)
    {
        cout << __FILE__ << ", line" << __LINE__ << endl;
    }

    int v1, v2;
    v1 = s->costValueMLP;
    s->update2opt(i, j);
    //s->computeCostValueMLP();
    v2 = s->costValueMLP;

    if (v1 != v2)
    {
        cout << __FILE__ << __LINE__ << endl;
        exit(0);
    }
}

void Neighborhood::bestReInsertion(Solution *s, int size)
{

    temp1 = std::chrono::system_clock::now();

    delta_best = INT_MAX;
    for (int origin = 1; origin < s->size - size; ++origin)
    {
        for (int destination = 1; destination < s->size - size; ++destination)
        {
            if (origin != destination && abs(origin - destination) > size)
            {
                delta = reInsertionDeltaEvaluation(s, origin, destination, size);

                if (delta < 0 && delta < delta_best)
                {
                    delta_best = delta;
                    origin_best = origin;
                    destination_best = destination;
                }
            }
        }
    }
    temp2 = std::chrono::system_clock::now();
    if (size == 1)
        tempo[1] += std::chrono::duration_cast<std::chrono::microseconds>(temp2 - temp1).count();
    if (delta_best < 0)
    {
        temp1 = std::chrono::system_clock::now();
        reInsertionMove(s, origin_best, destination_best, size, delta_best);
        temp2 = std::chrono::system_clock::now();
        if (size == 1)
            tempo[0] += std::chrono::duration_cast<std::chrono::microseconds>(temp2 - temp1).count();
    }
}

double Neighborhood::reInsertionDeltaEvaluation(Solution *s, int origin, int destination, int size)
{

    t = 0;
    c = 0;

    if (origin < destination)
    {
        a1 = 0;
        a2 = origin - 1;

        b1 = origin + size;
        b2 = destination + size - 1;

        c1 = origin;
        c2 = origin + size - 1;

        d1 = destination + size;
        d2 = in->dimensionGet();

        t = s->duration[a1][a2] + in->dist[s->location[a2]][s->location[b1]] + s->duration[b1][b2];
        c = s->cost[a1][a2] + s->W(b1, b2) * (s->duration[a1][a2] + s->t_(a2, b1)) + s->cost[b1][b2];
        c = c + s->W(c1, c2) * (t + s->t_(b2, c1)) + s->cost[c1][c2];
        t = t + in->dist[s->location[b2]][s->location[c1]] + s->duration[c1][c2];
        c = c + s->W(d1, d2) * (t + s->t_(c2, d1)) + s->cost[d1][d2];

        return c - s->costValueMLP;
    }
    else
    {
        a1 = 0;
        a2 = destination - 1;

        b1 = origin;
        b2 = origin + size - 1;

        c1 = destination;
        c2 = origin - 1;

        d1 = origin + size;
        d2 = in->dimensionGet();

        t = s->duration[a1][a2] + in->dist[s->location[a2]][s->location[b1]] + s->duration[b1][b2];
        c = s->cost[a1][a2] + s->W(b1, b2) * (s->duration[a1][a2] + s->t_(a2, b1)) + s->cost[b1][b2];
        c = c + s->W(c1, c2) * (t + s->t_(b2, c1)) + s->cost[c1][c2];
        t = t + in->dist[s->location[b2]][s->location[c1]] + s->duration[c1][c2];
        c = c + s->W(d1, d2) * (t + s->t_(c2, d1)) + s->cost[d1][d2];

        return c - s->costValueMLP;
    }
}

void Neighborhood::reInsertionMove(Solution *s, int origin, int destination, int size, double delta)
{

    int last = s->location.size();
    s->costValueMLP += delta;
    if (origin < destination)
    {
        rotate(s->location.begin() + origin, s->location.begin() + origin + size, s->location.begin() + destination + size);
        s->update2opt(origin, destination + size);
    }
    else
    {
        rotate(s->location.begin() + destination, s->location.begin() + s->location.size() - (last - origin), s->location.end() - (last - origin) + size);
        s->update2opt(destination, origin + size);
    }
}
