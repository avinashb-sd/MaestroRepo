/*
 *  RotateHandMovement.cpp
 *  
 *
 *  Created by Avinash BharathSingh on 5/1/12.
 *  Copyright 2012 The George Washington University. All rights reserved.
 *
 */

#include "RotateHandMovement.h"
#define PI 3.14159265

RotateHandMovement::RotateHandMovement(){
	shiftLeft = FALSE;
	shiftRight = FALSE;
	
	facingLeft = FALSE;
	facingRight = FALSE;
	facingForward = FALSE;
	
	rotatedXEndM = 0;
	rotatedZEndM = 0;
	rotatedXStartM = 0;
	rotatedZStartM = 0;
	rotatedXCenterM = 0;
	rotatedZCenterM = 0;
}

float RotateHandMovement::CenterOfLine(XnFloat p1, XnFloat p2){
	return (p1 + p2)/2.0f;
}

void RotateHandMovement::CalculateOrientation(XnSkeletonJointPosition leftShoulder, XnSkeletonJointPosition rightShoulder, bool orientationOnly){
	currentLeftShoulderVector[0] = leftShoulder.position.X;
	currentLeftShoulderVector[1] = leftShoulder.position.Y;
	currentLeftShoulderVector[2] = leftShoulder.position.Z;
	currentRightShoulderVector[0] = rightShoulder.position.X;
	currentRightShoulderVector[1] = rightShoulder.position.Y;
	currentRightShoulderVector[2] = rightShoulder.position.Z;
	
	currentCenterVector[0] = CenterOfLine(leftShoulder.position.X,rightShoulder.position.X);
	currentCenterVector[1] = CenterOfLine(leftShoulder.position.Y,rightShoulder.position.Y);
	currentCenterVector[2] = CenterOfLine(leftShoulder.position.Z,rightShoulder.position.Z);
	
	translationVector[0] = 0 - currentCenterVector[0];
	translationVector[1] = 0 - currentCenterVector[1];
	translationVector[2] = 0 - currentCenterVector[2];
	
	translatedLeftShoulderVector[0] = currentLeftShoulderVector[0] + translationVector[0];
	translatedLeftShoulderVector[1] = currentLeftShoulderVector[1] + translationVector[1];
	translatedLeftShoulderVector[2] = currentLeftShoulderVector[2] + translationVector[2];
	translatedRightShoulderVector[0] = currentRightShoulderVector[0] + translationVector[0];
	translatedRightShoulderVector[1] = currentRightShoulderVector[1] + translationVector[1];
	translatedRightShoulderVector[2] = currentRightShoulderVector[2] + translationVector[2];
	translatedCenterVector[0] = currentCenterVector[0] + translationVector[0];
	translatedCenterVector[1] = currentCenterVector[1] + translationVector[1];
	translatedCenterVector[2] = currentCenterVector[2] + translationVector[2];
	
	hyp = sqrtf( powf((translatedRightShoulderVector[0] - translatedCenterVector[0]), 2.0f) + powf((translatedRightShoulderVector[2] - translatedCenterVector[2]), 2.0f));
	adj = translatedRightShoulderVector[0];
	
	theta = acos (adj/hyp);	
	
	if(orientationOnly){
		if(rightShoulder.position.Z > leftShoulder.position.Z){
			theta = 0 - theta;
		}
		
		theta = theta * 180.0 / PI;
		
		if (theta >= 30.0f) {
			facingLeft = TRUE;
			facingRight = FALSE;
			facingForward = FALSE;
		}
		else if (theta <= -30.0f) {
			facingLeft = FALSE;
			facingRight = TRUE;
			facingForward = FALSE;
		}
		else {
			facingLeft = FALSE;
			facingRight = FALSE;
			facingForward = TRUE;
		}
	}
}


void RotateHandMovement::RotateMovementClockwise(){
	rotatedXEndM = (currentEndMovementVector[0]*cos(0-theta)) + (currentEndMovementVector[2]*sin(0-theta));
	rotatedXStartM = (currentStartMovementVector[0]*cos(0-theta)) + (currentStartMovementVector[2]*sin(0-theta));
	rotatedXCenterM = (currentCenterMovementVector[0]*cos(0-theta)) + (currentCenterMovementVector[2]*sin(0-theta));
	rotatedZEndM = ((0 - currentEndMovementVector[0])*sin(0-theta)) + (currentEndMovementVector[2]*cos(0-theta));
	rotatedZStartM = ((0 - currentStartMovementVector[0])*sin(0-theta)) + (currentStartMovementVector[2]*cos(0-theta));
	rotatedZCenterM = ((0 - currentCenterMovementVector[0])*sin(0-theta)) + (currentCenterMovementVector[2]*cos(0-theta));
}

