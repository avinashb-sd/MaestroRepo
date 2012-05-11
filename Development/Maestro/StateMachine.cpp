/*
 *  StateMachine.cpp
 *  
 *
 *  Created by Avinash BharathSingh on 11/17/11.
 *  Copyright 2011 The George Washington University. All rights reserved.
 *
 */

#include "StateMachine.h"

using namespace std;

State::State()
{
	stateName = "                 ";
	currentState = false;
	inPossibleGesture = false;
	numberOfPossibleGestures = 0;
	orderNumber = 0;
	left = FALSE;
	right = FALSE;
	up = FALSE;
	down = FALSE;
	forward = FALSE;
	backward = FALSE;
	
	numLeft = 0;
	numRight = 0;
	numUp = 0;
	numDown = 0;
	numForward = 0;
	numBackward = 0;
	
	numLeftUp = 0;
	numLeftDown = 0;
	numLeftForward = 0;
	numLeftBackward = 0;
	numLeftUpForward = 0;
	numLeftUpBackward = 0;
	numLeftDownForward = 0;
	numLeftDownBackward = 0;
	
	numRightUp = 0;
	numRightDown = 0;
	numRightForward = 0;
	numRightBackward = 0;
	numRightUpForward = 0;
	numRightUpBackward = 0;
	numRightDownForward = 0;
	numRightDownBackward = 0;
	
	numUpForward = 0;
	numUpBackward = 0;
	numDownForward = 0;
	numDownBackward = 0;
	
	numNothing = 0;
	
	probLeft = 0;
	probRight = 0;
	probUp = 0;
	probDown = 0;
	probForward = 0;
	probBackward = 0;
	
	probLeftUp = 0;
	probLeftDown = 0;
	probLeftForward = 0;
	probLeftBackward = 0;
	probLeftUpForward = 0;
	probLeftUpBackward = 0;
	probLeftDownForward = 0;
	probLeftDownBackward = 0;
	
	probRightUp = 0;
	probRightDown = 0;
	probRightForward = 0;
	probRightBackward = 0;
	probRightUpForward = 0;
	probRightUpBackward = 0;
	probRightDownForward = 0;
	probRightDownBackward = 0;
	
	probUpForward = 0;
	probUpBackward = 0;
	probDownForward = 0;
	probDownBackward = 0;
	
	probNothing = 0;
}

//Constructor
StateMachine::StateMachine()
{
	currentState = NULL;
	startState = NULL;
	numStates = 0;
	numDetectedStates = 0;
	stateMachineNumber = -1;
	previousTotalTime = 2.0;
	detectWaitNewSig = FALSE;
	detectWaitContSig = FALSE;
	detectRecWaitNewSig = FALSE;
	detectRecWaitContSig = FALSE;
	detectNewSigWaitCont = FALSE;
	detectRecNewSigWaitCont = FALSE;
	detectContSig = FALSE;
	detectRecContSig = FALSE;
	gestureStarted = FALSE;
	overallTrackedProb = 1;
	finishedTrackedProb = 0;
	dominantHandRecording = TRUE;
	dominantHandDetection = TRUE;
	detectThreeSignatures = FALSE;
	detectRecThreeSignatures = FALSE;
	tempFSM = NULL;
	gestureDetected = FALSE;
}

StateMachine::StateMachine(const StateMachine& a)
{
	cout << "Current state: " << a.startState->getStateName() << " \n";
	currentState = a.startState;
	cout << "Start state: " << a.startState->getStateName() << " \n";
	startState = a.startState;
	numStates = a.numStates;
	stateMachineNumber = a.stateMachineNumber;
	dominantHandRecording = a.dominantHandRecording;
	dominantHandDetection = a.dominantHandDetection;
	
	stateBank = a.stateBank;
	
	numDetectedStates = 1;
	detectWaitNewSig = FALSE;
	detectWaitContSig = FALSE;
	detectRecWaitNewSig = FALSE;
	detectRecWaitContSig = FALSE;
	detectNewSigWaitCont = FALSE;
	detectRecNewSigWaitCont = FALSE;
	detectContSig = FALSE;
	detectRecContSig = FALSE;
	gestureStarted = FALSE;	
	overallTrackedProb = 1;
	finishedTrackedProb = 0;
	detectThreeSignatures = FALSE;
	detectRecThreeSignatures = FALSE;
	tempFSM = NULL;
	gestureDetected = FALSE;
	
}

