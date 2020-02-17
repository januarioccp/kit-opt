#include "solution.h"
#include "colormod.h"

Solution::Solution(Input *input){
    this->in = input;
    for(int i=0; i < this->in->dimension; i++)
        location.push_back(i);
}

void Solution::reset(){
    costValue = 0;
    location.resize(0);
}

void Solution::computeCostValue(){
    costValue = 0;
    for(int i=0; i < location.size()-1; i++)
        costValue += in->matrizAdj[i][i+1];
}

ostream & operator << (ostream &out, const Solution &s)
{

    Color::Modifier red(Color::FG_RED);
    Color::Modifier deff(Color::FG_DEFAULT);

    if(s.location.size() < s.in->dimension)
        out << red << "Warning! Incomplete solution!!!" << deff<< endl;

    out << "Dimension: "<<s.in->dimension << endl;
    out << "Cost value: "<<s.costValue << endl;
    cout<< "s = {";
    for(int i=0; i < s.location.size(); i++)
        if( i < s.location.size() - 1)
            out << setw(3) << s.location[i] << ",";
        else
            out << setw(3) << s.location[i];
    cout << "}" << endl;
    return out;
}