#include "readData.h"
#include <algorithm>
#include <cmath>
#include <ctime>
#include <chrono>
#include <climits>
#include <cstring>
#include <fstream>
#include <iostream>
#include <vector>
#include <string>
using namespace std;

// Structures
struct InsertionInfo
{
    int noInserido;     // no k a ser inserido
    int arestaRemovida; // aresta {i,j} onde o no k sera inserido
    double custo;       // delta ao inserir k na aresta {i , j}
};

// Functions
void GILSRVND(int &Imax, int &Iils, vector<double> &R, vector<int> &s, int &costMLP);
void constructiveProcedure(vector<int> &s, int &costMLP, double &alpha);
vector<int> construction(double alpha);
static bool compareByCost(const InsertionInfo &a, const InsertionInfo &b) { return a.custo < b.custo; }
void calculaCustoInsercaoMLP(vector<int> &s);
void computeCostValueMLP(vector<int> &s);
void bridgePerturbation(vector<int> &s);
void RVND(vector<int> &s, int &costMLP);

void TwoOpt(vector<int> &s, int &costMLP);
void Swap(vector<int> &s, int &costMLP);
void ReInsertion(vector<int> &s, int &costMLP, int size);

void swapMove(vector<int> &s, int &costMLP, int &a, int &b, int &delta);
void twoOptMove(vector<int> &s, int &costMLP, int &i, int &j, int &delta);
void reInsertionMove(vector<int> &s, int &costMLP, int &origin, int &destination, int &size, int &delta);

void updateSwap(vector<int> &s, int a, int b);
void update2opt(vector<int> &s, int a, int b);

void init();

// Variables
vector<InsertionInfo> custoInsercao;
vector<int> s_line;
int s_lineCost;
vector<int> s_star;
int s_starCost;

vector<int> CL;
vector<int> s_rvnd;
int s_rvndCost;

int a1, a2, b1, b2, c1, c2, d1, d2, e1, e2;
int ta, tb, tc, ca, cb, cc, cd, wa, wb, wc;
int delta_best = INT_MAX;
int delta = 0;
int origin_best;
int destination_best;
double alpha;
int iterILS;
int N;
int **distancia;
int ***reOpt;

int main(int argc, char **argv)
{
    chrono::time_point<chrono::system_clock> temp1, temp2;
    int seed = time(0);
    srand(seed);

    readData(argc, argv, &N, &distancia);
    init();

#ifdef DEBUG
    cout << __LINE__ << endl;
#endif

    // BEGIN GILSRVND parameters
    int Imax = 10;
    int Iils = min(N, 100);
    vector<double> R;
    R.push_back(0.00);
    for (int i = 1; i <= 25; i++)
        R.push_back(R[i - 1] + 0.01);
    // END GILSRVND parameters

    vector<int> sol;
    int solCost;

    for (int i = 1; i <= 10; i++)
    {
        srand(seed);
        // clock_t beginC = clock();
        temp1 = std::chrono::system_clock::now();
        GILSRVND(Imax, Iils, R, sol, solCost);
        temp2 = std::chrono::system_clock::now();
        // clock_t endC = clock();
        // double elapsed_secs = double(endC - beginC) / CLOCKS_PER_SEC;

        printf("%.*s", int(strlen(argv[1])) - 14, argv[1] + 10);
        // cout<<","<<elapsed_secs<<",";
        cout << "," << chrono::duration_cast<chrono::microseconds>(temp2 - temp1).count() / 1000000.0 << ",";
        cout << fixed << s_starCost;
        cout << "," << seed << endl;
        seed++;
    }

    //cout<<ls<<endl;

    // cout<<sol<<endl;

    return 0;
}