//Update each tick
void StateMachine::Update(const float& dt)
{
	//Make sure a current state is loaded
	if (currentState == NULL) return;
	
}

//Called to transition to another state
//@param stateName the name of the state to transition to
void StateMachine::TransitionTo(std::string stateName)
{
	//Find the named state
	State *goToState = NULL;
	for(iter= stateBank.begin(); iter!= stateBank.end(); iter++)
		if ( (*iter)->stateName == stateName )
			goToState = iter->get();
	
	//Error, trying to transition to a non-existant state
	if (goToState == NULL)
	{
		//Print an error here, or assert if you want
		return;
	}
	
	currentState = goToState;
}

void StateMachine::AddState(State *newState, bool makeCurrent)
{
	if (stateBank.size() == 0) {
		startState = newState;
	}
	else {
		lastState->nextSignature = newState;
		newState->previousSignature = lastState;
	}
	
	//Add this state to the StateMachine
	std::tr1::shared_ptr<State> newStatePtr(newState);
	stateBank.push_back(newStatePtr);
	
	numStates++;
		
	//Make this the current state?
	if (makeCurrent) currentState = newState;
	
	lastState = newState;
}

int StateMachine::GetNumStates()
{
	return numStates;
}

void StateMachine::NextState()
{
	currentState = currentState->nextSignature;
}

void StateMachine::ResetCurrentState()
{
	currentState = startState;
}

void StateMachine::ResetPreviousState()
{
	previousState = startState;
}

bool StateMachine::IsCurrentStart(){
	if (currentState == startState) {
		return TRUE;
	}
	else{
		return FALSE;
	}
}

std::string StateMachine::GetState()
{
	return currentState->stateName;
}

