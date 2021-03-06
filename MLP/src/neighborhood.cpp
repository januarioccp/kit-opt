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

    for(int i=0; i < 5; i++)
        NL.push_back(i);

    tempo.resize(10);
    contador.resize(20);
}

ostream &operator<<(ostream &out, const Neighborhood &nb)
{
    out << setw(20) << "re-insertion move:\t" << nb.tempo[0] / 1000000.0 << " seconds\t"<<nb.contador[0]<<" times"<<endl;

    out << setw(20) << "re-insertion delta:\t" << nb.tempo[1] / 1000000.0 <<" seconds\t"<<nb.contador[1]<<" times"<<endl;

    out << setw(20) << "or-opt2 move:\t" << nb.tempo[2] / 1000000.0 << " seconds\t"<<nb.contador[2]<<" times"<<endl;

    out << setw(20) << "or-opt2 delta:\t" << nb.tempo[3] / 1000000.0 << " seconds\t"<<nb.contador[3]<<" times"<<endl;

    out << setw(20) << "or-opt3 move:\t" << nb.tempo[4] / 1000000.0 << " seconds\t"<<nb.contador[4]<<" times"<<endl;

    out << setw(20) << "or-opt3 delta:\t" << nb.tempo[5] / 1000000.0 << " seconds\t"<<nb.contador[5]<<" times"<<endl;

    out << setw(20) << "swap move:\t" << nb.tempo[6] / 1000000.0 << " seconds\t"<<nb.contador[6]<<" times"<<endl;

    out << setw(20) << "swap delta:\t" << nb.tempo[7] / 1000000.0 << " seconds\t"<<nb.contador[7]<<" times"<<endl;

    out << setw(20) << "2opt move:\t" << nb.tempo[8] / 1000000.0 << " seconds\t"<<nb.contador[8]<<" times"<<endl;

    out << setw(20) << "2opt delta:\t" << nb.tempo[9] / 1000000.0 << " seconds\t"<<nb.contador[9]<<" times"<<endl;

    return out;
}

void Neighborhood::Swap(Solution *s)
{
    int delta = 0;
    int i_best;
    int j_best;
    int delta_best = delta_best = INT_MAX;

    temp1 = std::chrono::system_clock::now();
    for (unsigned i = 1; i < s->location.size() - 2; i++)
        for (unsigned j = i + 1; j < s->location.size() - 1; j++)
        {
            delta = 0;
            if (i + 1 == j)
            { //consecutives = 3 operations
                a1 = 0;
                a2 = i - 1;

                b1 = j;
                b2 = j;

                c1 = i;
                c2 = i;

                d1 = j + 1;
                d2 = in->dimension_;

                // e1;
                // e2;

                ta = s->reOpt[0][a1][a2] + in->dist[s->location[a2]][s->location[b1]] + s->reOpt[0][b1][b2];
                ca = s->reOpt[1][a1][a2] + (b2 - b1 + 1) * (s->reOpt[0][a1][a2] + in->dist[s->location[a2]][s->location[b1]]) + s->reOpt[1][b1][b2];
                // wa = s->W(a1,a2) + s->W(b1,b2);

                tb = s->reOpt[0][c1][c2] + in->dist[s->location[c2]][s->location[d1]] + s->reOpt[0][d1][d2];
                cb = s->reOpt[1][c1][c2] + (d2 - d1 + 1) * (s->reOpt[0][c1][c2] + in->dist[s->location[c2]][s->location[d1]]) + s->reOpt[1][d1][d2];
                wb = c2 - c1 + d2 - d1 + 2;

                cc = ca + wb * (ta + in->dist[s->location[b2]][s->location[c1]]) + cb;
                // tc = ta + in->dist[s->location[b2]][s->location[c1]] + tb;

                delta = cc - s->costValueMLP;
            }
            else{
                a1 = 0;
                a2 = i - 1;

                b1 = j;
                b2 = j;

                c1 = i + 1;
                c2 = j - 1;

                d1 = i;
                d2 = i;

                e1 = j + 1;
                e2 = in->dimension_;

                ta = s->reOpt[0][a1][a2] + in->dist[s->location[a2]][s->location[b1]] + s->reOpt[0][b1][b2];
                ca = s->reOpt[1][a1][a2] + (b2 - b1 +1) * (s->reOpt[0][a1][a2] + s->t_(a2, b1)) + s->reOpt[1][b1][b2];
                cb = ca + (c2 - c1 + 1) * (ta + s->t_(b2, c1)) + s->reOpt[1][c1][c2];
                tb = ta + in->dist[s->location[b2]][s->location[c1]] + s->reOpt[0][c1][c2];
                cc = cb + (d2 - d1 +1) * (tb + s->t_(c2, d1)) + s->reOpt[1][d1][d2];
                tc = tb + in->dist[s->location[c2]][s->location[d1]] + s->reOpt[0][d1][d2];
                cd = cc + (e2 - e1 +1) * (tc + s->t_(d2, e1)) + s->reOpt[1][e1][e2];
                delta = cd - s->costValueMLP;
            }
            if (delta < 0 && delta < delta_best)
            {
                delta_best = delta;
                i_best = i;
                j_best = j;
            }
        }
        temp2 = std::chrono::system_clock::now();
        tempo[7] += std::chrono::duration_cast<std::chrono::microseconds>(temp2 - temp1).count();
        contador[7]++;
    if (delta_best < 0)
    {
        temp1 = std::chrono::system_clock::now();
        swapMove(s, i_best, j_best, delta_best);
        temp2 = std::chrono::system_clock::now();
        tempo[6] += chrono::duration_cast<chrono::microseconds>(temp2 - temp1).count();
        contador[6]++;
    }
}

