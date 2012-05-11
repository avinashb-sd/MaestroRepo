/*
 *  SignatureDetection.h
 *  
 *
 *  Created by Avinash BharathSingh on 11/17/11.
 *  Copyright 2011 The George Washington University. All rights reserved.
 *
 */


#ifndef SIGNATUREDETECTION_H
#define SIGNATUREDETECTION_H

#include <XnCppWrapper.h>
#include <XnOpenNI.h>
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

// required distance for a signature
#define MIN_DISTANCE 250 
#define MIN_LATERAL_DISTANCE 400
#define MIN_HEIGHT_DISTANCE 400
#define MIN_DEPTH_DISTANCE 400


// max mount of variation on other axes
#define MAX_DEVIATION 150

bool IsMovementLongEnough(XnPoint3D point1, XnPoint3D point2);

bool LeftSignature(XnPoint3D point1, XnPoint3D point2);

bool RightSignature(XnPoint3D point1, XnPoint3D point2);

bool UpSignature(XnPoint3D point1, XnPoint3D point2);

bool DownSignature(XnPoint3D point1, XnPoint3D point2);

bool ForwardSignature(XnPoint3D point1, XnPoint3D point2);

bool BackwardSignature(XnPoint3D point1, XnPoint3D point2);


bool LeftSignature(float point1, float point2);

bool RightSignature(float point1, float point2);

bool UpSignature(float point1, float point2);

bool DownSignature(float point1, float point2);

bool ForwardSignature(float point1, float point2);

bool BackwardSignature(float point1, float point2);

#endif