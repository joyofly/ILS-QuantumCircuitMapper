#include <iostream>
#include <cstring>
#include <vector>
#include <list>
#include <map>
#include <queue>
#include <fstream>
#include <iomanip>
#include <ctime>
#include <chrono>
#include<io.h>
#include "gate.h"
#include "QASMReader.h"
#include "CouplingGraph.h"
#include "WeightedGate.h"
#include <algorithm>
#define DEBUG

using namespace std;

// Log file
ofstream fout;

/**
*Generate a random initial mapping
*
*/
void randInitMapping(vector<int> & qubits, vector<int> & locations,int nlocs, int nqubits)
{
    for(int i = 0;i < nlocs; i++)
    {
        if(i < nqubits)
            qubits[i] = i;
        else
            qubits[i] = -1;
    }
    std::mt19937 g(std::chrono::system_clock::now().time_since_epoch().count());
    std::shuffle(qubits.begin(), qubits.begin() + nlocs, g);
    //std::shuffle(qubits.begin(), qubits.begin() + nlocs, g);
    for(int i = 0;i < nlocs; i++)
    {
        if(qubits[i] == -1)
            continue;
        locations[qubits[i]] = i;
    }
}

/**
*Generate a random initial mapping
*
*/
void PerurbationByShuffle(vector<int> & qubits, vector<int> & locations,int nlocs, int nqubits)
{
    std::mt19937 g(std::chrono::system_clock::now().time_since_epoch().count());
    std::shuffle(qubits.begin(), qubits.begin() + nlocs, g);

    for(int i = 0;i < nlocs; i++)
    {
        if(qubits[i] == -1)
            continue;
        locations[qubits[i]] = i;
    }
}

/**
*Generate a IdentityInitMapping initial mapping
*
*/
void identityInitMapping(vector<int> & qubits, vector<int> & locations,int nlocs, int nqubits)
{
    for(int i = 0;i < nlocs; i++)
    {
        if(i < nqubits)
            qubits[i] = i;
        else
            qubits[i] = -1;
    }
    for(int i = 0;i < nlocs; i++)
    {
        if(qubits[i] == -1)
            continue;
        locations[qubits[i]] = i;
    }
}

/**
* The interaction matrix of logical qubits
* interact[i][j]:number gate(i,j)
*/
int ** initInteractionMatrix(int nqubits)
{
    int ** interactionMat = new int*[nqubits];
    for(int i=0;i<nqubits;i++)
        interactionMat[i] = new int[nqubits];
    for(int i=0;i<nqubits;i++)
        for(int j=0;j<nqubits;j++)
            interactionMat[i][j] = 0;
    return interactionMat;
}

void clearInteractionMatrix(int ** interationMat,int nqubits)
{
    for(int i=0;i<nqubits;i++)
        delete [] interationMat[i];
    delete [] interationMat;
}

void getInteractionMatrix( vector<gate> gateList, int ** interactMat, int startIndex = 0)
{
    for(size_t i=startIndex;i<gateList.size();i++)
    {
        gate g1 = gateList[i];
        int c1 = g1.control;
        int t1 = g1.target;
        interactMat[c1][t1]++;
        interactMat[t1][c1]++;
    }
}

/**
* Sum of distance between 2 qubits of all 2-qubit gates
* under given mapping
*/
int getTotalCost(int ** interactMat, int nqubits, int ** dist, vector<int> locations)
{
    int totalCost = 0;
    for(int i=0;i<nqubits;i++)
        for(int j=0; j<i; j++)
            totalCost = totalCost + interactMat[i][j] * dist[locations[i]][locations[j]];
    return totalCost;
}

/**
* Generate an random distance matrix
*
*/
int ** getRandDistMatrix(int nlocations)
{
    int ** distMat = new int*[nlocations];
    for(int i=0;i<nlocations;i++)
        distMat[i] = new int[nlocations];
    for(int i=0;i<nlocations;i++)
        for(int j=0;j<nlocations;j++)
            distMat[i][j] = rand()%5;
    return distMat;
}

void clearRandDistMatrix(int ** distMat,int nlocations)
{
    for(int i=0;i<nlocations;i++)
        delete [] distMat[i];
    delete [] distMat;
}

