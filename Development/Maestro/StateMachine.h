/*
 *  StateMachine.h
 *  
 *
 *  Created by Avinash BharathSingh on 11/17/11.
 *  Copyright 2011 The George Washington University. All rights reserved.
 *
 */

//StateMachine.h
#ifndef STATEMACHINE_H
#define STATEMACHINE_H

//using namespace std;

#include <iostream>
#include <string>
#include <vector>
#include <time.h>
#include <boost/tr1/memory.hpp>
#include <XnCppWrapper.h>

class StateMachine;

class State
{
public:
	State();
	bool left;
	bool right;
	bool up;
	bool down;
	bool forward;
	bool backward;
	clock_t secondsInState;
	State *previousSignature;
	State *nextSignature;
	XnPoint3D location;
	bool inPossibleGesture;
	int numberOfPossibleGestures;
	int orderNumber;
	
	int numLeft;
	int numRight;
	int numUp;
	int numDown;
	int numForward;
	int numBackward;
	
	int numLeftUp;
	int numLeftDown;
	int numLeftForward;
	int numLeftBackward;
	int numLeftUpForward;
	int numLeftUpBackward;
	int numLeftDownForward;
	int numLeftDownBackward;
	
	int numRightUp;
	int numRightDown;
	int numRightForward;
	int numRightBackward;
	int numRightUpForward;
	int numRightUpBackward;
	int numRightDownForward;
	int numRightDownBackward;
	
	int numNothing;
	
	int numUpForward;
	int numUpBackward;
	int numDownForward;
	int numDownBackward;
	
	double probLeft;
	double probRight;
	double probUp;
	double probDown;
	double probForward;
	double probBackward;
	
	double probLeftUp;
	double probLeftDown;
	double probLeftForward;
	double probLeftBackward;
	double probLeftUpForward;
	double probLeftUpBackward;
	double probLeftDownForward;
	double probLeftDownBackward;
	
	double probRightUp;
	double probRightDown;
	double probRightForward;
	double probRightBackward;
	double probRightUpForward;
	double probRightUpBackward;
	double probRightDownForward;
	double probRightDownBackward;
	
	double probUpForward;
	double probUpBackward;
	double probDownForward;
	double probDownBackward;
	
	double probNothing;
	
	std::string stateName;
	
	bool currentState;
	void setStateSignatureLocation(XnPoint3D a)
	{location = a;}
	XnPoint3D getLocation()
	{return location;}
	
	bool getStateLeftBool() {return left;}
	bool getStateRightBool() {return right;}
	bool getStateUpBool() {return up;}
	bool getStateDownBool() {return down;}
	bool getStateForwardBool() {return forward;}
	bool getStateBackwardBool() {return backward;}
	
	int setStateDirectionCounts(bool l, bool r, bool u, bool d, bool f, bool b)
	{if (l && u && f){ numLeftUpForward++; printf("LeftUpForward Inc\n"); return 0;}      		
		if (l && u && b){ numLeftUpBackward++; printf("LeftUpBackward Inc\n"); return 0;} else if (l && u){ numLeftUp++; printf("LeftUp Inc\n"); return 0;}
		if (r && u && f){ numRightUpForward++; printf("RightUpForward Inc\n"); return 0;} else if (u && f){ numUpForward++; printf("UpForward Inc\n"); return 0;}
		if (r && u && b){ numRightUpBackward++; printf("RightUpBackward Inc\n"); return 0;} else if (r && u){ numRightUp++; printf("RightUp Inc\n"); return 0;} else if (u && b){ numUpBackward++; printf("UpBackward Inc\n"); return 0;} else if(u){ numUp++; printf("Up Inc\n"); return 0;} 
		if (l && d && f){ numLeftDownForward++; printf("LeftDownForward Inc\n"); return 0;} else if (l && f){ numLeftForward++; printf("LeftForward Inc\n"); return 0;} 
		if (l && d && b){ numLeftDownBackward++; printf("LeftDownBackward Inc\n");return 0;} else if (l && d){ numLeftDown++; printf("LeftDown Inc\n");return 0;} else if (l && b){ numLeftBackward++; printf("LeftBackward Inc\n");return 0;} else if(l){ numLeft++; printf("Left Inc\n");return 0;} 
		if (r && d && f){ numRightDownForward++; printf("RightDownForward Inc\n");return 0;} else if (d && f){ numDownForward++; printf("DownForward Inc\n"); return 0;} else if (r && f){ numRightForward++; printf("RightForward Inc\n"); return 0;} else if(f){ numForward++; printf("Forward Inc\n"); return 0;} 
		if (r && d && b){ numRightDownBackward++; printf("RightDownBackward Inc\n");return 0;} else if (r && d){ numRightDown++; printf("RightDown Inc\n"); return 0;} else if (r && b){ numRightBackward++; printf("RightBackward Inc\n");return 0;} else if (d && b){ numDownBackward++; printf("DownBackward Inc\n"); return 0;} else if(r){ numRight++; printf("Right Inc\n");return 0;} else if(d){ numDown++; printf("Down Inc\n");return 0;} else if(b){ numBackward++; printf("Backward Inc\n"); return 0;} 
		
		return 0;
	}
	
