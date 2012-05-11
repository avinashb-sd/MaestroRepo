/*
 *  DetectHandSwipe.h
 *  
 *
 *  Created by Avinash BharathSingh on 5/1/12.
 *  Copyright 2012 The George Washington University. All rights reserved.
 *
 */
#include "SignatureDetection.h"
#include "StateMachine.h"
#include <iostream>
#include <string>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>   // Declaration for exit()
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <fstream>
#include <time.h>
#include <cmath>
#include <queue>
#include <math.h>
#include <XnCppWrapper.h>
#include "RotateHandMovement.h"

#ifndef DETECTHANDSWIPE_H
#define DETECTHANDSWIPE_H

class DetectHandSwipe;

class DetectHandSwipe{
public:
	DetectHandSwipe();
	//~DetectHandSwipe();
	bool DetectSwipe(XnPoint3D point, XnSkeletonJointPosition leftShoulder, XnSkeletonJointPosition rightShoulder, bool RecordPhase, bool DetectPhase, RotateHandMovement rotator);
	
	bool IsNewSignatureDetected();
	bool IsContSignatureDetected();
	bool IsaSignatureDetected();
	
	void SetNewSignatureDetected(bool detected);
	void SetContSignatureDetected(bool detected);
	void SetaSignatureDetected(bool detected);
	
	XnPoint3D GetGestureStartCompare();
	
	bool GetLastLeft();
	bool GetLastRight();
	bool GetLastUp();
	bool GetLastDown();
	bool GetLastForward();
	bool GetLastBackward();

	bool GetPreviousLeft();
	bool GetPreviousRight();
	bool GetPreviousUp();
	bool GetPreviousDown();
	bool GetPreviousForward();
	bool GetPreviousBackward();
	
	void GetLastSigPerformed(char CHAR_INFO[]);
	void GetLastFullSigPerformed(char CHAR_INFO[]);

	void SetStartComparePoint(XnSkeletonJointPosition point);
	
private:
	void writeAnalyze();
	void analyzeMovement();
	
	//RotateHandMovement rotatingObject = new RotateHandMovement();
	
	bool newSignatureDetected;
	bool contSignatureDetected;
	bool aSignatureDetected;
		
	bool initializePoints;
	
	bool timerReset;
	
	XnPoint3D gestureStartCompare;
	XnPoint3D gestureDetectedLocation;
	XnPoint3D previousGestureStartCompare;
	
	int changeInXBetweenGesturePoints;
	int	changeInYBetweenGesturePoints;
	int newChangeInXBetweenGesturePoints;
	int	newChangeInYBetweenGesturePoints;
	
	int numSignaturePerformed;
	
	//Used for storing the directions detected and printed to a file
	char sigPrint[60];
	
	//Used for storing the directions detected and storing for recording states on the current movement
	char sigPrintLast[60];
	
	//Used for storing the directions detected and storing for recording states for the last complete movement
	char LastSignature[60];
	
	//all directions detected since the last new movement; movement could still be in progress
	bool previousLeft;
	bool previousRight;
	bool previousUp;
	bool previousDown;
	bool previousForward;
	bool previousBackward;
	
	//all directions detected in the last complete movement
	bool lastFullLeft;
	bool lastFullRight;
	bool lastFullUp;
	bool lastFullDown;
	bool lastFullForward;
	bool lastFullBackward;
	
	//Directions Detected on Currently Detected
	bool leftSwipe;
	bool rightSwipe;
	bool upSwipe;
	bool downSwipe;
	bool forwardSwipe;
	bool backwardSwipe;
	
	//Write movements to files
	bool analyzeLeft;
	bool analyzeRight;
	bool analyzeUp;
	bool analyzeDown;
	bool analyzeForward;
	bool analyzeBackward;
	
	time_t startSigTime;
	time_t previousSigTime;
	time_t currentSigTime;
	
};
#endif