int Neighborhood::swapDeltaEvaluation(Solution *s, int i, int j)
{

        // nonconsecutives = 4 operations
        a1 = 0;
        a2 = i - 1;

        b1 = j;
        b2 = j;

        c1 = i + 1;
        c2 = j - 1;

        d1 = i;
        d2 = i;

        e1 = j + 1;
        e2 = in->dimensionGet();

        ta = s->reOpt[0][a1][a2] + in->dist[s->location[a2]][s->location[b1]] + s->reOpt[0][b1][b2];
        ca = s->reOpt[1][a1][a2] + s->W(b1, b2) * (s->reOpt[0][a1][a2] + s->t_(a2, b1)) + s->reOpt[1][b1][b2];
        cb = ca + s->W(c1, c2) * (ta + s->t_(b2, c1)) + s->reOpt[1][c1][c2];
        tb = ta + in->dist[s->location[b2]][s->location[c1]] + s->reOpt[0][c1][c2];
        cc = cb + s->W(d1, d2) * (tb + s->t_(c2, d1)) + s->reOpt[1][d1][d2];
        tc = tb + in->dist[s->location[c2]][s->location[d1]] + s->reOpt[0][d1][d2];
        cd = cc + s->W(e1, e2) * (tc + s->t_(d2, e1)) + s->reOpt[1][e1][e2];

        return cd - s->costValueMLP;
}

void Neighborhood::swapMove(Solution *s, int a, int b, int delta)
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
    v2 = s->costValueMLP;

    if (v1 != v2)
    {
        cout << __FILE__ << ", line " << __LINE__ << endl;
        exit(0);
    }
}

void Neighborhood::bestTwoOpt(Solution *s)
{
    int delta = 0;
    int i_best;
    int j_best;
    int delta_best = INT_MAX;
    int last = s->location.size() - 1;
    a1 = 0;
    c2 = in->dimensionGet();
    temp1 = std::chrono::system_clock::now();
    for (int i = 1; i < last; i++)
        for (int j = i + 1; j < last - 1; j++)
        {
                
                a2 = i;

                b1 = j;
                b2 = i + 1;

                c1 = j + 1;

                ta = s->reOpt[0][a1][a2] + in->dist[s->location[a2]][s->location[b1]] + s->reOpt[0][b1][b2];
                cb = s->reOpt[1][a1][a2] + (b1 - b2 + 1) * (s->reOpt[0][a1][a2] + s->t_(a2, b1)) + s->reOpt[1][b1][b2] +
                     (c2 - c1 + 1) * (ta + s->t_(b2, c1)) + s->reOpt[1][c1][c2];

                delta = cb - s->costValueMLP;

                if (delta < 0 && delta < delta_best)
                {
                    delta_best = delta;
                    i_best = i;
                    j_best = j;
                }
            
        }
    temp2 = std::chrono::system_clock::now();
    tempo[9] += std::chrono::duration_cast<std::chrono::microseconds>(temp2 - temp1).count();
    contador[9]++;
    if (delta_best < 0)
    {
        temp1 = std::chrono::system_clock::now();
        twoOptMove(s, i_best, j_best, delta_best);
        temp2 = std::chrono::system_clock::now();
        tempo[8] += std::chrono::duration_cast<std::chrono::microseconds>(temp2 - temp1).count();
        contador[8]++;
    }
}

int Neighborhood::twoOptDeltaEvaluation(Solution *s, int i, int j)
{
    if (i > j)
        swap(i, j);

    a1 = 0;
    a2 = i;

    b1 = j;
    b2 = i + 1;

    c1 = j + 1;
    c2 = in->dimensionGet();

    ta = s->reOpt[0][a1][a2] + in->dist[s->location[a2]][s->location[b1]] + s->reOpt[0][b1][b2];
    cb = s->reOpt[1][a1][a2] + s->W(b1, b2) * (s->reOpt[0][a1][a2] + s->t_(a2, b1)) + s->reOpt[1][b1][b2] +
         s->W(c1, c2) * (ta + s->t_(b2, c1)) + s->reOpt[1][c1][c2];

    return cb - s->costValueMLP;
}

