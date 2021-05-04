#include "QASMReader.h"
#include <string>
using namespace std;

void QASMReader::parse(vector<gate>& gateList){
    string line="";
    int id = 0;
    while(getline(fin,line)){
        linenum++;
        //Ignore the lines without "CX" gates
        if(!line.empty() && line.find("cx")!=string::npos ){
            int start1 = line.find("[");
            int end1 = line.find("]");
            //Control qubit
            string numStr1 = line.substr(start1+1,end1-start1);
            int control = stoi(numStr1);
            vertices.insert(control);

            int start2 = line.find("[",start1+1);
            int end2 = line.find("]",end1+1);
            //Target qubit
            string numStr2 = line.substr(start2+1,end2-start2);
            int target = stoi(numStr2);
            vertices.insert(target);

            gate gate1(control,target,id++);
            gateList.push_back(gate1);
        }
    }
}