/**
* Partition the logic circuit into n gate arrays,
* lines[i] holds all the gate along the ith qubit.
*/
void genLines(vector<list<gate> > & lines, vector<gate> & gateList)
{
    for(unsigned int i=0; i<gateList.size(); i++)
    {
        gate g1 = gateList[i];
        int c1 = g1.control;
        int t1 = g1.target;
        lines[c1].push_back(g1);
        lines[t1].push_back(g1);
    }
}

/**
*Fill in the Active gate set
*lines:gate sequence of each logical qubit/line
*activeGate: array of active gates
*/
void findActivegates(vector<list<gate> > & lines,vector<gate> & activeGates,vector<gate> & phyCir,set<int> & activeLines, vector<int> locations, int ** dist)
{
    while(!activeLines.empty())
    {
        for(set<int>::iterator it = activeLines.begin();it != activeLines.end(); it++)
        {
            int i = * it;
            int j = 0;
            if(!lines[i].empty())
            {
                gate g1 = lines[i].front();
                int c = g1.control;
                int t = g1.target;
                if(c == i)
                    j = t;
                else
                    j = c;
                gate g2 = lines[j].front();
                if(g1==g2) //active gate
                {
                    int locOfC = locations[c];
                    int locOfT = locations[t];
                    //executable gate
                    if(dist[locOfC][locOfT] == 1)
                    {
                        gate pg(locOfC,locOfT, g1.weight);
                        lines[i].pop_front();
                        lines[j].pop_front();
                        phyCir.push_back(pg);
                        activeLines.insert(j);
                    }
                    else
                    {
                        activeGates.push_back(g1);
                        activeLines.erase(i);
                        activeLines.erase(j);
                    }

                }
                else
                {
                    activeLines.erase(i);
                }
                break;
            }
            else
            {
                activeLines.erase(i);
                break;
            }
        }
    }
}

/**
*Get the number of executable gates under the mapping
*lines:gate sequence of each logical qubit/line
*activeGate: array of active gates
*/
int numOfExecuted(vector<list<gate> > & lines, const vector<int> & locations, int ** dist)
{
    int exeNum = 0;
    set<int> activeLines;
    //Save gate deleted from lines for recovery
    vector<gate> executedGates;

    for(size_t i=0 ; i<lines.size(); i++)
        activeLines.insert(i);

    while(!activeLines.empty())
    {
        for(set<int>::iterator it = activeLines.begin();it != activeLines.end(); it++)
        {
            int i = * it;
            int j = 0;
            if(!lines[i].empty())
            {
                gate g1 = lines[i].front();
                int c = g1.control;
                int t = g1.target;
                if(c == i)
                    j = t;
                else
                    j = c;
                gate g2 = lines[j].front();
                if(g1==g2) //active gate
                {
                    int locOfC = locations[c];
                    int locOfT = locations[t];
                    //executable gate
                    if(dist[locOfC][locOfT] == 1)
                    {
                        exeNum++;
                        lines[i].pop_front();
                        lines[j].pop_front();
                        activeLines.insert(j);
                        executedGates.push_back(g1);
                    }
                    else
                    {
                        activeLines.erase(i);
                        activeLines.erase(j);
                    }

                }
                else
                {
                    activeLines.erase(i);
                }
                break;
            }
            else
            {
                activeLines.erase(i);
                break;
            }
        }
    }
    //Recover lines
    for(int i=executedGates.size()-1;i>=0;i--)
    {
        gate g1 = executedGates[i];
        int c = g1.control;
        int t = g1.target;
        lines[c].push_front(g1);
        lines[t].push_front(g1);
    }
    return exeNum;
}

