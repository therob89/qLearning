//
//  Maze.cpp
//  qLearning
//
//  Created by Roberto Palamaro on 11/04/15.
//  Copyright (c) 2015 Roberto Palamaro. All rights reserved.
//

#include <stdio.h>
#include <tuple>
#include <string>
#include <vector>
#include <iostream>
#include <fstream>

#define GOAL    "G"
#define WALL    "#"
#define AGENT   "A"
#define VOID    "_"
using namespace std;


class Maze{
    
    vector<vector<string>> maze;
    tuple<int,int> currentAgentPos;
    vector<tuple<int,int>> listOFWalls;
    int x_dim,y_dim;
    
public:
    Maze(int x, int y){
        if (x && y){
            x_dim = x;
            y_dim = y;
            vector<vector<string>>temp(x_dim,vector<string>(y_dim,"|_|"));
            maze = temp;
            currentAgentPos = {-1,-1};
        }
        
    }
    tuple<int,int> getDimensionOfMaze(){
        return {x_dim,y_dim};
    }
    vector<tuple<int,int>> getWallsOfMaze(){
        return this->listOFWalls;
    }
    
    void setWalls(vector<tuple<int,int>> listOfWalls, tuple<int,int> goal){
        for(int i=0;i<listOfWalls.size();i++){
            int x = get<0>(listOfWalls[i]);
            int y = get<1>(listOfWalls[i]);
            if((x>=0 && x<x_dim)&&(y>=0 && y<y_dim)){
                maze[x][y].replace(1, 1, WALL);
            }
        }
        int x = get<0>(goal);
        int y = get<1>(goal);
        if((x>=0 && x<x_dim)&&(y>=0 && y<y_dim)){
            maze[x][y].replace(1, 1, GOAL);
        }
        this->listOFWalls = listOFWalls;
    }
    void setAgent(tuple<int,int> agentPos){
        int x = get<0>(agentPos);
        int y = get<1>(agentPos);
        if(get<0>(currentAgentPos)==-1){
            currentAgentPos = {x,y};
        }
        else{
            maze[get<0>(currentAgentPos)][get<1>(currentAgentPos)].replace(1,1,VOID);
            currentAgentPos = {x,y};
        }
        if((x>=0 && x<x_dim)&&(y>=0 && y<y_dim)){
            maze[x][y].replace(1, 1, AGENT);
        }
        
    }
    void setGoal(tuple<int,int> goal){
        int x = get<0>(goal);
        int y = get<1>(goal);
        cout << "setGoal method goal is at " << x << ":" << y <<endl;
        maze[x][y].replace(1, 1, GOAL);
    }
    void printMaze(ofstream& logFile){
        for(int i=0;i<x_dim;i++){
            logFile << i << ": ";
            for(int j=0;j<y_dim;j++){
                logFile << this->maze[i][j];
            }
            logFile << endl;
        }
    }
    void printMaze2(){
        for(int i=0;i<x_dim;i++){
            cout << i << ": ";
            for(int j=0;j<y_dim;j++){
                cout << this->maze[i][j];
            }
            cout << endl;
        }
    }
};