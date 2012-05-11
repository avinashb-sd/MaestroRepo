/*
 *  DetectHandSwipe.cpp
 *  
 *
 *  Created by Avinash BharathSingh on 5/1/12.
 *  Copyright 2012 The George Washington University. All rights reserved.
 *
 */

#include "DetectHandSwipe.h"

using namespace std;

ofstream signatureFile;
ofstream myfile;

DetectHandSwipe::DetectHandSwipe(){

	sprintf(strchr(sigPrint, 0), "");
	sprintf(strchr(sigPrintLast, 0), "");
	sprintf(strchr(LastSignature, 0), "");
	
	initializePoints = FALSE;
	//rotatingObject = new RotateHandMovement();
	
	newSignatureDetected = FALSE;
	contSignatureDetected = FALSE;
	aSignatureDetected = FALSE;
	
	previousLeft = FALSE;
	previousRight = FALSE;
	previousUp = FALSE;
	previousDown = FALSE;
	previousForward = FALSE;
	previousBackward = FALSE;
	
	lastFullLeft = FALSE;
	lastFullRight = FALSE;
	lastFullUp = FALSE;
	lastFullDown = FALSE;
	lastFullForward = FALSE;
	lastFullBackward = FALSE;
	
	leftSwipe = FALSE;
	rightSwipe = FALSE;
	upSwipe = FALSE;
	downSwipe = FALSE;
	forwardSwipe = FALSE;
	backwardSwipe = FALSE;
	
	analyzeLeft = FALSE;
	analyzeRight = FALSE;
	analyzeUp = FALSE;
	analyzeDown = FALSE;
	analyzeForward = FALSE;
	analyzeBackward = FALSE;
	
	startSigTime = time(NULL);
	previousSigTime = time(NULL);
	currentSigTime = time(NULL);
}

void DetectHandSwipe::writeAnalyze(){
	for (int i = 0; i < strlen(sigPrint); i++) {
		signatureFile << sigPrint[i];
	}
	signatureFile << "\n";
	signatureFile.close();
}
	
void DetectHandSwipe::analyzeMovement(){
	if(analyzeLeft && previousLeft){
		signatureFile.open ("../../Config/left.txt",ios::app);
		writeAnalyze();
	}
	
	if(analyzeRight && previousRight){
		signatureFile.open ("../../Config/right.txt",ios::app);
		writeAnalyze();
	}
	
	if(analyzeUp && previousUp){
		signatureFile.open ("../../Config/up.txt",ios::app);
		writeAnalyze();
	}
	
	if(analyzeDown && previousDown){
		signatureFile.open ("../../Config/down.txt",ios::app);		
		writeAnalyze();
	}
	
	if(analyzeForward && previousForward){
		signatureFile.open ("../../Config/forward.txt",ios::app);
		writeAnalyze();
	}
	
	if(analyzeBackward && previousBackward){
		signatureFile.open ("../../Config/backward.txt",ios::app);
		writeAnalyze();
	}
	
}