/**
*Return all the executable gates under the mapping
*lines:gate sequence of each logical qubit/line
*activeGate: array of active gates
*/
vector<gate> findExecuted(vector<list<gate> >  lines, vector<int> locations, int ** dist)
{
    vector<gate> exexutedGate;
    set<int> activeLines;

    for(size_t i=0 ; i<lines.size(); i++)
        activeLines.insert(i);

    while(!activeLines.empty())
    {
        for(set<int>::iterator it = activeLines.begin();it != activeLines.end(); it++)
        {
            int i = * it;
            int j = 0;
            if(!lines[i].empty())
            {
                gate g1 = lines[i].front();
                int c = g1.control;
                int t = g1.target;
                if(c == i)
                    j = t;
                else
                    j = c;
                gate g2 = lines[j].front();
                if(g1==g2) //active gate
                {
                    int locOfC = locations[c];
                    int locOfT = locations[t];
                    //executable gate
                    if(dist[locOfC][locOfT] == 1)
                    {
                        gate pg(locOfC,locOfT, g1.weight);
                        lines[i].pop_front();
                        lines[j].pop_front();
                        exexutedGate.push_back(pg);
                        activeLines.insert(j);
                    }
                    else
                    {
                        activeLines.erase(i);
                        activeLines.erase(j);
                    }

                }
                else
                {
                    activeLines.erase(i);
                }
                break;
            }
            else
            {
                activeLines.erase(i);
                break;
            }
        }
    }
    return exexutedGate;
}

/**
*Check each gate in active gate set for whether it satisfy the connectivity constraints
*and append such gates to physical circuit.
*/
void checkActivegates(vector<list<gate> > & lines,vector<gate> & activeGates,vector<gate> & phyCir,set<int> & activeLines, vector<int> locations, int ** dist)
{
    for(vector<gate>::iterator it = activeGates.begin(); it!=activeGates.end(); )
    {
        gate g = * it;
        int c = g.control;
        int t = g.target;
        int locOfC = locations[c];
        int locOfT = locations[t];
        //execute gate
        if(dist[locOfC][locOfT] == 1)
        {
            gate pg(locOfC,locOfT, g.weight);
            lines[c].pop_front();
            lines[t].pop_front();
            phyCir.push_back(pg);
            activeLines.insert(c);
            activeLines.insert(t);
            it = activeGates.erase(it);
        }
        else
        {
            it++;
        }
    }
}

/**
* Calculate how much physical distance is reduced after application of a SWAP
*/
int effOfSWAP(pair<int,int> & swapGate, gate g1, vector<int> locations,  int ** dist)
{
    //two logical qubits of g1
    int i1 = g1.control;
    int j1 = g1.target;

    //two physical qubits of g1 before SWAP gate
    int locOfI1 = locations[i1];
    int locOfJ1 = locations[j1];
    if(locOfI1 > locOfJ1)
        swap(locOfI1, locOfJ1);

    #ifdef DEBUG
    if(locOfI1==-1 || locOfJ1==-1)
        fout<<"error in effOfSWAP, active gate must have two physical qubits"<<endl;
    #endif // DEBUG

    // two physical qubits of swapGate
    int s1 = swapGate.first;
    int s2 = swapGate.second;
    if(s1 > s2)
        swap(s1, s2);

    if(s1 == locOfI1 && s2 == locOfJ1)
        return 0;
    else if(s1 == locOfI1)
        // two physical qubits of g1 after SWAP gate
        return dist[locOfI1][locOfJ1] - dist[s2][locOfJ1];
    else if(s1 == locOfJ1)
        return dist[locOfI1][locOfJ1] - dist[locOfI1][s2];
    else if(s2 == locOfI1)
        return dist[locOfI1][locOfJ1] - dist[s1][locOfJ1];
    else if(s2 == locOfJ1)
        return dist[locOfI1][locOfJ1] - dist[locOfI1][s1];
    else
        return 0;
}


/**
* Generate relevant SWAPs which have at least one common qubit with some active gate
*
*/
void genRelevantSwaps(CouplingGraph * p_cg,vector<gate> & activeGates, set<pair<int,int> > & candiSwap, vector<int> locations, int ** dist )
{
    for(unsigned int i=0; i<activeGates.size(); i++)
    {
        gate g1 = activeGates[i];
        int c = g1.control;
        int t = g1.target;
        int cAndT[2] = {locations[c], locations[t]};

        //for all possible SWAPs on c and t
        for(int ct=0; ct < 2;ct++)
        {
            int locOfC = cAndT[ct];
            for(unsigned int j=0; j<p_cg->adjList[locOfC].size(); j++)
            {
                int s1 = p_cg->adjList[locOfC][j];
                pair<int,int> swap1;

                if(locOfC < s1)
                    swap1 = make_pair(locOfC,s1);
                else
                    swap1 = make_pair(s1,locOfC);
                candiSwap.insert(swap1);
            }
        }
    }
}

