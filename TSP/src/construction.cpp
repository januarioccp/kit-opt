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

void Construction::trivial(Solution* s){
    s->location.resize(0);
    for(unsigned i = 1; i <= s->in->dimensionGet(); i++)
        s->location.push_back(i);
    s->location.push_back(s->location[0]);
    s->computeCostValueTSP();
}

void Construction::constructiveProcedure(Solution* s, const double alpha){
    
        int position;
        calculaCustoInsercao(s);
        // clock_t begin;
        while ( CL.size() > 0)
        {
            // begin = clock();
            position = rand()%(int(floor(custoInsercao.size()*alpha))+1);
            // cout<<"position = "<<(double)(clock() - begin) / CLOCKS_PER_SEC << endl;

            // begin = clock();
            s->location.insert(s->location.begin() + custoInsercao[position].arestaRemovida +1, custoInsercao[position].noInserido); 
            // cout<<"insert = "<<(double)(clock() - begin) / CLOCKS_PER_SEC << endl;

            s->costValueTSP+=custoInsercao[position].custo;

            // begin = clock();
            remove(CL.begin(), CL.end(), custoInsercao[position].noInserido);
            // cout<<"remove = "<<(double)(clock() - begin) / CLOCKS_PER_SEC << endl;

            // begin = clock();
            CL.resize(CL.size()-1);
            // cout<<"resize = "<<(double)(clock() - begin) / CLOCKS_PER_SEC << endl;

            // begin = clock();
            calculaCustoInsercao(s);
            // cout<<"calculaCustoInsercao = "<<(double)(clock() - begin) / CLOCKS_PER_SEC << endl;

            // cout<<endl;
        }
    
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