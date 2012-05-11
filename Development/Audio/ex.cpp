/*
 *  ex.cpp
 *  
 *
 *  Created by Avinash BharathSingh on 10/14/11.
 *  Copyright 2011 The George Washington University. All rights reserved.
 *
 */


/* system example : DIR */
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <string>
#include <sys/types.h>
#include <unistd.h>
#include <fstream>
#include <time.h>

using namespace std;

int globalVariable = 2;

void wait ( int seconds )
{
  clock_t endwait;
  endwait = clock () + seconds * CLOCKS_PER_SEC ;
  while (clock() < endwait) {}
}

main()
{
	string sIdentifier;
	int    iStackVariable = 20;
	
	pid_t pID = fork();
	if (pID == 0)                // child
	{
		// Code only executed by child process
		int i;
		printf ("Checking if processor is available...");
		if (system(NULL)) puts ("Ok");
		else exit (1);
		i=system ("pwd");
		i=system ("./GroupA");

		sIdentifier = "Child Process: ";
		globalVariable++;
		iStackVariable++;
    }
    else if (pID < 0)            // failed to fork
    {
        cerr << "Failed to fork" << endl;
        exit(1);
        // Throw exception
    }
    else                                   // parent
    {
		// Code only executed by parent process
		ofstream myfile;
		myfile.open ("../../Config/groupa.txt",ios::app);
		wait(3);
		myfile << "start\n";
		myfile.close();	
		
		myfile.open ("../../Config/groupa.txt",ios::app);
		wait(3);
		myfile << "iVol\n";
		myfile.close();	
		
		sIdentifier = "Parent Process:";
    }
	
    // Code executed by both parent and child.
	
    cout << sIdentifier;
	 cout << "\n Global variable: " << globalVariable;
	 cout << "\n Stack variable: "  << iStackVariable << endl;
	 
	 return 0;
}