/**
* Generate relevant SWAPs or Cnots which have at least one common qubit with some active gate
*
*/
void genRelevantGates(CouplingGraph * p_cg,vector<gate> & activeGates, set<weightedGate> & candiGates, vector<int> & locations, int ** dist )
{
    for(unsigned int i=0; i<activeGates.size(); i++)
    {
        gate g1 = activeGates[i];
        int c = g1.control;
        int t = g1.target;
        int cAndT[2] = {locations[c], locations[t]};
        if(dist[cAndT[0]][cAndT[1]] == 2)
        {
            weightedGate cnot1("CNOT",cAndT[0],cAndT[1]);
            candiGates.insert(cnot1);
        }

        //for all possible SWAPs on c and t
        for(int ct=0; ct < 2;ct++)
        {
            int locOfC = cAndT[ct];
            for(unsigned int j=0; j<p_cg->adjList[locOfC].size(); j++)
            {
                int s1 = p_cg->adjList[locOfC][j];
                weightedGate swap1("SWAP");

                if(locOfC < s1)
                {
                    swap1.q1 = locOfC;
                    swap1.q2 = s1;
                }
                else
                {
                    swap1.q1 = s1;
                    swap1.q2 = locOfC;
                }
                candiGates.insert(swap1);
            }
        }
    }
}

/**
* Look ahead through lines and set depth for gate within look-ahead window
*
*/
void setLookheadWindow(vector<list<gate> > & lines, int nqubits, int windowSize)
{
    vector<int> layer(nqubits,-1); //depth of each line during iteration
    vector<list<gate>::iterator> itOfLines(nqubits);
    for(int i=0;i<nqubits;i++)
    {
        itOfLines[i] = lines[i].begin();
    }

    bool isAllLineEmpty = false;
    bool isAllOverWindow = false;
    while(!isAllLineEmpty && !isAllOverWindow)
    {
        isAllLineEmpty = true;
        isAllOverWindow = true;
        for(int i=0;i<nqubits;i++)
        {
            if((itOfLines[i]!=lines[i].end()) && layer[i]<windowSize)
            {
                isAllLineEmpty = false;
                gate g1 = * itOfLines[i];
                int c_g1 = g1.control;
                int t_g1 = g1.target;
                if((*itOfLines[c_g1]) == (*itOfLines[t_g1]))
                {
                    int layerOfG1 = max(layer[c_g1],layer[t_g1])+1;
                    layer[c_g1]=layerOfG1;
                    layer[t_g1]=layerOfG1;
                    if(layerOfG1 <= windowSize)
                    {
                        (* itOfLines[c_g1]).depth = layerOfG1;
                        (* itOfLines[t_g1]).depth = layerOfG1;
                        itOfLines[c_g1]++;
                        itOfLines[t_g1]++;
                        isAllOverWindow = false;
                    }

                }
            }
        }
    }
}

/**
* Get the first nth layers of logical circuit
*
*/
vector<vector<gate> > getFrontLayers(vector<list<gate> > & lines, int layerDepth)
{
    vector<vector<gate> > layers;
    int nqubits = (int) lines.size();
    vector<int> layer(nqubits,-1); //depth of each line during iteration
    vector<list<gate>::iterator> itOfLines(nqubits);
    for(int i=0;i<nqubits;i++)
    {
        itOfLines[i] = lines[i].begin();
    }

    bool isAllLineEmpty = false;
    bool isAllOverWindow = false;
    while(!isAllLineEmpty && !isAllOverWindow)
    {
        isAllLineEmpty = true;
        isAllOverWindow = true;
        for(int i=0;i<nqubits;i++)
        {
            if((itOfLines[i]!=lines[i].end()) && layer[i]<layerDepth)
            {
                isAllLineEmpty = false;
                //isAllOverWindow = false;
                gate g1 = * itOfLines[i];
                int c_g1 = g1.control;
                int t_g1 = g1.target;
                if((*itOfLines[c_g1]) == (*itOfLines[t_g1]))
                {
                    int layerOfG1 = max(layer[c_g1],layer[t_g1])+1;
                    layer[c_g1]=layerOfG1;
                    layer[t_g1]=layerOfG1;
                    if(layerOfG1 <= layerDepth)
                    {
                        if(layers.size()<= (size_t)layerOfG1)
                            layers.push_back(vector<gate>());
                        layers[layerOfG1].push_back(g1);
                        itOfLines[c_g1]++;
                        itOfLines[t_g1]++;
                        isAllOverWindow = false;
                    }

                }
            }
        }
    }
    return layers;
}


