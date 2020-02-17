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

Construction::Construction( Solution* s, double alpha )
{
    int j;
    int tamanhoSubtourInicial = 3;

    CL.clear();
    for(int i =0; i < s->in->dimension; i++)
        CL.push_back(i);

    j = rand() % CL.size() ;

    s->reset();
    //Insert first location twice in the solution
    s->location.push_back(j);
    s->location.push_back(j);

    // Remove the first location from the CL
    CL.erase(CL.begin() + j);
    for ( int i = 0; i < tamanhoSubtourInicial ; i++)
    {
        j = rand() % CL.size () ;
        s->location.insert(s->location.begin() + 1, CL[j]); 
        CL.erase(CL.begin() + j);
    }

    s->computeCostValue();

}

void Construction::calculaCustoInsercao(Solution* s){

    custoInsercao.resize((s->location.size() - 1) * CL.size());

    for(int i = 0, j = 1, l = 0; i < s->location.size() - 1; i++, j++) {
        for (auto k : CL ) {
            custoInsercao[l].custo = 
                                    s->in->matrizAdj[s->location[i]][k] + 
                                    s->in->matrizAdj[s->location[j]][k] -
                                    s->in->matrizAdj[s->location[i]][s->location[j]]; 
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
}