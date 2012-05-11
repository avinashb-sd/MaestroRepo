/****************************************************************************
 *                                                                           *
 *  OpenNI 1.1 Alpha                                                         *
 *  Copyright (C) 2011 PrimeSense Ltd.                                       *
 *                                                                           *
 *  This file is part of OpenNI.                                             *
 *                                                                           *
 *  OpenNI is free software: you can redistribute it and/or modify           *
 *  it under the terms of the GNU Lesser General Public License as published *
 *  by the Free Software Foundation, either version 3 of the License, or     *
 *  (at your option) any later version.                                      *
 *                                                                           *
 *  OpenNI is distributed in the hope that it will be useful,                *
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of           *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the             *
 *  GNU Lesser General Public License for more details.                      *
 *                                                                           *
 *  You should have received a copy of the GNU Lesser General Public License *
 *  along with OpenNI. If not, see <http://www.gnu.org/licenses/>.           *
 *                                                                           *
 ****************************************************************************/

//---------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------
#include <XnOpenNI.h>
#include <XnCodecIDs.h>
#include <XnCppWrapper.h>
#include "SceneDrawer.h"
#include "StateMachine.h"
//#include "StoreGesture.h"
#include "DetectHandSwipe.h"
#include "RotateHandMovement.h"
#include <stdio.h>
#include <stdlib.h>
#include <fmod.hpp>
#include <fmod_dsp.h>
#include <fmod_errors.h>
#include <wincompat.h>
#include <iostream>
#include <string>
#include <cstring>
#include <sys/types.h>
#include <unistd.h>
#include <fstream>
#include <time.h>
#include <vector>
#include <boost/tr1/memory.hpp>
#include <boost/lexical_cast.hpp>


using namespace std;

//---------------------------------------------------------------------------
// Globals
//---------------------------------------------------------------------------
xn::Context g_Context;
xn::ScriptNode g_scriptNode;
xn::DepthGenerator g_DepthGenerator;
xn::UserGenerator g_UserGenerator;
xn::Player g_Player;

#define NUM_GESTURES 14
#define NUM_STATES_PER_GESTURE 6

RotateHandMovement rotatingObject;
DetectHandSwipe	dominantHandTracker;
DetectHandSwipe	weakHandTracker;

XnBool g_bNeedPose = FALSE;
XnChar g_strPose[20] = "";
XnBool g_bDrawBackground = TRUE;
XnBool g_bDrawPixels = TRUE;
XnBool g_bDrawSkeleton = TRUE;
XnBool g_bPrintID = TRUE;
XnBool g_bPrintState = TRUE;
bool g_bRecordGesture = FALSE;
bool g_bDetectGesture = FALSE;
bool initialRecorded = FALSE;
bool recordComplete = FALSE;
bool ready = FALSE;
bool printGestureDetection = FALSE;
bool printGestureSelection = FALSE;
bool printNumExecuted = FALSE;
bool printRecordPrompt = FALSE;
bool gestureSelectedPrinted = FALSE;

bool playing = false;

bool groupAStarted = false;
bool groupBStarted = false;

bool aHandMoved = FALSE;

XnSkeletonJointPosition rightHand;
XnSkeletonJointPosition leftHand;
XnSkeletonJointPosition leftShoulder;
XnSkeletonJointPosition rightShoulder;

bool TimerRelease = FALSE;

bool firstRound = FALSE;

int numRecordedStates = 0;
int numDetectedStates = 0;
int numExecuted = 1;
int numTrackedStates = 0;
int gestureNum = -1;
int minRecordings = 5;

bool somethingSelectedRecord = FALSE;

bool gestureDetected = FALSE;
vector<StateMachine> gesturesDetected;

pid_t pID;
pid_t pID2;

StateMachine *incVolume;
StateMachine *decVolume;
StateMachine *incTempo;
StateMachine *decTempo;
StateMachine *incPitch;
StateMachine *decPitch;
StateMachine gestures[NUM_GESTURES];
State *state;

vector<StateMachine> recursiveGestureVector;
vector<StateMachine>::iterator it;

string SongFileLeft;
float initialVolumeLeft;
float initialPitchLeft;
float initialTempoLeft;

string SongFileRight;
float initialVolumeRight;
float initialPitchRight;
float initialTempoRight;

char signatureName[60] = "";

clock_t timer;
clock_t dtimer = clock();

time_t startSigTime = time(NULL);
time_t previousSigTime = time(NULL);
time_t currentSigTime = time(NULL);
time_t w_startSigTime = time(NULL);
time_t w_previousSigTime = time(NULL);
time_t w_currentSigTime = time(NULL);

double CPS = 1000000;
double nTime = 0.5;
int detectTime = 1;

int vectorSize = 0;

bool detectWaitNewSig = FALSE;
bool detectWaitContSig = FALSE;

StateMachine *tempFSM;

#ifndef USE_GLES
#if (XN_PLATFORM == XN_PLATFORM_MACOSX)
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif
#else
#include "opengles.h"
#endif

#ifdef USE_GLES
static EGLDisplay display = EGL_NO_DISPLAY;
static EGLSurface surface = EGL_NO_SURFACE;
static EGLContext context = EGL_NO_CONTEXT;
#endif

#define GL_WIN_SIZE_X 720
#define GL_WIN_SIZE_Y 480

float WIN_HEIGHT = GL_WIN_SIZE_Y;
float WIN_WIDTH = GL_WIN_SIZE_X;

XnBool g_bPause = false;
XnBool g_bRecord = false;
XnBool g_bQuit = false;

//---------------------------------------------------------------------------
// Code
//---------------------------------------------------------------------------

void SaveGestures(){
	printf("Saving\n");
	ofstream myfile;
	myfile.open ("../../Config/gestures.txt",ios::app);
	vector<StateMachine>::iterator iter;
	int stateNum = 1;
	
	for(int i = 0; i < NUM_GESTURES; i++){
		stateNum = 1;
		printf("Gesture Num: %d\n", i);
		printf("Num States: %d\n", gestures[i].numStates);
		
		myfile << "index: \n";
		myfile << i << "\n";
		myfile << gestures[i].overallTrackedProb << "\n";
		myfile << gestures[i].numStates << "\n";
		
		for(int j = 0; j < gestures[i].numStates; j++)
		{
			printf("index: %d\n", j);
			myfile << gestures[i].stateBank.at(j)->getStateName() << "\n";
			
			if (gestures[i].stateBank.at(j)->left) {
				myfile << "Yes\n";
			}
			else {
				myfile << "No\n";
			}
			
			if (gestures[i].stateBank.at(j)->right) {
				myfile << "Yes\n";
			}
			else {
				myfile << "No\n";
			}
			
			if (gestures[i].stateBank.at(j)->up) {
				myfile << "Yes\n";
			}
			else {
				myfile << "No\n";
			}
			
			if (gestures[i].stateBank.at(j)->down) {
				myfile << "Yes\n";
			}
			else {
				myfile << "No\n";
			}
			
			if (gestures[i].stateBank.at(j)->forward) {
				myfile << "Yes\n";
			}
			else {
				myfile << "No\n";
			}
			
			if (gestures[i].stateBank.at(j)->backward) {
				myfile << "Yes\n";
			}
			else {
				myfile << "No\n";
			}
			
			myfile << gestures[i].stateBank.at(j)->numLeft << "\n";
			myfile << gestures[i].stateBank.at(j)->numRight << "\n";
			myfile << gestures[i].stateBank.at(j)->numUp << "\n";
			myfile << gestures[i].stateBank.at(j)->numDown << "\n";
			myfile << gestures[i].stateBank.at(j)->numForward << "\n";
			myfile << gestures[i].stateBank.at(j)->numBackward << "\n";
			
			myfile << gestures[i].stateBank.at(j)->numLeftUp << "\n";
			myfile << gestures[i].stateBank.at(j)->numLeftDown << "\n";
			myfile << gestures[i].stateBank.at(j)->numLeftForward << "\n";
			myfile << gestures[i].stateBank.at(j)->numLeftBackward << "\n";
			myfile << gestures[i].stateBank.at(j)->numLeftUpForward << "\n";
			myfile << gestures[i].stateBank.at(j)->numLeftUpBackward << "\n";
			myfile << gestures[i].stateBank.at(j)->numLeftDownForward << "\n";
			myfile << gestures[i].stateBank.at(j)->numLeftDownBackward << "\n";
			
			myfile << gestures[i].stateBank.at(j)->numRightUp << "\n";
			myfile << gestures[i].stateBank.at(j)->numRightDown << "\n";
			myfile << gestures[i].stateBank.at(j)->numRightForward << "\n";
			myfile << gestures[i].stateBank.at(j)->numRightBackward << "\n";
			myfile << gestures[i].stateBank.at(j)->numRightUpForward << "\n";
			myfile << gestures[i].stateBank.at(j)->numRightUpBackward << "\n";
			myfile << gestures[i].stateBank.at(j)->numRightDownForward << "\n";
			myfile << gestures[i].stateBank.at(j)->numRightDownBackward << "\n";
			
			myfile << gestures[i].stateBank.at(j)->numUpForward << "\n";
			myfile << gestures[i].stateBank.at(j)->numUpBackward << "\n";
			myfile << gestures[i].stateBank.at(j)->numDownForward << "\n";
			myfile << gestures[i].stateBank.at(j)->numDownBackward << "\n";
			
			myfile << gestures[i].stateBank.at(j)->numNothing << "\n";
			
			myfile << gestures[i].stateBank.at(j)->probLeft << "\n";
			myfile << gestures[i].stateBank.at(j)->probRight << "\n";
			myfile << gestures[i].stateBank.at(j)->probUp << "\n";
			myfile << gestures[i].stateBank.at(j)->probDown << "\n";
			myfile << gestures[i].stateBank.at(j)->probForward << "\n";
			myfile << gestures[i].stateBank.at(j)->probBackward << "\n";
			
			myfile << gestures[i].stateBank.at(j)->probLeftUp << "\n";
			myfile << gestures[i].stateBank.at(j)->probLeftDown << "\n";
			myfile << gestures[i].stateBank.at(j)->probLeftForward << "\n";
			myfile << gestures[i].stateBank.at(j)->probLeftBackward << "\n";
			myfile << gestures[i].stateBank.at(j)->probLeftUpForward << "\n";
			myfile << gestures[i].stateBank.at(j)->probLeftUpBackward << "\n";
			myfile << gestures[i].stateBank.at(j)->probLeftDownForward << "\n";
			myfile << gestures[i].stateBank.at(j)->probLeftDownBackward << "\n";
			
			myfile << gestures[i].stateBank.at(j)->probRightUp << "\n";
			myfile << gestures[i].stateBank.at(j)->probRightDown << "\n";
			myfile << gestures[i].stateBank.at(j)->probRightForward << "\n";
			myfile << gestures[i].stateBank.at(j)->probRightBackward << "\n";
			myfile << gestures[i].stateBank.at(j)->probRightUpForward << "\n";
			myfile << gestures[i].stateBank.at(j)->probRightUpBackward << "\n";
			myfile << gestures[i].stateBank.at(j)->probRightDownForward << "\n";
			myfile << gestures[i].stateBank.at(j)->probRightDownBackward << "\n";
			
			myfile << gestures[i].stateBank.at(j)->probUpForward << "\n";
			myfile << gestures[i].stateBank.at(j)->probUpBackward << "\n";
			myfile << gestures[i].stateBank.at(j)->probDownForward << "\n";
			myfile << gestures[i].stateBank.at(j)->probDownBackward << "\n";
			
			myfile << gestures[i].stateBank.at(j)->probNothing << "\n";
		}
	}	
	myfile.close();
}