void GILSRVND(int &Imax, int &Iils, vector<double> &R, vector<int> &sol, int &costMLP)
{
#ifdef DEBUG
    cout << __LINE__ << endl;
#endif
    s_starCost = INT_MAX;
    s_lineCost = INT_MAX;

    for (int i = 0; i < Imax; i++)
    {
        alpha = R[rand() % 26];

        constructiveProcedure(sol, costMLP, alpha);

        s_line = sol;
        s_lineCost = costMLP;

#ifdef DEBUG
        cout << __LINE__ << endl;
#endif
        iterILS = 0;
        while (iterILS < Iils)
        {

            RVND(sol, costMLP);

            if (costMLP < s_lineCost)
            {
                s_line = sol;
                s_lineCost = costMLP;
                iterILS = 0;
            } //end_if
            sol = s_line;

            bridgePerturbation(sol);
            costMLP = reOpt[1][0][N];

            iterILS = iterILS + 1;
        } //end_while
        if (s_lineCost < s_starCost)
        {
            s_starCost = s_lineCost;
            s_star = s_line;
        }
    }
#ifdef DEBUG
    cout << __LINE__ << endl;
#endif
    // cout << *n << endl;
}

vector<int> construction(double alpha)
{
  int r = 1;
  std::vector<int> s(N + 1), CL(N - 1);
  std::vector<double> distances(N - 1);

  s[0] = s[s.size() - 1] = r;

  for (int i = 2; i <= N; i++)
  {
    CL[i - 2] = i;
  }

  int c = 1;
  while (!CL.empty())
  {
    for (int i = 0; i < CL.size(); i++)
    {
      distances[i] = distancia[r][CL[i]];
    }
    std::sort(distances.begin(), distances.end());

    int a = rand() % (int)std::floor(alpha * (CL.size() - 1) + 1);
    r = CL[a];
    s[c] = r;
    c++;

    CL.erase(CL.begin() + a);
    distances.erase(distances.begin());
  }

  return s;
}

void constructiveProcedure(vector<int> &s, int &costMLP, double &alpha)
{
#ifdef DEBUG
    cout << __LINE__ << endl;
#endif
    // Local variables
    int position;

    // Algorithm 2
    // Line 2 - $s \cup {0}$
    costMLP = INT_MAX;
    s.resize(0);
    s.push_back(1);

    // Line 3,4 - Initialize Candidate list CL
    CL.resize(0);
    for (unsigned i = 2; i <= N; i++)
        CL.push_back(i);

    // Line 5,6
    while (CL.size() > 0)
    {
        // Sort CL in ascending order according to their distance with respect to r
        calculaCustoInsercaoMLP(s);
        position = rand() % (int(floor((custoInsercao.size() - 1) * alpha) + 1));
        s.push_back(custoInsercao[position].noInserido);
        remove(CL.begin(), CL.end(), custoInsercao[position].noInserido);
        CL.resize(CL.size() - 1);
    }
    s.push_back(s[0]);
    computeCostValueMLP(s);
    costMLP = reOpt[1][0][N];
#ifdef DEBUG
    cout << "Linha" << __LINE__ << " " << costMLP << endl;
#endif
}

void calculaCustoInsercaoMLP(vector<int> &s)
{

    custoInsercao.resize(CL.size());

    for (unsigned i = 0; i < CL.size(); i++)
    {
        custoInsercao[i].custo = distancia[s[s.size() - 1]][CL[i]];
        custoInsercao[i].noInserido = CL[i];
    }

    sort(custoInsercao.begin(), custoInsercao.end(), compareByCost);
}

void computeCostValueMLP(vector<int> &s)
{

    for (int i = 0; i <= N; i++)
        for (int j = i; j <= N; j++)
            if (i != j){
                reOpt[0][i][j] = reOpt[0][i][j - 1] + distancia[s[j - 1]][s[j]];
                reOpt[0][j][i] = reOpt[0][i][j];
            }
            else
                reOpt[0][j][j] = 0;

    for (int i = 0; i <= N; i++)
        for (int j = i; j <= N; j++)
            if (i != j){
                reOpt[1][i][j] = reOpt[1][i][j - 1] + reOpt[0][i][j];
            }
            else
                reOpt[1][i][j] = 0;

    for (int i = 0; i <= N; i++)
        for (int j = i; j >= 0; j--)
            if (i != j)
                reOpt[1][i][j] = reOpt[1][i][j + 1] + reOpt[0][i][j];
  
}