bool DetectHandSwipe::DetectSwipe(XnPoint3D point, XnSkeletonJointPosition leftShoulder, XnSkeletonJointPosition rightShoulder, bool RecordPhase, bool DetectPhase, RotateHandMovement rotator){
	
	newSignatureDetected = FALSE;
	aSignatureDetected = FALSE;	
	
	//If environment starts now
	if(initializePoints){
		gestureStartCompare = point;
		previousGestureStartCompare.X = 1.0f;
		previousGestureStartCompare.Y = 1.0f;
		changeInXBetweenGesturePoints = 0;
		changeInYBetweenGesturePoints = 0;
		initializePoints = !initializePoints;
	}
	
	/*
	 *Checking to see the direction of the passed point
	 */
	if (point.Y - gestureStartCompare.Y < 0) {
		newChangeInYBetweenGesturePoints = -1;
	}
	else {
		newChangeInYBetweenGesturePoints = 1;
	}
	
	if (point.X - gestureStartCompare.X < 0) {
		newChangeInXBetweenGesturePoints = -1;
	}
	else {
		newChangeInXBetweenGesturePoints = 1;
	}
	
	/*
	 *if the arms path is continuing in the same path as the last detected signature --> append detection onto previous, else its a new gesture
	 */
	if (newChangeInXBetweenGesturePoints == changeInXBetweenGesturePoints && newChangeInYBetweenGesturePoints == changeInYBetweenGesturePoints && timerReset == FALSE) {
		
		newSignatureDetected = FALSE;
		aSignatureDetected = FALSE;
		
		if(!RecordPhase && !DetectPhase){
			contSignatureDetected = FALSE;
		}
		
		if (IsMovementLongEnough(previousGestureStartCompare,point)) {
			
			rotator.NormalizeMovement(previousGestureStartCompare, point, leftShoulder, rightShoulder);
			
			leftSwipe = LeftSignature(rotator.GetStartX(), rotator.GetEndX());
			rightSwipe = RightSignature(rotator.GetStartX(), rotator.GetEndX());
			upSwipe = UpSignature(previousGestureStartCompare.Y, point.Y);
			downSwipe = DownSignature(previousGestureStartCompare.Y, point.Y);
			forwardSwipe = ForwardSignature(rotator.GetStartZ(), rotator.GetEndZ());
			backwardSwipe = BackwardSignature(rotator.GetStartZ(), rotator.GetEndZ());
			
			/*
			 * If a swipe was detected & it there was a new direction detected since the last signature detection.
			 * The second condition eliminates checking points going in the same path but serve no purpose in determinig a new signature
			 */
			if((leftSwipe && leftSwipe != previousLeft) || (rightSwipe && rightSwipe != previousRight) || (upSwipe && upSwipe != previousUp)|| (downSwipe && downSwipe != previousDown)|| (forwardSwipe && forwardSwipe != previousForward)|| (backwardSwipe && backwardSwipe != previousBackward))
			{
				//rotator.PrintNormalizedMovement(previousGestureStartCompare, point);
								
				gestureDetectedLocation = point;
				gestureStartCompare = point;

				//Update naming for current movement
				if (leftSwipe && leftSwipe != previousLeft)
				{
					
					if (analyzeLeft) {
						numSignaturePerformed++;
						printf("Number of Left Signatures Detected: %d\n", numSignaturePerformed);
					}
					
					/*
					 *Adding to the array that will be written into the probability file
					 */
					sprintf(strchr(sigPrint, 0), "left ");
					
					/*
					 *Only if being recorded, write to array that will be used for storing the name in the FSM
					 */
					if (RecordPhase || DetectPhase) {
						sprintf(strchr(sigPrintLast, 0), "left ");
					}
				}
				
				if (rightSwipe && rightSwipe != previousRight)
				{
					
					if (analyzeRight) {
						numSignaturePerformed++;
						printf("Number of Right Signatures Detected: %d\n", numSignaturePerformed);
					}
					
					sprintf(strchr(sigPrint, 0), "right ");
					
					if (RecordPhase || DetectPhase) {
						sprintf(strchr(sigPrintLast, 0), "right ");
					}
				}
				
				if (upSwipe && upSwipe != previousUp)
				{
					if (analyzeUp)
					{
						numSignaturePerformed++;
						printf("Number of Up Signatures Detected: %d\n", numSignaturePerformed);
					}
					
					sprintf(strchr(sigPrint, 0), "up ");
					
					if (RecordPhase || DetectPhase) {
						sprintf(strchr(sigPrintLast, 0), "up ");
					}
				}
				
				if (downSwipe && downSwipe != previousDown)
				{
					if (analyzeDown)
					{
						numSignaturePerformed++;
						printf("Number of Down Signatures Detected: %d\n", numSignaturePerformed);
					}
					
					sprintf(strchr(sigPrint, 0), "down ");
					
					if (RecordPhase || DetectPhase) {
						sprintf(strchr(sigPrintLast, 0), "down ");
					}
				}
				
				if (forwardSwipe && forwardSwipe != previousForward)
				{
					if (analyzeForward)
					{
						numSignaturePerformed++;
						printf("Number of Forward Signatures Detected: %d\n", numSignaturePerformed);
					}
					
					sprintf(strchr(sigPrint, 0), "forward ");
					
					if (RecordPhase || DetectPhase) {
						sprintf(strchr(sigPrintLast, 0), "forward ");
					}
				}
				
				if (backwardSwipe && backwardSwipe != previousBackward)
				{
					if (analyzeBackward)
					{
						numSignaturePerformed++;
						printf("Number of Backward Signatures Detected: %d\n", numSignaturePerformed);
					}
					
					sprintf(strchr(sigPrint, 0), "backward ");
					
					if (RecordPhase || DetectPhase) {
						sprintf(strchr(sigPrintLast, 0), "backward ");
					}
				}
				
				/*
				 * Maintaining the last detected signature, and adding new movements when they weren't true before
				 */
				if (!previousLeft)
				{
					previousLeft = leftSwipe;
				}			
				
				if (!previousRight)
				{
					previousRight = rightSwipe;
				}
				
				if (!previousUp)
				{
					previousUp = upSwipe;
				}
				
				if (!previousDown)
				{
					previousDown = downSwipe;
				}
				
				if (!previousForward)
				{
					previousForward = forwardSwipe;
				}
				
				if (!previousBackward)
				{
					previousBackward = backwardSwipe;
				}
				
				
				lastFullLeft = FALSE;
				lastFullRight = FALSE;
				lastFullUp = FALSE;
				lastFullDown = FALSE;
				lastFullForward = FALSE;
				lastFullBackward = FALSE;
				
				leftSwipe = FALSE;
				rightSwipe = FALSE;
				upSwipe = FALSE;
				downSwipe = FALSE;
				forwardSwipe = FALSE;
				backwardSwipe = FALSE;
				
				contSignatureDetected = TRUE;
				aSignatureDetected = TRUE;
				
				return false;
			}
		}
		
	}
	else {
		
		if(!RecordPhase && !DetectPhase){
			newSignatureDetected = FALSE;
		}
		
		aSignatureDetected = FALSE;
		
		if (IsMovementLongEnough(gestureStartCompare,point)) {

			rotator.NormalizeMovement(gestureStartCompare, point, leftShoulder, rightShoulder);
			
			leftSwipe = LeftSignature(rotator.GetStartX(), rotator.GetEndX());
			rightSwipe = RightSignature(rotator.GetStartX(), rotator.GetEndX());
			upSwipe = UpSignature(gestureStartCompare.Y, point.Y);
			downSwipe = DownSignature(gestureStartCompare.Y, point.Y);
			forwardSwipe = ForwardSignature(rotator.GetStartZ(), rotator.GetEndZ());
			backwardSwipe = BackwardSignature(rotator.GetStartZ(), rotator.GetEndZ());
			
			//if any swipe detect
			if(leftSwipe || rightSwipe || upSwipe || downSwipe || forwardSwipe || backwardSwipe)
			{
				timerReset = FALSE;
				//rotator.PrintNormalizedMovement(gestureStartCompare, point);
				
				//write movements to files
				analyzeMovement();
				
				//clear array for next sig
				xnOSMemSet(LastSignature, 0, sizeof(LastSignature));
				
				//
				for (int i = 0; i < strlen(sigPrintLast); i++) {
					LastSignature[i] = sigPrintLast[i];
				}	
				
				//clear array for next sig
				xnOSMemSet(sigPrint, 0, sizeof(sigPrint));
				
				//clear array for next sig
				xnOSMemSet(sigPrintLast, 0, sizeof(sigPrintLast));
				
				
				
				//Check to see if the same movement is detected, if it is ignore it
				if(previousLeft == leftSwipe && previousRight == rightSwipe && previousUp == upSwipe && previousDown == downSwipe && previousForward == forwardSwipe && previousBackward == backwardSwipe){
					//same signature detection
					
					printf("\n SAME SIGNATURE DETECTED \n");
					
					newSignatureDetected = FALSE;
					aSignatureDetected = FALSE;
				}
				else{
					//perform normally
					startSigTime = previousSigTime;
					previousSigTime = currentSigTime;		
					currentSigTime = time(NULL);
					
					newSignatureDetected = TRUE;
					aSignatureDetected = TRUE;
					
					
					previousGestureStartCompare = gestureStartCompare;
					
					//update the previously full movement with the last component detected 
					lastFullLeft = previousLeft;
					lastFullRight = previousRight;
					lastFullUp = previousUp;
					lastFullDown = previousDown;
					lastFullForward = previousForward;
					lastFullBackward = previousBackward;
					
				}
				
				//updating the variable that holds all directions detected for the current movement
				previousLeft = leftSwipe;
				previousRight = rightSwipe;
				previousUp = upSwipe;
				previousDown = downSwipe;
				previousForward = forwardSwipe;
				previousBackward = backwardSwipe;
				
				gestureStartCompare = point;
				gestureDetectedLocation = point;

				/*
				 *timer = clock() + nTime * CPS;
				 */
				 
				
				//calculating the direction of previous movement
				if (gestureStartCompare.Y - previousGestureStartCompare.Y < 0) {
					changeInYBetweenGesturePoints = -1;
				}
				else {
					changeInYBetweenGesturePoints = 1;
				}
				
				if (gestureStartCompare.X - previousGestureStartCompare.X < 0) {
					changeInXBetweenGesturePoints = -1;
				}
				else {
					changeInXBetweenGesturePoints = 1;
				}
				

				
				//Update the naming for the current movement
				if (leftSwipe)
				{
					if (analyzeLeft)
					{
						numSignaturePerformed++;
						printf("Number of Left Signatures Detected: %d\n", numSignaturePerformed);
					}
					
					sprintf(strchr(sigPrint, 0), "left ");
					
					if (RecordPhase || DetectPhase) {
						sprintf(strchr(sigPrintLast, 0), "left ");
					}
				}
				
				if (rightSwipe)
				{
					if (analyzeRight)
					{
						numSignaturePerformed++;
						printf("Number of Right Signatures Detected: %d\n", numSignaturePerformed);
					}
					
					sprintf(strchr(sigPrint, 0), "right ");
					
					if (RecordPhase || DetectPhase) {
						sprintf(strchr(sigPrintLast, 0), "right ");
					}
				}
				
				if (upSwipe)
				{
					if (analyzeUp)
					{
						numSignaturePerformed++;
						printf("Number of Up Signatures Detected: %d\n", numSignaturePerformed);
					}
					
					sprintf(strchr(sigPrint, 0), "up ");
					
					if (RecordPhase || DetectPhase) {
						sprintf(strchr(sigPrintLast, 0), "up ");
					}
				}
				
				if (downSwipe)
				{
					if (analyzeDown)
					{
						numSignaturePerformed++;
						printf("Number of Down Signatures Detected: %d\n", numSignaturePerformed);
					}
					
					sprintf(strchr(sigPrint, 0), "down ");
					
					if (RecordPhase || DetectPhase) {
						sprintf(strchr(sigPrintLast, 0), "down ");
					}
				}
				
				if (forwardSwipe)
				{
					if (analyzeForward)
					{
						numSignaturePerformed++;
						printf("Number of Forward Signatures Detected: %d\n", numSignaturePerformed);
					}
					
					sprintf(strchr(sigPrint, 0), "forward ");
					
					if (RecordPhase || DetectPhase) {
						sprintf(strchr(sigPrintLast, 0), "forward ");
					}
				}
				
				if (backwardSwipe)
				{
					if (analyzeBackward)
					{
						numSignaturePerformed++;
						printf("Number of Backward Signatures Detected: %d\n", numSignaturePerformed);
					}
					
					sprintf(strchr(sigPrint, 0), "backward ");
					
					if (RecordPhase || DetectPhase) {
						sprintf(strchr(sigPrintLast, 0), "backward ");
					}
				}
				
				
				//Housekeeping
				
				/*
				 * Printing a new line to the fill that stores all detected movements once a new signature is begining
				 * Side Note: This file contain all signatures and does not indicate which signature is being attempted.  The specific movement attempts are stored in left.txt, right.txt, etc.
				 */
				myfile.open ("../../Config/signatures.txt",ios::app);
				myfile << "\n";
				myfile.close();
				
				leftSwipe = FALSE;
				rightSwipe = FALSE;
				upSwipe = FALSE;
				downSwipe = FALSE;
				forwardSwipe = FALSE;
				backwardSwipe = FALSE;
				
				contSignatureDetected = FALSE;
				
				return true;
			}
			
		}
		
	}
	return false;
}

