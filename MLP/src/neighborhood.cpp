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
using namespace std;

Neighborhood::Neighborhood( Input* input){
    this->in = input;
    
    NL.push_back("bestSwap");
    // NL.push_back("firstSwap");
    //NL.push_back("firstTwoOpt");
    // NL.push_back("bestTwoOpt");
    //NL.push_back("firstReInsertion-1");
    // NL.push_back("bestReInsertion-1");
    //NL.push_back("firstReInsertion-2");
    // NL.push_back("bestReInsertion-2");
    //NL.push_back("firstReInsertion-3");
    // NL.push_back("bestReInsertion-3");
    // NL.push_back("bestReInsertion-4");

    // So far 4 subsequences is enough
    sigma.resize(5);
}

void Neighborhood::improove(Solution* s,string choosenNeighborhood){
    if(choosenNeighborhood == "bestSwap")
        bestSwap(s);
    else if(choosenNeighborhood == "firstSwap")
        firstSwap(s);
    else if(choosenNeighborhood == "firstTwoOpt")
        firstTwoOpt(s);
    else if(choosenNeighborhood == "bestTwoOpt")
        bestTwoOpt(s);
    else if(choosenNeighborhood == "firstReInsertion-1")
        firstReInsertion(s,1);
    else if(choosenNeighborhood == "firstReInsertion-2")
        firstReInsertion(s,2);
    else if(choosenNeighborhood == "firstReInsertion-3")
        firstReInsertion(s,3);
    else if(choosenNeighborhood == "bestReInsertion-1")
        bestReInsertion(s,1);
    else if(choosenNeighborhood == "bestReInsertion-2")
        bestReInsertion(s,2);
    else if(choosenNeighborhood == "bestReInsertion-3")
        bestReInsertion(s,3);
    else if(choosenNeighborhood == "bestReInsertion-4")
        bestReInsertion(s,4);
    else if(choosenNeighborhood == "firstReInsertion-4")
        firstReInsertion(s,4);
}

void Neighborhood::firstSwap(Solution* s){
    bool stuck = false;
    double delta = 0;
    //while(!stuck){
        stuck = true;
        for(unsigned i=0; i < s->location.size()-1; i++)
            for(unsigned j=i+1; j < s->location.size()-1; j++){
                delta = swapDeltaEvaluation(s,i,j);
                if(delta < 0){
                    swapMove(s,i,j,delta);
                    stuck = false;
                }
            }
    //}
}

void Neighborhood::bestSwap(Solution* s){
    bool stuck = false;
    double delta = 0;

    int i_best;
    int j_best;
    double delta_best;

    //while(!stuck){
        stuck = true;
        delta_best = INT_MAX;
        for(unsigned i=0; i < s->location.size()-1; i++)
            for(unsigned j=i+1; j < s->location.size()-1; j++){
                // clock_t beginC = clock();
                delta = swapDeltaEvaluation(s,i,j);
                // st->insert(beginC,clock(),0);
                if(delta < 0 && delta < delta_best){
                    delta_best = delta;
                    i_best = i;
                    j_best = j;
                }
            }
        if(delta_best < 0){
            swapMove(s,i_best,j_best,delta_best);
            stuck = false;
        }
    //}
}