void Neighborhood::twoOptMove(Solution *s, int i, int j, int delta)
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
    d2 = in->dimensionGet();
    delta_best = INT_MAX;
    for (int origin = 1; origin < s->size - size; ++origin)
    {
        for (int destination = 1; destination < s->size - size; ++destination)
        {
            delta = 0;
            if (origin < destination)
            {
                //continue;
                a1 = 0;
                a2 = origin - 1;

                b1 = origin + size;
                b2 = destination + size - 1;

                c1 = origin;
                c2 = origin + size - 1;

                d1 = destination + size;
                // d2 = in->dimensionGet();

                ta = s->reOpt[0][a1][a2] + in->dist[s->location[a2]][s->location[b1]] + s->reOpt[0][b1][b2];
                ca = s->reOpt[1][a1][a2] + (b2 - b1 + 1) * (s->reOpt[0][a1][a2] + in->dist[s->location[a2]][s->location[b1]]) + s->reOpt[1][b1][b2];
                // wa = s->W(a1,a2) + s->W(b1,b2);

                tb = s->reOpt[0][c1][c2] + in->dist[s->location[c2]][s->location[d1]] + s->reOpt[0][d1][d2];
                cb = s->reOpt[1][c1][c2] + (d2 - d1 + 1) * (s->reOpt[0][c1][c2] + in->dist[s->location[c2]][s->location[d1]]) + s->reOpt[1][d1][d2];
                wb = c2 - c1 + d2 - d1 + 2;

                cc = ca + wb * (ta + in->dist[s->location[b2]][s->location[c1]]) + cb;
                tc = ta + in->dist[s->location[b2]][s->location[c1]] + tb;

                delta = cc - s->costValueMLP;
            }
            if (origin > destination)
            {
                a1 = 0;
                a2 = destination - 1;

                b1 = origin;
                b2 = origin + size - 1;

                c1 = destination;
                c2 = origin - 1;

                d1 = origin + size;

                ta = s->reOpt[0][a1][a2] + in->dist[s->location[a2]][s->location[b1]] + s->reOpt[0][b1][b2];
                ca = s->reOpt[1][a1][a2] + (b2 - b1 + 1) * (s->reOpt[0][a1][a2] + in->dist[s->location[a2]][s->location[b1]]) + s->reOpt[1][b1][b2];
                // wa = s->W(a1,a2) + s->W(b1,b2);

                tb = s->reOpt[0][c1][c2] + in->dist[s->location[c2]][s->location[d1]] + s->reOpt[0][d1][d2];
                cb = s->reOpt[1][c1][c2] + (d2 - d1 + 1) * (s->reOpt[0][c1][c2] + in->dist[s->location[c2]][s->location[d1]]) + s->reOpt[1][d1][d2];
                wb = c2 - c1 + d2 - d1 + 2;

                cc = ca + wb * (ta + in->dist[s->location[b2]][s->location[c1]]) + cb;
                tc = ta + in->dist[s->location[b2]][s->location[c1]] + tb;

                delta = cc - s->costValueMLP;
            }

            if (delta < 0 && delta < delta_best)
            {
                delta_best = delta;
                origin_best = origin;
                destination_best = destination;
            }
        }
    }

    temp2 = std::chrono::system_clock::now();
    if (size == 1){
        tempo[1] += std::chrono::duration_cast<std::chrono::microseconds>(temp2 - temp1).count();
        contador[1]++;
    }
    if (size == 2){
        tempo[3] += std::chrono::duration_cast<std::chrono::microseconds>(temp2 - temp1).count();
        contador[3]++;
    }
    if (size == 3){
        tempo[5] += std::chrono::duration_cast<std::chrono::microseconds>(temp2 - temp1).count();
        contador[5]++;
    }
    if (delta_best < 0)
    {
        temp1 = std::chrono::system_clock::now();
        reInsertionMove(s, origin_best, destination_best, size, delta_best);
        temp2 = std::chrono::system_clock::now();
        if (size == 1){
            tempo[0] += std::chrono::duration_cast<std::chrono::microseconds>(temp2 - temp1).count();
            contador[0]++;
        }
        if (size == 2){
            tempo[2] += std::chrono::duration_cast<std::chrono::microseconds>(temp2 - temp1).count();
            contador[2]++;
        }
        if (size == 3){
            tempo[4] += std::chrono::duration_cast<std::chrono::microseconds>(temp2 - temp1).count();
            contador[4]++;
        }
    }
}

void Neighborhood::reInsertionMove(Solution *s, int &origin, int &destination, int &size, int &delta)
{

    int last = s->location.size();
    s->costValueMLP += delta;
    int v1 = s->costValueMLP;
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
    int v2 = s->costValueMLP;
    s->computeCostValueMLP();
    int v3 = s->costValueMLP;
    if (v1 != v2)
    {
        cout << v1 << " " << v2 << endl;
        cout << origin << " " << destination << endl;
        cout << __FILE__ << __LINE__ << endl;
        exit(0);
    }
}
