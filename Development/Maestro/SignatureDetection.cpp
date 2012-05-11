/*
 *  SignatureDetection.cpp
 *  
 *
 *  Created by Avinash BharathSingh on 11/17/11.
 *  Copyright 2011 The George Washington University. All rights reserved.
 *
 */

#include "SignatureDetection.h"

using namespace std;

extern queue<XnPoint3D> points;
extern int previousSignature;

bool IsMovementLongEnough(XnPoint3D point1, XnPoint3D point2)
{
	//Distance between 2 3-dimensional points meet requirements
	if(sqrtf( powf((point2.X - point1.X), 2.0f) +  powf((point2.Y - point1.Y), 2.0f) + powf((point2.Z - point1.Z), 2.0f)) > MIN_DISTANCE){
		return true;
	}
	return false;
}

bool LateralMovement(XnPoint3D point1, XnPoint3D point2){
		//if X coordinates travel more than defined
		if(abs(point2.X - point1.X) > MIN_DISTANCE)
		{
			//printf("LATERAL MOVEMENT\n");
			return true;
		}

	return false;
}

bool HeightMovement(XnPoint3D point1, XnPoint3D point2){
	//if Y coordinates travel more than defined
	if(abs(point2.Y - point1.Y) > MIN_DISTANCE)
	{
		//printf("HEIGHT MOVEMENT\n");
		return true;
	}
	
	return false;
}

bool DepthMovement(XnPoint3D point1, XnPoint3D point2){
	//if Z coordinates travel more than defined
	if(abs(point2.Z - point1.Z) > MIN_DISTANCE)
	{
		//printf("DEPTH MOVEMENT\n");
		return true;
	}
	
	return false;
}

bool LeftSignature(XnPoint3D point1, XnPoint3D point2)
{
	if(LateralMovement(point1,point2) && point2.X < point1.X){
		//printf("LEFT MOVEMENT	");
		//previousSignature = 1;
		ofstream myfile;
		myfile.open ("../../Config/signatures.txt",ios::app);
		myfile << "left	";
		myfile.close();
		return true;
	}
	
	return false;
}

bool RightSignature(XnPoint3D point1, XnPoint3D point2)
{
	if(LateralMovement(point1,point2) && point2.X > point1.X){
		//printf("RIGHT MOVEMENT	");
		//previousSignature = 2;
		ofstream myfile;
		myfile.open ("../../Config/signatures.txt",ios::app);
		myfile << "right	";
		myfile.close();
		return true;
	}
	return false;
}

bool UpSignature(XnPoint3D point1, XnPoint3D point2)
{
	if(HeightMovement(point1,point2) && point2.Y > point1.Y){
		//printf("UP MOVEMENT	");
		//previousSignature = 3;
		ofstream myfile;
		myfile.open ("../../Config/signatures.txt",ios::app);
		myfile << "up	";
		myfile.close();
		return true;
	}
	return false;
}

bool DownSignature(XnPoint3D point1, XnPoint3D point2)
{
	if(HeightMovement(point1,point2) && point2.Y < point1.Y){
		//printf("DOWN MOVEMENT	");
		//previousSignature = 4;
		ofstream myfile;
		myfile.open ("../../Config/signatures.txt",ios::app);
		myfile << "down	";
		myfile.close();
		return true;
	}
	return false;
}

bool ForwardSignature(XnPoint3D point1, XnPoint3D point2)
{
	if(DepthMovement(point1,point2) && point2.Z < point1.Z){
		//printf("FORWARD MOVEMENT	");
		//previousSignature = 5;
		ofstream myfile;
		myfile.open ("../../Config/signatures.txt",ios::app);
		myfile << "forward	";
		myfile.close();
		return true;
	}
	return false;
}

bool BackwardSignature(XnPoint3D point1, XnPoint3D point2)
{
	if(DepthMovement(point1,point2) && point2.Z > point1.Z){
		//printf("BACKWARD MOVEMENT	");
		//previousSignature = 6;
		ofstream myfile;
		myfile.open ("../../Config/signatures.txt",ios::app);
		myfile << "back	";
		myfile.close();
		return true;
	}
	return false;
}




bool LateralMovement(float point1, float point2){
	//if X coordinates travel more than defined
	if(abs(point2 - point1) > MIN_DISTANCE)
	{
		//printf("LATERAL MOVEMENT\n");
		return true;
	}
	
	return false;
}

bool HeightMovement(float point1, float point2){
	//if Y coordinates travel more than defined
	if(abs(point2 - point1) > MIN_DISTANCE)
	{
		//printf("HEIGHT MOVEMENT\n");
		return true;
	}
	
	return false;
}

bool DepthMovement(float point1, float point2){
	//if Z coordinates travel more than defined
	if(abs(point2 - point1) > MIN_DISTANCE)
	{
		//printf("DEPTH MOVEMENT\n");
		return true;
	}
	
	return false;
}

bool LeftSignature(float point1, float point2)
{
	if(LateralMovement(point1,point2) && point2 < point1){
		//printf("LEFT MOVEMENT	");
		//previousSignature = 1;
		ofstream myfile;
		myfile.open ("../../Config/signatures.txt",ios::app);
		myfile << "left	";
		myfile.close();
		return true;
	}
	
	return false;
}

bool RightSignature(float point1, float point2)
{
	if(LateralMovement(point1,point2) && point2 > point1){
		//printf("RIGHT MOVEMENT	");
		//previousSignature = 2;
		ofstream myfile;
		myfile.open ("../../Config/signatures.txt",ios::app);
		myfile << "right	";
		myfile.close();
		return true;
	}
	return false;
}

bool UpSignature(float point1, float point2)
{
	if(HeightMovement(point1,point2) && point2 > point1){
		//printf("UP MOVEMENT	");
		//previousSignature = 3;
		ofstream myfile;
		myfile.open ("../../Config/signatures.txt",ios::app);
		myfile << "up	";
		myfile.close();
		return true;
	}
	return false;
}

bool DownSignature(float point1, float point2)
{
	if(HeightMovement(point1,point2) && point2 < point1){
		//printf("DOWN MOVEMENT	");
		//previousSignature = 4;
		ofstream myfile;
		myfile.open ("../../Config/signatures.txt",ios::app);
		myfile << "down	";
		myfile.close();
		return true;
	}
	return false;
}

bool ForwardSignature(float point1, float point2)
{
	if(DepthMovement(point1,point2) && point2 < point1){
		//printf("FORWARD MOVEMENT	");
		//previousSignature = 5;
		ofstream myfile;
		myfile.open ("../../Config/signatures.txt",ios::app);
		myfile << "forward	";
		myfile.close();
		return true;
	}
	return false;
}

bool BackwardSignature(float point1, float point2)
{
	if(DepthMovement(point1,point2) && point2 > point1){
		//printf("BACKWARD MOVEMENT	");
		//previousSignature = 6;
		ofstream myfile;
		myfile.open ("../../Config/signatures.txt",ios::app);
		myfile << "back	";
		myfile.close();
		return true;
	}
	return false;
}