void LoadGestures(){
	printf("Loading \n");
	
	ifstream myfile;
	myfile.open("../../Config/gestures.txt");
	int		gestureNum;
	int		numStates;
	int		stateCounter = 0;
	int		lineNumber;
	string	line;
	char *convert;
	State *state;
	
	getline (myfile,line);
	while (line.compare("index: ") == 0) {
		printf("In While \n");
		
		getline (myfile,line);
		
		gestureNum = atoi(line.c_str());
		
		printf("GestureNum: %d\n", gestureNum);
		
		gestures[gestureNum].stateMachineNumber = gestureNum;
		
		getline (myfile,line);
		gestures[gestureNum].overallTrackedProb = atof(line.c_str());
		
		getline (myfile,line);
		numStates = atoi(line.c_str());
		
		printf("Read NumStates: %d\n", numStates);
		
		stateCounter = 0;
		while(stateCounter < numStates){
			state = new State();
			printf("stateCounter: %d\n", stateCounter);
			
			getline (myfile,line);	
			state->setStateName(line);
			
			printf("State Name: %s\n", (state->getStateName()).c_str());
			
			getline (myfile,line);
			if (line.compare("Yes") == 0) {
				state->left = TRUE;
			}
			else {
				state->left = FALSE;
			}
			
			getline (myfile,line);
			if (line.compare("Yes") == 0) {
				state->right = TRUE;
			}
			else {
				state->right = FALSE;
			}
			
			getline (myfile,line);
			if (line.compare("Yes") == 0) {
				state->up = TRUE;
			}
			else {
				state->up = FALSE;
			}
			
			getline (myfile,line);
			if (line.compare("Yes") == 0) {
				state->down = TRUE;
			}
			else {
				state->down = FALSE;
			}
			
			getline (myfile,line);
			if (line.compare("Yes") == 0) {
				state->forward = TRUE;
			}
			else {
				state->forward = FALSE;
			}
			
			getline (myfile,line);
			if (line.compare("Yes") == 0) {
				state->backward = TRUE;
			}
			else {
				state->backward = FALSE;
			}
			
			
			getline (myfile,line);
			
			state->numLeft = atoi(line.c_str());
			getline (myfile,line);
			state->numRight = atoi(line.c_str());
			getline (myfile,line);
			state->numUp = atoi(line.c_str());
			getline (myfile,line);
			state->numDown = atoi(line.c_str());
			getline (myfile,line);
			state->numForward = atoi(line.c_str());
			getline (myfile,line);
			state->numBackward = atoi(line.c_str());
			
			getline (myfile,line);
			state->numLeftUp = atoi(line.c_str());
			getline (myfile,line);
			state->numLeftDown = atoi(line.c_str());
			getline (myfile,line);
			state->numLeftForward = atoi(line.c_str());
			getline (myfile,line);
			state->numLeftBackward = atoi(line.c_str());
			getline (myfile,line);
			state->numLeftUpForward = atoi(line.c_str());
			getline (myfile,line);
			state->numLeftUpBackward = atoi(line.c_str());
			getline (myfile,line);
			state->numLeftDownForward = atoi(line.c_str());
			getline (myfile,line);
			state->numLeftDownBackward = atoi(line.c_str());
			
			getline (myfile,line);
			state->numRightUp = atoi(line.c_str());
			getline (myfile,line);
			state->numRightDown = atoi(line.c_str());
			getline (myfile,line);
			state->numRightForward = atoi(line.c_str());
			getline (myfile,line);
			state->numRightBackward = atoi(line.c_str());
			getline (myfile,line);
			state->numRightUpForward = atoi(line.c_str());
			getline (myfile,line);
			state->numRightUpBackward = atoi(line.c_str());
			getline (myfile,line);
			state->numRightDownForward = atoi(line.c_str());
			getline (myfile,line);
			state->numRightDownBackward = atoi(line.c_str());
			
			getline (myfile,line);
			state->numUpForward = atoi(line.c_str());
			getline (myfile,line);
			state->numUpBackward = atoi(line.c_str());
			getline (myfile,line);
			state->numDownForward = atoi(line.c_str());
			getline (myfile,line);
			state->numDownBackward = atoi(line.c_str());
			
			getline (myfile,line);
			state->numNothing = atoi(line.c_str());
			
			
			
			
			getline (myfile,line);
			state->probLeft = atof(line.c_str());
			getline (myfile,line);
			state->probRight = atof(line.c_str());
			getline (myfile,line);
			state->probUp = atof(line.c_str());
			getline (myfile,line);
			state->probDown = atof(line.c_str());
			getline (myfile,line);
			state->probForward = atof(line.c_str());
			getline (myfile,line);
			state->probBackward = atof(line.c_str());
			
			getline (myfile,line);
			state->probLeftUp = atof(line.c_str());
			getline (myfile,line);
			state->probLeftDown = atof(line.c_str());
			getline (myfile,line);
			state->probLeftForward = atof(line.c_str());
			getline (myfile,line);
			state->probLeftBackward = atof(line.c_str());
			getline (myfile,line);
			state->probLeftUpForward = atof(line.c_str());
			getline (myfile,line);
			state->probLeftUpBackward = atof(line.c_str());
			getline (myfile,line);
			state->probLeftDownForward = atof(line.c_str());
			getline (myfile,line);
			state->probLeftDownBackward = atof(line.c_str());
			
			getline (myfile,line);
			state->probRightUp = atof(line.c_str());
			getline (myfile,line);
			state->probRightDown = atof(line.c_str());
			getline (myfile,line);
			state->probRightForward = atof(line.c_str());
			getline (myfile,line);
			state->probRightBackward = atof(line.c_str());
			getline (myfile,line);
			state->probRightUpForward = atof(line.c_str());
			getline (myfile,line);
			state->probRightUpBackward = atof(line.c_str());
			getline (myfile,line);
			state->probRightDownForward = atof(line.c_str());
			getline (myfile,line);
			state->probRightDownBackward = atof(line.c_str());
			
			getline (myfile,line);
			state->probUpForward = atof(line.c_str());
			getline (myfile,line);
			state->probUpBackward = atof(line.c_str());
			getline (myfile,line);
			state->probDownForward = atof(line.c_str());
			getline (myfile,line);
			state->probDownBackward = atof(line.c_str());
			
			getline (myfile,line);
			state->probNothing = atof(line.c_str());
			
			state->printCounts();
			state->printProbabilities();
			
			if(stateCounter == 0){
				gestures[gestureNum].AddState(state,true);
			}
			else {
				gestures[gestureNum].AddState(state,false);
			}
			
			stateCounter++;
		}
		
		getline (myfile,line);
	}
	myfile.close();
}

void CleanupExit()
{
	g_scriptNode.Release();
	g_DepthGenerator.Release();
	g_UserGenerator.Release();
	g_Player.Release();
	g_Context.Release();
	
	exit (1);
}


// Callback: New user was detected
void XN_CALLBACK_TYPE User_NewUser(xn::UserGenerator& generator, XnUserID nId, void* pCookie)
{
	printf("New User %d\n", nId);
	// New user found
	if (g_bNeedPose)
	{
		g_UserGenerator.GetPoseDetectionCap().StartPoseDetection(g_strPose, nId);
	}
	else
	{
		g_UserGenerator.GetSkeletonCap().RequestCalibration(nId, TRUE);
	}
}

// Callback: An existing user was lost
void XN_CALLBACK_TYPE User_LostUser(xn::UserGenerator& generator, XnUserID nId, void* pCookie)
{
	printf("Lost user %d\n", nId);
}

// Callback: Detected a pose
void XN_CALLBACK_TYPE UserPose_PoseDetected(xn::PoseDetectionCapability& capability, const XnChar* strPose, XnUserID nId, void* pCookie)
{
	printf("Pose %s detected for user %d\n", strPose, nId);
	g_UserGenerator.GetPoseDetectionCap().StopPoseDetection(nId);
	g_UserGenerator.GetSkeletonCap().RequestCalibration(nId, TRUE);
}

// Callback: Started calibration
void XN_CALLBACK_TYPE UserCalibration_CalibrationStart(xn::SkeletonCapability& capability, XnUserID nId, void* pCookie)
{
	printf("Calibration started for user %d\n", nId);
}

// Callback: Finished calibration
void XN_CALLBACK_TYPE UserCalibration_CalibrationEnd(xn::SkeletonCapability& capability, XnUserID nId, XnBool bSuccess, void* pCookie)
{
	if (bSuccess)
	{
		// Calibration succeeded
		printf("Calibration complete, start tracking user %d\n", nId);
		g_UserGenerator.GetSkeletonCap().StartTracking(nId);
	}
	else
	{
		// Calibration failed
		printf("Calibration failed for user %d\n", nId);
		if (g_bNeedPose)
		{
			g_UserGenerator.GetPoseDetectionCap().StartPoseDetection(g_strPose, nId);
		}
		else
		{
			g_UserGenerator.GetSkeletonCap().RequestCalibration(nId, TRUE);
		}
	}
}

void XN_CALLBACK_TYPE UserCalibration_CalibrationComplete(xn::SkeletonCapability& capability, XnUserID nId, XnCalibrationStatus eStatus, void* pCookie)
{
	if (eStatus == XN_CALIBRATION_STATUS_OK)
	{
		// Calibration succeeded
		printf("Calibration complete, start tracking user %d\n", nId);
		g_UserGenerator.GetSkeletonCap().StartTracking(nId);
	}
	else
	{
		// Calibration failed
		printf("Calibration failed for user %d\n", nId);
		if (g_bNeedPose)
		{
			g_UserGenerator.GetPoseDetectionCap().StartPoseDetection(g_strPose, nId);
		}
		else
		{
			g_UserGenerator.GetSkeletonCap().RequestCalibration(nId, TRUE);
		}
	}
}

#define XN_CALIBRATION_FILE_NAME "UserCalibration.bin"

// Save calibration to file
void SaveCalibration()
{
	XnUserID aUserIDs[20] = {0};
	XnUInt16 nUsers = 20;
	g_UserGenerator.GetUsers(aUserIDs, nUsers);
	for (int i = 0; i < nUsers; ++i)
	{
		// Find a user who is already calibrated
		if (g_UserGenerator.GetSkeletonCap().IsCalibrated(aUserIDs[i]))
		{
			// Save user's calibration to file
			g_UserGenerator.GetSkeletonCap().SaveCalibrationDataToFile(aUserIDs[i], XN_CALIBRATION_FILE_NAME);
			break;
		}
	}
}

// Load calibration from file
void LoadCalibration()
{
	XnUserID aUserIDs[20] = {0};
	XnUInt16 nUsers = 20;
	g_UserGenerator.GetUsers(aUserIDs, nUsers);
	for (int i = 0; i < nUsers; ++i)
	{
		// Find a user who isn't calibrated or currently in pose
		if (g_UserGenerator.GetSkeletonCap().IsCalibrated(aUserIDs[i])) continue;
		if (g_UserGenerator.GetSkeletonCap().IsCalibrating(aUserIDs[i])) continue;
		
		// Load user's calibration from file
		XnStatus rc = g_UserGenerator.GetSkeletonCap().LoadCalibrationDataFromFile(aUserIDs[i], XN_CALIBRATION_FILE_NAME);
		if (rc == XN_STATUS_OK)
		{
			// Make sure state is coherent
			g_UserGenerator.GetPoseDetectionCap().StopPoseDetection(aUserIDs[i]);
			g_UserGenerator.GetSkeletonCap().StartTracking(aUserIDs[i]);
		}
		break;
	}
}

int StartSong(){
	playing = true;
	pID = fork();
	
	//pID = getpid();
	//printf("PID: %d", pID);
	if (pID == 0)                // child
	{
		
		char *args[] = {"../../Config/GroupA", (char *) 0 };
		
		execv("../../Config/GroupA", args);		
    }
	
	pID2 = fork();
	
	//pID = getpid();
	//printf("PID: %d", pID);
	if (pID2 == 0)                // child
	{		
		char *args[] = {"../../Config/GroupB", (char *) 0 };
		
		execv("../../Config/GroupB", args);		
    }
	
	return 0;
}

void signatureGestureCurrentComparison(int i, bool dominantHand, bool l, bool r, bool u, bool d, bool f, bool b){
	
	if(gestures[i].SigStateMatch(l,r,u,d,f,b)){
		gestures[i].numDetectedStates++;
		/*if(dominantHand){
		 printf("Timer Dominant New Signature: %d	Num Detected States: %d\n", i, gestures[i].numDetectedStates);
		 printf("Timer Dominant New Signature Detected\n");
		 }
		 else{
		 printf("Timer Weak New Signature: %d	Num Detected States: %d\n", i, gestures[i].numDetectedStates);
		 printf("Timer Weak New Signature Detected\n");
		 }*/
		
		gestures[i].gestureStarted = TRUE;
		gestures[i].NextState();
		if (dominantHand) {
			if (gestures[i].numDetectedStates == 1) {
				gestures[i].started = previousSigTime;
			}
			if(gestures[i].numDetectedStates == gestures[i].numStates){
				gestures[i].ended = currentSigTime;
				gestures[i].previousTotalTime = gestures[i].totalTime;
				gestures[i].totalTime = difftime (gestures[i].ended,gestures[i].started) - 1.0;
				//printf ("\nGesture took %.2lf seconds to execute. Previous Time: %.2lf\n", gestures[i].totalTime,gestures[i].previousTotalTime);
			}
			gestures[i].dominantHandDetection = TRUE;
		}
		else {
			if (gestures[i].numDetectedStates == 1) {
				gestures[i].started = w_previousSigTime;
			}
			if(gestures[i].numDetectedStates == gestures[i].numStates){
				gestures[i].ended = w_currentSigTime;
				gestures[i].previousTotalTime = gestures[i].totalTime;
				gestures[i].totalTime = difftime (gestures[i].ended,gestures[i].started) - 1.0;
				//printf ("\nGesture took %.2lf seconds to execute. Previous Time: %.2lf\n", gestures[i].totalTime,gestures[i].previousTotalTime);
			}
			gestures[i].dominantHandDetection = FALSE;
		}
		
	}
	else {
		/*if(dominantHand){
		 printf("Timer Dominant New Signature Not Match Reset\n");
		 }
		 else{
		 printf("Timer Weak New Signature Not Match Reset\n");
		 }*/
		
		//Reset
		gestures[i].numDetectedStates = 0;
		gestures[i].ResetCurrentState();
		gestures[i].gestureStarted = FALSE;
		gestures[i].overallTrackedProb = 1;
		gestures[i].dominantHandDetection = FALSE;
		
	}
	
}