	void calculateProbabilities(int numAttempts){
		probLeft = double(numLeft) / double(numAttempts);
		probRight = double(numRight) / double(numAttempts);
		probUp = double(numUp) / double(numAttempts);
		probDown = double(numDown) / double(numAttempts);
		probForward = double(numForward) / double(numAttempts);
		probBackward = double(numBackward) / double(numAttempts);
		
		probLeftUp = double(numLeftUp) / double(numAttempts);
		probLeftDown = double(numLeftDown) / double(numAttempts);
		probLeftForward = double(numLeftForward) / double(numAttempts);
		probLeftBackward = double(numLeftBackward) / double(numAttempts);
		probLeftUpForward = double(numLeftUpForward) / double(numAttempts);
		probLeftUpBackward = double(numLeftUpBackward) / double(numAttempts);
		probLeftDownForward = double(numLeftDownForward) / double(numAttempts);
		probLeftDownBackward = double(numLeftDownBackward) / double(numAttempts);
		
		probRightUp = double(numRightUp) / double(numAttempts);
		probRightDown = double(numRightDown) / double(numAttempts);
		probRightForward = double(numRightForward) / double(numAttempts);
		probRightBackward = double(numRightBackward) / double(numAttempts);
		probRightUpForward = double(numRightUpForward) / double(numAttempts);
		probRightUpBackward = double(numRightUpBackward) / double(numAttempts);
		probRightDownForward = double(numRightDownForward) / double(numAttempts);
		probRightDownBackward = double(numRightDownBackward) / double(numAttempts);
		
		probUpForward = double(numUpForward) / double(numAttempts);
		probUpBackward = double(numUpBackward) / double(numAttempts);
		probDownForward = double(numDownForward) / double(numAttempts);
		probDownBackward = double(numDownBackward) / double(numAttempts);
	}
	
	void setStateBools(bool l, bool r, bool u, bool d, bool f, bool b)
	{left = l; right = r; up = u; down = d; forward = f; backward = b;
		//setStateDirectionCounts(l, r, u, d, f, b);
	}
	
	void printCounts(){
		if (numLeft > 0) printf(" Left Count: %d\n", numLeft);
		if (numRight > 0) printf(" Right Count: %d\n", numRight);
		if (numUp > 0) printf(" Up Count: %d\n", numUp);
		if (numDown > 0) printf(" Down Count: %d\n", numDown);
		if (numForward > 0) printf(" Forward Count: %d\n", numForward);
		if (numBackward > 0) printf(" Backward Count: %d\n", numBackward);
		if (numLeftUp > 0) printf(" LeftUp Count: %d\n", numLeftUp);
		if (numLeftDown > 0) printf(" LeftDown Count: %d\n", numLeftDown);
		if (numLeftForward > 0) printf(" LeftForward Count: %d\n", numLeftForward);
		if (numLeftBackward > 0) printf(" LeftBackward Count: %d\n", numLeftBackward);
		if (numLeftUpForward > 0) printf(" LeftUpForward Count: %d\n", numLeftUpForward);
		if (numLeftUpBackward > 0) printf(" LeftUpBackward Count: %d\n", numLeftUpBackward);
		if (numLeftDownForward > 0) printf(" LeftDownForward Count: %d\n", numLeftDownForward);
		if (numLeftDownBackward > 0) printf(" LeftDownBackward Count: %d\n", numLeftDownBackward);
		
		if (numRightUp > 0) printf(" RightUp Count: %d\n", numRightUp);
		if (numRightDown > 0) printf(" RightDown Count: %d\n", numRightDown);
		if (numRightForward > 0) printf(" RightForward Count: %d\n", numRightForward);
		if (numRightBackward > 0) printf(" RightBackward Count: %d\n", numRightBackward);
		if (numRightUpForward > 0) printf(" RightUpForward Count: %d\n", numRightUpForward);
		if (numRightUpBackward > 0) printf(" RightUpBackward Count: %d\n", numRightUpBackward);
		if (numRightDownForward > 0) printf(" RightDownForward Count: %d\n", numRightDownForward);
		if (numRightDownBackward > 0) printf(" RightDownBackward Count: %d\n", numRightDownBackward);
		
		if (numUpForward > 0) printf(" UpForward Count: %d\n", numUpForward);
		if (numUpBackward > 0) printf(" UpBackward Count: %d\n", numUpBackward);
		if (numDownForward > 0) printf(" DownForward Count: %d\n", numDownForward);
		if (numDownBackward > 0) printf(" DownBackward Count: %d\n", numDownBackward);
	}
	
