#ifndef COUPLINGGRAPH_H_INCLUDED
#define COUPLINGGRAPH_H_INCLUDED

#include <iostream>
#include <vector>
#include <stdlib.h>

using namespace std;

#define MAX_DIST 1000

class CouplingGraph{
    private:
        void initDist();

    public:
        vector<vector<int>> adjList;
        int positions;  //number of physical qubits
        int ** dist; //distance matrix
        int ** path; //path matrix
    public:
        CouplingGraph() {
            positions = 0;
        }
        CouplingGraph(int pos);
        ~CouplingGraph();

        int ** getDist() {return dist;}

        int getPositions() {return positions;}

        //build coupling graph for IBM Q Tokyo
        void buildQX20();

        void buildQ9();

        void buildQ10();

        void buildQ16();

        //build testing coupling graph of square
        void buildSquare();

        //get the distance matrix with Floyd-Warshall algorithm
        void computeDist();

        vector<int> getPath(int i,int j);

        void showInfo();

};

#endif