int signatureGestureCurrentComparisonRec(int i, bool dominantHand, bool l, bool r, bool u, bool d, bool f, bool b){
	
	if(recursiveGestureVector.at(i).SigStateMatch(l,r,u,d,f,b)){
		recursiveGestureVector.at(i).numDetectedStates++;
		/*if(dominantHand){
		 printf("Timer Rec Dominant New Signature: %d	Num Detected States: %d\n", i, recursiveGestureVector.at(i).numDetectedStates);
		 printf("Timer Rec Dominant New Signature Detected\n");
		 }
		 else{
		 printf("Timer Rec Weak New Signature: %d	Num Detected States: %d\n", i, recursiveGestureVector.at(i).numDetectedStates);
		 printf("Timer Rec Weak New Signature Detected\n");
		 }*/
		
		recursiveGestureVector.at(i).gestureStarted = TRUE;
		recursiveGestureVector.at(i).NextState();	
		if (dominantHand) {
			if (recursiveGestureVector.at(i).numDetectedStates == 1) {
				recursiveGestureVector.at(i).started = previousSigTime;
			}
			if(recursiveGestureVector.at(i).numDetectedStates == recursiveGestureVector.at(i).numStates){
				recursiveGestureVector.at(i).ended = currentSigTime;
				recursiveGestureVector.at(i).previousTotalTime = recursiveGestureVector.at(i).totalTime;
				recursiveGestureVector.at(i).totalTime = difftime (recursiveGestureVector.at(i).ended,recursiveGestureVector.at(i).started) - 1.0;
				//	printf ("\nGesture took %.2lf seconds to execute. Previous Time: %.2lf\n", recursiveGestureVector.at(i).totalTime,recursiveGestureVector.at(i).previousTotalTime);
			}
			recursiveGestureVector.at(i).dominantHandDetection = TRUE;
		}
		else {
			if (recursiveGestureVector.at(i).numDetectedStates == 1) {
				recursiveGestureVector.at(i).started = w_previousSigTime;
			}
			if(recursiveGestureVector.at(i).numDetectedStates == recursiveGestureVector.at(i).numStates){
				recursiveGestureVector.at(i).ended = w_currentSigTime;
				recursiveGestureVector.at(i).previousTotalTime = recursiveGestureVector.at(i).totalTime;
				recursiveGestureVector.at(i).totalTime = difftime (recursiveGestureVector.at(i).ended,recursiveGestureVector.at(i).started) - 1.0;
				//	printf ("\nGesture took %.2lf seconds to execute. Previous Time: %.2lf\n", recursiveGestureVector.at(i).totalTime,recursiveGestureVector.at(i).previousTotalTime);
			}
			recursiveGestureVector.at(i).dominantHandDetection = FALSE;
		}
	}
	else {
		/*if(dominantHand){
		 printf("Timer Rec Dominant New Signature Not Match Reset\n");
		 }
		 else{
		 printf("Timer Rec Weak New Signature Not Match Reset\n");
		 }*/
		//Remove from vector
		recursiveGestureVector.erase(recursiveGestureVector.begin()+i);
		if(i <= recursiveGestureVector.size() && i > 0) i--;
		vectorSize--;
		//printf("GestureNum After Removal: %d\n", i);
		//printf("Vector Size After Removal: %d\n", recursiveGestureVector.size());
	}				
	
	return i;
}

