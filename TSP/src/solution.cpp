#include "solution.h"
#include "colormod.h"
#include <cmath>
#include <climits>
using namespace std;

Solution::Solution(Input *input){

    this->in = input;

    // Create a generic basic solution
    for(unsigned i=1; i <= this->in->dimensionGet(); i++)
        location.push_back(i);
    location.push_back(1);

    // Resize already fill the ellements with zeros!!!
    duration.resize(in->dimensionGet()+1, vector<int>(in->dimensionGet()+1));
    cost.resize(in->dimensionGet()+1, vector<int>(in->dimensionGet()+1));

    computeCostValueTSP();
    computeCostValueMLP();

}

double Solution::t_(unsigned i, unsigned j){
    return in->distanceGet(location[i],location[j]);
}

void Solution::reset(){
    costValueTSP = INT_MAX;
    costValueMLP = INT_MAX;
    for (auto &i : duration)
        fill(i.begin(), i.end(), -1);
    for (auto &i : cost)
        fill(i.begin(), i.end(), -1);
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

int Solution::T(int begin, int end){
    if(duration[begin][end]!= -1)
        return duration[begin][end];
    int answer = 0;
    if(begin >= end){
        answer = 0;
    }
    else{
        int middle = floor((begin+end)/2);
        answer = T(begin,middle) + in->distanceGet(location[middle],location[middle+1]) + T(middle+1,end);
    }
    duration[begin][end] = answer;
    return answer;
}

// int Solution::C(int begin, int end){
//     return cost[begin][end];
// }

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
                T(begin,end-1) +
                in->distanceGet(location[end-1],location[end])) 
                + C_recursive(end,end);
    }
    cost[begin][end] = answer;
    return answer;
}

void Solution::computeCostValueMLP(){

    for (auto &i : duration)
        fill(i.begin(), i.end(), -1);

    for (auto &i : cost)
        fill(i.begin(), i.end(), -1);

    costValueMLP = C_recursive(0,this->in->dimensionGet());
}

void Solution::computeCostValueTSP(){
    costValueTSP = 0.0;
    for(unsigned i=0; i < location.size()-1; i++)
        costValueTSP += in->distanceGet(location[i],location[i+1]);
}

ostream & operator << (ostream &out, const Solution &s)
{
    Color::Modifier red(Color::FG_RED);
    Color::Modifier deff(Color::FG_DEFAULT);
    double myCost = 0;
    if(s.location.size() <= s.in->dimensionGet())
        out << red << "Warning! Incomplete solution!!!" << deff<< endl;
    if(s.costValueTSP < 0)
        out << red << "Negative distance!!!" << deff<< endl;

    out << "Dimension: "<<s.in->dimensionGet() << endl;
    out << "Distance: "<<setprecision (1)<<fixed<<s.costValueTSP << endl;
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

    for(unsigned i=0; i < s.location.size()-1; i++){
        myCost+=s.in->distanceGet(s.location[i],s.location[i+1]);
    }

    if(abs(myCost - s.costValueTSP) > 0.0001)
        out << red << "Something wrong...the cost should be: " <<myCost<<deff<<endl;
    return out;
}