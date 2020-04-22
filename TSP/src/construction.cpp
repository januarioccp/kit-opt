#include "construction.h"
#include "solution.h"
#include <iostream>
#include <cstdlib>
#include <fstream>
#include <cmath>
#include <iomanip>
#include <set>
#include <algorithm> 
using namespace std;

Construction::Construction( Solution* s)
{
    int j;
    int tamanhoSubtourInicial = 3;

    CL.clear();
    for(unsigned i =1; i <= s->in->dimensionGet(); i++)
        CL.push_back(i);

    j = rand() % CL.size() ;

    s->reset();
    //Insert first location twice in the solution
    s->location.push_back(CL[j]);
    s->location.push_back(CL[j]);

    // Remove the first location from the CL
    CL.erase(CL.begin() + j);
    for ( int i = 0; i < tamanhoSubtourInicial ; i++)
    {
        j = rand() % CL.size () ;
        s->location.insert(s->location.begin() + 1, CL[j]); 
        CL.erase(CL.begin() + j);
    }

    s->computeCostValueTSP();

}

void Construction::constructiveProcedure(Solution* s, const double alpha){
    
    if(s->in->problemGet() == 0)
    {// IF it is the Traveling Salesman Problem
        int position;
        calculaCustoInsercao(s);
        clock_t begin;
        while ( CL.size() > 0)
        {
            begin = clock();
            position = rand()%(int(floor(custoInsercao.size()*alpha))+1);
            cout<<"position = "<<(double)(clock() - begin) / CLOCKS_PER_SEC << endl;

            begin = clock();
            s->location.insert(s->location.begin() + custoInsercao[position].arestaRemovida +1, custoInsercao[position].noInserido); 
            cout<<"insert = "<<(double)(clock() - begin) / CLOCKS_PER_SEC << endl;

            s->costValueTSP+=custoInsercao[position].custo;

            begin = clock();
            remove(CL.begin(), CL.end(), custoInsercao[position].noInserido);
            cout<<"remove = "<<(double)(clock() - begin) / CLOCKS_PER_SEC << endl;

            begin = clock();
            CL.resize(CL.size()-1);
            cout<<"resize = "<<(double)(clock() - begin) / CLOCKS_PER_SEC << endl;

            begin = clock();
            calculaCustoInsercao(s);
            cout<<"calculaCustoInsercao = "<<(double)(clock() - begin) / CLOCKS_PER_SEC << endl;

            cout<<endl;
        }
    }else
    { // if it is the Minimum Latency Problem
        constructiveProcedureMLP(s,alpha);
    }
    
}

void Construction::constructiveProcedureMLP(Solution* s, const double alpha){
    
    // Local variables
    int position;

    // Algorithm 2
    // Line 2 - $s \cup {0}$
    s->reset();
    s->location.push_back(1);

    // Line 3,4 - Initialize Candidate list CL
    CL.resize(0);
    for(unsigned int i=2; i <= s->in->dimensionGet(); i++)
        CL.push_back(i);
    
    // Line 5,6 
    while ( CL.size() > 0){
        // Sort CL in ascending order according to their distance with respect to r
        calculaCustoInsercaoMLP(s);
        position = rand()%(int(floor(custoInsercao.size()*alpha))+1);
        s->location.push_back(custoInsercao[position].noInserido);
        remove(CL.begin(), CL.end(), custoInsercao[position].noInserido);
        CL.resize(CL.size()-1);    
    }
    s->location.push_back(s->location[0]);
    s->computeCostValueMLP();
}

void Construction::calculaCustoInsercao(Solution* s){

    custoInsercao.resize((s->location.size() - 1) * CL.size());

    for(unsigned i = 0, j = 1, l = 0; i < s->location.size() - 1; i++, j++) {
        for (auto k : CL ) {
            custoInsercao[l].custo = 
                                    s->in->distanceGet(s->location[i],k) + 
                                    s->in->distanceGet(s->location[j],k) -
                                    s->in->distanceGet(s->location[i],s->location[j]); 
            custoInsercao[l].noInserido = k;
            custoInsercao[l].arestaRemovida = i ; 
            l++;
        } 
    }
    sort(custoInsercao.begin(), custoInsercao.end(), compareByCost);
}

void Construction::calculaCustoInsercaoMLP(Solution* s){

    custoInsercao.resize(CL.size());
    
    for (unsigned i=0; i <CL.size(); i++ ) {
        custoInsercao[i].custo = s->in->distanceGet(s->location[s->location.size()-1],CL[i]); 
        custoInsercao[i].noInserido = CL[i];
    }
    
    sort(custoInsercao.begin(), custoInsercao.end(), compareByCost);
}

ostream & operator << (ostream &out, const Construction &c){
    Color::Modifier red(Color::FG_RED);
    Color::Modifier green(Color::GREEN);
    Color::Modifier blue(Color::FG_BLUE);
    Color::Modifier deff(Color::FG_DEFAULT);

    out << 
        red << "noInserido" << deff<<" | " << 
        green<<"arestaRemovida"<< deff <<" | " << 
        blue<<"custo"<< deff << endl;

    for (size_t i = 0; i < c.custoInsercao.size(); i++) {
        out << 
        red << setw(10) << c.custoInsercao[i].noInserido << deff<<" | " << 
        green<< setw(14) << c.custoInsercao[i].arestaRemovida<< deff <<" | " << 
        blue<< setw(5) << c.custoInsercao[i].custo<< deff << endl;
    }
    return out;
}