// this function is called each frame
void glutDisplay (void)
{
	
	glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	// Setup the OpenGL viewpoint
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	
	xn::SceneMetaData sceneMD;
	xn::DepthMetaData depthMD;
	g_DepthGenerator.GetMetaData(depthMD);
#ifndef USE_GLES
	glOrtho(0, depthMD.XRes(), depthMD.YRes(), 0, -1.0, 1.0);
#else
	glOrthof(0, depthMD.XRes(), depthMD.YRes(), 0, -1.0, 1.0);
#endif
	
	glDisable(GL_TEXTURE_2D);
	
	if (!g_bPause)
	{
		// Read next available data
		g_Context.WaitOneUpdateAll(g_DepthGenerator);
	}
	
	// Process the data
	g_DepthGenerator.GetMetaData(depthMD);
	g_UserGenerator.GetUserPixels(0, sceneMD);
	DrawDepthMap(depthMD, sceneMD);
	
	rotatingObject.CalculateOrientation(leftShoulder, rightShoulder, TRUE);
	if(rightHand.fConfidence >= 0.5){
		dominantHandTracker.DetectSwipe(rightHand.position, leftShoulder, rightShoulder, g_bRecordGesture, g_bDetectGesture, rotatingObject);
	}
	if(!dominantHandTracker.IsNewSignatureDetected()){
		if(leftHand.fConfidence >= 0.5){
			weakHandTracker.DetectSwipe(leftHand.position, leftShoulder, rightShoulder, g_bRecordGesture, g_bDetectGesture, rotatingObject);
		}
	}
	
	DrawOrientationBoxes(rotatingObject.FacingLeft(), rotatingObject.FacingRight(), rotatingObject.FacingForward());
	
	if((weakHandTracker.IsaSignatureDetected() || dominantHandTracker.IsaSignatureDetected())){
		timer = clock() + nTime * CPS;
	}
	
	//If there was any movement by either hand
	if((weakHandTracker.IsNewSignatureDetected() || dominantHandTracker.IsNewSignatureDetected())){
		aHandMoved = TRUE;
	}
	else{
		aHandMoved = FALSE;
	}
	
	/*
	 * Timer checking to see if the limit is breached
	 */
	if(!aHandMoved && clock() >= timer){
		
		timer = clock() + nTime * CPS;
		
		if(!g_bDetectGesture && !g_bRecordGesture){
			dominantHandTracker.SetStartComparePoint(rightHand);
			weakHandTracker.SetStartComparePoint(leftHand);
		}
		TimerRelease = TRUE;
		printGestureDetection = FALSE;

		
		//if in detect mode, wipe all progress
		//if(g_bDetectGesture){
		/* recursiveGestureVector.clear();
		// for(int j = 0; j < NUM_GESTURES; j++){
			
		//	 gestures[j].gestureStarted = FALSE;
		//	 gestures[j].numDetectedStates = 0;
		//	 gestures[j].ResetCurrentState();
		 //}
		}*/
	}
	
	/*
	 * Only print that a gesture is detected up until the next signature is detected
	 */
	if (printGestureDetection && aHandMoved) {
		printGestureDetection = FALSE;
	}
	
	
	//last go the vector detection a gesture so the gesture array was skipped, no putting it back in play
	if (gestureDetected) {
		gestureDetected = FALSE;
	}
	
	if(g_bDetectGesture && aHandMoved && !firstRound){
		firstRound = TRUE;
	}
	
	/*
	 * Hack Solution for a bug
	 */
	if (g_bDetectGesture && !firstRound) {
		for(int i = 0; i < NUM_GESTURES; i++){
			
			if(gestures[i].numStates > 0){
				
				if (gestures[i].numDetectedStates == gestures[i].numStates-1) {
					if (gestures[i].dominantHandDetection) {
						signatureGestureCurrentComparison(i, TRUE, dominantHandTracker.GetPreviousLeft(),dominantHandTracker.GetPreviousRight(),dominantHandTracker.GetPreviousUp(),dominantHandTracker.GetPreviousDown(),dominantHandTracker.GetPreviousForward(),dominantHandTracker.GetPreviousBackward());
					}
					else{
						signatureGestureCurrentComparison(i, FALSE, weakHandTracker.GetPreviousLeft(),weakHandTracker.GetPreviousRight(),weakHandTracker.GetPreviousUp(),weakHandTracker.GetPreviousDown(),weakHandTracker.GetPreviousForward(),weakHandTracker.GetPreviousBackward());
					}
				}
				
				if(gestures[i].numDetectedStates == gestures[i].numStates){
					
					gestures[i].finishedTrackedProb == gestures[i].overallTrackedProb;
					//printf ("\nPre PUSH Gesture took %.2lf seconds to execute. Previous Time: %.2lf\n", gestures[i].totalTime,gestures[i].previousTotalTime);
					gesturesDetected.push_back(gestures[i]);
					//printf ("\nPost PUSH Gesture took %.2lf seconds to execute. Previous Time: %.2lf\n", gesturesDetected.back().totalTime,gesturesDetected.back().previousTotalTime);
					gesturesDetected.back().totalTime = gestures[i].totalTime;
					gesturesDetected.back().previousTotalTime = gestures[i].previousTotalTime;
					//printf ("\nPost PUSH UPDATE Gesture took %.2lf seconds to execute. Previous Time: %.2lf\n", gesturesDetected.back().totalTime,gesturesDetected.back().previousTotalTime);
					gestureDetected = TRUE;
					//printf("Timer Release Gesture Detected: %d\n", gestures[i].stateMachineNumber);
					
					gestures[i].gestureStarted = FALSE;
					
					gestures[i].numDetectedStates = 0;
					gestures[i].overallTrackedProb = 1;
					gestures[i].ResetCurrentState();
				}
			}
		}
		
		vectorSize = recursiveGestureVector.size();
		
		for(int i = 0; i < vectorSize; i++){
			
			if(recursiveGestureVector.at(i).numDetectedStates == recursiveGestureVector.at(i).numStates-1)
				if (recursiveGestureVector.at(i).dominantHandDetection) {
					signatureGestureCurrentComparisonRec(i, TRUE, dominantHandTracker.GetPreviousLeft(),dominantHandTracker.GetPreviousRight(),dominantHandTracker.GetPreviousUp(),dominantHandTracker.GetPreviousDown(),dominantHandTracker.GetPreviousForward(),dominantHandTracker.GetPreviousBackward());
				}
				else{
					i = signatureGestureCurrentComparisonRec(i, FALSE, weakHandTracker.GetPreviousLeft(),weakHandTracker.GetPreviousRight(),weakHandTracker.GetPreviousUp(),weakHandTracker.GetPreviousDown(),weakHandTracker.GetPreviousForward(),weakHandTracker.GetPreviousBackward());
				}
			
			if(recursiveGestureVector.size() <= 0){
				break;
			}
			
			if(recursiveGestureVector.at(i).numDetectedStates == recursiveGestureVector.at(i).numStates){
				recursiveGestureVector.at(i).finishedTrackedProb == recursiveGestureVector.at(i).overallTrackedProb;
				gesturesDetected.push_back(recursiveGestureVector.at(i));
				
				gestureDetected = TRUE;
				//printf("Wait Timer Release Gesture Detected: %d\n", recursiveGestureVector.at(i).stateMachineNumber);
				
				recursiveGestureVector.at(i).numDetectedStates = 0;
				recursiveGestureVector.at(i).ResetCurrentState();
				recursiveGestureVector.at(i).gestureStarted = FALSE;
				recursiveGestureVector.at(i).overallTrackedProb = 1;
				recursiveGestureVector.at(i).dominantHandDetection = FALSE;
				recursiveGestureVector.at(i).tempFSM = NULL;
				
			}
		}
		if (gestureDetected == TRUE) {
			timer = clock() + nTime * CPS;
		}
	}
	
	
	
	if(firstRound){
		if(g_bDetectGesture && TimerRelease){
			
			printf("TimerRelease\n");
			
			for(int i = 0; i < NUM_GESTURES; i++){
				
				if(gestures[i].numStates > 0){
					
					if (gestures[i].numDetectedStates == 0) {
						signatureGestureCurrentComparison(i, TRUE, dominantHandTracker.GetPreviousLeft(),dominantHandTracker.GetPreviousRight(),dominantHandTracker.GetPreviousUp(),dominantHandTracker.GetPreviousDown(),dominantHandTracker.GetPreviousForward(),dominantHandTracker.GetPreviousBackward());
						if (!gestures[i].gestureStarted) {
							signatureGestureCurrentComparison(i, FALSE, weakHandTracker.GetPreviousLeft(),weakHandTracker.GetPreviousRight(),weakHandTracker.GetPreviousUp(),weakHandTracker.GetPreviousDown(),weakHandTracker.GetPreviousForward(),weakHandTracker.GetPreviousBackward());
						}
					}
					else{
						if (gestures[i].dominantHandDetection) {
							signatureGestureCurrentComparison(i, TRUE, dominantHandTracker.GetPreviousLeft(),dominantHandTracker.GetPreviousRight(),dominantHandTracker.GetPreviousUp(),dominantHandTracker.GetPreviousDown(),dominantHandTracker.GetPreviousForward(),dominantHandTracker.GetPreviousBackward());
						}
						else{
							signatureGestureCurrentComparison(i, FALSE, weakHandTracker.GetPreviousLeft(),weakHandTracker.GetPreviousRight(),weakHandTracker.GetPreviousUp(),weakHandTracker.GetPreviousDown(),weakHandTracker.GetPreviousForward(),weakHandTracker.GetPreviousBackward());
						}
					}
					
					
					if(gestures[i].numDetectedStates == gestures[i].numStates){
						
						gestures[i].finishedTrackedProb == gestures[i].overallTrackedProb;
						//printf ("\nPre PUSH Gesture took %.2lf seconds to execute. Previous Time: %.2lf\n", gestures[i].totalTime,gestures[i].previousTotalTime);
						gesturesDetected.push_back(gestures[i]);
						//printf ("\nPost PUSH Gesture took %.2lf seconds to execute. Previous Time: %.2lf\n", gesturesDetected.back().totalTime,gesturesDetected.back().previousTotalTime);
						gesturesDetected.back().totalTime = gestures[i].totalTime;
						gesturesDetected.back().previousTotalTime = gestures[i].previousTotalTime;
						//printf ("\nPost PUSH UPDATE Gesture took %.2lf seconds to execute. Previous Time: %.2lf\n", gesturesDetected.back().totalTime,gesturesDetected.back().previousTotalTime);
						gestureDetected = TRUE;
						//printf("Timer Release Gesture Detected: %d\n", gestures[i].stateMachineNumber);
						
						gestures[i].gestureStarted = FALSE;
						
						gestures[i].numDetectedStates = 0;
						gestures[i].overallTrackedProb = 1;
						gestures[i].ResetCurrentState();
					}
				}
			}
			
			vectorSize = recursiveGestureVector.size();
			
			for(int i = 0; i < vectorSize; i++){
				
				//printf("Vector Size = %d,%d				Within Loop\n", vectorSize, i);
				
				if (recursiveGestureVector.at(i).dominantHandDetection) {
					signatureGestureCurrentComparisonRec(i, TRUE, dominantHandTracker.GetPreviousLeft(),dominantHandTracker.GetPreviousRight(),dominantHandTracker.GetPreviousUp(),dominantHandTracker.GetPreviousDown(),dominantHandTracker.GetPreviousForward(),dominantHandTracker.GetPreviousBackward());
				}
				else{
					i = signatureGestureCurrentComparisonRec(i, FALSE, weakHandTracker.GetPreviousLeft(),weakHandTracker.GetPreviousRight(),weakHandTracker.GetPreviousUp(),weakHandTracker.GetPreviousDown(),weakHandTracker.GetPreviousForward(),weakHandTracker.GetPreviousBackward());
				}
				
				if(recursiveGestureVector.size() <= 0){
					break;
				}
				
				if(recursiveGestureVector.at(i).numDetectedStates == recursiveGestureVector.at(i).numStates){
					recursiveGestureVector.at(i).finishedTrackedProb == recursiveGestureVector.at(i).overallTrackedProb;
					gesturesDetected.push_back(recursiveGestureVector.at(i));
					
					gestureDetected = TRUE;
					//printf("Rec Timer Release Gesture Detected: %d\n", recursiveGestureVector.at(i).stateMachineNumber);
					
					recursiveGestureVector.at(i).numDetectedStates = 0;
					recursiveGestureVector.at(i).ResetCurrentState();
					recursiveGestureVector.at(i).gestureStarted = FALSE;
					recursiveGestureVector.at(i).overallTrackedProb = 1;
					recursiveGestureVector.at(i).dominantHandDetection = FALSE;
					recursiveGestureVector.at(i).tempFSM = NULL;
					
				}
			}
			
			startSigTime = time(NULL);
			previousSigTime = time(NULL);
			currentSigTime = time(NULL);
			w_startSigTime = time(NULL);
			w_previousSigTime = time(NULL);
			w_currentSigTime = time(NULL);
			
			dominantHandTracker.SetStartComparePoint(rightHand);
			weakHandTracker.SetStartComparePoint(leftHand);
		}
		
		if((!gestureDetected || !firstRound) && !gestureDetected){
			if(g_bDetectGesture && aHandMoved && recursiveGestureVector.size() > 0){
				//printf("Vector Size: %d\n", recursiveGestureVector.size());
				
				vectorSize = recursiveGestureVector.size();
				
				for(int i = 0; i < vectorSize; i++){
					//printf("Checking Vector Gesture Number: %d	", i);
					
					if (recursiveGestureVector.at(i).numDetectedStates < recursiveGestureVector.at(i).numStates && recursiveGestureVector.at(i).numStates > 0){
						//printf("NumStates Detected: %d\n", recursiveGestureVector.at(i).numDetectedStates);
						
						if(dominantHandTracker.IsNewSignatureDetected() && recursiveGestureVector.at(i).dominantHandDetection){
							//printf("Rec New Case\n");
							
							//cout << "Rec Current state: " << recursiveGestureVector.at(i).currentState->getStateName() << " \n";
							
							if(recursiveGestureVector.at(i).gestureStarted && recursiveGestureVector.at(i).SigStartStateMatch(dominantHandTracker.GetLastLeft(),dominantHandTracker.GetLastRight(),dominantHandTracker.GetLastUp(),dominantHandTracker.GetLastDown(),dominantHandTracker.GetLastForward(),dominantHandTracker.GetLastBackward())){
								//	printf("Rec Recursive\n");
								recursiveGestureVector.at(i).tempFSM = new StateMachine(recursiveGestureVector.at(i));
								recursiveGestureVector.at(i).tempFSM->dominantHandDetection = TRUE;
								 recursiveGestureVector.at(i).tempFSM->gestureStarted = TRUE;
								 recursiveGestureVector.at(i).tempFSM->started = startSigTime;
								 recursiveGestureVector.at(i).tempFSM->NextState();
								 recursiveGestureVector.at(i).tempFSM->SetOverallTrackedProb(dominantHandTracker.GetLastLeft(),dominantHandTracker.GetLastRight(),dominantHandTracker.GetLastUp(),dominantHandTracker.GetLastDown(),dominantHandTracker.GetLastForward(),dominantHandTracker.GetLastBackward());
								//recursiveGestureVector.push_back(*tempFSM);
								/*recursiveGestureVector.push_back(*recursiveGestureVector.at(i).tempFSM);
								recursiveGestureVector.back().dominantHandDetection = TRUE;
								recursiveGestureVector.back().gestureStarted = TRUE;
								recursiveGestureVector.back().started = startSigTime;
								recursiveGestureVector.back().SetOverallTrackedProb(dominantHandTracker.GetLastLeft(),dominantHandTracker.GetLastRight(),dominantHandTracker.GetLastUp(),dominantHandTracker.GetLastDown(),dominantHandTracker.GetLastForward(),dominantHandTracker.GetLastBackward());
								recursiveGestureVector.back().NextState();*/
								//	cout << "REC Recursive State state: " << recursiveGestureVector.back().startState->getStateName() << " \n";
								//	cout << "REC Recursive Current state: " << recursiveGestureVector.back().currentState->getStateName() << " \n";
							}
							
							if(recursiveGestureVector.at(i).SigStateMatch(dominantHandTracker.GetLastLeft(),dominantHandTracker.GetLastRight(),dominantHandTracker.GetLastUp(),dominantHandTracker.GetLastDown(),dominantHandTracker.GetLastForward(),dominantHandTracker.GetLastBackward())){
								recursiveGestureVector.at(i).numDetectedStates++;
								if(recursiveGestureVector.at(i).numDetectedStates == recursiveGestureVector.at(i).numStates){
									recursiveGestureVector.at(i).ended = previousSigTime;
									recursiveGestureVector.at(i).previousTotalTime = recursiveGestureVector.at(i).totalTime;
									recursiveGestureVector.at(i).totalTime = difftime (recursiveGestureVector.at(i).ended,recursiveGestureVector.at(i).started);
									//		printf ("\nGesture took %.2lf seconds to execute. Previous Time: %.2lf\n", recursiveGestureVector.at(i).totalTime,recursiveGestureVector.at(i).previousTotalTime);
								}
								//printf("Rec Dominant New Signature: %d	Num Detected States: %d\n", i, recursiveGestureVector.at(i).numDetectedStates);
								//	printf("Rec Dominant New Signature Detected\n");
								recursiveGestureVector.at(i).gestureStarted = TRUE;
								recursiveGestureVector.at(i).NextState();	
								recursiveGestureVector.at(i).dominantHandDetection = TRUE;
							}
							else {
								//	printf("Rec Dominant New Signature Not Match Reset\n");
								
								//Remove from vector
								recursiveGestureVector.erase(recursiveGestureVector.begin()+i);
								if(i <= recursiveGestureVector.size() && i > 0) i--;
								vectorSize--;
								//printf("GestureNum After Removal: %d\n", i);
								//printf("Vector Size After Removal: %d\n", recursiveGestureVector.size());
							}
							
							if(recursiveGestureVector.size() <= 0){
								break;
							}
						}
						else if(weakHandTracker.IsNewSignatureDetected() && !recursiveGestureVector.at(i).dominantHandDetection){
							//printf("Rec Weak New Case\n");
							
							if(recursiveGestureVector.at(i).gestureStarted && recursiveGestureVector.at(i).SigStartStateMatch(weakHandTracker.GetLastLeft(),weakHandTracker.GetLastRight(),weakHandTracker.GetLastUp(),weakHandTracker.GetLastDown(),weakHandTracker.GetLastForward(),weakHandTracker.GetLastBackward())){
								//	printf("Rec Weak Recursive\n");
								recursiveGestureVector.at(i).tempFSM = new StateMachine(recursiveGestureVector.at(i));
								recursiveGestureVector.at(i).tempFSM->dominantHandDetection = FALSE;
								 recursiveGestureVector.at(i).tempFSM->gestureStarted = TRUE;
								 recursiveGestureVector.at(i).tempFSM->started = w_startSigTime;
								 recursiveGestureVector.at(i).tempFSM->NextState();
								 recursiveGestureVector.at(i).tempFSM->SetOverallTrackedProb(weakHandTracker.GetLastLeft(),weakHandTracker.GetLastRight(),weakHandTracker.GetLastUp(),weakHandTracker.GetLastDown(),weakHandTracker.GetLastForward(),weakHandTracker.GetLastBackward());
								//recursiveGestureVector.push_back(*tempFSM);
								/*recursiveGestureVector.push_back(*recursiveGestureVector.at(i).tempFSM);
								recursiveGestureVector.back().dominantHandDetection = FALSE;
								recursiveGestureVector.back().gestureStarted = TRUE;
								recursiveGestureVector.back().started = startSigTime;
								recursiveGestureVector.back().SetOverallTrackedProb(weakHandTracker.GetLastLeft(),weakHandTracker.GetLastRight(),weakHandTracker.GetLastUp(),weakHandTracker.GetLastDown(),weakHandTracker.GetLastForward(),weakHandTracker.GetLastBackward());
								recursiveGestureVector.back().NextState();*/
								//	cout << "Weak REC Recursive State state: " << recursiveGestureVector.back().startState->getStateName() << " \n";
								//	cout << "Weak REC Recursive Current state: " << recursiveGestureVector.back().currentState->getStateName() << " \n";
								
							}
							
							if(recursiveGestureVector.at(i).SigStateMatch(weakHandTracker.GetLastLeft(),weakHandTracker.GetLastRight(),weakHandTracker.GetLastUp(),weakHandTracker.GetLastDown(),weakHandTracker.GetLastForward(),weakHandTracker.GetLastBackward())){
								recursiveGestureVector.at(i).numDetectedStates++;
								if(recursiveGestureVector.at(i).numDetectedStates == recursiveGestureVector.at(i).numStates){
									recursiveGestureVector.at(i).ended = w_previousSigTime;
									recursiveGestureVector.at(i).previousTotalTime = recursiveGestureVector.at(i).totalTime;
									recursiveGestureVector.at(i).totalTime = difftime (recursiveGestureVector.at(i).ended,recursiveGestureVector.at(i).started);
									//printf ("\nGesture took %.2lf seconds to execute. Previous Time: %.2lf\n", recursiveGestureVector.at(i).totalTime,recursiveGestureVector.at(i).previousTotalTime);
								}
								//	printf("Rec Weak New Signature: %d	Num Detected States: %d\n", i, recursiveGestureVector.at(i).numDetectedStates);
								//		printf("Rec Weak New Signature Detected\n");
								recursiveGestureVector.at(i).gestureStarted = TRUE;
								recursiveGestureVector.at(i).NextState();	
								recursiveGestureVector.at(i).dominantHandDetection = FALSE;
							}
							else {
								//	printf("Rec Weak New Signature Not Match Reset\n");
								
								//Remove from vector
								recursiveGestureVector.erase(recursiveGestureVector.begin()+i);
								if(i <= recursiveGestureVector.size() && i > 0) i--;
								vectorSize--;
								//printf("GestureNum After Removal: %d\n", i);
								//printf("Vector Size After Removal: %d\n", recursiveGestureVector.size());
							}
							if(recursiveGestureVector.size() <= 0){
								break;
							}
						}
						
						if (recursiveGestureVector.at(i).numDetectedStates == recursiveGestureVector.at(i).numStates) {
							
							recursiveGestureVector.at(i).finishedTrackedProb == recursiveGestureVector.at(i).overallTrackedProb;
							gesturesDetected.push_back(recursiveGestureVector.at(i));
							gestureDetected = TRUE;
							
							if(recursiveGestureVector.at(i).dominantHandDetection){
								//	printf("Rec Dominant Hand ");
							}
							else {
								//	printf("Rec Weak Hand ");
							}
							printf("Gesture Detected: %d\n", recursiveGestureVector.at(i).stateMachineNumber);
							
							recursiveGestureVector.at(i).numDetectedStates = 0;
							recursiveGestureVector.at(i).ResetCurrentState();
							recursiveGestureVector.at(i).gestureStarted = FALSE;
							recursiveGestureVector.at(i).overallTrackedProb = 1;
							recursiveGestureVector.at(i).dominantHandDetection = FALSE;
							recursiveGestureVector.at(i).tempFSM = NULL;
						}
						
					}
				}
			}
			
			if(g_bDetectGesture && aHandMoved){
				
				for(int i = 0; i < NUM_GESTURES; i++){
					//printf("Checking Gesture Number: %d\n", i);
					
					if (gestures[i].numDetectedStates < gestures[i].numStates && gestures[i].numStates > 0){
						//printf("NumStates Detected: %d\n", gestures[i].numDetectedStates);
						
						if(dominantHandTracker.IsNewSignatureDetected() && (gestures[i].dominantHandDetection || gestures[i].numDetectedStates == 0)){
							//printf("Reg New Case\n");
							
							if(gestures[i].gestureStarted && gestures[i].SigStartStateMatch(dominantHandTracker.GetLastLeft(),dominantHandTracker.GetLastRight(),dominantHandTracker.GetLastUp(),dominantHandTracker.GetLastDown(),dominantHandTracker.GetLastForward(),dominantHandTracker.GetLastBackward())){
								//printf("Reg Recursive\n");
								gestures[i].tempFSM = new StateMachine(gestures[i]);
								gestures[i].tempFSM->dominantHandDetection = TRUE;
								 gestures[i].tempFSM->gestureStarted = TRUE;
								 gestures[i].tempFSM->started = startSigTime;
								 gestures[i].tempFSM->NextState();
								 gestures[i].tempFSM->SetOverallTrackedProb(dominantHandTracker.GetLastLeft(),dominantHandTracker.GetLastRight(),dominantHandTracker.GetLastUp(),dominantHandTracker.GetLastDown(),dominantHandTracker.GetLastForward(),dominantHandTracker.GetLastBackward());
								
								/*recursiveGestureVector.push_back(*gestures[i].tempFSM);
								recursiveGestureVector.back().dominantHandDetection = TRUE;
								recursiveGestureVector.back().gestureStarted = TRUE;
								recursiveGestureVector.back().started = startSigTime;
								recursiveGestureVector.back().SetOverallTrackedProb(dominantHandTracker.GetLastLeft(),dominantHandTracker.GetLastRight(),dominantHandTracker.GetLastUp(),dominantHandTracker.GetLastDown(),dominantHandTracker.GetLastForward(),dominantHandTracker.GetLastBackward());
								recursiveGestureVector.back().NextState();*/
								//cout << "Reg Recursive State state: " << recursiveGestureVector.back().startState->getStateName() << " \n";
								//cout << "Reg Recursive Current state: " << recursiveGestureVector.back().currentState->getStateName() << " \n";
							}
							
							if(gestures[i].SigStateMatch(dominantHandTracker.GetLastLeft(),dominantHandTracker.GetLastRight(),dominantHandTracker.GetLastUp(),dominantHandTracker.GetLastDown(),dominantHandTracker.GetLastForward(),dominantHandTracker.GetLastBackward())){
								gestures[i].numDetectedStates++;
								if (gestures[i].numDetectedStates == 1) {
									gestures[i].started = startSigTime;
								}
								if(gestures[i].numDetectedStates == gestures[i].numStates){
									gestures[i].ended = previousSigTime;
									gestures[i].previousTotalTime = gestures[i].totalTime;
									gestures[i].totalTime = difftime (gestures[i].ended,gestures[i].started);
									//	printf ("\nGesture took %.2lf seconds to execute. Previous Time: %.2lf\n", gestures[i].totalTime,gestures[i].previousTotalTime);
								}
								//printf("Dominant New Signature: %d	Num Detected States: %d\n", i, gestures[i].numDetectedStates);
								//printf("Dominant New Signature Detected\n");
								gestures[i].gestureStarted = TRUE;
								gestures[i].NextState();	
								gestures[i].dominantHandDetection = TRUE;
							}
							else {
								//printf("Dominant New Signature Not Match Reset\n");
								
								//Reset
								gestures[i].numDetectedStates = 0;
								gestures[i].ResetCurrentState();
								gestures[i].gestureStarted = FALSE;
								gestures[i].overallTrackedProb = 1;
								gestures[i].dominantHandDetection = FALSE;
								
							}
						}
						else if(weakHandTracker.IsNewSignatureDetected() && (!gestures[i].dominantHandDetection || gestures[i].numDetectedStates == 0)){
							//printf("Weak Reg New Case\n");
							
							if(gestures[i].gestureStarted && gestures[i].SigStartStateMatch(weakHandTracker.GetLastLeft(),weakHandTracker.GetLastRight(),weakHandTracker.GetLastUp(),weakHandTracker.GetLastDown(),weakHandTracker.GetLastForward(),weakHandTracker.GetLastBackward())){
								//printf("Weak Reg Recursive\n");
								gestures[i].tempFSM = new StateMachine(gestures[i]);
								gestures[i].tempFSM->dominantHandDetection = TRUE;
								 gestures[i].tempFSM->gestureStarted = TRUE;
								 gestures[i].tempFSM->started = w_startSigTime;
								 gestures[i].tempFSM->NextState();
								 gestures[i].tempFSM->SetOverallTrackedProb(weakHandTracker.GetLastLeft(),weakHandTracker.GetLastRight(),weakHandTracker.GetLastUp(),weakHandTracker.GetLastDown(),weakHandTracker.GetLastForward(),weakHandTracker.GetLastBackward());
								//recursiveGestureVector.push_back(*tempFSM);
								/*recursiveGestureVector.push_back(*gestures[i].tempFSM);
								recursiveGestureVector.back().dominantHandDetection = FALSE;
								recursiveGestureVector.back().gestureStarted = TRUE;
								recursiveGestureVector.back().started = startSigTime;
								recursiveGestureVector.back().SetOverallTrackedProb(weakHandTracker.GetLastLeft(),weakHandTracker.GetLastRight(),weakHandTracker.GetLastUp(),weakHandTracker.GetLastDown(),weakHandTracker.GetLastForward(),weakHandTracker.GetLastBackward());
								recursiveGestureVector.back().NextState();*/
								//cout << "Weak Recursive State state: " << recursiveGestureVector.back().startState->getStateName() << " \n";
								//cout << "Weak Recursive Current state: " << recursiveGestureVector.back().currentState->getStateName() << " \n";
								
							}
							
							if(gestures[i].SigStateMatch(weakHandTracker.GetLastLeft(),weakHandTracker.GetLastRight(),weakHandTracker.GetLastUp(),weakHandTracker.GetLastDown(),weakHandTracker.GetLastForward(),weakHandTracker.GetLastBackward())){
								gestures[i].numDetectedStates++;
								if (gestures[i].numDetectedStates == 1) {
									gestures[i].started = w_startSigTime;
								}
								if(gestures[i].numDetectedStates == gestures[i].numStates){
									gestures[i].ended = w_previousSigTime;
									gestures[i].previousTotalTime = gestures[i].totalTime;
									gestures[i].totalTime = difftime (gestures[i].ended,gestures[i].started);
									//	printf ("\nGesture took %.2lf seconds to execute. Previous Time: %.2lf\n", gestures[i].totalTime,gestures[i].previousTotalTime);
								}
								//printf("Weak Right New Signature: %d	Num Detected States: %d\n", i, gestures[i].numDetectedStates);
								//printf("Weak Right New Signature Detected\n");
								gestures[i].gestureStarted = TRUE;
								gestures[i].NextState();
								gestures[i].dominantHandDetection = TRUE;
							}
							else {
								//printf("Weak Right New Signature Not Match Reset\n");
								
								//Reset
								gestures[i].numDetectedStates = 0;
								gestures[i].ResetCurrentState();
								gestures[i].gestureStarted = FALSE;
								gestures[i].overallTrackedProb = 1;
								gestures[i].dominantHandDetection = FALSE;
								
							}
							
						}
						
						if (gestures[i].numDetectedStates == gestures[i].numStates) {
							
							gestures[i].finishedTrackedProb == gestures[i].overallTrackedProb;
							gesturesDetected.push_back(gestures[i]);
							gesturesDetected.back().totalTime = gestures[i].totalTime;
							gesturesDetected.back().previousTotalTime = gestures[i].previousTotalTime;
							//printf ("\nPost PUSH UPDATE Gesture took %.2lf seconds to execute. Previous Time: %.2lf\n", gesturesDetected.back().totalTime,gesturesDetected.back().previousTotalTime);
							gestureDetected = TRUE;
							
							/*if(gestures[i].dominantHandDetection){
							 printf("Dominant Hand ");
							 }
							 else {
							 printf("Weak Hand ");
							 }*/
							printf("Gesture Detected: %d\n", gestures[i].stateMachineNumber);
							
							gestures[i].numDetectedStates = 0;
							gestures[i].ResetCurrentState();
							gestures[i].gestureStarted = FALSE;
							gestures[i].overallTrackedProb = 1;
							gestures[i].dominantHandDetection = FALSE;
							
						}
					}
				}
			}
		}
		
	}
	if (g_bDetectGesture) {
		dominantHandTracker.SetNewSignatureDetected(FALSE);
		dominantHandTracker.SetaSignatureDetected(FALSE);
		weakHandTracker.SetNewSignatureDetected(FALSE);
		weakHandTracker.SetaSignatureDetected(FALSE);
		
		if (gestureDetected && gesturesDetected.size() > 0) {
			
			int maxProb = 0;
			int pLocation = 0;
			int maxStates = 0;
			int sLocation = 0;
			
			for (int i = 0; i < gesturesDetected.size(); i++) {
				if (gesturesDetected.at(i).finishedTrackedProb > maxProb) {
					maxProb = gesturesDetected.at(i).finishedTrackedProb;
					pLocation = i;
				}
				/*if (gesturesDetected.at(i).numStates > maxStates) {
				 maxProb = gesturesDetected.at(i).numStates;
				 sLocation = i;
				 }*/
			}
			
			printf("pLocation: %d\n", pLocation);
			printf("maxProb: %d\n", maxProb);
			printf("Detected Recorded Gesture Number: %d\n", gesturesDetected.at(pLocation).stateMachineNumber);
			printGestureDetection = TRUE;
			
			if(playing){
				//Make Global
				ofstream groupAFile;
				ofstream groupBFile;
				if(rotatingObject.FacingLeft() || rotatingObject.FacingForward()){
					groupAFile.open ("../../Config/groupa.txt",ios::app);
				}
				if(rotatingObject.FacingRight() || rotatingObject.FacingForward()){
					groupBFile.open ("../../Config/groupb.txt",ios::app);
				}
				
				if (gesturesDetected.at(pLocation).stateMachineNumber == 0) {
					if((rotatingObject.FacingLeft() || rotatingObject.FacingForward()) && groupAStarted){
						groupAFile << "iVol\n";
					}
					if((rotatingObject.FacingRight() || rotatingObject.FacingForward()) && groupBStarted){
						groupBFile << "iVol\n";
					}
				}
				if (gesturesDetected.at(pLocation).stateMachineNumber == 1) {
					if((rotatingObject.FacingLeft() || rotatingObject.FacingForward()) && groupAStarted){
						groupAFile << "dVol\n";
					}
					if((rotatingObject.FacingRight() || rotatingObject.FacingForward()) && groupBStarted){
						groupBFile << "dVol\n";
					}
				}
				if (gesturesDetected.at(pLocation).stateMachineNumber == 2) {
					if((rotatingObject.FacingLeft() || rotatingObject.FacingForward()) && groupAStarted){
						groupAFile << "iTempo\n";
					}
					if((rotatingObject.FacingRight() || rotatingObject.FacingForward()) && groupBStarted){
						groupBFile << "iTempo\n";
					}
				}
				if (gesturesDetected.at(pLocation).stateMachineNumber == 3) {
					if((rotatingObject.FacingLeft() || rotatingObject.FacingForward()) && groupAStarted){
						groupAFile << "dTempo\n";
					}
					if((rotatingObject.FacingRight() || rotatingObject.FacingForward()) && groupBStarted){
						groupBFile << "dTempo\n";
					}
				}
				if (gesturesDetected.at(pLocation).stateMachineNumber == 4) {
					if((rotatingObject.FacingLeft() || rotatingObject.FacingForward()) && groupAStarted){
						groupAFile << "iPitch\n";
					}
					if((rotatingObject.FacingRight() || rotatingObject.FacingForward()) && groupBStarted){
						groupBFile << "iPitch\n";
					}
				}
				if (gesturesDetected.at(pLocation).stateMachineNumber == 5) {
					if((rotatingObject.FacingLeft() || rotatingObject.FacingForward()) && groupAStarted){
						groupAFile << "dPitch\n";
					}
					if((rotatingObject.FacingRight() || rotatingObject.FacingForward()) && groupBStarted){
						groupBFile << "dPitch\n";
					}
				}
				if (gesturesDetected.at(pLocation).stateMachineNumber == 6) {
					if((rotatingObject.FacingLeft() || rotatingObject.FacingForward()) && groupAStarted){
						groupAFile << "lowpass\n";
					}
					if((rotatingObject.FacingRight() || rotatingObject.FacingForward()) && groupBStarted){
						groupBFile << "lowpass\n";
					}
				}
				if (gesturesDetected.at(pLocation).stateMachineNumber == 7) {
					if((rotatingObject.FacingLeft() || rotatingObject.FacingForward()) && groupAStarted){
						groupAFile << "highpass\n";
					}
					if((rotatingObject.FacingRight() || rotatingObject.FacingForward()) && groupBStarted){
						groupBFile << "highpass\n";
						
					}
				}
				if (gesturesDetected.at(pLocation).stateMachineNumber == 8) {
					if((rotatingObject.FacingLeft() || rotatingObject.FacingForward()) && groupAStarted){
						groupAFile << "echo\n";
					}
					if((rotatingObject.FacingRight() || rotatingObject.FacingForward()) && groupBStarted){
						groupBFile << "echo\n";
					}
				}
				if (gesturesDetected.at(pLocation).stateMachineNumber == 9) {
					if((rotatingObject.FacingLeft() || rotatingObject.FacingForward()) && groupAStarted){
						groupAFile << "flange\n";
					}
					if((rotatingObject.FacingRight() || rotatingObject.FacingForward()) && groupBStarted){
						groupBFile << "flange\n";
					}
				}
				if (gesturesDetected.at(pLocation).stateMachineNumber == 10) {
					if((rotatingObject.FacingLeft() || rotatingObject.FacingForward()) && groupAStarted){
						groupAFile << "distortion\n";
					}
					if((rotatingObject.FacingRight() || rotatingObject.FacingForward()) && groupBStarted){
						groupBFile << "distortion\n";
					}
				}
				if (gesturesDetected.at(pLocation).stateMachineNumber == 11) {
					if((rotatingObject.FacingLeft() || rotatingObject.FacingForward()) && groupAStarted){
						groupAFile << "chorus\n";
					}
					if((rotatingObject.FacingRight() || rotatingObject.FacingForward()) && groupBStarted){
						groupBFile << "chorus\n";
					}
				}
				if (gesturesDetected.at(pLocation).stateMachineNumber == 12) {
					if((rotatingObject.FacingLeft() || rotatingObject.FacingForward()) && groupAStarted){
						groupAFile << "parameq\n";
					}
					if((rotatingObject.FacingRight() || rotatingObject.FacingForward()) && groupBStarted){
						groupBFile << "parameq\n";
					}
				}
				if (gesturesDetected.at(pLocation).stateMachineNumber == 13) {
					if((rotatingObject.FacingLeft() || rotatingObject.FacingForward()) && groupAStarted){
						groupAFile << "quit\n";
					}
					if((rotatingObject.FacingRight() || rotatingObject.FacingForward()) && groupBStarted){
						groupBFile << "quit\n";
					}
				}
				groupAFile.close();
				groupBFile.close();
				
				
			}
			
			for(int j = 0; j < NUM_GESTURES; j++){
				gestures[j].gestureStarted = FALSE;
				gestures[j].detectRecNewSigWaitCont = FALSE;
				gestures[j].detectRecWaitNewSig = FALSE;
				gestures[j].detectNewSigWaitCont = FALSE;
				gestures[j].detectWaitNewSig = FALSE;
				gestures[j].detectRecContSig = FALSE;
				gestures[j].detectRecThreeSignatures = FALSE;
				gestures[j].detectRecWaitContSig = FALSE;
				gestures[j].detectContSig = FALSE;
				gestures[j].detectThreeSignatures = FALSE;
				gestures[j].detectWaitContSig = FALSE;
				gestures[j].numDetectedStates = 0;
				gestures[j].overallTrackedProb = 1;
				gestures[j].ResetCurrentState();
			}
			recursiveGestureVector.clear();
			
			gesturesDetected.clear();
			
			gestureDetected = FALSE;
			dtimer = clock() + detectTime * CLOCKS_PER_SEC;
			timer = clock() + nTime * CPS;
		}
	}
	
	if (!groupAStarted && playing) {
		groupAStarted = startGesture(1);
		if (groupAStarted) {
			ofstream groupAFile;
			groupAFile.open ("../../Config/groupa.txt",ios::app);
			groupAFile << "start\n";
			groupAFile.close();
			for(int j = 0; j < NUM_GESTURES; j++){
				gestures[j].gestureStarted = FALSE;
				gestures[j].detectRecNewSigWaitCont = FALSE;
				gestures[j].detectRecWaitNewSig = FALSE;
				gestures[j].detectNewSigWaitCont = FALSE;
				gestures[j].detectWaitNewSig = FALSE;
				gestures[j].detectRecContSig = FALSE;
				gestures[j].detectRecThreeSignatures = FALSE;
				gestures[j].detectRecWaitContSig = FALSE;
				gestures[j].detectContSig = FALSE;
				gestures[j].detectThreeSignatures = FALSE;
				gestures[j].detectWaitContSig = FALSE;
				gestures[j].numDetectedStates = 0;
				gestures[j].overallTrackedProb = 1;
				gestures[j].ResetCurrentState();
			}
			recursiveGestureVector.clear();
			
			gesturesDetected.clear();
			
			gestureDetected = FALSE;
			dtimer = clock() + detectTime * CLOCKS_PER_SEC;
			timer = clock() + nTime * CPS;
			dominantHandTracker.SetStartComparePoint(rightHand);
			weakHandTracker.SetStartComparePoint(leftHand);
		}
	}
	if (!groupBStarted && playing) {
		groupBStarted = startGesture(2);
		if (groupBStarted) {
			ofstream groupBFile;
			groupBFile.open ("../../Config/groupb.txt",ios::app);
			groupBFile << "start\n";
			groupBFile.close();
			gesturesDetected.clear();
			
			for(int j = 0; j < NUM_GESTURES; j++){
				gestures[j].gestureStarted = FALSE;
				gestures[j].detectRecNewSigWaitCont = FALSE;
				gestures[j].detectRecWaitNewSig = FALSE;
				gestures[j].detectNewSigWaitCont = FALSE;
				gestures[j].detectWaitNewSig = FALSE;
				gestures[j].detectRecContSig = FALSE;
				gestures[j].detectRecThreeSignatures = FALSE;
				gestures[j].detectRecWaitContSig = FALSE;
				gestures[j].detectContSig = FALSE;
				gestures[j].detectThreeSignatures = FALSE;
				gestures[j].detectWaitContSig = FALSE;
				gestures[j].numDetectedStates = 0;
				gestures[j].overallTrackedProb = 1;
				gestures[j].ResetCurrentState();
			}
			recursiveGestureVector.clear();
			
			gesturesDetected.clear();
			
			gestureDetected = FALSE;
			dtimer = clock() + detectTime * CLOCKS_PER_SEC;
			timer = clock() + nTime * CPS;
			dominantHandTracker.SetStartComparePoint(rightHand);
			weakHandTracker.SetStartComparePoint(leftHand);
		}
	}
	
	
	
	/*
	 * Signature Recording
	 */
	
	if(g_bRecordGesture || initialRecorded) xnOSMemSet(signatureName, 0, sizeof(signatureName));
	
	
	if(g_bRecordGesture && gestureNum == -1 && !gestureSelectedPrinted){
		printGestureSelection = TRUE;
		printf("Select: Inc Vol (1); Dec Vol (2); Inc Tempo (3); Dec Tempo (4); Inc Pitch (5); Dec Pitch(6);\n");
		gestureSelectedPrinted = TRUE;
		dominantHandTracker.SetNewSignatureDetected(FALSE);
		weakHandTracker.SetNewSignatureDetected(FALSE);
		
		dominantHandTracker.SetaSignatureDetected(FALSE);
		weakHandTracker.SetaSignatureDetected(FALSE);
		
	}
	/*
	 * End recording if state machine is full and new signature detected
	 */
	if (g_bRecordGesture && gestureNum > -1 && gestures[gestureNum].numStates == NUM_STATES_PER_GESTURE && aHandMoved) {
		
		if(dominantHandTracker.IsNewSignatureDetected() && gestures[gestureNum].dominantHandRecording){
			dominantHandTracker.GetLastFullSigPerformed(signatureName);
			printf("Signature Array: \n");
			for (int i = 0; i < strlen(signatureName); i++) {
				printf("%c", signatureName[i]);
			}
			printf("\n");
			gestures[gestureNum].stateBank.at(gestures[gestureNum].numStates-1)->setStateName(signatureName);
			gestures[gestureNum].stateBank.at(gestures[gestureNum].numStates-1)->setStateBools(dominantHandTracker.GetLastLeft(),dominantHandTracker.GetLastRight(),dominantHandTracker.GetLastUp(),dominantHandTracker.GetLastDown(),dominantHandTracker.GetLastForward(),dominantHandTracker.GetLastBackward());
			gestures[gestureNum].stateBank.at(gestures[gestureNum].numStates-1)->setStateDirectionCounts(dominantHandTracker.GetLastLeft(),dominantHandTracker.GetLastRight(),dominantHandTracker.GetLastUp(),dominantHandTracker.GetLastDown(),dominantHandTracker.GetLastForward(),dominantHandTracker.GetLastBackward());
		}
		else if(weakHandTracker.IsNewSignatureDetected() && !gestures[gestureNum].dominantHandRecording){
			weakHandTracker.GetLastFullSigPerformed(signatureName);
			printf("Signature Array: \n");
			for (int i = 0; i < strlen(signatureName); i++) {
				printf("%c", signatureName[i]);
			}
			printf("\n");
			gestures[gestureNum].stateBank.at(gestures[gestureNum].numStates-1)->setStateName(signatureName);
			gestures[gestureNum].stateBank.at(gestures[gestureNum].numStates-1)->setStateBools(weakHandTracker.GetPreviousLeft(),weakHandTracker.GetPreviousRight(),weakHandTracker.GetPreviousUp(),weakHandTracker.GetPreviousDown(),weakHandTracker.GetPreviousForward(),weakHandTracker.GetPreviousBackward());
			gestures[gestureNum].stateBank.at(gestures[gestureNum].numStates-1)->setStateDirectionCounts(weakHandTracker.GetPreviousLeft(),weakHandTracker.GetPreviousRight(),weakHandTracker.GetPreviousUp(),weakHandTracker.GetPreviousDown(),weakHandTracker.GetPreviousForward(),weakHandTracker.GetPreviousBackward());
			
		}
		printf("Num State:	%d\n", gestures[gestureNum].numStates);
		
		printf("GESTURE FULL\n");
		printf("Gesture Execution %d is Complete\n", numExecuted);
		printf("Place Hand at Starting Positon and press 'a' to continue\n", numExecuted);
		numExecuted++;
		g_bRecordGesture = FALSE;
		initialRecorded = TRUE;
		
		dominantHandTracker.SetNewSignatureDetected(FALSE);
		weakHandTracker.SetNewSignatureDetected(FALSE);
		dominantHandTracker.SetaSignatureDetected(FALSE);
		weakHandTracker.SetaSignatureDetected(FALSE);
		aHandMoved = FALSE;
	}
	/*record more times to create probabilities*/
	else if (g_bRecordGesture && gestureNum > -1 && gestures[gestureNum].numStates < NUM_STATES_PER_GESTURE && aHandMoved) {
		
		printf("Gesture Num:	%d\n", gestureNum);
		printf("Num State:	%d\n", gestures[gestureNum].numStates);
		
		if(dominantHandTracker.IsNewSignatureDetected() && (gestures[gestureNum].dominantHandRecording || gestures[gestureNum].numStates == 0)){	
			
			printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
			dominantHandTracker.GetLastFullSigPerformed(signatureName);
			printf("Signature Array: \n");
			for (int i = 0; i < strlen(signatureName); i++) {
				printf("%c", signatureName[i]);
			}
			printf("\n");
			
			if (gestures[gestureNum].numStates == 0) {
				state = new State();
				gestures[gestureNum].AddState(state,true);
				
				gestures[gestureNum].dominantHandRecording = TRUE;
			}
			
			else if(gestures[gestureNum].numStates < NUM_STATES_PER_GESTURE){
				gestures[gestureNum].stateBank.at(gestures[gestureNum].numStates-1)->setStateName(signatureName);
				gestures[gestureNum].stateBank.at(gestures[gestureNum].numStates-1)->setStateBools(dominantHandTracker.GetLastLeft(),dominantHandTracker.GetLastRight(),dominantHandTracker.GetLastUp(),dominantHandTracker.GetLastDown(),dominantHandTracker.GetLastForward(),dominantHandTracker.GetLastBackward());
				gestures[gestureNum].stateBank.at(gestures[gestureNum].numStates-1)->setStateDirectionCounts(dominantHandTracker.GetLastLeft(),dominantHandTracker.GetLastRight(),dominantHandTracker.GetLastUp(),dominantHandTracker.GetLastDown(),dominantHandTracker.GetLastForward(),dominantHandTracker.GetLastBackward());
				
				
				state = new State();
				gestures[gestureNum].AddState(state,false);			
			}
			
			numRecordedStates++;
			dominantHandTracker.SetNewSignatureDetected(FALSE);
			dominantHandTracker.SetaSignatureDetected(FALSE);
		}
		else if(weakHandTracker.IsNewSignatureDetected() && (!gestures[gestureNum].dominantHandRecording || gestures[gestureNum].numStates == 0)){
			
			printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
			weakHandTracker.GetLastFullSigPerformed(signatureName);
			printf("Signature Array: \n");
			for (int i = 0; i < strlen(signatureName); i++) {
				printf("%c", signatureName[i]);
			}
			printf("\n");
			
			
			if (gestures[gestureNum].numStates == 0) {
				state = new State();
				gestures[gestureNum].AddState(state,true);
				gestures[gestureNum].dominantHandRecording = FALSE;
			}
			
			else if (gestures[gestureNum].numStates < NUM_STATES_PER_GESTURE) {						
				gestures[gestureNum].stateBank.at(gestures[gestureNum].numStates-1)->setStateName(signatureName);
				gestures[gestureNum].stateBank.at(gestures[gestureNum].numStates-1)->setStateBools(weakHandTracker.GetPreviousLeft(),weakHandTracker.GetPreviousRight(),weakHandTracker.GetPreviousUp(),weakHandTracker.GetPreviousDown(),weakHandTracker.GetPreviousForward(),weakHandTracker.GetPreviousBackward());
				gestures[gestureNum].stateBank.at(gestures[gestureNum].numStates-1)->setStateDirectionCounts(weakHandTracker.GetPreviousLeft(),weakHandTracker.GetPreviousRight(),weakHandTracker.GetPreviousUp(),weakHandTracker.GetPreviousDown(),weakHandTracker.GetPreviousForward(),weakHandTracker.GetPreviousBackward());			
				
				state = new State();
				gestures[gestureNum].AddState(state,false);
			}
			
			
			numRecordedStates++;
			weakHandTracker.SetNewSignatureDetected(FALSE);
			weakHandTracker.SetaSignatureDetected(FALSE);
		}
		aHandMoved = FALSE;
	}
	
	if (numExecuted > 0 && !recordComplete && initialRecorded) {
		printNumExecuted = TRUE;
	}
	else {
		printNumExecuted = FALSE;
	}
	
	if (initialRecorded && !ready && numExecuted <= minRecordings) {
		printRecordPrompt = TRUE;
	}
	else {
		printRecordPrompt = FALSE;
	}
	
	
	if (initialRecorded && gestureNum > -1 && numExecuted > minRecordings && aHandMoved && !recordComplete){
		//printf("Gesture Execution %d is Complete\n", numExecuted);
		
		gestures[gestureNum].CalculateStateProbabilities(minRecordings);
		
		somethingSelectedRecord = FALSE;
		
		numRecordedStates = 0;
		numExecuted = 1;
		recordComplete = TRUE;
		dominantHandTracker.SetNewSignatureDetected(FALSE);
		weakHandTracker.SetNewSignatureDetected(FALSE);
		dominantHandTracker.SetaSignatureDetected(FALSE);
		weakHandTracker.SetaSignatureDetected(FALSE);
		initialRecorded = FALSE;
		gestureNum = -1;
		numTrackedStates = 0;
		ready = FALSE;
		aHandMoved = FALSE;
	}
	/*
	 * End recording if state machine is full and new signature detected
	 */
	else if (initialRecorded && gestureNum > -1 && numExecuted <= minRecordings && numTrackedStates == gestures[gestureNum].numStates && aHandMoved && ready) {
		
		if(dominantHandTracker.IsNewSignatureDetected() && gestures[gestureNum].dominantHandRecording){
			gestures[gestureNum].stateBank.at(gestures[gestureNum].numStates-1)->setStateDirectionCounts(dominantHandTracker.GetLastLeft(),dominantHandTracker.GetLastRight(),dominantHandTracker.GetLastUp(),dominantHandTracker.GetLastDown(),dominantHandTracker.GetLastForward(),dominantHandTracker.GetLastBackward());
			//gestures[gestureNum].stateBank.at(5)->printCounts();
		}
		else if(weakHandTracker.IsNewSignatureDetected() && !gestures[gestureNum].dominantHandRecording){
			gestures[gestureNum].stateBank.at(gestures[gestureNum].numStates-1)->setStateDirectionCounts(weakHandTracker.GetPreviousLeft(),weakHandTracker.GetPreviousRight(),weakHandTracker.GetPreviousUp(),weakHandTracker.GetPreviousDown(),weakHandTracker.GetPreviousForward(),weakHandTracker.GetPreviousBackward());
			//gestures[gestureNum].stateBank.at(5)->printCounts();
		}
		
		printf("Gesture Execution %d is Complete\n", numExecuted);
		if (numExecuted < minRecordings) {
			printf("Place Hand at Starting Positon and press 'a' to continue\n");
		}
		
		numExecuted++;
		numTrackedStates = 0;
		ready = FALSE;
		dominantHandTracker.SetNewSignatureDetected(FALSE);
		weakHandTracker.SetNewSignatureDetected(FALSE);
		dominantHandTracker.SetaSignatureDetected(FALSE);
		weakHandTracker.SetaSignatureDetected(FALSE);
		aHandMoved = FALSE;
	}
	/*record more times to create probabilities*/
	else if (initialRecorded && gestureNum > -1 && numExecuted <= minRecordings && numTrackedStates < gestures[gestureNum].numStates && ready && aHandMoved) {
		
		printf("Gesture Num:	%d\n", gestureNum);
		printf("Num Tracked States:	%d\n", numTrackedStates);
		
		if (gestures[gestureNum].dominantHandRecording) {
			printf("Dominant Hand Record: TRUE\n");
		}
		else{
			printf("Dominant Hand Record: FALSE\n");
		}
		
		if(dominantHandTracker.IsNewSignatureDetected() && gestures[gestureNum].dominantHandRecording){	
			if (numTrackedStates == 0) {
				//state1->setStateDirectionCounts(GetLastLeft(),GetLastRight(),GetLastUp(),GetLastDown(),GetLastForward(),GetLastBackward());
			}
			
			else {
				gestures[gestureNum].stateBank.at(numTrackedStates-1)->setStateDirectionCounts(dominantHandTracker.GetLastLeft(),dominantHandTracker.GetLastRight(),dominantHandTracker.GetLastUp(),dominantHandTracker.GetLastDown(),dominantHandTracker.GetLastForward(),dominantHandTracker.GetLastBackward());
				//gestures[gestureNum].stateBank.at(numTrackedStates-1)->printCounts();
			}
			
			numTrackedStates++;
			dominantHandTracker.SetNewSignatureDetected(FALSE);
			dominantHandTracker.SetaSignatureDetected(FALSE);
		}
		else if(weakHandTracker.IsNewSignatureDetected() && !gestures[gestureNum].dominantHandRecording){
			if (numTrackedStates == 0) {
			}
			else{
				gestures[gestureNum].stateBank.at(numTrackedStates-1)->setStateDirectionCounts(weakHandTracker.GetPreviousLeft(),weakHandTracker.GetPreviousRight(),weakHandTracker.GetPreviousUp(),weakHandTracker.GetPreviousDown(),weakHandTracker.GetPreviousForward(),weakHandTracker.GetPreviousBackward());
			}
			
			numTrackedStates++;
			weakHandTracker.SetNewSignatureDetected(FALSE);
			weakHandTracker.SetaSignatureDetected(FALSE);
		}
		aHandMoved = FALSE;
	}
	
	
	TimerRelease = FALSE;
	
#ifndef USE_GLES
	glutSwapBuffers();
#endif
}

