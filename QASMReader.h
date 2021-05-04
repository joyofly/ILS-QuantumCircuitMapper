#ifndef QASMREADER_H_INCLUDED
#define QASMREADER_H_INCLUDED

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <set>
#include <stdlib.h>
#include "gate.h"

using namespace std;

class QASMReader{
private:
    string fileName;
    int linenum;
    ifstream fin;
    //逻辑量子位集合
    set<int> vertices;

public:
    QASMReader(){linenum=0;}
    QASMReader(string filename):fin(filename){
        fileName = filename;
        linenum=0;
        //fin.open(fileName.c_str(),ios::in);
        if(!fin.is_open()){
             cerr<<fileName<<"does not exist"<<endl;
             exit(-1);
        }

    }
    ~QASMReader(){
        fin.close();
    }

    int getlinenum(){return linenum;}
    int getVerNum(){return vertices.size();}
    /*read reduced quantum circuits containing only CNOT gates*/
    void parse(vector<gate>& gateList);
};

#endif // QASMREADER_H_INCLUDED