bool StateMachine::SigStateMatch(bool l, bool r, bool u, bool d, bool f, bool b){
	
	printf("State Signature: ");
	cout << GetState() << endl;
	
	/*printf("Detect : ");
	if (l) {
		printf("-left-");
	}
	if (r) {
		printf("-right-");
	}
	if (u) {
		printf("-up-");
	}
	if (d) {
		printf("-down-");
	}
	if (f) {
		printf("-forward-");
	}
	if (b) {
		printf("-backward-");
	}
	printf("\n");*/
		
	bool ret = FALSE;
	
	
	if (l && u && f){if(currentState->probLeftUpForward > 0){printf("			SigStateMatch LeftUpForward\n");UpdateOverallTrackedProb(l,r,u,d,f,b);return TRUE;}return FALSE;}      		
	if (l && u && b){if(currentState->probLeftUpBackward > 0){printf("			SigStateMatch LeftUpBackward\n");UpdateOverallTrackedProb(l,r,u,d,f,b);return TRUE;}return FALSE;} else if (l && u){if(currentState->probLeftUp > 0){printf("			SigStateMatch LeftUp\n");UpdateOverallTrackedProb(l,r,u,d,f,b); return TRUE;}return FALSE;}
	if (r && u && f){if(currentState->probRightUpForward > 0){printf("			SigStateMatch RightUpForward\n");UpdateOverallTrackedProb(l,r,u,d,f,b);return TRUE;}return FALSE;} else if (u && f){if(currentState->probUpForward > 0){printf("			SigStateMatch UpForward\n");UpdateOverallTrackedProb(l,r,u,d,f,b); return TRUE;}return FALSE;}
	if (r && u && b){if(currentState->probRightUpBackward > 0){printf("			SigStateMatch RightUpBackward\n");UpdateOverallTrackedProb(l,r,u,d,f,b); return TRUE;}return FALSE;} else if (r && u){if(currentState->probRightUp > 0){printf("			SigStateMatch RightUp\n");UpdateOverallTrackedProb(l,r,u,d,f,b); return TRUE;}return FALSE;} else if (u && b){if(currentState->probUpBackward > 0){printf("			SigStateMatch UpBackward\n");UpdateOverallTrackedProb(l,r,u,d,f,b); return TRUE;}return FALSE;} else if(u){if(currentState->probUp > 0){printf("			SigStateMatch Up\n");UpdateOverallTrackedProb(l,r,u,d,f,b); return TRUE;}return FALSE;} 
	if (l && d && f){if(currentState->probLeftDownForward > 0){printf("			SigStateMatch LeftDownForward\n");UpdateOverallTrackedProb(l,r,u,d,f,b); return TRUE;}return FALSE;} else if (l && f){if(currentState->probLeftForward > 0){printf("			SigStateMatch LeftForward\n");UpdateOverallTrackedProb(l,r,u,d,f,b); return TRUE;}return FALSE;} 
	if (l && d && b){if(currentState->probLeftDownBackward > 0){printf("			SigStateMatch LeftDownBackward\n");UpdateOverallTrackedProb(l,r,u,d,f,b); return TRUE;}return FALSE;} else if (l && d){if(currentState->probLeftDown > 0){printf("			SigStateMatch LeftDown\n");UpdateOverallTrackedProb(l,r,u,d,f,b); return TRUE;}return FALSE;} else if (l && b){if(currentState->probLeftBackward > 0){printf("			SigStateMatch LeftBackward\n");UpdateOverallTrackedProb(l,r,u,d,f,b); return TRUE;}return FALSE;} else if(l){if(currentState->probLeft > 0){printf("			SigStateMatch Left\n");UpdateOverallTrackedProb(l,r,u,d,f,b); return TRUE;}return FALSE;} 
	if (r && d && f){if(currentState->probRightDownForward > 0){printf("			SigStateMatch RightDownForward\n");UpdateOverallTrackedProb(l,r,u,d,f,b); return TRUE;}return FALSE;} else if (d && f){if(currentState->probDownForward > 0){printf("			SigStateMatch DownForward\n");UpdateOverallTrackedProb(l,r,u,d,f,b); return TRUE;}return FALSE;} else if (r && f){if(currentState->probRightForward > 0){printf("			SigStateMatch RightForward\n");UpdateOverallTrackedProb(l,r,u,d,f,b); return TRUE;}return FALSE;} else if(f){if(currentState->probForward > 0){printf("			SigStateMatch Forward\n");UpdateOverallTrackedProb(l,r,u,d,f,b); return TRUE;}return FALSE;} 
	if (r && d && b){if(currentState->probRightDownBackward > 0){printf("			SigStateMatch RightDownBackward\n");UpdateOverallTrackedProb(l,r,u,d,f,b); return TRUE;}return FALSE;} else if (r && d){if(currentState->probRightDown > 0){printf("			SigStateMatch RightDown\n");UpdateOverallTrackedProb(l,r,u,d,f,b); return TRUE;}return FALSE;} else if (r && b){if(currentState->probRightBackward > 0){printf("			SigStateMatch RightBackward\n");UpdateOverallTrackedProb(l,r,u,d,f,b); return TRUE;}return FALSE;} else if (d && b){if(currentState->probDownBackward > 0){printf("			SigStateMatch DownBackward\n");UpdateOverallTrackedProb(l,r,u,d,f,b); return TRUE;}return FALSE;} else if(r){if(currentState->probRight > 0){printf("			SigStateMatch Right\n");UpdateOverallTrackedProb(l,r,u,d,f,b); return TRUE;}return FALSE;} else if(d){if(currentState->probDown > 0){printf("			SigStateMatch Down\n");UpdateOverallTrackedProb(l,r,u,d,f,b); return TRUE;}return FALSE;} else if(b){if(currentState->probBackward > 0){printf("			SigStateMatch Backward\n");UpdateOverallTrackedProb(l,r,u,d,f,b); return TRUE;}return FALSE;} 
	
	
	return ret;
	
}

