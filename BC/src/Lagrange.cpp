#include "Lagrange.h"
#include "Kruskal.h"

Lagrange::Lagrange(Data *in)
{
    N = in->getDimension();
    lambda.resize(in->getDimension());

    degree.resize(N);

    cost2.resize(N - 1);
    for (int i = 0; i < N-1; i++)
        cost2[i].resize(N - 1);

    cost.resize(N);
    for (int i = 0; i < cost.size(); i++){
        cost[i].resize(N);
        for (int j = 0; j < cost.size(); j++)
            cost[i][j] = in->getDistance(i,j);
    }
    isFeasible = false;
}

double Lagrange::lowerBound(vector<int> &g,double &LB, double &UB)
{
    ek = 1.0;
    int limite = 30;
    double bestLB = 0.0;
    // O valor de ek é reduzido pela metade quanto o limite de tentativas chega a zero
    while (true)
    {
        // Os valores das distancias sao atualizados com base nos valores de lambda
        for (int i = 1; i <= N - 1; i++){
            for (int j = 1; j <= N - 1; j++){
                cost2[i - 1][j - 1] = cost[i][j] - lambda[i] - lambda[j];
            }
        }
        
        // A matriz de distancia eh passada para a classe Kruskal, responsavel pelo calculo da AGM
        Kruskal k(cost2);
        LB = k.MST(N - 1);

        // As duas arestas de menor custo sao adicionadas posteriormente
        lowestCostValue.first = lowestCostValue.second = 99999998;
        lowestCostEdges.first = lowestCostEdges.second = 0;
        for (int i = 1; i <= N - 1; i++)
        {
            if (lowestCostValue.first > cost[i][0]) //M[0][i])//
            {
                lowestCostEdges.second = lowestCostEdges.first;
                lowestCostValue.second = lowestCostValue.first;

                lowestCostValue.first = cost[i][0]; //M[0][i]; //
                lowestCostEdges.first = i;
            }
            else if (lowestCostValue.second > cost[i][0]) // M[0][i])//
            {
                lowestCostValue.second = cost[i][0]; // M[0][i]; //
                lowestCostEdges.second = i;
            }
        }
        LB += lowestCostValue.first - lambda[lowestCostEdges.first];
        LB += lowestCostValue.second - lambda[lowestCostEdges.second];

        // Inicio do calculo so subgradiente
        fill(degree.begin(),degree.end(),0);
        
        g.resize(N);
        
        aresta = k.getEdges();
        // Compute node degree
        for (int i = 0; i < aresta.size(); i++)
        {
            degree[aresta[i].first + 1]++;
            degree[aresta[i].second + 1]++;
        }
        degree[0] = 2;
        degree[lowestCostEdges.first]++;
        degree[lowestCostEdges.second]++;

        isFeasible = true;
        // Subgradiente
        double penalidade = 0.0;
        for (int i = 0; i < g.size(); i++){
            g[i] = 2 - degree[i];
            penalidade += pow(g[i], 2);
              if (std::abs(g[i]) > 0.000001)
            {
                isFeasible = false;
            }
        }

        for (int i = 0; i < lambda.size(); i++)
        {
            lambda[i] = lambda[i] + ((ek * (UB - LB)) / penalidade) * g[i];
        }

        if (isFeasible || UB - LB <= 0.000001)
        {
            pruning = true;
            // s = kr.getEdges();
            break;
        }

        if (bestLB < LB)
        {
            bestLB = LB;
            limite = 30;
        }
        else
        {
            limite--;
            if (limite <= 0)
            {
                ek /= 2.0;
                limite = 30;
            }
        }
        if(ek <= __DBL_EPSILON__)
            break;
    }
    // for (int i = 0; i < aresta.size(); i++){
    //     aresta[i].first++;
    //     aresta[i].second++;
    // }
    // aresta.push_back(make_pair(lowestCostEdges.first,0));
    // aresta.push_back(make_pair(0,lowestCostEdges.second));

    LB = bestLB;
    return LB;
}

ostream &operator<<(ostream &os, const Lagrange &l)
{
    os << "digraph G {" << endl;
    for (int i = 0; i < l.aresta.size(); i++)
    {
        os << l.aresta[i].first + 2 << "->" << l.aresta[i].second + 2;
        os << endl;
    }
    os << "1->" << l.lowestCostEdges.first + 1 << endl;
    os << "1->" << l.lowestCostEdges.second + 1 << endl;
    os << "}" << endl;

    return os;
}