double Neighborhood::swapDeltaEvaluation(Solution* s,int i,int j){

    if(s->in->problemGet()==0)
    { // TSP
            
        int last = s->location.size()-2;

        if(i > j)
            swap(i,j);

        if( i == 0){
            if(j==1){ //It doesn't work for non simmetric instances
                return
                in->distanceGet(s->location[last],s->location[j]) +
                in->distanceGet(s->location[i],s->location[j+1]) -
                in->distanceGet(s->location[last],s->location[i]) -
                in->distanceGet(s->location[j],s->location[j+1]);
            } if(j==last){
                return in->distanceGet(s->location[last],s->location[i+1]) +
                in->distanceGet(s->location[last-1],s->location[i]) -
                in->distanceGet(s->location[j],s->location[j-1]) -
                in->distanceGet(s->location[i],s->location[i+1]);
            } else{
                return in->distanceGet(s->location[last],s->location[j])
                + in->distanceGet(s->location[j],s->location[i+1])
                + in->distanceGet(s->location[j-1],s->location[i])
                + in->distanceGet(s->location[i],s->location[j+1])
                - in->distanceGet(s->location[last],s->location[i])
                - in->distanceGet(s->location[i],s->location[i+1])
                - in->distanceGet(s->location[j-1],s->location[j])
                - in->distanceGet(s->location[j],s->location[j+1]);
            }
        }
        else if(i+1==j){ //It doesn't work for non simmetric instances
            return
            in->distanceGet(s->location[j],s->location[i-1]) +
            in->distanceGet(s->location[i],s->location[j+1]) -
            in->distanceGet(s->location[i],s->location[i-1]) -
            in->distanceGet(s->location[j],s->location[j+1]);
        }
        else {// Exactly like in "O kit"
            return in->distanceGet(s->location[i],s->location[j-1]) +
                        in->distanceGet(s->location[i],s->location[j+1]) +
                        in->distanceGet(s->location[j],s->location[i-1]) +
                        in->distanceGet(s->location[j],s->location[i+1]) -
                        in->distanceGet(s->location[i],s->location[i-1]) -
                        in->distanceGet(s->location[i],s->location[i+1]) -
                        in->distanceGet(s->location[j],s->location[j-1]) -
                        in->distanceGet(s->location[j],s->location[j+1]);
        }
        
    }
    else
    {// MLP
        int t;
            int c;
            int w;

        if( i == 0 )
            return INT_MAX;

        if(i+1 == j)
        {//consecutives = 3 operations

            sigma[0].first = 0;
            sigma[0].second = i-1;

            sigma[1].first = j;
            sigma[1].second = j;

            sigma[2].first = i;
            sigma[2].second = i;

            sigma[3].first = j+1;
            sigma[3].second = in->dimensionGet();

            t = s->T(sigma[0].first,sigma[0].second) + 
                s->t_(sigma[0].second,sigma[1].first) + 
                s->T(sigma[1].first,sigma[1].second);
            
            c = s->C(sigma[0].first,sigma[0].second) + 
                s->W(sigma[1].first,sigma[1].second) * ( 
                    s->T(sigma[0].first,sigma[0].second) +
                    s->t_(sigma[0].second,sigma[1].first)
                ) + 
                s->C(sigma[1].first,sigma[1].second);

            w = s->W(sigma[0].first,sigma[0].second) + 
                s->W(sigma[1].first,sigma[1].second);

            for(int sub=1; sub < 3; sub++){
                c = c + 
                    s->W(sigma[sub+1].first,sigma[sub+1].second) * 
                        ( t + s->t_(sigma[sub].second,sigma[sub+1].first) ) + 
                    s->C(sigma[sub+1].first,sigma[sub+1].second);

                t = t + 
                s->t_(sigma[sub].second,sigma[sub+1].first) + 
                s->T(sigma[sub+1].first,sigma[sub+1].second);
                
                w = w + s->W(sigma[1].first,sigma[1].second);             
            }

            return c - s->costValueMLP;

        }
        else
        { // nonconsecutives = 4 operations
            sigma[0].first = 0;
            sigma[0].second = i-1;

            sigma[1].first = j;
            sigma[1].second = j;

            sigma[2].first = i+1;
            sigma[2].second = j-1;

            sigma[3].first = i;
            sigma[3].second = i;

            sigma[4].first = j+1;
            sigma[4].second = in->dimensionGet();

            t = s->T(sigma[0].first,sigma[0].second) + 
                s->t_(sigma[0].second,sigma[1].first) + 
                s->T(sigma[1].first,sigma[1].second);
            
            c = s->C(sigma[0].first,sigma[0].second) + 
                s->W(sigma[1].first,sigma[1].second) * ( 
                    s->T(sigma[0].first,sigma[0].second) +
                    s->t_(sigma[0].second,sigma[1].first)
                ) + 
                s->C(sigma[1].first,sigma[1].second);

            w = s->W(sigma[0].first,sigma[0].second) + 
                s->W(sigma[1].first,sigma[1].second);

            for(int sub=1; sub < 4; sub++){
                c = c + 
                    s->W(sigma[sub+1].first,sigma[sub+1].second) * 
                        ( t + s->t_(sigma[sub].second,sigma[sub+1].first) ) + 
                    s->C(sigma[sub+1].first,sigma[sub+1].second);

                t = t + 
                s->t_(sigma[sub].second,sigma[sub+1].first) + 
                s->T(sigma[sub+1].first,sigma[sub+1].second);
                
                w = w + s->W(sigma[1].first,sigma[1].second);             
            }

            return c - s->costValueMLP;
        }
    }
}