int StateMachine::UpdateOverallTrackedProb(bool l, bool r, bool u, bool d, bool f, bool b){
	
	if (currentState != NULL) {
		if (l && u && f && currentState->probLeftUpForward > 0){ overallTrackedProb += currentState->probLeftUpForward; return 0;}      		
		if (l && u && b && currentState->probLeftUpBackward > 0){ overallTrackedProb += currentState->probLeftUpBackward; return 0;} else if (l && u && currentState->probLeftUp > 0){ overallTrackedProb += currentState->probLeftUp; return 0;}
		if (r && u && f && currentState->probRightUpForward > 0){  overallTrackedProb += currentState->probRightUpForward; return 0;} else if (u && f && currentState->probUpForward > 0){  overallTrackedProb += currentState->probUpForward; return 0;}
		if (r && u && b && currentState->probRightUpBackward > 0){  overallTrackedProb += currentState->probRightUpBackward; return 0;} else if (r && u && currentState->probRightUp > 0){  overallTrackedProb += currentState->probRightUp; return 0;} else if (u && b && currentState->probUpBackward > 0){  overallTrackedProb += currentState->probUpBackward; return 0;} else if(u && currentState->probUp > 0){  overallTrackedProb += currentState->probUp; return 0;} 
		if (l && d && f && currentState->probLeftDownForward > 0){ overallTrackedProb += currentState->probLeftDownForward; return 0;} else if (l && f && currentState->probLeftForward > 0){ overallTrackedProb += currentState->probLeftForward; return 0;} 
		if (l && d && b && currentState->probLeftDownBackward > 0){ overallTrackedProb += currentState->probLeftDownBackward; return 0;} else if (l && d && currentState->probLeftDown > 0){ overallTrackedProb += currentState->probLeftDown; return 0;} else if (l && b && currentState->probLeftBackward > 0){ overallTrackedProb += currentState->probLeftBackward; return 0;} else if(l && currentState->probLeft > 0){ overallTrackedProb += currentState->probLeft; return 0;} 
		if (r && d && f && currentState->probRightDownForward > 0){  overallTrackedProb += currentState->probRightDownForward; return 0;} else if (d && f && currentState->probDownForward > 0){  overallTrackedProb += currentState->probDownForward; return 0;} else if (r && f && currentState->probRightForward > 0){  overallTrackedProb += currentState->probRightForward; return 0;} else if(f && currentState->probForward > 0){  overallTrackedProb += currentState->probForward; return 0;} 
		if (r && d && b && currentState->probRightDownBackward > 0){  overallTrackedProb += currentState->probRightDownBackward; return 0;} else if (r && d && currentState->probRightDown > 0){  overallTrackedProb += currentState->probRightDown; return 0;} else if (r && b && currentState->probRightBackward > 0){  overallTrackedProb += currentState->probRightBackward; return 0;} else if (d && b && currentState->probDownBackward > 0){  overallTrackedProb += currentState->probDownBackward; return 0;} else if(r && currentState->probRight > 0){  overallTrackedProb += currentState->probRight; return 0;} else if(d && currentState->probDown > 0){  overallTrackedProb += currentState->probDown; return 0;} else if(b && currentState->probBackward > 0){  overallTrackedProb += currentState->probBackward; return 0;} 		
	}

	return 0;
		
}


bool StateMachine::SigStartStateMatch(bool l, bool r, bool u, bool d, bool f, bool b){
	
	printf("Start State Signature: ");
	cout << startState->getStateName() << endl;
	
	printf("Detect : ");
	if (l) {
		printf("-left-");
	}
	if (r) {
		printf("-right-");
	}
	if (u) {
		printf("-up-");
	}
	if (d) {
		printf("-down-");
	}
	if (f) {
		printf("-forward-");
	}
	if (b) {
		printf("-backward-");
	}
	printf("\n");
	
	bool ret = FALSE;
	 
	 if (l && u && f && startState->probLeftUpForward > 0){ return TRUE;}      		
	 if (l && u && b && startState->probLeftUpBackward > 0){ return TRUE;} else if (l && u && startState->probLeftUp > 0){ return TRUE;}
	 if (r && u && f && startState->probRightUpForward > 0){ return TRUE;} else if (u && f && startState->probUpForward > 0){ return TRUE;}
	 if (r && u && b && startState->probRightUpBackward > 0){ return TRUE;} else if (r && u && startState->probRightUp > 0){ return TRUE;} else if (u && b && startState->probUpBackward > 0){ return TRUE;} else if(u && startState->probUp > 0){ return TRUE;} 
	 if (l && d && f && startState->probLeftDownForward > 0){ return TRUE;} else if (l && f && startState->probLeftForward > 0){ return TRUE;} 
	 if (l && d && b && startState->probLeftDownBackward > 0){ return TRUE;} else if (l && d && startState->probLeftDown > 0){ return TRUE;} else if (l && b && startState->probLeftBackward > 0){ return TRUE;} else if(l && startState->probLeft > 0){ return TRUE;} 
	 if (r && d && f && startState->probRightDownForward > 0){ return TRUE;} else if (d && f && startState->probDownForward > 0){ return TRUE;} else if (r && f && startState->probRightForward > 0){ return TRUE;} else if(f && startState->probForward > 0){ return TRUE;} 
	 if (r && d && b && startState->probRightDownBackward > 0){ return TRUE;} else if (r && d && startState->probRightDown > 0){ return TRUE;} else if (r && b && startState->probRightBackward > 0){ return TRUE;} else if (d && b && startState->probDownBackward > 0){ return TRUE;} else if(r && startState->probRight > 0){ return TRUE;} else if(d && startState->probDown > 0){ return TRUE;} else if(b && startState->probBackward > 0){ return TRUE;} 
	 
	 
	return ret;
	
}

