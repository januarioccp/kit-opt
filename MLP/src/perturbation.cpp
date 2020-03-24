#include "perturbation.h"
#include "solution.h"
#include "colormod.h"
#include <iostream>
#include <cstdlib>
#include <fstream>
#include <cmath>
#include <iomanip>
#include <set>
#include <algorithm> 
#include <vector>
#include <utility>
using namespace std;

Perturbation::Perturbation( Input* input)
{
    this->in = input;
    localSolution = new Solution(in);
}

Perturbation::~Perturbation()
{
    delete localSolution;
}

void Perturbation::bridgePerturbation(Solution* s, int bridge){

    int arc[4];
    int tamanho = s->location.size()-1;
    int ultimo = tamanho-1;
    bool largada = false;
    bool chegada = false;

    arc[0] = rand()%tamanho;

    if(arc[0]==0)
        largada = true;
    if(arc[0]==ultimo)
        chegada = true;

    do{
        arc[1] = rand()%tamanho;
    }while(arc[1] == arc[0] || 
           abs(arc[0]-arc[1]) < 2 ||
           (largada && arc[1]==ultimo) ||
           (chegada && arc[1]==0));

    if(arc[1]==0)
        largada = true;
    if(arc[1]==ultimo)
        chegada = true;

    if(arc[0] > arc[1])
        swap(arc[0],arc[1]);

    do{
        arc[2] = rand()%tamanho;
    }while(arc[2] == arc[0] || 
           arc[2] == arc[1] || 
           abs(arc[2]-arc[0]) < 2 ||
           abs(arc[2]-arc[1]) < 2 ||
           (largada && arc[2]==ultimo) ||
           (chegada && arc[2]==0));

    if(arc[2]==0)
        largada = true;
    if(arc[2]==ultimo)
        chegada = true;

    if(arc[1] > arc[2])
        swap(arc[2],arc[1]);
    if(arc[0] > arc[1])
        swap(arc[0],arc[1]);

    do{
        arc[3] = rand()%tamanho;
    }while(arc[3] == arc[0] || 
           arc[3] == arc[1] || 
           arc[3] == arc[2] || 
           abs(arc[3]-arc[0]) < 2 ||
           abs(arc[3]-arc[1]) < 2 ||
           abs(arc[3]-arc[2]) < 2 ||
           (largada && arc[3]==ultimo) ||
           (chegada && arc[3]==0));
    
    if(arc[2] > arc[3])
        swap(arc[2],arc[3]);
    if(arc[1] > arc[2])
        swap(arc[2],arc[1]);
    if(arc[0] > arc[1])
        swap(arc[0],arc[1]);

    rotate(s->location.begin()+arc[0]+1,s->location.begin()+arc[2]+1,s->location.begin()+arc[3]+1);
    rotate(s->location.begin()+arc[0]+(arc[3]-arc[2])+1,s->location.begin()+arc[1]+(arc[3]-arc[2])+1,s->location.begin()+arc[3]+1);

    s->computeCostValueMLP();

}


/**
 * Removing and re-inserting four arcs in such a way that a feasible tour is generated 
*/
vector<pair<int,int> > Perturbation::makeBridges(const Solution* s, int bridges){

    

    int minimum = 2;
    vector< pair<int,int> > section;
    bool selected;
    for(int i=0; i < bridges; i++){
        pair<int,int> myPosition(0,0);
        do{
            selected = true;
            myPosition.second = rand()%(s->location.size()-1);
            for(unsigned j=0; j < section.size() && selected; j++)
            {
                if(myPosition == section[j])
                    selected = false;
                //Guarantee minimum size for all subsequences
                if(abs(myPosition.second - section[j].second) < 2)
                    selected = false;
                if(section[j].second == 0 && myPosition.second >= int(s->location.size()) - minimum)
                    selected = false;
                if(myPosition.second == 0 && section[j].second >= int(s->location.size()) - minimum)
                    selected = false;
            }
        }while(!selected);
        section.push_back(myPosition);
        // Sorting...
        for(int k = section.size()-1; k > 0; --k)
            if(section[k].second < section[k-1].second){
                swap(section[k],section[k-1]);
            }
    }
    // Fixing first pair
    section[0].first = (section[section.size()-1].second+1)%(s->location.size()-1);;
    for(int k = section.size()-1; k > 0; --k)
        section[k].first = (section[k-1].second+1)%(s->location.size()-1);
    return section;
}