//Get last fully completed movement in text
void DetectHandSwipe::GetLastFullSigPerformed(char CHAR_INFO[]){
	
	for (int i = 0; i < strlen(LastSignature); i++) {
		CHAR_INFO[i] = LastSignature[i];
	}	
}

//Get current completed movement in text
void DetectHandSwipe::GetLastSigPerformed(char CHAR_INFO[]){
	
	for (int i = 0; i < strlen(sigPrintLast); i++) {
		CHAR_INFO[i] = sigPrintLast[i];
	}	
}

bool DetectHandSwipe::GetLastLeft(){
	return lastFullLeft;
}

bool DetectHandSwipe::GetLastRight(){
	return lastFullRight;
}

bool DetectHandSwipe::GetLastUp(){
	return lastFullUp;
}

bool DetectHandSwipe::GetLastDown(){
	return lastFullDown;
}

bool DetectHandSwipe::GetLastForward(){
	return lastFullForward;
}

bool DetectHandSwipe::GetLastBackward(){
	return lastFullBackward;
}

bool DetectHandSwipe::GetPreviousLeft(){
	return previousLeft;
}

bool DetectHandSwipe::GetPreviousRight(){
	return previousRight;
}

bool DetectHandSwipe::GetPreviousUp(){
	return previousUp;
}