	void printProbabilities(){
		
		if(probLeft > 0) printf("Left Probability: %f\n", probLeft);
		if(probRight > 0) printf("Right Probability: %f\n", probRight);
		if(probUp > 0) printf("Up Probability: %f\n", probUp);
		if(probDown > 0) printf("Down Probability: %f\n", probDown);
		if(probForward > 0) printf("Forward Probability: %f\n", probForward);
		if(probBackward > 0) printf("Backward Probability: %f\n", probBackward);
		
		if(probLeftUp > 0) printf("LeftUp Probability: %f\n", probLeftUp);
		if(probLeftDown > 0) printf("LeftDown Probability: %f\n", probLeftDown);
		if(probLeftForward > 0) printf("LeftForward Probability: %f\n", probLeftForward);
		if(probLeftBackward > 0) printf("LeftBackward Probability: %f\n", probLeftBackward);
		if(probLeftUpForward > 0) printf("LeftUpForward Probability: %f\n", probLeftUpForward);
		if(probLeftUpBackward > 0) printf("LeftUpBackward Probability: %f\n", probLeftUpBackward);
		if(probLeftDownForward > 0) printf("LeftDownForward Probability: %f\n", probLeftDownForward);
		if(probLeftDownBackward > 0) printf("LeftDownBackward Probability: %f\n", probLeftDownBackward);
		
		if(probRightUp > 0) printf("RightUp Probability: %f\n", probRightUp);
		if(probRightDown > 0) printf("RightDown Probability: %f\n", probRightDown);
		if(probRightForward > 0) printf("RightForward Probability: %f\n", probRightForward);
		if(probRightBackward > 0) printf("RightBackward Probability: %f\n", probRightBackward);
		if(probRightUpForward > 0) printf("RightUpForward Probability: %f\n", probRightUpForward);
		if(probRightUpBackward > 0) printf("RightUpBackward Probability: %f\n", probRightUpBackward);
		if(probRightDownForward > 0) printf("RightDownForward Probability: %f\n", probRightDownForward);
		if(probRightDownBackward > 0) printf("RightDownBackward Probability: %f\n", probRightDownBackward);
		
		if(probUpForward > 0) printf("UpForward Probability: %f\n", probUpForward);
		if(probUpBackward > 0) printf("UpBackward Probability: %f\n", probUpBackward);
		if(probDownForward > 0) printf("DownForward Probability: %f\n", probDownForward);
		if(probDownBackward > 0) printf("DownBackward Probability: %f\n", probDownBackward);
		
	}
	
	void setStateName(std::string a)
	{stateName = a;}	
	std::string getStateName()
	{return stateName;}
};

//A vector of shared pointers housing all the states in the machine
typedef std::vector< std::tr1::shared_ptr<State> > StateBank;

class StateMachine: public State
{
public:
	StateMachine();
	StateMachine(const StateMachine& a);
	void Update(const float& dt);
	void TransitionTo(std::string stateName);
	void AddState(State *newState, bool makeCurrent);
	void PrintStateMachine();
	void PrintStateMachineBools();
	void PrintStateMachineProbs();
	void ResetCurrentState();
	void ResetPreviousState();
	void CalculateStateProbabilities(int num);
	int SetOverallTrackedProb(bool l, bool r, bool u, bool d, bool f, bool b);
	int UpdateOverallTrackedProb(bool l, bool r, bool u, bool d, bool f, bool b);
	bool SigStateMatch(bool l, bool r, bool u, bool d, bool f, bool b);
	bool SigStartStateMatch(bool l, bool r, bool u, bool d, bool f, bool b);
	bool IsCurrentStart();
	int GetNumStates();
	
	
	std::string GetState();
	void NextState();
	
	
public:
	State *currentState;
	State *previousState;
	State *startState;
	State *lastState;
	time_t started;
	time_t ended;
	double totalTime;
	double previousTotalTime;
	
	int numStates;
	int numDetectedStates;
	int stateMachineNumber;
	bool detectWaitNewSig;
	bool detectWaitContSig;
	bool detectRecWaitNewSig;
	bool detectRecWaitContSig;
	bool gestureStarted;
	bool gestureDetected;
	bool detectNewSigWaitCont;
	bool detectRecNewSigWaitCont;
	bool detectContSig;
	bool detectRecContSig;
	double overallTrackedProb;
	double finishedTrackedProb;
	bool dominantHandRecording;
	bool dominantHandDetection;
	bool detectThreeSignatures;
	bool detectRecThreeSignatures;
	StateMachine *tempFSM;
	
	/*
	 int verticalRegion;
	
	 
	 */
	
	//Holds States
	StateBank stateBank;
	std::vector< std::tr1::shared_ptr<State> >::iterator iter;
};

#endif