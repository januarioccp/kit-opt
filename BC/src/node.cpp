#include "node.h"
#include "Kruskal.h"
#include <list>
#include <algorithm>
#include <cmath>
using namespace std;

Node::Node(int dimension)
{
    this->isFeasible = false;
    this->pruning = false;
    lambda.resize(dimension);
    fill(lambda.begin(), lambda.end(),0);
}

void Node::calculateLB(int dimension, std::vector<std::vector<double> > &M, const double &UB)
{   
    double e_k = 1;
    std::vector<std::vector<double> > cost;
    double lastZ = 0;
    double deltaZ;
    int p = 0;
    double e_k_lim = 1.0 / 16364.0;
    Kruskal kr(cost);

    std::list<Node *> nodes;

    std::vector<double> vetorInfinito1(arcosProibidos.size(), std::numeric_limits<double>::infinity());
    std::vector<double> vetorInfinito2(arcosProibidos.size(), std::numeric_limits<double>::infinity());

    double bestLB = -std::numeric_limits<double>::infinity();
    std::vector<double> bestLambda;

    int cont = 0;
    for (const auto &arco : arcosProibidos)
    {
        std::swap(M[arco.first][arco.second], vetorInfinito1[cont]);
        std::swap(M[arco.second][arco.first], vetorInfinito2[cont]);
        cont++;
    }

    while (true)
    {
        cost = M;
        // Os valores das distancias sao atualizados com base nos valores de lambda
        for (int i = 0; i < dimension; i++)
            for (int j = 0; j < dimension; j++)    
                cost[i][j] = M[i][j] - lambda[i] - lambda[j];
            
        kr = Kruskal(cost);

        LB = kr.MST(dimension);
        if (LB > bestLB)
        {
            bestLB = LB;
            bestLambda = lambda;
        }

        for (int i = 1; i < dimension; i++)
            LB += 2 * lambda[i];

        deltaZ = LB - lastZ;

        if (deltaZ <= std::numeric_limits<double>::epsilon())
            p++;
        if (p == 30)
        {
            e_k = e_k / 2;
            p = 0;
        }

        penalizacao.resize(dimension);
        fill(penalizacao.begin(), penalizacao.end(),2);// = std::vector<int>(dimension, 2);

        isFeasible = true;

        double sumPenalizacoes = 0;

        for (int v = 0; v < dimension; v++)
        {
            for (const auto &arco : kr.getEdges())
            {
                if (arco.first == v || arco.second == v)
                {
                    penalizacao[v]--;
                }
            }
            if (std::abs(penalizacao[v]) > 0.000001)
            {
                isFeasible = false;
            }
            sumPenalizacoes += penalizacao[v] * penalizacao[v];
        }

        if (isFeasible || abs(UB - LB) <= 0.000001)
        {
            pruning = true;
            arestas = kr.getEdges();
            break;
        }

        double stepSize = e_k * (UB - LB);

        stepSize = stepSize / sumPenalizacoes;

        for (int i = 0; i < dimension; i++)
        {
            lambda[i] += stepSize * penalizacao[i];
            //std::cout << lambda[i] << "\n";
        }

        //std::cout << "------------------------------\n\n";

        lastZ = LB;

        if (e_k < e_k_lim)
        {
            pruning = false;
            isFeasible = false;
            arestas = kr.getEdges();

            break;
        }
    }

    cont = 0;
    for (const auto &arco : arcosProibidos)
    {
        std::swap(M[arco.first][arco.second], vetorInfinito1[cont]);
        std::swap(M[arco.second][arco.first], vetorInfinito2[cont]);
        cont++;
    }

    lambda = bestLambda;
    LB = bestLB;
}