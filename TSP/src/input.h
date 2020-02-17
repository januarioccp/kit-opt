#ifndef INPUT_H_INCLUDED
#define INPUT_H_INCLUDED
#include <iostream>
using namespace std;

class Input{
    public:
        Input(){};
        Input( int , char** );
        double ** matrizAdj; // matriz de adjacencia
        int dimension; // quantidade total de vertices
        friend ostream & operator << (ostream &out, const Input &c);
        
};

#endif //INPUT_H_INCLUDED