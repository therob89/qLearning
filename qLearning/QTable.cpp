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
#define ALFA    0.1

#define QLEARNING   "QLEARNING"
#define SARSA       "SARSA"

using namespace std;

class Q_Table{
    size_t x_dim,y_dim,z_dim;
    vector<double> m_data;
    vector<int> reward_data;
    tuple<int,int> current_position;
    vector<tuple<int,int>> listOFWalls;
    Maze maze;
    double epsilon;
    string typeOfAlg;
    public:
    ofstream logFile;

    
public:
    
    Q_Table(size_t a,size_t b, string algType):
    x_dim(a),y_dim(b),z_dim(N_OF_ACTIONS),m_data(a*b*z_dim,0.0),maze((int)x_dim,(int)y_dim),reward_data(a*b*z_dim,0){
        if(algType.compare(QLEARNING)==0){
            logFile.open("./log.txt",ios::out);
        }
        else{
           logFile.open("./logSarsa.txt",ios::out);
            typeOfAlg = 1;
        }
        typeOfAlg = algType;
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
        if(listOfWalls.size()!=0){
            maze.setWalls(listOfWalls, goal);
            listOFWalls = listOfWalls;
            disableCellsWithWalls();
        }
        initActions();

    }
    
    void disableCellsWithWalls(){
        int x,y;
        for(vector<tuple<int,int>>::iterator it=this->listOFWalls.begin();it!=this->listOFWalls.end();it++){
            x = get<0>(*it);
            y = get<1>(*it);
            if((x>=0 && x<x_dim) && (y>=0 && y<y_dim)){
                for(int k = 0;k<N_OF_ACTIONS;k++){
                    m_data.at(x + y * x_dim + k * x_dim * y_dim)  = -1.0;
                }
            }
        }
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
        if(x<0 || x>x_dim || j<0 || j>y_dim){
            return;
        }
        reward_data.at(x + j * x_dim + action * x_dim * y_dim) = reward;
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

    
    bool checkIfIsTheGoal(tuple<int,int> new_pos,int goal_x,int goal_y){
        if(get<0>(new_pos)== goal_x && get<1>(new_pos)  == goal_y){
            return true;
        }
        return false;
    }
    
#pragma mark FACILITIES
    
  
    
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
    
    int getBestActionFromCurrentState(tuple<int,int> current_pos){
        int action = -1;
        double temp = -1;
        double t = action;
        for(int k=0;k<N_OF_ACTIONS;k++){
            if((temp = m_data.at(get<0>(current_pos) + get<1>(current_pos) * x_dim + k * x_dim * y_dim))!=-1.0){
                if(temp>=t){
                    t = temp;
                    action = k;
                }
            }
        }
        return action;
    }
    
    tuple<int,int> observeNewState(tuple<int,int> current_pos, int action){
        int x_res = -1;
        int y_res = -1;
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
    
     /*
        Select action policy = Soft-max strategy
     */
    int computeActionGivenProbability(double k_value, tuple<int,int> current_pos){
        
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
    
    tuple<int,int> getRandomPosition(){
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<int>dis(0,(int)(x_dim-1));
        tuple<int,int> start; //= {2,1};
        do{
            get<0>(start)= dis(gen);
            get<1>(start)= dis(gen);
        }while (find(listOFWalls.begin(),listOFWalls.end(),start)!=listOFWalls.end());
        return start;
        
    }
    
    void printValuesInReadableForm(){
    logFile << "****************************** COMPLETE LIST OF VALUES ****************************** " << endl;
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
    logFile << "************************************************************************************" << endl;
}
#pragma mark Q_LEARNING METHODS
    void doLearning(int goal_x, int goal_y){
        this->epsilon = 1.0;
        int c=0;
        int episodes = 100;
        double tempMetrics = 0,tempMetrics1 = 0;
        double constant = 0.2;
        std::clock_t start;
        start = std::clock();
        ofstream metrics;
        metrics.open("./metrics.txt",ios::out);
        do{
            Maze temp((int)x_dim,(int)y_dim);
            temp.setWalls(listOFWalls, tuple<int,int>(goal_x,goal_y));
            this->maze = temp;
            c+=1;
            logFile << "\t\t\t EPISODE: " << c << endl;
            //metrics << "Episode :" << c <<"QLEARning value: \t" << runQLearningEpisode(goal_x, goal_y,constant) << endl;
            //tempMetrics1 = runQLearningEpisodeDeterministic(goal_x, goal_y,constant);
            tempMetrics1 = runQLearningEpisode(goal_x, goal_y,constant);
            if(tempMetrics1>0){
                tempMetrics = tempMetrics1;
                metrics << "Episode :" << c <<"QLEARning value: \t" << tempMetrics << endl;
                
            }else{
                metrics << "Episode :" << c <<"QLEARning value: \t" << tempMetrics << endl;
            }
            this->printValuesInReadableForm();
            constant += 0.2;
            if((this->epsilon - 0.01) >=0){
                this->epsilon -= 0.01;
            }
            episodes-=1;
        }while (episodes!=0);
        cout << "QLEARNING completed in : " << (clock() - start)/(double)CLOCKS_PER_SEC<< " seconds" << endl;
        logFile.close();
        metrics.close();
    }

    double runQLearningEpisode(int goal_x, int goal_y, double k_val){
       
        tuple<int,int> start = getRandomPosition();
        tuple<int,int> new_state;
        int action;
        int act;
        double sum_of_rew = 0;
        int num_of_steps = 0;
        maze.setAgent(start);
        while(get<0>(start) != goal_x || get<1>(start) != goal_y){
            maze.printMaze(logFile);
            action = chooseActionWithPolicyEGreedy(start);
            act = returnARandomAction(start, action);
            new_state = observeNewState(start, act);
            m_data.at(get<0>(start) + get<1>(start) * x_dim + action * x_dim * y_dim) = reward_data.at(get<0>(start) + get<1>(start) * x_dim + act * x_dim * y_dim) + (GAMMA * getMaxFromNewState(new_state));
            sum_of_rew +=  m_data.at(get<0>(start) + get<1>(start) * x_dim + action * x_dim * y_dim);
            maze.setAgent(new_state);
            start = new_state;
            num_of_steps+=1;
        }
        maze.printMaze(logFile);
        if(num_of_steps == 0){
            return 0;
        }
        return (sum_of_rew/num_of_steps);
    }
    double runQLearningEpisodeDeterministic(int goal_x, int goal_y, double k_val){
        
        tuple<int,int> start = getRandomPosition();
        tuple<int,int> new_state;
        int action;
        double sum_of_rew = 0;
        int num_of_steps = 0;
        maze.setAgent(start);
        while(get<0>(start) != goal_x || get<1>(start) != goal_y){
            maze.printMaze(logFile);
            action = chooseActionWithPolicyEGreedy(start);
            new_state = observeNewState(start, action);
            m_data.at(get<0>(start) + get<1>(start) * x_dim + action * x_dim * y_dim) = reward_data.at(get<0>(start) + get<1>(start) * x_dim + action * x_dim * y_dim) + (GAMMA * getMaxFromNewState(new_state));
            sum_of_rew +=  m_data.at(get<0>(start) + get<1>(start) * x_dim + action * x_dim * y_dim);
            maze.setAgent(new_state);
            start = new_state;
            num_of_steps+=1;
        }
        maze.printMaze(logFile);
        if(num_of_steps == 0){
            return 0;
        }
        return (sum_of_rew/num_of_steps);
    }

#pragma mark SARSA METHOD
       
    int chooseARandomAction(tuple<int,int> currentPos){
        std::random_device rd2;
        std::mt19937 gen(rd2());
        std::uniform_int_distribution<int> dist(0,N_OF_ACTIONS-1);
        int action = dist(gen);
        while(!this->checkAction(currentPos, action)){
            action = (int) dist(gen);
        }
        return action;
    }
    int chooseActionWithPolicyEGreedy(tuple<int,int> current_pos){
        std::random_device rd;
        std::mt19937 gen(rd());
        uniform_real_distribution<double>dis(0,1);
        double rnd = dis(gen);
        int action;
        if(rnd>this->epsilon){
            action =  getBestActionFromCurrentState(current_pos);
            if(action == -1){
                cout << "Current pos IS AT x:"<< get<0>(current_pos) << " y: " << get<1>(current_pos) << endl;
                cout << "Error with action...that is --->" << action <<  endl << "List of available actions " << endl;
                for(int i =0;i<N_OF_ACTIONS;i++){
                    cout << "VALUES: " << m_data.at(get<0>(current_pos) + get<1>(current_pos) * x_dim + i * x_dim * y_dim);
                }
                exit(-1);
            }
           
        }else{
            action = chooseARandomAction(current_pos);
        }
        
        return action;
    }
    
    
    void completeSARSA(int goal_x, int goal_y){
        int c=0;
        this->epsilon = 1.0;
        int episodes = 100;
        double tempMetrics = 0,tempMetrics1 = 0;
        std::clock_t start;
        start = std::clock();
        ofstream metrics;
        metrics.open("./metricsSarsa2_1.txt",ios::out);
        logFile << "************************************* SARSA ALGORITHM ************************************** " << endl;
        while (episodes!=0){
            Maze temp((int)x_dim,(int)y_dim);
            temp.setWalls(listOFWalls, tuple<int,int>(goal_x,goal_y));
            this->maze = temp;
            c+=1;
            logFile << "\t\t\t EPISODE: " << c << endl;
            //tempMetrics1 = runAnEpisodeOfSARSA(goal_x, goal_y, this->epsilon);
            tempMetrics1 =  runAnEpisodeWithRandomEffect(goal_x, goal_y, this->epsilon);
            if(tempMetrics1>0){
                tempMetrics = tempMetrics1;
                metrics << "Episode :" << c <<"AVG_SARSA_REWARD value: \t" << tempMetrics << endl;
                
            }else{
                metrics << "Episode :" << c <<"AVG_SARSA_REWARD value: \t" << tempMetrics << endl;
            }
            //metrics<< "Episode " << c << "AverageReward " << runAnEpisodeOfSARSA(goal_x, goal_y, this->epsilon) << endl;
            //metrics<< "Episode " << c << "AverageReward " << runAnEpisodeWithRandomEffect(goal_x, goal_y, this->epsilon) << endl;
            this->printValuesInReadableForm();
            if((this->epsilon - 0.01) >=0){
                this->epsilon -= 0.01;
            }
            episodes-=1;
        }
        cout << "SARSA completed in : " << (clock() - start)/(double)CLOCKS_PER_SEC<< " seconds" << endl;
        logFile.close();
        metrics.close();
        

    }
    double runAnEpisodeWithRandomEffect(int goal_x, int goal_y, double epsilon){
        tuple<int,int> currentPos = getRandomPosition();
        int action = chooseActionWithPolicyEGreedy(currentPos);
        tuple<int,int> new_state;
        int reward,action2,r_act,r_act2;
        int x1,y1,x2,y2;
        double average_reward = 0;
        int number_of_steps = 1;
        while(get<0>(currentPos) != goal_x || get<1>(currentPos) != goal_y){
            logFile << "Starting position is at : " << get<0>(currentPos) << get<1>(currentPos)<< endl;
            r_act = returnARandomAction(currentPos, action);
            new_state = observeNewState(currentPos, r_act);
            reward = reward_data.at(get<0>(currentPos) + get<1>(currentPos) *x_dim + r_act * x_dim * y_dim);
            action2 = chooseActionWithPolicyEGreedy(new_state);
            r_act2 = returnARandomAction(currentPos, action2);
            logFile << "Observing new state is: " << get<0>(new_state) << get<1>(new_state)<< endl;
            logFile << "Action1 =  " << action << "Action2 is= " << action2<< endl;
            x1 = get<0>(currentPos);
            y1 = get<1>(currentPos);
            x2 = get<0>(new_state);
            y2 = get<1>(new_state);
            m_data.at(x1 + y1 * x_dim + r_act * x_dim * y_dim) += ALFA*(reward + (GAMMA*m_data.at(x2 + y2 * x_dim + r_act2 * x_dim * y_dim))
                                                                        - m_data.at(x1 + y1 * x_dim + r_act * x_dim * y_dim));
            //average_reward+=m_data.at(x1 + y1 * x_dim + action * x_dim * y_dim);
            average_reward+=  m_data.at(x1 + y1 * x_dim + r_act * x_dim * y_dim);
            currentPos = new_state;
            action = action2;
            maze.setAgent(currentPos);
            maze.printMaze(logFile);
            number_of_steps+=1;
        }
        return (average_reward/number_of_steps);

    }
    double runAnEpisodeOfSARSA(int goal_x, int goal_y, double epsilon){
        
        tuple<int,int> currentPos = getRandomPosition();
        int action = chooseActionWithPolicyEGreedy(currentPos);
        tuple<int,int> new_state;
        int reward,action2;
        int x1,y1,x2,y2;
        double average_reward = 0;
        int number_of_steps = 1;
        while(get<0>(currentPos) != goal_x || get<1>(currentPos) != goal_y){
            logFile << "Starting position is at : " << get<0>(currentPos) << get<1>(currentPos)<< endl;
            new_state = observeNewState(currentPos, action);
            reward = reward_data.at(get<0>(currentPos) + get<1>(currentPos) *x_dim + action * x_dim * y_dim);
            action2 = chooseActionWithPolicyEGreedy(new_state);
            logFile << "Observing new state is: " << get<0>(new_state) << get<1>(new_state)<< endl;
            logFile << "Action1 =  " << action << "Action2 is= " << action2<< endl;
            x1 = get<0>(currentPos);
            y1 = get<1>(currentPos);
            x2 = get<0>(new_state);
            y2 = get<1>(new_state);
            m_data.at(x1 + y1 * x_dim + action * x_dim * y_dim) += ALFA*(reward + (GAMMA*m_data.at(x2 + y2 * x_dim + action2 * x_dim * y_dim))
                                                                                - m_data.at(x1 + y1 * x_dim + action * x_dim * y_dim));
            average_reward+=m_data.at(x1 + y1 * x_dim + action * x_dim * y_dim);
            currentPos = new_state;
            action = action2;
            maze.setAgent(currentPos);
            maze.printMaze(logFile);
            number_of_steps+=1;
        }
        return (average_reward/number_of_steps);
    }
  
#pragma mark RANDOM EFFECT 
    
    tuple<int,int> returnAfunctionWithARandomEffect(tuple<int,int> currentPos, int normal_action){
        int action;
        std::random_device rd2;
        std::mt19937 gen(rd2());
        std::uniform_int_distribution<int> dist(0,N_OF_ACTIONS-1);
        action = dist(gen);
        int x,y;
        tuple<int,int> res = observeNewState(currentPos, action);
        x = get<0>(currentPos);
        y = get<1>(currentPos);
        while((get<0>(res)==-1 || get<1>(res)==-1)){
            action = dist(gen);
            res = observeNewState(currentPos, action);
        }
        return res;
    }
    int returnARandomAction(tuple<int,int> currentPos, int normal_action){
        int action;
        std::random_device rd2;
        std::mt19937 gen(rd2());
        std::uniform_int_distribution<int> dist(0,N_OF_ACTIONS-1);
        action = dist(gen);
        int x,y;
        tuple<int,int> res = observeNewState(currentPos, action);
        x = get<0>(currentPos);
        y = get<1>(currentPos);
        while((get<0>(res)==-1 || get<1>(res)==-1)){
            action = dist(gen);
            res = observeNewState(currentPos, action);
        }
        return action;
    }
};