bool DetectHandSwipe::GetPreviousDown(){
	return previousDown;
}

bool DetectHandSwipe::GetPreviousForward(){
	return previousForward;
}

bool DetectHandSwipe::GetPreviousBackward(){
	return previousBackward;
}

void DetectHandSwipe::SetStartComparePoint(XnSkeletonJointPosition point){
	gestureStartCompare = point.position;
	
	lastFullLeft = FALSE;
	lastFullRight = FALSE;
	lastFullUp = FALSE;
	lastFullDown = FALSE;
	lastFullForward = FALSE;
	lastFullBackward = FALSE;
	
	previousLeft = FALSE;
	previousRight = FALSE;
	previousUp = FALSE;
	previousDown = FALSE;
	previousForward = FALSE;
	previousBackward = FALSE;
	
	leftSwipe = FALSE;
	rightSwipe = FALSE;
	upSwipe = FALSE;
	downSwipe = FALSE;
	forwardSwipe = FALSE;
	backwardSwipe = FALSE;
		
	timerReset = TRUE;
}

bool DetectHandSwipe::IsNewSignatureDetected(){
	return newSignatureDetected;
}

bool DetectHandSwipe::IsContSignatureDetected(){
	return contSignatureDetected;
}

bool DetectHandSwipe::IsaSignatureDetected(){
	return aSignatureDetected;
}

void DetectHandSwipe::SetNewSignatureDetected(bool detected){
	newSignatureDetected = detected;
}
void DetectHandSwipe::SetContSignatureDetected(bool detected){
	contSignatureDetected = detected;
}
void DetectHandSwipe::SetaSignatureDetected(bool detected){
	aSignatureDetected = detected;
}

XnPoint3D DetectHandSwipe::GetGestureStartCompare(){
	return gestureStartCompare;
}