/**
*
* Update the mapping between physical and logical qubits with a SWAP gate
*/
void updateMapping(pair<int,int> & swapGate, vector<int> & qubits, vector<int> & locations )
{
    //Two physical qubits of swapGate
    int pFirst = swapGate.first;
    int pSecond = swapGate.second;
    //Two logical qubits of swapGate
    int lFirst = qubits[pFirst];
    int lSecond = qubits[pSecond];

    //Update qubits array
    int temp = 0;
    temp = qubits[pFirst];
    qubits[pFirst] = qubits[pSecond];
    qubits[pSecond] = temp;

    //update locations array
    if(lFirst != -1) // corresponding to a unoccupied physical qubit
        locations[lFirst] = pSecond;
    if(lSecond != -1)
        locations[lSecond] = pFirst;
}

/**
* Compute cost function for  bridge CNOT gate (dist == 2)
*
*/
int costOfInsertedCnot(vector<list<gate> > & lines, pair<int,int> & cnotGate, const vector<int> & qubits,const vector<int> & locations,  int ** dist)
{
    int totalEffect = 0;
    int c = qubits[cnotGate.first];
    int t = qubits[cnotGate.second];
    gate g1 = lines[c].front();
    lines[c].pop_front();
    gate g2 = lines[t].front();
    lines[t].pop_front();
    if(!(g1 == g2) || !(g1.control==c && g1.target==t))
    {
        cout<<"error in 980" <<endl;
        cout<<g1.control<<","<<g1.target<<","<<g1.weight<<endl;
        cout<<g2.control<<","<<g2.target<<","<<g2.weight<<endl;
        return -1;
    }
    totalEffect = numOfExecuted(lines,locations,dist);
    lines[c].push_front(g1);
    lines[t].push_front(g1);

    return totalEffect+2; // and itself
}

/**
* Compute cost function for each SWAP
* propagate positive effect across layers until an negative occurs
*/
int costOfInsertedSwap(vector<list<gate> > & lines, pair<int,int> & swapGate,const vector<vector<gate> > & layers, vector<int> qubits,vector<int> locations,  int ** dist)
{
    int totalEffect = 0;
    bool hasExecutable = false;
    bool isNegative = false;

    for(size_t i=0;i<layers.size();i++)
    {
        int effOfLayer = 0;
        for(size_t j=0;j<layers[i].size();j++)
        {
            gate g1 = layers[i][j];
            int effectOnG1 = effOfSWAP(swapGate, g1, locations, dist);
            if(i==0) //gate of layer 0
            {
                int loc1_g1 = locations[g1.control];
                int loc2_g1 = locations[g1.target];
                int dist_g1 = dist[loc1_g1][loc2_g1];
                if(dist_g1 == 2 && effectOnG1==1)
                    hasExecutable = true;
            }
            if(effectOnG1 < 0) //Stop propagation at this layer
                isNegative = true;
            effOfLayer += effectOnG1;
        }
        if(isNegative == true)
            break;
        else
            totalEffect += effOfLayer;
    }

    //cout<<"Swap effect: "<<totalEffect<<endl;
    if(hasExecutable)
    {
        updateMapping(swapGate,qubits,locations);
        int exeGateCounts = numOfExecuted(lines,locations,dist);
        totalEffect += exeGateCounts;
        //cout<<"Executable gate count: "<<exeGateCounts<<endl;
    }


    return totalEffect;
}

/**
* Select the best Swap gate
*
*/

