//
//  QTable.cpp
//  qLearning
//
//  Created by Roberto Palamaro on 03/04/15.
//  Copyright (c) 2015 Roberto Palamaro. All rights reserved.
//

#include <stdio.h>
#include <iostream>
#include <map>
#include <vector>
#include <chrono>
#include <random>
#include <thread>
#include <tuple>
#include <fstream>
#include <math.h>
#include "Maze.cpp"

#define N_ITERATIONS 1000
#define N_OF_ACTIONS 4

#define UP      0
#define DOWN    1
#define LEFT    2
#define RIGHT   3

#define GAMMA   0.9
#define EPSILON 0.1
#define ALFA    0.1

using namespace std;

class Q_Table{
    size_t x_dim,y_dim,z_dim;
    vector<double> m_data;
    tuple<int,int> current_position;
    vector<tuple<int,int>> listOFWalls;
    Maze maze;
    public:
    ofstream logFile;
    
    
public:
    
    Q_Table(size_t a,size_t b,int init = 0):
    x_dim(a),y_dim(b),z_dim(N_OF_ACTIONS),m_data(a*b*z_dim,0.0),maze((int)x_dim,(int)y_dim){
        cout << "Initially there are " << count(m_data.begin(), m_data.end(), 0) << " zeros values" <<endl;
        if(init==0) logFile.open("./log.txt",ios::out);
        else logFile.open("./logSarsa.txt",ios::out);
    }
    
#pragma mark GET METHODS
    Maze getMaze(){
        return this->maze;
    }
    
    vector<double> getValuesOfTable(){
        return this->m_data;
    }
#pragma mark INIT_METHODS
    
    void initMazeWallsGoalandAgentPos(vector<tuple<int,int>> listOfWalls, tuple<int,int> goal){
        maze.setWalls(listOfWalls, goal);
        listOFWalls = listOfWalls;
        initActions();
    }
    void initActions(){
        cout << "Initializing Actions values" << endl ;
        tuple<int,int> temp;
        for(int x=0;x<x_dim;x++){
            for(int j=0;j<y_dim;j++){
                if(x-1<0){
                    m_data.at(x + j * x_dim + UP * x_dim * y_dim)    = -1.0;
                }
                temp = {x-1,j};
                if(find(listOFWalls.begin(),listOFWalls.end(),temp)!=listOFWalls.end()){
                    m_data.at(x + j * x_dim + UP * x_dim * y_dim)    = -1.0;
                }
                if(x+1 > (x_dim-1)){
                    m_data.at(x + j * x_dim + DOWN * x_dim * y_dim)  = -1.0;
                }
                temp = {x+1,j};
                if(find(listOFWalls.begin(),listOFWalls.end(),temp)!=listOFWalls.end()){
                    m_data.at(x + j * x_dim + DOWN * x_dim * y_dim)    = -1.0;
                }
                temp = {x,j-1};
                if(find(listOFWalls.begin(),listOFWalls.end(),temp)!=listOFWalls.end()){
                    m_data.at(x + j * x_dim + LEFT * x_dim * y_dim)    = -1.0;
                }
                if(j-1 <0){
                    m_data.at(x + j * x_dim + LEFT * x_dim * y_dim)  = -1.0;
                }
                temp = {x,j+1};
                if(find(listOFWalls.begin(),listOFWalls.end(),temp)!=listOFWalls.end()){
                    m_data.at(x + j * x_dim + RIGHT * x_dim * y_dim)    = -1.0;
                }
                if(j+1 > (y_dim-1)){
                    m_data.at(x + j * x_dim + RIGHT * x_dim * y_dim) = -1.0;
                    
                }
                
                
            }
        }
    }
    
    void setReward(int x,int j,int action,int reward){
        m_data.at(x + j * x_dim + action * x_dim * y_dim) = reward;
    }
    
    double& operator()(size_t x, size_t y, size_t z) {
        return m_data.at(x + y * x_dim + z * x_dim * y_dim);
    }
    
   
#pragma mark CHECKING METHODS
    bool checkAction(tuple<int,int> pos,int action){
        
        int x = get<0>(pos);
        int j = get<1>(pos);
        if(m_data.at(x + j * x_dim + action * x_dim * y_dim)!=-1.0){
            return true;
        }else{
            return false;
        }
    }
    
