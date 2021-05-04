#ifndef GATE_H_INCLUDED
#define GATE_H_INCLUDED

//CNOT gate
struct gate{
    int control;
    int target;
    int weight; //order in original circuit
    int depth;
    gate(int c=-1,int t=-1,int w=-1, int d=-1){
        control = c;
        target = t;
        weight = w;
        depth = d;
    }
    bool operator ==(const gate & otherGate){
        return control == otherGate.control && target == otherGate.target  && weight == otherGate.weight ;
    }
};


#endif // GATE_H_INCLUDED