void init()
{
    reOpt = new int **[2];
    reOpt[0] = new int *[N + 1];
    reOpt[1] = new int *[N + 1];

    for (int i = 0; i <= N; i++)
    {
        reOpt[0][i] = new int[N + 1];
        reOpt[1][i] = new int[N + 1];
        reOpt[0][i][i] = 0;
        reOpt[1][i][i] = 0;
    }
}

void bridgePerturbation(vector<int> &s)
{

    int arc[4];
    int tamanho = s.size() - 1;
    int ultimo = tamanho - 1;
    bool largada = false;
    bool chegada = false;

    arc[0] = rand() % tamanho;

    if (arc[0] == 0)
        largada = true;
    if (arc[0] == ultimo)
        chegada = true;

    do
    {
        arc[1] = rand() % tamanho;
    } while (arc[1] == arc[0] ||
             abs(arc[0] - arc[1]) < 2 ||
             (largada && arc[1] == ultimo) ||
             (chegada && arc[1] == 0));

    if (arc[1] == 0)
        largada = true;
    if (arc[1] == ultimo)
        chegada = true;

    if (arc[0] > arc[1])
        swap(arc[0], arc[1]);

    do
    {
        arc[2] = rand() % tamanho;
    } while (arc[2] == arc[0] ||
             arc[2] == arc[1] ||
             abs(arc[2] - arc[0]) < 2 ||
             abs(arc[2] - arc[1]) < 2 ||
             (largada && arc[2] == ultimo) ||
             (chegada && arc[2] == 0));

    if (arc[2] == 0)
        largada = true;
    if (arc[2] == ultimo)
        chegada = true;

    if (arc[1] > arc[2])
        swap(arc[2], arc[1]);
    if (arc[0] > arc[1])
        swap(arc[0], arc[1]);

    do
    {
        arc[3] = rand() % tamanho;
    } while (arc[3] == arc[0] ||
             arc[3] == arc[1] ||
             arc[3] == arc[2] ||
             abs(arc[3] - arc[0]) < 2 ||
             abs(arc[3] - arc[1]) < 2 ||
             abs(arc[3] - arc[2]) < 2 ||
             (largada && arc[3] == ultimo) ||
             (chegada && arc[3] == 0));

    if (arc[2] > arc[3])
        swap(arc[2], arc[3]);
    if (arc[1] > arc[2])
        swap(arc[2], arc[1]);
    if (arc[0] > arc[1])
        swap(arc[0], arc[1]);

    rotate(s.begin() + arc[0] + 1, s.begin() + arc[2] + 1, s.begin() + arc[3] + 1);
    rotate(s.begin() + arc[0] + (arc[3] - arc[2]) + 1, s.begin() + arc[1] + (arc[3] - arc[2]) + 1, s.begin() + arc[3] + 1);

    computeCostValueMLP(s);
}

void RVND(vector<int> &s, int &costMLP)
{
    std::chrono::time_point<std::chrono::system_clock> temp1, temp2;
    s_rvnd = s;
    s_rvndCost = costMLP;
    vector<int> NL = {0, 1, 2, 3, 4};
    int choosenNeighborhood;
    while (!NL.empty())
    {
        choosenNeighborhood = rand() % NL.size();

        switch (NL[choosenNeighborhood])
        {
        case 0:
            Swap(s_rvnd, s_rvndCost);
            break;
        case 1:
            TwoOpt(s_rvnd, s_rvndCost);
            break;
        case 2:
            ReInsertion(s_rvnd, s_rvndCost, 1);
            break;
        case 3:
            ReInsertion(s_rvnd, s_rvndCost, 2);
            break;
        case 4:
            ReInsertion(s_rvnd, s_rvndCost, 3);
            break;
        default:
            break;
        }

        if (s_rvndCost < costMLP)
        {
            s = s_rvnd;
            costMLP = s_rvndCost;
            NL = {0, 1, 2, 3, 4};
        }
        else
        {
            NL.erase(NL.begin() + choosenNeighborhood);
        }
    }
}