    int getMaxFromNewState(tuple<int,int> new_pos){
        int k,temp,res=0;
        for(k=0;k<N_OF_ACTIONS;k++){
            temp = m_data.at(get<0>(new_pos) + get<1>(new_pos) * x_dim + k * x_dim * y_dim);
            if(temp>res){
                res = temp;
            }
        }
        return res;
    }
    
    bool checkIfIsTheGoal(tuple<int,int> new_pos,int goal_x,int goal_y){
        if(get<0>(new_pos)== goal_x && get<1>(new_pos)  == goal_y){
            return true;
        }
        return false;
    }
    
#pragma mark ACTION METHOD
    tuple<int,int> performAction(tuple<int,int> current_pos,int action,int goal_x,int goal_y){
        
        int x,j,res;
        res = 0;
        switch (action) {
            case UP:
                if(checkAction(current_pos, UP)){
                    j = get<1>(current_pos);
                    x = get<0>(current_pos);
                    get<0>(current_pos) = x-1;
                    if(!checkIfIsTheGoal(current_pos, goal_x, goal_y)){
                        m_data.at(x + j * x_dim + UP * x_dim * y_dim) = GAMMA * getMaxFromNewState(current_pos);
                    }
                    logFile << " \t Action is : ↑ ";
                }
                break;
            case DOWN:
                if(checkAction(current_pos, DOWN)){
                    j = get<1>(current_pos);
                    x = get<0>(current_pos);
                    get<0>(current_pos) = x+1;
                    if(!checkIfIsTheGoal(current_pos, goal_x, goal_y)){
                        m_data.at(x + j * x_dim + DOWN * x_dim * y_dim) = GAMMA * getMaxFromNewState(current_pos);
                    }
                    logFile << " \t Action is : ↓ " ;
                }
                break;
            case LEFT:
                if(checkAction(current_pos, LEFT)){
                    j = get<1>(current_pos);
                    x = get<0>(current_pos);
                    get<1>(current_pos) = j-1;
                    if(!checkIfIsTheGoal(current_pos, goal_x, goal_y)){
                        m_data.at(x + j * x_dim + LEFT * x_dim * y_dim) = GAMMA * getMaxFromNewState(current_pos);
                    }
                    logFile << " \t Action is : ← ";
                }
                break;
            case RIGHT:
                if(checkAction(current_pos, RIGHT)){
                    j = get<1>(current_pos);
                    x = get<0>(current_pos);
                    get<1>(current_pos) = j+1;
                    if(!checkIfIsTheGoal(current_pos, goal_x, goal_y)){
                        m_data.at(x + j * x_dim + RIGHT * x_dim * y_dim) = GAMMA * getMaxFromNewState(current_pos);
                    }
                    logFile << " \t Action is : → ";
                }
                break;
                
        }
        logFile << endl;
        return current_pos;
    }
    
#pragma mark Q_LEARNING METHODS
    void doLearning(int goal_x, int goal_y){
        
        int c=0;
        do{
            Maze temp((int)x_dim,(int)y_dim);
            temp.setWalls(listOFWalls, tuple<int,int>(goal_x,goal_y));
            this->maze = temp;
            c+=1;
            logFile << "\t\t\t EPISODE: " << c << endl;
            runAnEpisode(goal_x, goal_y);
            
        }while (count(m_data.begin(),m_data.end(),0)>5);
        cout << "Episode needed to convergence = " <<c << endl;
        logFile.close();
    }
    
