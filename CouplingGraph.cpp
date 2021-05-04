#include "CouplingGraph.h"

CouplingGraph::~CouplingGraph(){
    for(int i=0; i<positions; i++)
        delete [] dist[i];
    delete [] dist;
}

void CouplingGraph::initDist(){
    dist = new int*[positions];
    path = new int*[positions];
    for(int i=0; i<positions; i++)
    {
        dist[i] = new int[positions];
        path[i] = new int[positions];
    }

    for(int i=0; i<positions; i++)
        for(int j=0; j<positions; j++)
        {
            if(i==j)
                dist[i][j]=0;
            else
                dist[i][j]=MAX_DIST;
            path[i][j] = j;
        }


    //Initialize distance matrix to a adjacency matrix
    for(int i=0;i<positions;i++){
        for(unsigned int j=0;j<adjList[i].size();j++){
            int k=adjList[i][j];
            dist[i][k]=1;
        }
    }

    computeDist();
}



void CouplingGraph::buildQX20(){
    positions=20;
    adjList.resize(positions);
    adjList[0].push_back(1);
    adjList[0].push_back(5);
    adjList[1].push_back(0);
    adjList[1].push_back(2);
    adjList[1].push_back(6);
    adjList[1].push_back(7);
    adjList[2].push_back(1);
    adjList[2].push_back(3);
    adjList[2].push_back(6);
    adjList[2].push_back(7);
    adjList[3].push_back(2);
    adjList[3].push_back(4);
    adjList[3].push_back(8);
    adjList[3].push_back(9);
    adjList[4].push_back(3);
    adjList[4].push_back(8);
    adjList[4].push_back(9);
    adjList[5].push_back(0);
    adjList[5].push_back(6);
    adjList[5].push_back(10);
    adjList[5].push_back(11);
    adjList[6].push_back(1);
    adjList[6].push_back(2);
    adjList[6].push_back(5);
    adjList[6].push_back(7);
    adjList[6].push_back(10);
    adjList[6].push_back(11);
    adjList[7].push_back(1);
    adjList[7].push_back(2);
    adjList[7].push_back(6);
    adjList[7].push_back(8);
    adjList[7].push_back(12);
    adjList[7].push_back(13);
    adjList[8].push_back(3);
    adjList[8].push_back(4);
    adjList[8].push_back(7);
    adjList[8].push_back(9);
    adjList[8].push_back(12);
    adjList[8].push_back(13);
    adjList[9].push_back(3);
    adjList[9].push_back(4);
    adjList[9].push_back(8);
    adjList[9].push_back(14);
    adjList[10].push_back(5);
    adjList[10].push_back(6);
    adjList[10].push_back(11);
    adjList[10].push_back(15);
    adjList[11].push_back(5);
    adjList[11].push_back(6);
    adjList[11].push_back(10);
    adjList[11].push_back(12);
    adjList[11].push_back(16);
    adjList[11].push_back(17);
    adjList[12].push_back(7);
    adjList[12].push_back(8);
    adjList[12].push_back(11);
    adjList[12].push_back(13);
    adjList[12].push_back(16);
    adjList[12].push_back(17);
    adjList[13].push_back(7);
    adjList[13].push_back(8);
    adjList[13].push_back(12);
    adjList[13].push_back(14);
    adjList[13].push_back(18);
    adjList[13].push_back(19);
    adjList[14].push_back(9);
    adjList[14].push_back(13);
    adjList[14].push_back(18);
    adjList[14].push_back(19);
    adjList[15].push_back(10);
    adjList[15].push_back(16);
    adjList[16].push_back(11);
    adjList[16].push_back(12);
    adjList[16].push_back(15);
    adjList[16].push_back(17);
    adjList[17].push_back(11);
    adjList[17].push_back(12);
    adjList[17].push_back(16);
    adjList[17].push_back(18);
    adjList[18].push_back(13);
    adjList[18].push_back(14);
    adjList[18].push_back(17);
    adjList[18].push_back(19);
    adjList[19].push_back(13);
    adjList[19].push_back(14);
    adjList[19].push_back(18);
    initDist();
}

void CouplingGraph::buildQ16(){
    positions=16;
    adjList.resize(positions);

    adjList[0].push_back(1);
    adjList[0].push_back(4);
    adjList[0].push_back(5);

    adjList[1].push_back(0);
    adjList[1].push_back(2);
    adjList[1].push_back(4);
    adjList[1].push_back(5);

    adjList[2].push_back(1);
    adjList[2].push_back(3);
    adjList[2].push_back(6);
    adjList[2].push_back(7);

    adjList[3].push_back(2);
    adjList[3].push_back(6);
    adjList[3].push_back(7);

    adjList[4].push_back(0);
    adjList[4].push_back(1);
    adjList[4].push_back(5);
    adjList[4].push_back(8);

    adjList[5].push_back(0);
    adjList[5].push_back(1);
    adjList[5].push_back(4);
    adjList[5].push_back(6);
    adjList[5].push_back(9);
    adjList[5].push_back(10);

    adjList[6].push_back(2);
    adjList[6].push_back(3);
    adjList[6].push_back(5);
    adjList[6].push_back(7);
    adjList[6].push_back(9);
    adjList[6].push_back(10);

    adjList[7].push_back(2);
    adjList[7].push_back(3);
    adjList[7].push_back(6);
    adjList[7].push_back(11);

    adjList[8].push_back(4);
    adjList[8].push_back(9);
    adjList[8].push_back(12);
    adjList[8].push_back(13);

    adjList[9].push_back(5);
    adjList[9].push_back(6);
    adjList[9].push_back(8);
    adjList[9].push_back(10);
    adjList[9].push_back(12);
    adjList[9].push_back(13);

    adjList[10].push_back(5);
    adjList[10].push_back(6);
    adjList[10].push_back(9);
    adjList[10].push_back(11);
    adjList[10].push_back(14);
    adjList[10].push_back(15);

    adjList[11].push_back(7);
    adjList[11].push_back(10);
    adjList[11].push_back(14);
    adjList[11].push_back(15);

    adjList[12].push_back(8);
    adjList[12].push_back(9);
    adjList[12].push_back(13);

    adjList[13].push_back(8);
    adjList[13].push_back(9);
    adjList[13].push_back(12);
    adjList[13].push_back(14);

    adjList[14].push_back(10);
    adjList[14].push_back(11);
    adjList[14].push_back(13);
    adjList[14].push_back(15);

    adjList[15].push_back(10);
    adjList[15].push_back(11);
    adjList[15].push_back(14);
    //Initialize distance matrix to a adjacency matrix
    initDist();
}