void Swap(vector<int> &s, int &costMLP)
{
    int delta = 0;
    int i_best;
    int j_best;
    int delta_best = delta_best = INT_MAX;

    for (unsigned i = 1; i < s.size() - 2; i++)
        for (unsigned j = i + 1; j < s.size() - 1; j++)
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
                d2 = N;

                // e1;
                // e2;

                ta = reOpt[0][a1][a2] + distancia[s[a2]][s[b1]] + reOpt[0][b1][b2];
                ca = reOpt[1][a1][a2] + (b2 - b1 + 1) * (reOpt[0][a1][a2] + distancia[s[a2]][s[b1]]) + reOpt[1][b1][b2];
                // wa = W(a1,a2) + W(b1,b2);

                tb = reOpt[0][c1][c2] + distancia[s[c2]][s[d1]] + reOpt[0][d1][d2];
                cb = reOpt[1][c1][c2] + (d2 - d1 + 1) * (reOpt[0][c1][c2] + distancia[s[c2]][s[d1]]) + reOpt[1][d1][d2];
                wb = c2 - c1 + d2 - d1 + 2;

                cc = ca + wb * (ta + distancia[s[b2]][s[c1]]) + cb;
                // tc = ta + distancia[s[b2]][s[c1]] + tb;

                delta = cc - reOpt[1][0][N];
            }
            else
            {
                a1 = 0;
                a2 = i - 1;

                b1 = j;
                b2 = j;

                c1 = i + 1;
                c2 = j - 1;

                d1 = i;
                d2 = i;

                e1 = j + 1;
                e2 = N;

                ta = reOpt[0][a1][a2] + distancia[s[a2]][s[b1]] + reOpt[0][b1][b2];
                ca = reOpt[1][a1][a2] + (b2 - b1 + 1) * (reOpt[0][a1][a2] + distancia[s[a2]][s[b1]]) + reOpt[1][b1][b2];
                cb = ca + (c2 - c1 + 1) * (ta + distancia[s[b2]][s[c1]]) + reOpt[1][c1][c2];
                tb = ta + distancia[s[b2]][s[c1]] + reOpt[0][c1][c2];
                cc = cb + (d2 - d1 + 1) * (tb + distancia[s[c2]][s[d1]]) + reOpt[1][d1][d2];
                tc = tb + distancia[s[c2]][s[d1]] + reOpt[0][d1][d2];
                cd = cc + (e2 - e1 + 1) * (tc + distancia[s[d2]][s[e1]]) + reOpt[1][e1][e2];
                delta = cd - reOpt[1][0][N];
            }
            if (delta < 0 && delta < delta_best)
            {
                delta_best = delta;
                i_best = i;
                j_best = j;
            }
        }

    if (delta_best < 0)
    {
        swapMove(s, costMLP, i_best, j_best, delta_best);
    }
}

void twoOptMove(vector<int> &s, int &costMLP, int &i, int &j, int &delta)
{
    if (i > j)
        swap(i, j);

    reverse(s.begin() + i + 1, s.begin() + j + 1);
    costMLP += delta;

    if (s[0] != 1)
    {
        cout << __FILE__ << ", line" << __LINE__ << endl;
    }

    update2opt(s, i, j);

    if (costMLP != reOpt[1][0][N])
    {
        cout << __FILE__ << ", line " << __LINE__ << endl;
        exit(0);
    }
}

void swapMove(vector<int> &s, int &costMLP, int &a, int &b, int &delta)
{
    // cout<<*in<<endl;

    if (a > b)
        swap(a, b);
    swap(s[a], s[b]);
    if (a == 0 || s[0] != 1)
    {
        cout << __FILE__ << ", line " << __LINE__ << endl;
        s[s.size() - 1] = s[a];
    }

    costMLP += delta;

    updateSwap(s, a, b);

    if (costMLP != reOpt[1][0][N])
    {
        cout << __FILE__ << ", line " << __LINE__ << endl;
        exit(0);
    }
}