    tuple<int,int> getRandomPosition(){
        srand((unsigned)time(0));
        tuple<int,int> start; //= {2,1};
        do{
            get<0>(start)= rand() % x_dim;
            get<1>(start)= rand() % y_dim;
        }while (find(listOFWalls.begin(),listOFWalls.end(),start)!=listOFWalls.end());
        return start;
        
    }
    void runAnEpisode(int goal_x, int goal_y){
       
        tuple<int,int> start = getRandomPosition();
        float constant = 0.5;
        int action;
        cout << "Start position is at x: " << get<0>(start) << " y: " << get<1>(start) << endl;
        maze.setAgent(start);
        while(get<0>(start) != goal_x || get<1>(start) != goal_y){
            maze.printMaze(logFile);
            action = computeActionGivenProbability(constant, start);
            if(action == -1){
                //cout << "action is -1" <<endl;
                continue;
            }
            constant +=0.1;
            start = performAction(start,action,goal_x,goal_y);
            maze.setAgent(start);
        }
        maze.printMaze(logFile);
        cout << endl << "Arrived At destination number of zeros are " << count(m_data.begin(),m_data.end(), 0)<< endl;
    }
    void printValuesInReadableForm(){
        for(int i=0;i<x_dim;i++){
            for(int j=0;j<y_dim;j++){
                for(int k=0;k<N_OF_ACTIONS;k++){
                    int val = m_data.at(i + j * x_dim + k * x_dim * y_dim);
                    //if(val !=-1 && val !=-0){
                    logFile << "x: " <<i<<" y: " <<j;
                    switch (k) {
                        case UP:
                            logFile << " ↑ values: " <<val;
                            break;
                        case DOWN:
                            logFile << " ↓ values: " <<val;
                            break;
                        case LEFT:
                            logFile << " ← values: " <<val;
                            break;
                        case RIGHT:
                            logFile << " → values: " <<val;
                            break;
                    }
                    logFile << endl;
                    //}
                }
            }
        }
    }
    
    int computeActionGivenProbability(float k_value, tuple<int,int> current_pos){
        
        double den,num,res,temp;
        int action;
        res = -1.0;
        action = -1.0;
        temp = -1.0;
        vector<tuple<int,double>> listOfProbs;
        
        for(int k=0;k<N_OF_ACTIONS;k++){
            if(m_data.at(get<0>(current_pos) + get<1>(current_pos) * x_dim + k * x_dim * y_dim)!=-1){
                temp = pow(k_value,m_data.at(get<0>(current_pos) + get<1>(current_pos) * x_dim + k * x_dim * y_dim));
                den+=temp;
                
            }
        }
        for(int k=0;k<N_OF_ACTIONS;k++){
            if(m_data.at(get<0>(current_pos) + get<1>(current_pos) * x_dim + k * x_dim * y_dim)!=-1){
                num =  pow(k_value,m_data.at(get<0>(current_pos) + get<1>(current_pos) * x_dim + k * x_dim * y_dim));
                tuple<int,double> t_temp = {k,num/den};
                listOfProbs.insert(listOfProbs.end(), t_temp);
            }
            
        }
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_real_distribution<> dis(0, 1);
        double rnd = dis(gen);
        for(int i=0; i<listOfProbs.size(); i++) {
            if(rnd < get<1>(listOfProbs[i])){
                return get<0>(listOfProbs[i]);
            }
            rnd -= get<1>(listOfProbs[i]);
        }
        return -1;
    }
    
#pragma mark SARSA METHOD
    
    tuple<int,int> observeNewState(tuple<int,int> current_pos, int action){
        int x_res = get<0> (current_pos);
        int y_res = get<1> (current_pos);
        int j = get<1>(current_pos);
        int x = get<0>(current_pos);
        switch (action) {
            case UP:
                if(checkAction(current_pos, UP)){
                    x_res = x-1;
                    y_res = j;
                    logFile << " \t Action is : ↑ ";
                }
                break;
            case DOWN:
                if(checkAction(current_pos, DOWN)){
                    x_res = x+1;
                    y_res = j;
                    logFile << " \t Action is : ↓ " ;
                }
                break;
            case LEFT:
                if(checkAction(current_pos, LEFT)){
                    x_res = x;
                    y_res = j-1;
                    logFile << " \t Action is : ← ";
                }
                break;
            case RIGHT:
                if(checkAction(current_pos, RIGHT)){
                    x_res = x;
                    y_res = j+1;
                    logFile << " \t Action is : → ";
                }
                break;
        }
        logFile<<endl;
        return {x_res,y_res};
    }
    