pair<int,int> selectBestSwap(int & maxCost, const set<pair<int,int> > & candiSwap, vector<list<gate> > & lines, int maxLookSize,  const vector<int> & qubits,const vector<int> & locations,  int ** dist)
{
    vector<pair<int,int> > bestSwaps;
    pair<int,int>  bestSwap;
    maxCost = -1;
    vector<vector<gate> > layers = getFrontLayers(lines, maxLookSize);
    for(set<pair<int,int>>::iterator it = candiSwap.begin(); it!=candiSwap.end(); it++)
    {
        pair<int,int> swapGate1 = * it;
        int effOfGate1 = costOfInsertedSwap(lines,swapGate1,layers,qubits,locations,dist);
        if(effOfGate1 > maxCost)
        {
            maxCost = effOfGate1;
            bestSwaps.clear();
            bestSwaps.push_back(swapGate1);
        }
        else if(effOfGate1 == maxCost)
        {
            bestSwaps.push_back(swapGate1);
        }
    }
    srand((unsigned)time(NULL));
    if(bestSwaps.size()>1)
        bestSwap = bestSwaps[rand() % bestSwaps.size()];
    else
        bestSwap =  bestSwaps[0];
    return bestSwap;
}

/**
* Select the best gate
* Swap or Cnot
*/
weightedGate selectBestGate(const set<weightedGate > & candiSwap, vector<list<gate> > & lines, int maxLookSize,  const vector<int> & qubits,const vector<int> & locations,  int ** dist)
{
    vector<weightedGate > bestGates;
    weightedGate  bestGate;
    int maxCost = -1;
    vector<vector<gate> > layers = getFrontLayers(lines, maxLookSize);
    for(set<weightedGate>::iterator it = candiSwap.begin(); it!=candiSwap.end(); it++)
    {
        weightedGate gate1 = * it;
        int effOfGate1 = -1;
        if(gate1.type == "SWAP")
        {
            pair<int,int> equivSwap = make_pair(gate1.q1,gate1.q2);
            effOfGate1 = costOfInsertedSwap(lines,equivSwap,layers,qubits,locations,dist);
        }
        else if(gate1.type == "CNOT")
        {
            pair<int,int> equivCnot = make_pair(gate1.q1,gate1.q2);
            effOfGate1 = costOfInsertedCnot(lines,equivCnot,qubits,locations,dist);
        }
        //cout << gate1.type << ", "<<gate1.q1<<", "<<gate1.q2<<endl;
        //cout << effOfGate1 << endl;

        if(effOfGate1 > maxCost)
        {
            maxCost = effOfGate1;
            bestGates.clear();
            bestGates.push_back(gate1);
        }
        else if(effOfGate1 == maxCost)
        {
            bestGates.push_back(gate1);
        }
    }
    srand((unsigned)time(NULL));
    if(bestGates.size()>1)
        bestGate = bestGates[rand() % bestGates.size()];
    else
        bestGate =  bestGates[0];
    bestGate.weight = maxCost;
    return bestGate;
}

/**
*Insert a SWAP gate into the resultant circuit
*
*/
void insertSwap(pair<int,int> & swapGate, vector<gate> & finalList)
{
    int p1 = swapGate.first;
    int p2 = swapGate.second;
    finalList.push_back(gate(p1,p2));
    finalList.push_back(gate(p2,p1));
    finalList.push_back(gate(p1,p2));
}

/**
*Insert a bridge cnot gate into the resultant circuit
*
*/
void inserBridgeCnot(int c, int t, vector<int> & linkPath, vector<gate> & finalList)
{
    finalList.push_back(gate(linkPath[0],linkPath[1]));
    finalList.push_back(gate(linkPath[1],linkPath[2]));
    finalList.push_back(gate(linkPath[0],linkPath[1]));
    finalList.push_back(gate(linkPath[1],linkPath[2]));
}