void updateSwap(vector<int> &s, int a, int b)
{
    if (a > b)
        swap(a, b);

    for (int j = a; j <= b; j++)
    {
        for (int i = j - 1; i >= 0; i--)
        {

            reOpt[0][i][j] = reOpt[0][i][j - 1] + distancia[s[j - 1]][s[j]];
            reOpt[1][i][j] = reOpt[1][i][j - 1] + (reOpt[0][i][j - 1] + distancia[s[j - 1]][s[j]]) + reOpt[1][j][j];
            reOpt[0][j][i] = reOpt[0][i][j];
            reOpt[1][j][i] = reOpt[1][j - 1][i] + (j - 1 - i + 1) * (reOpt[0][j][j] + distancia[s[j]][s[j - 1]]) + reOpt[1][j][j];
        }
    }

    for (int i = a; i <= b; i++)
    {
        for (int j = b + 1; j <= N; j++)
        {
            reOpt[0][i][j] = reOpt[0][i][j - 1] + distancia[s[j - 1]][s[j]];
            reOpt[1][i][j] = reOpt[1][i][j - 1] + (reOpt[0][i][j - 1] + distancia[s[j - 1]][s[j]]) + reOpt[1][j][j];
            reOpt[0][j][i] = reOpt[0][i][j];
            reOpt[1][j][i] = reOpt[1][j - 1][i] + (j - 1 - i + 1) * (reOpt[0][j][j] + distancia[s[j]][s[j - 1]]) + reOpt[1][j][j];
        }
    }

    for (int i = a - 1, k = a; i >= 0; i--)
    {
        for (int j = b + 1; j <= N; j++)
        {

            reOpt[0][i][j] = reOpt[0][i][k] + distancia[s[k]][s[k + 1]] + reOpt[0][k + 1][j];
            reOpt[1][i][j] = reOpt[1][i][k] + (j - (k + 1) + 1) * (reOpt[0][i][k] + distancia[s[k]][s[k + 1]]) + reOpt[1][k + 1][j];

            reOpt[0][j][i] = reOpt[0][i][j];

            reOpt[1][j][i] = reOpt[1][k][i] + (k - i + 1) * (reOpt[0][j][k + 1] + distancia[s[k + 1]][s[k]]) + reOpt[1][j][k + 1];
        }
    }
}

void TwoOpt(vector<int> &s, int &costMLP)
{
    int delta = 0;
    int i_best;
    int j_best;
    delta_best = INT_MAX;

    a1 = 0;
    c2 = N;

    for (int i = 1; i < N; i++)
        for (int j = i + 1; j < N - 1; j++)
        {
            a2 = i;

            b1 = j;
            b2 = i + 1;

            c1 = j + 1;

            ta = reOpt[0][a1][a2] + distancia[s[a2]][s[b1]] + reOpt[0][b1][b2];
            cb = reOpt[1][a1][a2] + (b1 - b2 + 1) * (reOpt[0][a1][a2] + distancia[s[a2]][s[b1]]) + reOpt[1][b1][b2] +
                 (c2 - c1 + 1) * (ta + distancia[s[b2]][s[c1]]) + reOpt[1][c1][c2];

            delta = cb - costMLP;

            if (delta < 0 && delta < delta_best)
            {
                delta_best = delta;
                i_best = i;
                j_best = j;
            }
        }

    if (delta_best < 0)
    {
        twoOptMove(s, costMLP, i_best, j_best, delta_best);
    }
}