    int getBestActionFromCurrentState(tuple<int,int> current_pos){
        int action = -1;
        double temp = -1;
        double t = action;
        for(int k=0;k<N_OF_ACTIONS;k++){
            if((temp = m_data.at(get<0>(current_pos) + get<1>(current_pos) * x_dim + k * x_dim * y_dim))!=-1.0){
                if(temp>t){
                    t = temp;
                    action = k;
                }
            }
        }
        return action;
    }
    int chooseActionWithPolicyEGreedy(tuple<int,int> current_pos){
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_real_distribution<> dis(0, 1);
        double rnd = dis(gen);
        int action;
        if(rnd>EPSILON){
            action =  getBestActionFromCurrentState(current_pos);
            if(action == -1){
                cout << "Current pos IS AT x:"<< get<0>(current_pos) << " y: " << get<1>(current_pos);
                cout << "Error with action...that is --->" << action <<  endl << "List of available actions " << endl;
                for(int i =0;i<N_OF_ACTIONS;i++){
                    cout << "VALUES: " << m_data.at(get<0>(current_pos) + get<1>(current_pos) * x_dim + i * x_dim * y_dim);
                }
                exit(-1);
            }
           
        }else{
            std::uniform_int_distribution<int> idist(0,N_OF_ACTIONS-1);
            action = (int)idist(gen);
            while(!this->checkAction(current_pos, action)){
                std::uniform_int_distribution<int> idist(0,N_OF_ACTIONS-1);
                action = (int) idist(gen);
            }
        }
        
        return action;
    }
    void doesSARSA(int goal_x, int goal_y){
        logFile << "SARSA ALGORITHM " << endl;
        //cout << "SARSA " << endl;
        tuple<int,int> current_pos = getRandomPosition();
        int firstAction;
        double reward;
        firstAction = chooseActionWithPolicyEGreedy(current_pos);
        int secondAction = firstAction;
        double t_value1,t_value2;
        tuple<int,int> temp_position = current_pos;
        secondAction = firstAction;
        maze.setAgent(current_pos);
        logFile << "START POSITION IS AT x:"<< get<0>(current_pos) << " y: " << get<1>(current_pos) << endl;;
        while(get<0>(current_pos) != goal_x || get<1>(current_pos) != goal_y){
            reward = m_data.at(get<0>(current_pos) + get<1>(current_pos) * x_dim + firstAction * x_dim * y_dim);
            temp_position = observeNewState(temp_position, firstAction);
            secondAction = this->chooseActionWithPolicyEGreedy(temp_position);
            /*
            logFile << "current : "<< get<0>(current_pos) << get<1>(current_pos)
                        << " --- New pos : " << get<0>(temp_position) <<  get<1>(temp_position) << 
                            "FIRST ACTION" << firstAction << "NEW ACTION" << secondAction <<endl;*/
            t_value1 =  m_data.at(get<0>(current_pos) + get<1>(current_pos) *x_dim + firstAction *x_dim * y_dim);
            t_value2 = m_data.at(get<0>(temp_position) + get<1>(temp_position) *x_dim + secondAction *x_dim * y_dim);
            m_data.at(get<0>(current_pos) + get<1>(current_pos) *x_dim + firstAction *x_dim * y_dim)
                                                    = fmod((t_value1 + ALFA*(reward + (GAMMA*t_value2) - t_value1)),100);
            //cout << "New value :" << fmod((t_value1 + ALFA*(reward + (GAMMA*t_value2) - t_value1)),100) << endl;
            firstAction = secondAction;
            current_pos = temp_position;
            maze.setAgent(current_pos);
            maze.printMaze(logFile);
            
        }
        //cout << " \t END SARSA"<< endl;

    }
    
    void completeSARSA(int goal_x, int goal_y){
        int c=0;
        while(count(m_data.begin(),m_data.end(),0.0)>5){
            Maze temp((int)x_dim,(int)y_dim);
            temp.setWalls(listOFWalls, tuple<int,int>(goal_x,goal_y));
            this->maze = temp;
            c+=1;
            logFile << "\t\t\t EPISODE: " << c << endl;
            doesSARSA(goal_x, goal_y);
            this->printValuesInReadableForm();
            
        }
        cout << "Episode needed to convergence with SARSA  = " <<c << endl;
        logFile.close();
        

    }
    
};
