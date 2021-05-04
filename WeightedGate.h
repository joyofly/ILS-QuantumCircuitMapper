#ifndef WEIGHTEDGATE_H_INCLUDED
#define WEIGHTEDGATE_H_INCLUDED

//Record the cost of applying some SWAP gate or remote CNOT
struct weightedGate{
    string type;
    int q1;
    int q2;
    int weight; //cost
    //vector<gate> exeArr; //all executed gates
    weightedGate(string tp = "SWAP",int c=-1,int t=-1,int w=-1){
        type = tp;
        q1 = c;
        q2 = t;
        weight = w;
    }

    bool operator <(const weightedGate & otherGate) const{
        if(type < otherGate.type)
            return true;
        if(type > otherGate.type)
            return false;
        if(q1 < otherGate.q1)
            return true;
        if(q1 > otherGate.q1)
            return false;
        if(q2 < otherGate.q2)
            return true;
        return false;
    }
};

#endif // WEIGHTEDGATE_H_INCLUDED