/**
* The mapping procedure by using swap and bridge cnot gate
* return the number of ancillary cnot gates
*/
int swapAndCnotBasedMapping(int nqubits, vector<gate> & gateList, CouplingGraph * p_cg, vector<gate> & phyCir, int ** dist, vector<int> & qubits, vector<int> & locations)
{
    //(0) some initialization work
    int nCnots = 0;
    set<int> activeLines;
    for(int i=0 ; i<nqubits; i++)
        activeLines.insert(i);
    vector<list<gate> >  lines;
    lines.resize(nqubits);
    genLines(lines, gateList);

    vector<gate> activeGates;
    while(true)
    {
        //(1) generate and check active gate
        findActivegates(lines,activeGates,phyCir,activeLines, locations,dist);
        //fout<<"step 1 is finished."<<endl;

        if(activeGates.empty()) // all gate have been mapped
            break;

        //(2) generate candidate swap and bridge gates
        set<weightedGate > candiGates;
        genRelevantGates(p_cg, activeGates, candiGates, locations, dist );

        //(3) determine the swap/cnot with max cost among candidates
        weightedGate bestGate1 = selectBestGate(candiGates,lines,4,qubits,locations,dist);

        //(4) insert swap or cnot and update mapping
        int maxCost = bestGate1.weight;
        if(maxCost <= 0)
            cout<< maxCost <<endl;
        if(maxCost == 0)
        //No swap which does not increase the physical distance of any front gate and
        //can not make any front gate executable
        //Apply a swap along the shortest path of the front gate with minimum physical len
        {
            cout<<"maxCost == 0!!!!!!!!!!!!" <<endl;
            int minLenOfFrontCnot = numeric_limits<int>::max();
            int controlOfMinLenGate;
            int targetOfMinlenGate;
            for(size_t i = 0;i < activeGates.size();i++)
            {
                gate frontGate1 = activeGates[i];
                int locOfC = locations[frontGate1.control];
                int locOfT = locations[frontGate1.target];
                if(dist[locOfC][locOfT] < minLenOfFrontCnot)
                {
                    minLenOfFrontCnot = dist[locOfC][locOfT];
                    controlOfMinLenGate = locOfC;
                    targetOfMinlenGate = locOfT;
                }
            }
            vector<int> shortestPath = p_cg->getPath(locations[controlOfMinLenGate],locations[targetOfMinlenGate]);
            pair<int,int> swapg1 = make_pair(shortestPath[0],shortestPath[1]);
            insertSwap(swapg1, phyCir);
            nCnots += 3;
            updateMapping(swapg1, qubits, locations );
        }
        else if(bestGate1.type == "SWAP")
        {
            pair<int,int> swapg = make_pair(bestGate1.q1,bestGate1.q2);
            insertSwap(swapg, phyCir);
            nCnots += 3;
            updateMapping(swapg, qubits, locations );
        }
        else // insert CNOT
        {
            vector<int> linkedPath = p_cg->getPath(bestGate1.q1,bestGate1.q2);
            inserBridgeCnot(bestGate1.q1,bestGate1.q2,linkedPath, phyCir);
            nCnots +=3;
            int c = qubits[bestGate1.q1];
            int t = qubits[bestGate1.q2];
            gate g1 = lines[c].front();
            lines[c].pop_front();
            gate g2 = lines[t].front();
            lines[t].pop_front();
            if(!(g1 == g2))
            {
                cout<<"error in 1183"<<endl;
                return -1;
            }

            for(vector<gate>::iterator it = activeGates.begin(); it!=activeGates.end();it++ )
            {
                gate g2 = *it;
                if(c == g2.control && t == g2.target)
                {
                    activeGates.erase(it);
                    break;
                }
            }
            activeLines.insert(c);
            activeLines.insert(t);
        }


        //(5) check active gate whether some can be inserted into physical circuit
        checkActivegates(lines,activeGates, phyCir,activeLines, locations, dist);
    }

    return nCnots;
}


/**
*生成测试用的随机量子线路
*/
void generateRandomCircuit(int nqubits,int ngates,vector<gate> & randGateList)
{
    srand((unsigned)time(NULL));
    for(int i=0;i<ngates;i++)
    {
        int c = rand()% nqubits;
        int t = rand()% nqubits;
        while(c==t)
        {
            c = rand()% nqubits;
            t = rand()% nqubits;
        }
        gate g(c,t,i);
        randGateList.push_back(g);
    }
}