void update2opt(vector<int> &s, int a, int b)
{
    if (a > b)
        swap(a, b);

    for (int i = 0; i <= b; i++)
        for (int j = max(a, i); j <= N; j++)
            if (i != j)
                reOpt[0][i][j] = reOpt[0][i][j - 1] + distancia[s[j - 1]][s[j]];
            else
                reOpt[0][j][j] = 0;

    for (int i = a; i <= N; i++)
        for (int j = min(b, i); j >= 0; j--)
            if (i != j)
                reOpt[0][i][j] = reOpt[0][i][j + 1] + distancia[s[j]][s[j + 1]];
            else
                reOpt[0][j][j] = 0;

    for (int i = 0; i <= b; i++)
        for (int j = max(a, i); j <= N; j++)
            if (i != j)
                reOpt[1][i][j] = reOpt[1][i][j - 1] + reOpt[0][i][j];
            else
                reOpt[1][i][j] = 0;

    for (int i = a; i <= N; i++)
        for (int j = min(b, i); j >= 0; j--)
            if (i != j)
                reOpt[1][i][j] = reOpt[1][i][j + 1] + reOpt[0][i][j];
            else
                reOpt[1][i][j] = 0;
}

void ReInsertion(vector<int> &s, int &costMLP, int size)
{
    d2 = N;
    delta_best = INT_MAX;
    for (int origin = 1; origin < N - size; ++origin)
    {
        for (int destination = 1; destination < N - size; ++destination)
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

                ta = reOpt[0][a1][a2] + distancia[s[a2]][s[b1]] + reOpt[0][b1][b2];
                ca = reOpt[1][a1][a2] + (b2 - b1 + 1) * (reOpt[0][a1][a2] + distancia[s[a2]][s[b1]]) + reOpt[1][b1][b2];
                // wa = W(a1,a2) + W(b1,b2);

                tb = reOpt[0][c1][c2] + distancia[s[c2]][s[d1]] + reOpt[0][d1][d2];
                cb = reOpt[1][c1][c2] + (d2 - d1 + 1) * (reOpt[0][c1][c2] + distancia[s[c2]][s[d1]]) + reOpt[1][d1][d2];
                wb = c2 - c1 + d2 - d1 + 2;

                cc = ca + wb * (ta + distancia[s[b2]][s[c1]]) + cb;
                tc = ta + distancia[s[b2]][s[c1]] + tb;

                delta = cc - costMLP;
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

                ta = reOpt[0][a1][a2] + distancia[s[a2]][s[b1]] + reOpt[0][b1][b2];
                ca = reOpt[1][a1][a2] + (b2 - b1 + 1) * (reOpt[0][a1][a2] + distancia[s[a2]][s[b1]]) + reOpt[1][b1][b2];
                // wa = W(a1,a2) + W(b1,b2);

                tb = reOpt[0][c1][c2] + distancia[s[c2]][s[d1]] + reOpt[0][d1][d2];
                cb = reOpt[1][c1][c2] + (d2 - d1 + 1) * (reOpt[0][c1][c2] + distancia[s[c2]][s[d1]]) + reOpt[1][d1][d2];
                wb = c2 - c1 + d2 - d1 + 2;

                cc = ca + wb * (ta + distancia[s[b2]][s[c1]]) + cb;
                tc = ta + distancia[s[b2]][s[c1]] + tb;

                delta = cc - costMLP;
            }

            if (delta < 0 && delta < delta_best)
            {
                delta_best = delta;
                origin_best = origin;
                destination_best = destination;
            }
        }
    }

    if (delta_best < 0)
    {
        reInsertionMove(s, costMLP, origin_best,destination_best, size, delta_best);
    }
}

void reInsertionMove(vector<int> &s, int &costMLP, int &origin, int &destination, int &size, int &delta)
{

    if (origin < destination)
    {
        rotate(s.begin() + origin, s.begin() + origin + size, s.begin() + destination + size);
        //update2opt(s, origin, destination + size);
        updateSwap(s, origin, destination + size);
        
    }
    else
    {
        rotate(s.begin() + destination, s.begin() + origin, s.end() - s.size() + origin + size);
        updateSwap(s, destination, origin + size);
        // update2opt(s, destination, origin + size);
    }

    costMLP += delta;

    if (costMLP != reOpt[1][0][N])
    {
        cout << __FILE__ << ", line " << __LINE__ << endl;
        exit(0);
    }
}