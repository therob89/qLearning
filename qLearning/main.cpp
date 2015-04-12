//
//  main.cpp
//  qLearning
//
//  Created by Roberto Palamaro on 12/04/15.
//  Copyright (c) 2015 Roberto Palamaro. All rights reserved.
//


#include "QTable.cpp"
#include <cmath>
#include <iostream>
#include <limits>
#include <cstdlib>
#include <ctime>
using namespace std;

int main(int argc, const char * argv[]) {
    
    Q_Table qTable(3,3);
    qTable.setReward(1, 2, UP, 100);
    qTable.setReward(0, 1, RIGHT, 100);
    qTable.initMazeWallsGoalandAgentPos({{2,2},{1,2},{2,0}},{0,2});
    /*
     int x = 0;
     do{
     x+=1;
     qTable.runAnEpisode(0, 2);
     }while(x<10);
     
     qTable.printValuesInReadableForm();
     qTable.printMaze();
     */
    //qTable.runAnEpisode(0, 2);
    qTable.doLearning(0, 2);
    qTable.printValuesInReadableForm();
    return 0;
}