#ifndef USE_GLES
void glutIdle (void)
{
	if (g_bQuit) {
		CleanupExit();
	}
	
	// Display the frame
	glutPostRedisplay();
}

void glutKeyboard (unsigned char key, int x, int y)
{
	switch (key)
	{
		case 27:
			//ofstream file;
			//file.open ("../../Config/example.txt",ios::app);
			//file << "quit\n";
			//file.close();	
			CleanupExit();
		case 'b':
			// Draw background?
			g_bDrawBackground = !g_bDrawBackground;
			break;
		case 'd':
			// Look for recorded gesture?
			g_bDetectGesture = !g_bDetectGesture;
			timer = clock() + nTime * CPS;
			for(int j = 0; j < NUM_GESTURES; j++){
				gestures[j].detectWaitNewSig = FALSE;
				gestures[j].detectWaitContSig = FALSE;
				gestures[j].gestureStarted = FALSE;
				gestures[j].numDetectedStates = 0;
				gestures[j].ResetCurrentState();
			}
			recursiveGestureVector.clear();
			dominantHandTracker.SetStartComparePoint(rightHand);
			weakHandTracker.SetStartComparePoint(leftHand);
			
			if(g_bDetectGesture && !playing){
				StartSong();
			}
			else {
				if(playing){	
					ofstream myfile;
					ofstream myfile2;
					myfile.open ("../../Config/groupa.txt",ios::app);
					myfile2.open ("../../Config/groupb.txt",ios::app);
					myfile << "pause\n";
					myfile2 << "pause\n";
					myfile.close();
					myfile2.close();	
				}
			}
			
			
			break;
		case 'r':
			// Record Gesture
			if (initialRecorded) {
				if (gestures[gestureNum].dominantHandRecording) {
					gestures[gestureNum].stateBank.at(numTrackedStates-1)->setStateDirectionCounts(dominantHandTracker.GetPreviousLeft(),dominantHandTracker.GetPreviousRight(),dominantHandTracker.GetPreviousUp(),dominantHandTracker.GetPreviousDown(),dominantHandTracker.GetPreviousForward(),dominantHandTracker.GetPreviousBackward());
					gestures[gestureNum].stateBank.at(numTrackedStates)->numNothing++;
				}
				else if(!gestures[gestureNum].dominantHandRecording){
					gestures[gestureNum].stateBank.at(numTrackedStates-1)->setStateDirectionCounts(weakHandTracker.GetPreviousLeft(),weakHandTracker.GetPreviousRight(),weakHandTracker.GetPreviousUp(),weakHandTracker.GetPreviousDown(),weakHandTracker.GetPreviousForward(),weakHandTracker.GetPreviousBackward());
					gestures[gestureNum].stateBank.at(numTrackedStates)->numNothing++;
				}
				
				printf("Gesture Execution %d is Complete\n", numExecuted);
				if (numExecuted < minRecordings) {
					printf("Place Hand at Starting Positon and press 'a' to continue\n");
				}
				
				numExecuted++;
				numTrackedStates = 0;
				ready = FALSE;
			}
			else{
				if (g_bRecordGesture) {
					initialRecorded = TRUE;
					g_bRecordGesture = FALSE;
					
					if (gestures[gestureNum].dominantHandRecording) {
						dominantHandTracker.GetLastSigPerformed(signatureName);
						gestures[gestureNum].stateBank.at(gestures[gestureNum].numStates-1)->setStateName(signatureName);
						gestures[gestureNum].stateBank.at(gestures[gestureNum].numStates-1)->setStateBools(dominantHandTracker.GetPreviousLeft(),dominantHandTracker.GetPreviousRight(),dominantHandTracker.GetPreviousUp(),dominantHandTracker.GetPreviousDown(),dominantHandTracker.GetPreviousForward(),dominantHandTracker.GetPreviousBackward());
						gestures[gestureNum].stateBank.at(gestures[gestureNum].numStates-1)->setStateDirectionCounts(dominantHandTracker.GetPreviousLeft(),dominantHandTracker.GetPreviousRight(),dominantHandTracker.GetPreviousUp(),dominantHandTracker.GetPreviousDown(),dominantHandTracker.GetPreviousForward(),dominantHandTracker.GetPreviousBackward());
					}
					else if(!gestures[gestureNum].dominantHandRecording){
						weakHandTracker.GetLastSigPerformed(signatureName);
						gestures[gestureNum].stateBank.at(gestures[gestureNum].numStates-1)->setStateName(signatureName);
						gestures[gestureNum].stateBank.at(gestures[gestureNum].numStates-1)->setStateBools(weakHandTracker.GetPreviousLeft(),weakHandTracker.GetPreviousRight(),weakHandTracker.GetPreviousUp(),weakHandTracker.GetPreviousDown(),weakHandTracker.GetPreviousForward(),weakHandTracker.GetPreviousBackward());
						gestures[gestureNum].stateBank.at(gestures[gestureNum].numStates-1)->setStateDirectionCounts(weakHandTracker.GetPreviousLeft(),weakHandTracker.GetPreviousRight(),weakHandTracker.GetPreviousUp(),weakHandTracker.GetPreviousDown(),weakHandTracker.GetPreviousForward(),weakHandTracker.GetPreviousBackward());
					}
					printf("Num States:	%d\n", gestures[gestureNum].numStates);
					
					printf("GESTURE DONE\n");
					printf("Gesture Execution %d is Complete\n", numExecuted);
					printf("Place Hand at Starting Positon and press 'a' to continue\n", numExecuted);
					numExecuted++;
				}
				else {
					g_bRecordGesture = TRUE;
				}
				gestureSelectedPrinted = FALSE;
				recordComplete = FALSE;
				dominantHandTracker.SetStartComparePoint(rightHand);
				weakHandTracker.SetStartComparePoint(leftHand);
				timer = clock() + nTime * CPS;
			}
			//}
			
			break;
		case 't':
			// Print out states?
			for (int i = 0; i < NUM_GESTURES; i++) {
				printf("Gesture Number %d\n", i);
				if (gestures[i].stateBank.size() == 0) {
					printf("StateMachine Empty\n");
				}
				else {
					gestures[i].PrintStateMachine();
					//gestures[i].PrintStateMachineBools();
					gestures[i].PrintStateMachineProbs();
				}
			}
			break;
		case 'u':
			if (recursiveGestureVector.size() != 0) {
				printf("Recursive Vector\n");
				for ( it=recursiveGestureVector.begin() ; it < recursiveGestureVector.end(); it++ )
					cout << "State Machine Number: " << (*it).stateMachineNumber << ": Start State: " << (*it).startState->getStateName() << " : Current State: " << (*it).currentState->getStateName() << endl;
				printf("Complete\n");
			}
			else{
				printf("Vector Empty\n");
			}
			
			break;
		case 'a':
			// For recording
			ready = TRUE;
			timer = clock() + nTime * CPS;
			dominantHandTracker.SetStartComparePoint(rightHand);
			weakHandTracker.SetStartComparePoint(leftHand);
			break;
		case 'x':
			// Draw pixels at all?
			g_bDrawPixels = !g_bDrawPixels;
			break;
		case 's':
			// Draw Skeleton?
			g_bDrawSkeleton = !g_bDrawSkeleton;
			break;
		case 'i':
			// Print label?
			g_bPrintID = !g_bPrintID;
			break;
		case 'v':
			// Print ID & state as label, or only ID?
			g_bPrintState = !g_bPrintState;
			break;
		case'p':
			g_bPause = !g_bPause;
			break;
		case 'S':
			//SaveCalibration();
			SaveGestures();
			break;
		case 'L':
			//LoadCalibration();
			LoadGestures();
			break;
		case '1':
			if(somethingSelectedRecord != TRUE){
				somethingSelectedRecord = TRUE;
				gestureNum = 0;
				gestures[gestureNum].numStates = 0;
				gestures[gestureNum].stateMachineNumber = 0;
				printGestureSelection = FALSE;
			}
			break;
		case '2':
			if(somethingSelectedRecord != TRUE){
				somethingSelectedRecord = TRUE;
				gestureNum = 1;
				gestures[gestureNum].numStates = 0;
				gestures[gestureNum].stateMachineNumber = 1;
				printGestureSelection = FALSE;
			}
			break;
		case '3':
			if(somethingSelectedRecord != TRUE){
				somethingSelectedRecord = TRUE;
				gestureNum = 2;
				gestures[gestureNum].numStates = 0;
				gestures[gestureNum].stateMachineNumber = 2;
				printGestureSelection = FALSE;
			}
			break;
		case '4':
			if(somethingSelectedRecord != TRUE){
				somethingSelectedRecord = TRUE;
				gestureNum = 3;
				gestures[gestureNum].numStates = 0;
				gestures[gestureNum].stateMachineNumber = 3;
				printGestureSelection = FALSE;
			}
			break;
		case '5':
			if(somethingSelectedRecord != TRUE){
				somethingSelectedRecord = TRUE;
				gestureNum = 4;
				gestures[gestureNum].numStates = 0;
				gestures[gestureNum].stateMachineNumber = 4;
				printGestureSelection = FALSE;
			}
			break;
		case '6':
			if(somethingSelectedRecord != TRUE){
				somethingSelectedRecord = TRUE;
				gestureNum = 5;
				gestures[gestureNum].numStates = 0;
				gestures[gestureNum].stateMachineNumber = 5;
				printGestureSelection = FALSE;
			}
			break;
		case 'f':
			if(somethingSelectedRecord != TRUE){
				somethingSelectedRecord = TRUE;
				gestureNum = 6;
				gestures[gestureNum].numStates = 0;
				gestures[gestureNum].stateMachineNumber = 6;
				printGestureSelection = FALSE;
			}
			break;
		case 'g':
			if(somethingSelectedRecord != TRUE){
				somethingSelectedRecord = TRUE;
				gestureNum = 7;
				gestures[gestureNum].numStates = 0;
				gestures[gestureNum].stateMachineNumber = 7;
				printGestureSelection = FALSE;
			}
			break;
		case 'h':
			/* ECHO */
			if(somethingSelectedRecord != TRUE){
				somethingSelectedRecord = TRUE;
				gestureNum = 8;
				gestures[gestureNum].numStates = 0;
				gestures[gestureNum].stateMachineNumber = 8;
				printGestureSelection = FALSE;
			}
			break;
		case 'j':
			if(somethingSelectedRecord != TRUE){
				somethingSelectedRecord = TRUE;
				gestureNum = 9;
				gestures[gestureNum].numStates = 0;
				gestures[gestureNum].stateMachineNumber = 9;
				printGestureSelection = FALSE;
			}
			break;
		case 'k':
			if(somethingSelectedRecord != TRUE){
				somethingSelectedRecord = TRUE;
				gestureNum = 10;
				gestures[gestureNum].numStates = 0;
				gestures[gestureNum].stateMachineNumber = 10;
				printGestureSelection = FALSE;
			}
			break;
		case 'l':
			if(somethingSelectedRecord != TRUE){
				somethingSelectedRecord = TRUE;
				gestureNum = 11;
				gestures[gestureNum].numStates = 0;
				gestures[gestureNum].stateMachineNumber = 11;
				printGestureSelection = FALSE;
			}
			break;
		case 'c':
			if(somethingSelectedRecord != TRUE){
				somethingSelectedRecord = TRUE;
				gestureNum = 12;
				gestures[gestureNum].numStates = 0;
				gestures[gestureNum].stateMachineNumber = 12;
				printGestureSelection = FALSE;
			}
			break;
		case 'q':
			if(somethingSelectedRecord != TRUE){
				somethingSelectedRecord = TRUE;
				gestureNum = 13;
				gestures[gestureNum].numStates = 0;
				gestures[gestureNum].stateMachineNumber = 13;
				printGestureSelection = FALSE;
			}
			break;
	}
}