int StateMachine::SetOverallTrackedProb(bool l, bool r, bool u, bool d, bool f, bool b){
	if (l && u && f){ overallTrackedProb = startState->probLeftUpForward; return 0;}      		
	if (l && u && b){ overallTrackedProb = startState->probLeftUpBackward; return 0;} else if (l && u){ overallTrackedProb = startState->probLeftUp; return 0;}
	if (r && u && f){ overallTrackedProb = startState->probRightUpForward; return 0;} else if (u && f){ overallTrackedProb = startState->probUpForward; return 0;}
	if (r && u && b){ overallTrackedProb = startState->probRightUpBackward; return 0;} else if (r && u){ overallTrackedProb = startState->probRightUp; return 0;} else if (u && b){ overallTrackedProb = startState->probUpBackward; return 0;} else if(u){ overallTrackedProb = startState->probUp; return 0;} 
	if (l && d && f){ overallTrackedProb = startState->probLeftDownForward; return 0;} else if (l && f){ overallTrackedProb = startState->probLeftForward; return 0;} 
	if (l && d && b){ overallTrackedProb = startState->probLeftDownBackward; return 0;} else if (l && d){ overallTrackedProb = startState->probLeftDown; return 0;} else if (l && b){ overallTrackedProb = startState->probLeftBackward; return 0;} else if(l){ overallTrackedProb = startState->probLeft; return 0;} 
	if (r && d && f){ overallTrackedProb = startState->probRightDownForward; return 0;} else if (d && f){ overallTrackedProb = startState->probDownForward; return 0;} else if (r && f){ overallTrackedProb = startState->probRightForward; return 0;} else if(f){ overallTrackedProb = startState->probForward; return 0;} 
	if (r && d && b){ overallTrackedProb = startState->probRightDownBackward; return 0;} else if (r && d){ overallTrackedProb = startState->probRightDown; return 0;} else if (r && b){ overallTrackedProb = startState->probRightBackward; return 0;} else if (d && b){ overallTrackedProb = startState->probDownBackward; return 0;} else if(r){ overallTrackedProb = startState->probRight; return 0;} else if(d){ overallTrackedProb = startState->probDown; return 0;} else if(b){ overallTrackedProb = startState->probBackward; return 0;} 

	return 0;
}


void StateMachine::PrintStateMachineBools()
{
	int i = 1;
	
	for(iter= stateBank.begin(); iter!= stateBank.end(); iter++){
		
		
		printf("State %d: ", i);
		
		if((*iter)->getStateLeftBool()){
				printf("left ");
		}
		if((*iter)->getStateRightBool()){
			printf("right ");
		}
		if((*iter)->getStateUpBool()){
			printf("up ");
		}
		if((*iter)->getStateDownBool()){
			printf("down ");
		}
		if((*iter)->getStateForwardBool()){
			printf("forward ");
		}
		if((*iter)->getStateBackwardBool()){
			printf("backward ");
		}
		
		printf("\n");
		
		i++;
	}
	
}

void StateMachine::CalculateStateProbabilities(int num)
{
	for(iter= stateBank.begin(); iter!= stateBank.end(); iter++)
	{
		(*iter)->calculateProbabilities(num);
	}
}

void StateMachine::PrintStateMachine()
{
	for(iter= stateBank.begin(); iter!= stateBank.end(); iter++)
		cout << (*iter)->getStateName() << endl;
	
}

void StateMachine::PrintStateMachineProbs()
{
	int i = 1;
	for(iter= stateBank.begin(); iter!= stateBank.end(); iter++){
		printf("Probabilities for State #%d\n",i);
		i++;
		(*iter)->printProbabilities();
	}
	
}