void RotateHandMovement::RotateMovementCounterClockwise(){
	rotatedXEndM = (currentEndMovementVector[0]*cos(theta)) + (currentEndMovementVector[2]*sin(theta));
	rotatedXStartM = (currentStartMovementVector[0]*cos(theta)) + (currentStartMovementVector[2]*sin(theta));
	rotatedXCenterM = (currentCenterMovementVector[0]*cos(theta)) + (currentCenterMovementVector[2]*sin(theta));
	rotatedZEndM = ((0 - currentEndMovementVector[0])*sin(theta)) + (currentEndMovementVector[2]*cos(theta));
	rotatedZStartM = ((0 - currentStartMovementVector[0])*sin(theta)) + (currentStartMovementVector[2]*cos(theta));
	rotatedZCenterM = ((0 - currentCenterMovementVector[0])*sin(theta)) + (currentCenterMovementVector[2]*cos(theta));
}



void RotateHandMovement::NormalizeMovement(XnPoint3D startPoint, XnPoint3D endPoint, XnSkeletonJointPosition leftShoulder, XnSkeletonJointPosition rightShoulder){
	
	CalculateOrientation(leftShoulder, rightShoulder, FALSE);
	
	currentEndMovementVector[0] = endPoint.X;
	currentEndMovementVector[1] = endPoint.Y;
	currentEndMovementVector[2] = endPoint.Z;
	currentStartMovementVector[0] = startPoint.X;
	currentStartMovementVector[1] = startPoint.Y;
	currentStartMovementVector[2] = startPoint.Z;
	
	if(rightShoulder.position.Z > leftShoulder.position.Z){
		//right shoulder is further than left shoulder => acos should be negative
		//printf("Shifted Right\n");
		shiftRight = TRUE;
		shiftLeft = FALSE;
		
		RotateMovementCounterClockwise();
		
		theta = 0 - theta;
	}
	else if(rightShoulder.position.Z < leftShoulder.position.Z){
		//printf("Shifted Left\n");
		shiftLeft = TRUE;
		shiftRight = FALSE;
		
		RotateMovementClockwise();
	}
	
	theta = theta * 180.0 / PI;
}

void RotateHandMovement::PrintNormalizedMovement(XnPoint3D startPoint ,XnPoint3D endPoint){	
	printf("\n");
	if (shiftRight) {
		printf("Shifted Right\n");
	}
	if (shiftLeft) {
		printf("Shifted Left\n");
	}
	
	printf("Translated Center: (%f,%f,%f)\n", translatedCenterMovementVector[0],translatedCenterMovementVector[1],translatedCenterMovementVector[2]);
	
	printf ("The arc cosine is %lf degrees.\n", theta);
	printf ("Initial Start Movement: (%f,%f,%f)\n", currentStartMovementVector[0], currentStartMovementVector[1], currentStartMovementVector[2]);
	printf ("Initial End Movement: (%f,%f,%f)\n", currentEndMovementVector[0], currentEndMovementVector[1], currentEndMovementVector[2]);
	printf ("Rotated Start Movement: (%f,%f,%f)\n", rotatedXStartM, currentStartMovementVector[1], rotatedZStartM);
	printf ("Rotated End Movement: (%f,%f,%f)\n", rotatedXEndM, currentEndMovementVector[1], rotatedZEndM);
	
	printf ("Rotated Center Movement: (%f,%f,%f)\n", rotatedXCenterM, currentCenterMovementVector[1], rotatedZCenterM);

	printf ("Lateral Distance of Movement: %f\n", abs(startPoint.X - endPoint.X));
	printf ("Vertical  Distance of Movement: %f\n", abs(currentStartMovementVector[1] - currentEndMovementVector[1]));
	printf ("Depth Distance of Movement: %f\n", abs(currentStartMovementVector[2] - currentEndMovementVector[2]));
	printf ("Rotated Lateral Distance Btw Left and Right: %f\n", abs(rotatedXStartM - rotatedXEndM));
	//printf ("Translated Vertical  Distance of Movement: %f\n", abs(translatedStartMovementVector[1] - translatedEndMovementVector[1]));
	printf ("Rotated Depth Distance of Movement: %f\n", abs(rotatedZStartM - rotatedZEndM));
}

float RotateHandMovement::GetEndX(){
	return rotatedXEndM;
}

float RotateHandMovement::GetEndZ(){
	return rotatedZEndM;
}

float RotateHandMovement::GetStartX(){
	return rotatedXStartM;
}

float RotateHandMovement::GetStartZ(){
	return rotatedZStartM;
}


bool RotateHandMovement::FacingLeft(){
	return facingLeft;
}

bool RotateHandMovement::FacingRight(){
	return facingRight;
}

bool RotateHandMovement::FacingForward(){
	return facingForward;
}