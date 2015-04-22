//
//  main.cpp
//  qLearning
//
//  Created by Roberto Palamaro on 12/04/15.
//  Copyright (c) 2015 Roberto Palamaro. All rights reserved.
//


#include <cmath>
#include <iostream>
#include <limits>
#include <cstdlib>
#include <ctime>
#include "QTable.cpp"
using namespace std;

int main(int argc, const char * argv[]) {
    
    Q_Table qTable(4,4,1);
    qTable.initMazeWallsGoalandAgentPos({{2,2},{0,0},{2,0}},{0,2});
    qTable.setReward(1, 2, UP, 100);
    qTable.setReward(0, 1, RIGHT, 100);
    //qTable.setReward(0,3, LEFT, 90);
    qTable.printValuesInReadableForm();
    qTable.completeSARSA(0,2);
    //qTable.doLearning(0, 2);
    return 0;
}
