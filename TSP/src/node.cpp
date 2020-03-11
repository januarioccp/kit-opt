#include "node.h"
using namespace std;

Node::Node( Input* input){
    this->in = input;
}

Node::~Node(){
    this->in = NULL;
}