void glInit (int * pargc, char ** argv)
{
	glutInit(pargc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
	glutInitWindowSize(GL_WIN_SIZE_X, GL_WIN_SIZE_Y);
	glutCreateWindow ("Maestro");
	//glutFullScreen();
	glutSetCursor(GLUT_CURSOR_NONE);
	
	glutKeyboardFunc(glutKeyboard);
	glutDisplayFunc(glutDisplay);
	glutIdleFunc(glutIdle);
	
	glDisable(GL_DEPTH_TEST);
	glEnable(GL_TEXTURE_2D);
	
	glEnableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_COLOR_ARRAY);
}
#endif // USE_GLES

#define SAMPLE_XML_PATH "../../Config/SamplesConfig.xml"
//#define SAMPLE_XML_PATH "/Users/avbharathsingh/OpenNI/Platform/Linux-x86/Redist/Samples/Config/SamplesConfig.xml"

#define CHECK_RC(nRetVal, what)										\
if (nRetVal != XN_STATUS_OK)									\
{																\
printf("%s failed: %s\n", what, xnGetStatusString(nRetVal));\
return nRetVal;												\
}

void ERRCHECK(FMOD_RESULT result)
{
    if (result != FMOD_OK)
    {
        printf("FMOD error! (%d) %s\n", result, FMOD_ErrorString(result));
        exit(-1);
    }
}