/**
*The iterated local search
*
*/
void IteratedLocalSearchMapper(string ciucuitFileName,ofstream & fout, int outLimit,int inLimit)
{
    //ofstream fout(logFileName+"_log.txt");
    vector<gate> gateList;  //logical circuit
    vector<gate> phyCir;   //physical circuit
    int nlocations; //number of physical
    int nqubits; //number of logical
    vector<list<gate> >  lines;
    vector<gate> activeGates;
    set<int> activeLines;
    CouplingGraph cg;
    int ** dist;

    // Read qasm file into gate list
    QASMReader reader(ciucuitFileName);
    reader.parse(gateList);
    //vector<gate> gateList_bak = gateList;
    nqubits = reader.getVerNum();
    fout<<"The number of logic qubits:"<<nqubits<<endl;
    fout<<"The number of logical gates:"<<gateList.size()<<endl;


    //generate coupling map , distance matrix and interaction matrix
    cg.buildQX20();
    nlocations = cg.getPositions();
    cg.computeDist();
    dist = cg.getDist();
    int ** interactMat = initInteractionMatrix(nqubits);
    getInteractionMatrix(gateList,interactMat);


    //Initialize qubits[] and locations[]
    vector<int> locations(nqubits,-1);
    vector<int> qubits(nlocations,-1);

    int minCnotCount = numeric_limits<int>::max();

    vector<int> qubitsArrOfBest;
    int dirOfBest = 0;
    vector<gate> phyCirOfBest;
    int noBetterCounter = 0;

    randInitMapping(qubits,locations,nlocations,nqubits);

    for(int j=0; j<outLimit; j++)
    {
        PerurbationByShuffle(qubits,locations,nlocations,nqubits);
        noBetterCounter++;
        bool isNeedNoGate = false;
        for(int i=0; i<inLimit; i++)
        {

            vector<int> tmpQubitsArr = qubits;

            int curCnotCount = swapAndCnotBasedMapping(nqubits, gateList, &cg, phyCir, dist, qubits, locations);

            if(curCnotCount < minCnotCount)
            {
                noBetterCounter = 0;
                minCnotCount = curCnotCount;
                phyCirOfBest = phyCir;
                if(i%2 == 0) //forward traversal
                {
                    dirOfBest = 0;
                    qubitsArrOfBest = tmpQubitsArr;
                }
                else         //reverse traversal
                {
                    dirOfBest = 1;
                    qubitsArrOfBest = qubits;
                }
            }

            reverse(gateList.begin(),gateList.end());
            phyCir.clear();
             if(minCnotCount == 0)
            {
                isNeedNoGate = true;
                break;
            }
        }
        if(noBetterCounter == 300 || isNeedNoGate)
            break;
    }

    fout<<"Cnot gate count: " << minCnotCount << endl;
    fout<<"Size of the best physical circuit:" << phyCirOfBest.size()<<endl;
    fout<<"Direction of best mapping: " << dirOfBest <<endl;
    fout<<"------------------------------"<<endl;

    //clear job
    clearInteractionMatrix(interactMat,nqubits);
}


void getFiles( string path, vector<string>& files )
{
    //文件句柄
    long   hFile   =   0;
    //文件信息
    struct _finddata_t fileinfo;
    string p;
    if((hFile = _findfirst(p.assign(path).append("\\*").c_str(),&fileinfo)) !=  -1)
    {
        do
        {
            if(strcmp(fileinfo.name,".") != 0  &&  strcmp(fileinfo.name,"..") != 0)
            {
                string fileName(fileinfo.name);
                fileName = fileName.substr(0,fileName.find("."));
                files.push_back(fileName);
            }
        }
        while(_findnext(hFile, &fileinfo)  == 0);
        _findclose(hFile);
    }
}

int doExperiments()
{
    string path = ".\\benmark";
    vector<string> files;

    //获取该路径下的所有文件
    getFiles(path, files );

    int fsize = files.size();
    cout<<"#File:"<<fsize<<endl;
    for (int i = 0;i < fsize;i++)
    {
        string fname = files[i];
        cout<<"Benchmark circuit " << fname << " is in mapping...."<<endl;
        string circuitName = ".\\benmark\\" + fname + ".qasm";
        string logName =  ".\\log\\" + fname + "_log" +".txt";
        ofstream logFile(logName);
        for(int j=0;j<10;j++)
        {
            clock_t begin_time = clock();
            IteratedLocalSearchMapper(circuitName,logFile,1000,6);
            double runtime = double(clock() - begin_time) / CLOCKS_PER_SEC;
            logFile << "Total time is :" << runtime << " seconds."<<endl;

        }
        logFile.close();
        cout<<"Benchmark circuit " << fname << " has been mapped."<<endl;
    }
    return 0;
}


int main()
{
    doExperiments();
    return 0;
}