void CouplingGraph::buildQ10(){
    positions=10;
    adjList.resize(positions);

    adjList[0].push_back(1);
    adjList[0].push_back(3);

    adjList[1].push_back(0);
    adjList[1].push_back(2);
    adjList[1].push_back(4);
    adjList[1].push_back(5);

    adjList[2].push_back(1);
    adjList[2].push_back(4);
    adjList[2].push_back(5);

    adjList[3].push_back(0);
    adjList[3].push_back(4);
    adjList[3].push_back(6);
    adjList[3].push_back(7);

    adjList[4].push_back(1);
    adjList[4].push_back(2);
    adjList[4].push_back(3);
    adjList[4].push_back(5);
    adjList[4].push_back(6);
    adjList[4].push_back(7);

    adjList[5].push_back(1);
    adjList[5].push_back(2);
    adjList[5].push_back(4);
    adjList[5].push_back(8);
    adjList[5].push_back(9);

    adjList[6].push_back(3);
    adjList[6].push_back(4);
    adjList[6].push_back(7);

    adjList[7].push_back(3);
    adjList[7].push_back(4);
    adjList[7].push_back(6);
    adjList[7].push_back(8);

    adjList[8].push_back(5);
    adjList[8].push_back(7);
    adjList[8].push_back(9);

    adjList[9].push_back(5);
    adjList[9].push_back(8);

    //Initialize distance matrix to a adjacency matrix
    initDist();
}

void CouplingGraph::buildQ9(){
    positions=9;
    adjList.resize(positions);

    adjList[0].push_back(1);
    adjList[0].push_back(3);

    adjList[1].push_back(0);
    adjList[1].push_back(2);
    adjList[1].push_back(4);
    adjList[1].push_back(5);

    adjList[2].push_back(1);
    adjList[2].push_back(4);
    adjList[2].push_back(5);

    adjList[3].push_back(0);
    adjList[3].push_back(4);
    adjList[3].push_back(6);
    adjList[3].push_back(7);

    adjList[4].push_back(1);
    adjList[4].push_back(2);
    adjList[4].push_back(3);
    adjList[4].push_back(5);
    adjList[4].push_back(6);
    adjList[4].push_back(7);

    adjList[5].push_back(1);
    adjList[5].push_back(2);
    adjList[5].push_back(4);
    adjList[5].push_back(8);

    adjList[6].push_back(3);
    adjList[6].push_back(4);
    adjList[6].push_back(7);

    adjList[7].push_back(3);
    adjList[7].push_back(4);
    adjList[7].push_back(6);
    adjList[7].push_back(8);

    adjList[8].push_back(5);
    adjList[8].push_back(7);

    //Initialize distance matrix to a adjacency matrix
    initDist();
}

void CouplingGraph::buildSquare(){
    positions=6;
    adjList.resize(positions);
    adjList[0].push_back(1);
    adjList[0].push_back(2);
    adjList[1].push_back(0);
    adjList[1].push_back(3);
    adjList[2].push_back(0);
    adjList[2].push_back(3);
    adjList[2].push_back(4);
    adjList[3].push_back(1);
    adjList[3].push_back(2);
    adjList[3].push_back(5);
    adjList[4].push_back(2);
    adjList[4].push_back(5);
    adjList[5].push_back(3);
    adjList[5].push_back(4);

    //Initialize distance matrix to a adjacency matrix
    initDist();
}

//Floyd-Warshall Algorithm
 void CouplingGraph::computeDist(){
 for(int k = 0 ; k < positions ; k ++)
    {
        for(int i = 0 ; i < positions ; i ++)
        {
            for(int j = 0 ; j < positions ; j ++)
            {
                if(dist[i][j] > dist[i][k] + dist[k][j])
                {
                    dist[i][j] = dist[i][k] + dist[k][j];
                    path[i][j] = path[i][k];
                }
            }
        }
    }
 }

 vector<int> CouplingGraph::getPath(int i, int j){
    vector<int> pathBetween;
    pathBetween.push_back(i);
    int k = path[i][j];
    while(k!=j){
        pathBetween.push_back(k);
        k = path[k][j];
    }
    pathBetween.push_back(j);
    return pathBetween;
 }

 void CouplingGraph::showInfo(){
    for(unsigned int i=0; i<adjList.size(); i++)
    {
        cout<<i<<":";
        for(unsigned int j=0;j<adjList[i].size();j++)
            cout<<adjList[i][j]<<",";
        cout<<std::endl;
    }
 }
