/*
 *  RotateHandMovement.h
 *  
 *
 *  Created by Avinash BharathSingh on 5/1/12.
 *  Copyright 2012 The George Washington University. All rights reserved.
 *
 */

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

#ifndef ROTATEHANDMOVEMENT_H
#define ROTATEHANDMOVEMENT_H

class RotateHandMovement;

class RotateHandMovement{
public:
	RotateHandMovement();
	//~RotateHandMovement();

	void NormalizeMovement(XnPoint3D startPoint, XnPoint3D endPoint, XnSkeletonJointPosition leftShoulder, XnSkeletonJointPosition rightShoulder);
	void PrintNormalizedMovement(XnPoint3D startPoint ,XnPoint3D endPoint);
	
	void CalculateOrientation(XnSkeletonJointPosition leftShoulder, XnSkeletonJointPosition rightShoulder, bool orientationOnly);
		
	float CenterOfLine(XnFloat p1, XnFloat p2);
		
	bool FacingLeft();
	bool FacingRight();
	bool FacingForward();
		
	float GetEndX();
	float GetEndZ();
	float GetStartX();
	float GetStartZ();

	
private:
	void RotateMovementClockwise();
	void RotateMovementCounterClockwise();
		
	bool shiftLeft;
	bool shiftRight;
	
	bool facingLeft;
	bool facingRight;
	bool facingForward;
		
	float adj;
	float hyp;
		
	float translationVector[3];
	float currentLeftShoulderVector[3];
	float currentRightShoulderVector[3];
	float currentCenterVector[3];
	float translatedLeftShoulderVector[3];
	float translatedRightShoulderVector[3];
	float translatedCenterVector[3];
	
	float translationMovementVector[3];
	float currentEndMovementVector[3];
	float currentStartMovementVector[3];
	float currentCenterMovementVector[3];
	float translatedEndMovementVector[3];
	float translatedStartMovementVector[3];
	float translatedCenterMovementVector[3];
	
	float theta;
	
	float rotatedXEndM;
	float rotatedZEndM;
	float rotatedXStartM;
	float rotatedZStartM;
	float rotatedXCenterM;
	float rotatedZCenterM;
};
#endif