void Neighborhood::swapMove(Solution* s,int a,int b,double delta){
    if(a > b)
        swap(a,b);

    swap(s->location[a],s->location[b]);
    if(a==0)
            s->location[s->location.size()-1] = s->location[a];

    if(s->in->problemGet()==0){
        s->costValueTSP+=delta;
    }
    else{

        s->costValueMLP+=delta;
        int v1,v2;
        v1 = s->costValueMLP;
        s->updateStructures(a);
        v2 = s->costValueMLP;
        if(v1!=v2){
            cout<<__FILE__<<__LINE__<<endl;
            exit(0);
        }
    }
    
    
}

void Neighborhood::firstTwoOpt(Solution* s){
    bool stuck = false;
    double delta = 0;
    int last = s->location.size()-1;
    //while(!stuck){
        stuck = true;
        for(int i=0; i < last; i++)
            for(int j=i+2; j < last; j++){
                if(!(i==0 && j == last-1)){
                    delta = twoOptDeltaEvaluation(s,i,j);
                    if(delta < 0){
                        twoOptMove(s,i,j,delta);
                        stuck = false;
                    }
                }
            }
   // }
}

void Neighborhood::bestTwoOpt(Solution* s){
  bool stuck = false;
    double delta = 0;

    int i_best;
    int j_best;
    double delta_best;

    int last = s->location.size()-1;
    //while(!stuck){
        stuck = true;
        delta_best = INT_MAX;
        for(int i=0; i < last; i++)
            for(int j=i+2; j < last; j++){
                if(!((i==0 && j == last-1) || (abs(i-j)<2))){
                    delta = twoOptDeltaEvaluation(s,i,j);
                    if(delta < 0 && delta < delta_best){
                        delta_best = delta;
                        i_best = i;
                        j_best = j;
                    }
                }
            }
        if(delta_best < 0){
            twoOptMove(s,i_best,j_best,delta_best);
            stuck = false;
        }
    //}
}

double Neighborhood::twoOptDeltaEvaluation(Solution* s,int i,int j){
    if(i > j)
        swap(i,j);

    int subSequenceCost;
    int subSequenceDelay;
    int subSequenceDuration;
    int totalCost;
    int delta;

    if(s->in->problemGet()==0)
    { // TSP
        return
            in->distanceGet(s->location[i],s->location[j]) +
            in->distanceGet(s->location[i+1],s->location[j+1]) -
            in->distanceGet(s->location[i],s->location[i+1]) -
            in->distanceGet(s->location[j],s->location[j+1]);
    }
    else
    { // MLP

        cout<<__FILE__<<__LINE__<<endl;
        exit(0);
        return s->costValueMLP - totalCost;
    }
    
}

