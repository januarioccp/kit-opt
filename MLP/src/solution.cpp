#include "solution.h"
#include "colormod.h"
#include <cmath>
#include <climits>
using namespace std;

Solution::Solution(Input *input){

    this->in = input;
    contador = 0;
    // Create a generic basic solution
    for(unsigned i=1; i <= this->in->dimensionGet(); i++)
        location.push_back(i);
    location.push_back(1);

    // Resize already fill the ellements with zeros!!!
    duration.resize(in->dimensionGet()+1, vector<int>(in->dimensionGet()+1));
    cost.resize(in->dimensionGet()+1, vector<int>(in->dimensionGet()+1));

}

double Solution::t_(unsigned i, unsigned j){
    return in->distanceGet(location[i],location[j]);
}

void Solution::reset(){
    costValueMLP = INT_MAX;
    
    for(unsigned i=0; i < duration.size(); i++)
        for(unsigned j=i; j < duration[i].size(); j++)
            if(i!=j)
                duration[i][j] = -1;
            else
                duration[i][j] = 0;
        
    for(unsigned i=0; i < cost.size(); i++)
        for(unsigned j=i; j < cost[i].size(); j++)
            if(i!=j)
                cost[i][j] = -1;
            else
                cost[i][j] = 0;
    
    location.resize(0);
}

int Solution::W(int begin, int end){
    if(begin > end)
        swap(begin,end);
    if(begin == 0)
        begin++;
    if(end == 0)
        return 0;
    return end-begin+1;
}

int Solution::T_recursive(int begin, int end){
    if(duration[begin][end]!= -1)
        return duration[begin][end];
    int answer = 0;
    if(begin >= end){
        answer = 0;
    }
    else{
        // int middle = floor((begin+end)/2);
        // answer = T(begin,middle) + in->distanceGet(location[middle],location[middle+1]) + T(middle+1,end);
        answer = T_recursive(begin,end-1) + in->distanceGet(location[end-1],location[end]) + T_recursive(end,end);
    }
    duration[begin][end] = answer;
    
    return answer;
}

int Solution::C(int begin, int end){
     return cost[begin][end];
}

int Solution::C_recursive(int begin, int end){
    if(cost[begin][end]!= -1)
        return cost[begin][end];
    int answer = 0;
    if(begin >= end){
        answer = 0;
    }
    else{
        answer = C_recursive(begin,end-1) + 
            W(end,end) *(
                T_recursive(begin,end-1) +
                in->distanceGet(location[end-1],location[end])) 
                + C_recursive(end,end);
    }
    cost[begin][end] = answer;
    return answer;
}

void Solution::computeCostValueMLP(){

    for(unsigned i=0; i < duration.size(); i++)
        for(unsigned j=i; j < duration[i].size(); j++)
            if(i!=j)
                duration[i][j] =  duration[i][j-1] + in->distanceGet(location[j-1],location[j]);
            else
                duration[j][j] = 0;

    for(unsigned i=0; i < cost.size(); i++)
        for(unsigned j=i; j < cost[i].size(); j++)
            if(i!=j)
                cost[i][j] = cost[i][j-1] + duration[i][j];
            else
                cost[i][j] = 0;

    costValueMLP = C(0,this->in->dimensionGet());
}

void Solution::update(int a, int b){
    if(a>b)
        swap(a,b);

        for(unsigned i=0; i <= a; i++)
            for(unsigned j=a; j < duration[i].size(); j++)
                if(i!=j)
                    duration[i][j] =  duration[i][j-1] + in->distanceGet(location[j-1],location[j]);
                else
                    duration[j][j] = 0;

        for(unsigned i=a+1; i <= b; i++)
            for(unsigned j=b; j < duration[i].size(); j++)
                if(i!=j)
                    duration[i][j] =  duration[i][j-1] + in->distanceGet(location[j-1],location[j]);
                else
                    duration[j][j] = 0;

        for(unsigned i=0; i <= a; i++)
            for(unsigned j=a; j < cost[i].size(); j++)
                if(i!=j)
                    cost[i][j] = cost[i][j-1] + duration[i][j];
                else
                    cost[i][j] = 0;

        for(unsigned i=a+1; i <= b; i++)
            for(unsigned j=b; j < cost[i].size(); j++)
                if(i!=j)
                    cost[i][j] = cost[i][j-1] + duration[i][j];
                else
                    cost[i][j] = 0;

    costValueMLP = C(0,this->in->dimensionGet());
}

void Solution::copy(const Solution *s){
    this->location = s->location;
    this->costValueMLP = s->costValueMLP;
}

void Solution::printCost(){
    cout<<endl;
    for(unsigned i=0; i < cost.size(); i++){
        for(unsigned j=0; j < cost[i].size(); j++)
        {
            cout<<setw(6)<<cost[i][j];
        }
        cout<<endl;
    }
}

void Solution::printDuration(){
    cout<<endl;
    for(unsigned i=0; i < duration.size(); i++){
        for(unsigned j=0; j < duration[i].size(); j++)
        {
            cout<<setw(5)<<duration[i][j];
        }
        cout<<endl;
    }
}

ostream & operator << (ostream &out, const Solution &s)
{
    Color::Modifier red(Color::FG_RED);
    Color::Modifier deff(Color::FG_DEFAULT);
    
    if(s.location.size() <= s.in->dimensionGet())
        out << red << "Warning! Incomplete solution!!!" << deff<< endl;

    out << "Dimension: "<<s.in->dimensionGet() << endl;
    out << "Latency: "<<setprecision (1)<<fixed<<s.costValueMLP << endl;
    out<< "i = [";
    for(unsigned i=0; i < s.location.size(); i++)
        out << setw(3) << i << " ";
    out<< "]"<<endl;
    out<< "s = {";
    for(unsigned i=0; i < s.location.size(); i++)
        if( i < s.location.size() - 1){
            if(s.location[i] <= 7)
                    out << "\033[" << 30+s.location[i] << "m";
            if(8 <= s.location[i] && s.location[i] <= 14)
                    out << "\033[" << 83+s.location[i] << "m";
            out << setw(3) << s.location[i] <<deff<< ",";
        }
        else{
            if(s.location[i] <= 7)
                    out << "\033[" << 30+s.location[i] << "m";
            if(8 <= s.location[i] && s.location[i] <= 14)
                    out << "\033[" << 83+s.location[i] << "m";
            out << setw(3) << s.location[i]  <<deff;
        }
    out << "}" << endl;

    return out;
}