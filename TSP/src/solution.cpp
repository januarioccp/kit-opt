#include "solution.h"
#include "colormod.h"

Solution::Solution(Input *input){
    this->in = input;
    for(int i=1; i <= this->in->dimension; i++)
        location.push_back(i);
    location.push_back(1);
    this->computeCostValue();
}

void Solution::reset(){
    costValue = 0;
    location.resize(0);
}

void Solution::computeCostValue(){
    costValue = 0.0;
    for(int i=0; i < location.size()-1; i++)
        costValue += in->matrizAdj[location[i]][location[i+1]];
}

ostream & operator << (ostream &out, const Solution &s)
{

    Color::Modifier red(Color::FG_RED);
    Color::Modifier deff(Color::FG_DEFAULT);
    double myCost = 0;
    if(s.location.size() <= s.in->dimension)
        out << red << "Warning! Incomplete solution!!!" << deff<< endl;

    out << "Dimension: "<<s.in->dimension << endl;
    out << "Cost value: "<<setprecision (1)<<fixed<<s.costValue << endl;
    out<< "i = [";
    for(int i=0; i < s.location.size(); i++)
        out << setw(3) << i << " ";
    out<< "]"<<endl;
    out<< "s = {";
    for(int i=0; i < s.location.size(); i++)
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

    for(int i=0; i < s.location.size()-1; i++)
        myCost+=s.in->matrizAdj[s.location[i]][s.location[i+1]];

    if(myCost != s.costValue)
        out << red << "Something wrong...the cost should be: " <<myCost<<deff<<endl;
    return out;
}