void Neighborhood::twoOptMove(Solution* s,int i,int j, double delta){
    if(i > j)
        swap(i,j);
    reverse(s->location.begin()+i+1,s->location.begin()+j+1);
    if(in->problemGet() == 0)
    { // TSP
        s->costValueTSP+=delta;
    }
    else
    { // MLP
        s->costValueTSP+=delta;
    }
    
    
}

void Neighborhood::firstReInsertion(Solution* s, int size){
    bool stuck = false;
    double delta = 0;
    int last = s->location.size()-1;
    while(!stuck){
        stuck = true;
        for(int origin=0; origin < last; origin++){
            for(int destination=0; destination < last; destination++){
                delta = reInsertionDeltaEvaluation(s,origin,destination,size);
                if(delta < 0){
                    reInsertionMove(s,origin,destination,size,delta);
                    stuck = false;
                }
            }
        }
    }
}

void Neighborhood::bestReInsertion(Solution* s, int size){
    bool stuck = false;
    double delta = 0;
    int origin_best;
    int destination_best;
    double delta_best;
    int last = s->location.size()-1;

    //while(!stuck){
        stuck = true;
        delta_best = INT_MAX;
        for(int origin=0; origin < last; origin++){
            for(int destination=0; destination < last; destination++){
                delta = reInsertionDeltaEvaluation(s,origin,destination,size);
                if(delta < 0 && delta < delta_best){
                    delta_best = delta;
                    origin_best = origin;
                    destination_best = destination;
                }
            }
        }
        if(delta_best < 0){
            reInsertionMove(s,origin_best,destination_best,size,delta_best);
            stuck = false;
        }
    //}
}

double Neighborhood::reInsertionDeltaEvaluation(Solution* s,int origin, int destination, int size){
    Color::Modifier red(Color::FG_RED);
    Color::Modifier deff(Color::FG_DEFAULT);
    double delta = 0;
    int last = s->location.size()-1;
    if(origin + size <= destination && (destination+1)%last!=origin){
            if(origin == 0)
                delta-= in->distanceGet(s->location[last-1],s->location[origin]);
            else
                delta-= in->distanceGet(s->location[origin-1],s->location[origin]);

            delta-= in->distanceGet(s->location[origin+size-1],s->location[origin+size]);
            delta-= in->distanceGet(s->location[destination],s->location[destination+1]);

            if(origin == 0)
                delta+= in->distanceGet(s->location[last-1],s->location[origin+size]);
            else
                delta+= in->distanceGet(s->location[origin-1],s->location[origin+size]);
            delta+= in->distanceGet(s->location[origin],s->location[destination]);
            delta+= in->distanceGet(s->location[destination+1],s->location[origin+size-1]);

            return delta;      
    }
    return INT_MAX;
}

void Neighborhood::reInsertionMove(Solution* s, int origin,int destination, int size, double delta){
    s->costValueTSP+=delta;
    int new_origin;
    int new_destination;
    int last = s->location.size()-1;
    if(origin < destination){        
            rotate(s->location.begin()+origin,s->location.begin()+origin+size,s->location.begin()+destination+1);
    }
    else if(origin > destination){
        if(origin+size < int(s->location.size())){
            rotate(s->location.begin()+destination,s->location.begin()+origin,s->location.begin()+origin+size);
        }else{
            new_origin = (last + (origin-size)%last)%(last);
            new_destination = (last + (destination-size)%last)%(last);
            rotate(s->location.begin(),s->location.begin()+size,s->location.end()-1);            
            if(new_origin > new_destination){
                rotate(s->location.begin()+new_destination,s->location.begin()+new_origin,s->location.begin()+new_origin+size);
            } else if(new_origin < new_destination){
                rotate(s->location.begin()+new_origin,s->location.begin()+new_origin+size,s->location.begin()+new_destination+1);            
            }
        }

    } else {
        cout<<__FILE__<<__LINE__<<endl;
        cout<<"Origin and destination are the same!"<<endl;
    }
    s->location[last] =s->location[0];        
}