int main(int argc, char **argv)
{
	/*
	 // Code only executed by child process
	 int i;
	 printf ("Checking if processor is available...");
	 if (system(NULL)) puts ("Ok");
	 else exit (1);
	 i=system ("pwd");
	 i=system ("cd Maestro-build-desktop/Maestro.app/Contents/MacOS && ./Maestro");
	 
	 ifstream myfile;
	 myfile.open("../../Config/settings.txt");
	 string line;
	 
	 getline (myfile,line);
	 SongFile = line;
	 
	 getline (myfile,line);
	 while ( myfile.good() )
	 {
	 if(line.compare("tempo") == 0){
	 getline (myfile,line);
	 initialTempo = atof(line.c_str());
	 }
	 
	 if(line.compare("volume") == 0){
	 getline (myfile,line);
	 initialVolume = atof(line.c_str());
	 }
	 
	 if(line.compare("pitch") == 0){
	 getline (myfile,line);
	 initialPitch = atof(line.c_str());
	 }
	 getline (myfile,line);
	 }
	 
	 
	 printf("Song File: %s\n", SongFile.c_str());
	 printf("Initial Pitch Read: %f\n", initialPitch);
	 printf("Initial Volume Read: %f\n", initialVolume);
	 printf("Initial Tempo Read: %f\n", initialTempo);*/
	
	printf("Hello I am Maestro!!!\n");
	XnStatus nRetVal = XN_STATUS_OK;
	
	if (argc > 1)
	{
		nRetVal = g_Context.Init();
		CHECK_RC(nRetVal, "Init");
		nRetVal = g_Context.OpenFileRecording(argv[1], g_Player);
		if (nRetVal != XN_STATUS_OK)
		{
			printf("Can't open recording %s: %s\n", argv[1], xnGetStatusString(nRetVal));
			return 1;
		}
	}
	else
	{
		xn::EnumerationErrors errors;
		nRetVal = g_Context.InitFromXmlFile(SAMPLE_XML_PATH, g_scriptNode, &errors);
		if (nRetVal == XN_STATUS_NO_NODE_PRESENT)
		{
			XnChar strError[1024];
			errors.ToString(strError, 1024);
			printf("%s\n", strError);
			return (nRetVal);
		}
		else if (nRetVal != XN_STATUS_OK)
		{
			printf("I can't open SamplesConfig.xml for some reason\n");
			printf("This is the path I'm searching: %s\n", SAMPLE_XML_PATH);
			printf("Open failed: %s\n", xnGetStatusString(nRetVal));
			return (nRetVal);
		}
	}
	
	nRetVal = g_Context.FindExistingNode(XN_NODE_TYPE_DEPTH, g_DepthGenerator);
	CHECK_RC(nRetVal, "Find depth generator");
	nRetVal = g_Context.FindExistingNode(XN_NODE_TYPE_USER, g_UserGenerator);
	if (nRetVal != XN_STATUS_OK)
	{
		nRetVal = g_UserGenerator.Create(g_Context);
		CHECK_RC(nRetVal, "Find user generator");
	}
	
	XnCallbackHandle hUserCallbacks, hCalibrationStart, hCalibrationComplete, hPoseDetected, hCalibrationInProgress, hPoseInProgress;
	
	if (!g_UserGenerator.IsCapabilitySupported(XN_CAPABILITY_SKELETON))
	{
		printf("Supplied user generator doesn't support skeleton\n");
		return 1;
	}
	
	nRetVal = g_UserGenerator.RegisterUserCallbacks(User_NewUser, User_LostUser, NULL, hUserCallbacks);
	CHECK_RC(nRetVal, "Register to user callbacks");
	nRetVal = g_UserGenerator.GetSkeletonCap().RegisterToCalibrationStart(UserCalibration_CalibrationStart, NULL, hCalibrationStart);
	CHECK_RC(nRetVal, "Register to calibration start");
	nRetVal = g_UserGenerator.GetSkeletonCap().RegisterToCalibrationComplete(UserCalibration_CalibrationComplete, NULL, hCalibrationComplete);
	CHECK_RC(nRetVal, "Register to calibration complete");
	
	incVolume = new StateMachine();
	decVolume = new StateMachine();
	incTempo = new StateMachine();
	decTempo = new StateMachine();
	incPitch = new StateMachine();
	decPitch = new StateMachine();
	
	gestures[0] = *incVolume;
	gestures[1] = *decVolume;
	gestures[2] = *incTempo;
	gestures[3] = *decTempo;
	gestures[4] = *incPitch;
	gestures[5] = *decPitch;
	
	gestures[0].stateMachineNumber = 0;
	gestures[1].stateMachineNumber = 1;
	gestures[2].stateMachineNumber = 2;
	gestures[3].stateMachineNumber = 3;
	gestures[4].stateMachineNumber = 4;
	gestures[5].stateMachineNumber = 5;
	gestures[6].stateMachineNumber = 6;
	gestures[7].stateMachineNumber = 7;
	gestures[8].stateMachineNumber = 8;
	gestures[9].stateMachineNumber = 9;
	gestures[10].stateMachineNumber = 10;
	gestures[11].stateMachineNumber = 11;
	gestures[12].stateMachineNumber = 12;
	gestures[13].stateMachineNumber = 13;
	
	/*ofstream clearFile;
	 clearFile.open ("../../Config/groupa.txt",ios::out);
	 clearFile << "\n";
	 clearFile.close();
	 clearFile.open ("../../Config/groupb.txt",ios::out);
	 clearFile << "\n";
	 clearFile.close();*/
	
	if (g_UserGenerator.GetSkeletonCap().NeedPoseForCalibration())
	{
		g_bNeedPose = TRUE;
		if (!g_UserGenerator.IsCapabilitySupported(XN_CAPABILITY_POSE_DETECTION))
		{
			printf("Pose required, but not supported\n");
			return 1;
		}
		nRetVal = g_UserGenerator.GetPoseDetectionCap().RegisterToPoseDetected(UserPose_PoseDetected, NULL, hPoseDetected);
		CHECK_RC(nRetVal, "Register to Pose Detected");
		g_UserGenerator.GetSkeletonCap().GetCalibrationPose(g_strPose);
	}
	
	g_UserGenerator.GetSkeletonCap().SetSkeletonProfile(XN_SKEL_PROFILE_UPPER);
	
	nRetVal = g_UserGenerator.GetSkeletonCap().RegisterToCalibrationInProgress(MyCalibrationInProgress, NULL, hCalibrationInProgress);
	CHECK_RC(nRetVal, "Register to calibration in progress");
	
	nRetVal = g_UserGenerator.GetPoseDetectionCap().RegisterToPoseInProgress(MyPoseInProgress, NULL, hPoseInProgress);
	CHECK_RC(nRetVal, "Register to pose in progress");
	
	nRetVal = g_Context.StartGeneratingAll();
	CHECK_RC(nRetVal, "StartGenerating");
	
#ifndef USE_GLES
	glInit(&argc, argv);
	glutMainLoop();
#else
	if (!opengles_init(GL_WIN_SIZE_X, GL_WIN_SIZE_Y, &display, &surface, &context))
	{
		printf("Error initializing opengles\n");
		CleanupExit();
	}
	
	glDisable(GL_DEPTH_TEST);
	glEnable(GL_TEXTURE_2D);
	glEnableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_COLOR_ARRAY);
	
	while (!g_bQuit)
	{
		glutDisplay();
		eglSwapBuffers(display, surface);
	}
	opengles_shutdown(display, surface, context);
	
	CleanupExit();
#endif
}
