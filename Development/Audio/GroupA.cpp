#include "../../Include/wincompat.h"
#include "../../Include/fmod.hpp"
#include "../../Include/fmod_errors.h"
#include <iostream>
#include <fstream>
#include <string>
#include <math.h>

using namespace std;

void ERRCHECK(FMOD_RESULT result)
{
    if (result != FMOD_OK)
    {
        printf("FMOD error! (%d) %s\n", result, FMOD_ErrorString(result));
        exit(-1);
    }
}

int main(int argc, char *argv[])
{
	int numSounds = 1;
    FMOD::System        *system;
    FMOD::Sound         *sound[numSounds];
    FMOD::Channel       *channel[numSounds];
	FMOD::DSP          *mydsp;
    FMOD::DSP           *dsppitch;
	FMOD::DSP        *dsplowpass    = 0;
    FMOD::DSP        *dsphighpass   = 0;
    FMOD::DSP        *dspecho       = 0;
    FMOD::DSP        *dspflange     = 0;
    FMOD::DSP        *dspdistortion = 0;
    FMOD::DSP        *dspchorus     = 0;
    FMOD::DSP        *dspparameq    = 0;
    FMOD_RESULT          result;
    int                  key;
    unsigned int         version;
    float                pan = 0;
	float                volume;
	float                frequency;
	int					tempFrequency = 0;
	int					tempPitch = 0;
	int					frequencyCount = 0;
	int					tempoChange = 0;
	float                speed;
	float				pitch = 1;
	string				line;
	int					lineCount = 0;
	int inc = 0;
	int count;
	int pitchCount;
	float pitchf = 1.0f;
	
	float frequencyArray[numSounds];
	float frequencyCountArray[numSounds];
	float pitchfArray[numSounds];
	float volumeArray[numSounds];
	float panArray[numSounds];
	
    /*
	 Create a System object and initialize.
	 */
    result = FMOD::System_Create(&system);
    ERRCHECK(result);
	
    result = system->getVersion(&version);
    ERRCHECK(result);
	
    if (version < FMOD_VERSION)
    {
        printf("Error!  You are using an old version of FMOD %08x.  This program requires %08x\n", version, FMOD_VERSION);
        return 0;
    }
	
    result = system->init(32, FMOD_INIT_NORMAL, 0);
    ERRCHECK(result);
	
	result = system->createSound("../Config/Seven Nation Army Bass.mp3", FMOD_SOFTWARE | FMOD_LOOP_NORMAL | FMOD_2D, 0, &sound[0]);
    ERRCHECK(result);

	
    printf("===============================================================================\n");
    printf("									Maestro									   \n");
    printf("===============================================================================\n");
	printf("Press '1' to play left speaker only\n");
    printf("Press '2' to play right speaker only\n");
	printf("Press '3' to play from both speakers\n");
    printf("Press '[' to pan sound left\n");
    printf("Press ']' to pan sound right\n");
	printf("Press 'v/V' to increase volume\n");
	printf("Press 'd/D' to decrease volume\n");
	printf("Press '6' pitch octave down\n");
	printf("Press '7' pitch semitone down\n");
	printf("Press '8' pitch semitone up\n");
	printf("Press '9' pitch octave up\n");
    printf("Press 'Esc' to quit\n");
	printf("Press 'n' pitch scale down\n");
    printf("Press 'm' pitch scale up\n");
    printf("\n");
	
	for (count = 0; count < numSounds; count++)
    {
		result = system->playSound(FMOD_CHANNEL_FREE, sound[count], false, &channel[count]);
		ERRCHECK(result);
		
		bool paused;
		
		channel[0]->getPaused(&paused);
		ERRCHECK(result);
		
		paused = !paused;
		
		result = channel[0]->setPaused(paused);
		ERRCHECK(result);
	}
	
	
	//Create the DSP effects.
	result = system->createDSPByType(FMOD_DSP_TYPE_PITCHSHIFT, &dsppitch);
    ERRCHECK(result);
	result = system->createDSPByType(FMOD_DSP_TYPE_LOWPASS, &dsplowpass);
    ERRCHECK(result);
    result = system->createDSPByType(FMOD_DSP_TYPE_HIGHPASS, &dsphighpass);
    ERRCHECK(result);
    result = system->createDSPByType(FMOD_DSP_TYPE_ECHO, &dspecho);
    ERRCHECK(result);
    result = system->createDSPByType(FMOD_DSP_TYPE_FLANGE, &dspflange);
    ERRCHECK(result);
    result = system->createDSPByType(FMOD_DSP_TYPE_DISTORTION, &dspdistortion);
    ERRCHECK(result);
    result = system->createDSPByType(FMOD_DSP_TYPE_CHORUS, &dspchorus);
    ERRCHECK(result);
    result = system->createDSPByType(FMOD_DSP_TYPE_PARAMEQ, &dspparameq);
    ERRCHECK(result);
		
    /*
	 Main loop.
	 */
	
	for (count = 0; count < numSounds; count++)
    {
		channel[count]->getVolume(&volume);
		channel[count]->setVolume(.3);
		channel[count]->getVolume(&volume);
		
	}
	printf("Initial Volume: %f \n", volume);
	
	channel[0]->getFrequency(&frequency);
	printf("Initial Frequency: %f \n", frequency);
	
	
    /*
	 Main loop.
	 */
    do
    {
		ifstream myfile;
		myfile.open("../../Config/groupa.txt");
		while ( myfile.good() )
		{
			//printf("Line Count: %d \n", lineCount);
			for(int i = 0; i < lineCount; i++){
				getline (myfile,line);
			}
			getline (myfile,line);
			if (line.compare("start") == 0){
				bool paused;
				
				channel[0]->getPaused(&paused);
				ERRCHECK(result);
				
				paused = !paused;
				
				result = channel[0]->setPaused(paused);
				ERRCHECK(result);
				lineCount++;
				//printf("Line Count: %d \n", lineCount);
				break;
			}
			if (line.compare("iVol") == 0){
				channel[0]->getVolume(&volume);
				channel[0]->setVolume(1.0f);
				channel[0]->getVolume(&volume);
				//printf("			VOLUME: %f \n", volume);
				lineCount++;
				//printf("Line Count: %d \n", lineCount);
				break;
			}
			if (line.compare("dVol") == 0){
				channel[0]->getVolume(&volume);
				channel[0]->setVolume(.1);
				lineCount++;
				//printf("Line Count: %d \n", lineCount);
				break;
			}
			if (line.compare("iPitch") == 0){
				
				result = dsppitch->remove();
				ERRCHECK(result);
				
				result = system->addDSP(dsppitch, 0);
				ERRCHECK(result);
				
				inc++;
				pitch = pow(1.059f,inc);
				
				result = dsppitch->setParameter(FMOD_DSP_PITCHSHIFT_PITCH, pitch);
				ERRCHECK(result);
				
				lineCount++;
				//printf("Line Count: %d \n", lineCount);
				break;
			}
			if (line.compare("dPitch") == 0){
				result = dsppitch->remove();
				ERRCHECK(result);
				
				result = system->addDSP(dsppitch, 0);
				ERRCHECK(result);
				
				inc--;
				pitch = pow(1.059,inc);
				
				result = dsppitch->setParameter(FMOD_DSP_PITCHSHIFT_PITCH, pitch);
				ERRCHECK(result);
				
				
				lineCount++;
				//printf("Line Count: %d \n", lineCount);
				break;
			}
			if (line.compare("left") == 0){
				result = channel[0]->setSpeakerMix(1.0f, 0, 0, 0, 0, 0, 0, 0);
				ERRCHECK(result);
				lineCount++;
				//printf("Line Count: %d \n", lineCount);
				break;
			}
			if (line.compare("right") == 0){
				result = channel[0]->setSpeakerMix(0, 1.0f, 0, 0, 0, 0, 0, 0);
				ERRCHECK(result);
				lineCount++;
				//printf("Line Count: %d \n", lineCount);
				break;
			}
			if (line.compare("forward") == 0){
				result = channel[0]->setSpeakerMix(1.0f, 1.0f, 0, 0, 0, 0, 0, 0);
				ERRCHECK(result);
				lineCount++;
				//printf("Line Count: %d \n", lineCount);
				break;
			}
			if (line.compare("pitch") == 0){
				bool active;
				
				result = dsppitch->getActive(&active);
				ERRCHECK(result);
				
				if (active)
				{
					result = dsppitch->remove();
					ERRCHECK(result);
				}
				else
				{
					result = system->addDSP(dsppitch, 0);
					ERRCHECK(result);
					
					result = dsppitch->setParameter(FMOD_DSP_PITCHSHIFT_PITCH, 1.2f);
					ERRCHECK(result);
					
				}
				
				lineCount++;
				break;
				
			}
			if (line.compare("pause") == 0){
				bool paused;
				
				channel[0]->getPaused(&paused);
				ERRCHECK(result);
				
				paused = !paused;
				
				result = channel[0]->setPaused(paused);
				ERRCHECK(result);
				lineCount++;
				break;
				
			}
			
			if (line.compare("lowpass") == 0){
				bool active;
				
				result = dsplowpass->getActive(&active);
				ERRCHECK(result);
				
				if (active)
				{
					result = dsplowpass->remove();
					ERRCHECK(result);
				}
				else
				{
					result = system->addDSP(dsplowpass, 0);
					ERRCHECK(result);
				}
				lineCount++;
				break;
				
			}
			
			if (line.compare("highpass") == 0){
				bool active;
				
				result = dsphighpass->getActive(&active);
				ERRCHECK(result);
				
				if (active)
				{
					result = dsphighpass->remove();
					ERRCHECK(result);
				}
				else
				{
					result = system->addDSP(dsphighpass, 0);
					ERRCHECK(result);
				}
				lineCount++;
				break;
				
			}
			
			if (line.compare("echo") == 0){
				bool active;
				
				result = dspecho->getActive(&active);
				ERRCHECK(result);
				
				if (active)
				{
					result = dspecho->remove();
					ERRCHECK(result);
				}
				else
				{
					result = system->addDSP(dspecho, 0);
					ERRCHECK(result);
					
					result = dspecho->setParameter(FMOD_DSP_ECHO_DELAY, 50.0f);
					ERRCHECK(result);
				}
				lineCount++;
				break;
				
			}
			
			if (line.compare("flange") == 0){
				bool active;
				
				result = dspflange->getActive(&active);
				ERRCHECK(result);
				
				if (active)
				{
					result = dspflange->remove();
					ERRCHECK(result);
				}
				else
				{
					result = system->addDSP(dspflange, 0);
					ERRCHECK(result);
				}
				lineCount++;
				break;
				
			}
			
			if (line.compare("distortion") == 0){
				bool active;
				
				result = dspdistortion->getActive(&active);
				ERRCHECK(result);
				
				if (active)
				{
					result = dspdistortion->remove();
					ERRCHECK(result);
				}
				else
				{
					result = system->addDSP(dspdistortion, 0);
					ERRCHECK(result);
					
					result = dspdistortion->setParameter(FMOD_DSP_DISTORTION_LEVEL, 0.8f);
					ERRCHECK(result);
				}
				lineCount++;
				break;
				
			}
			
			if (line.compare("chorus") == 0){
				bool active;
				
				result = dspchorus->getActive(&active);
				ERRCHECK(result);
				
				if (active)
				{
					result = dspchorus->remove();
					ERRCHECK(result);
				}
				else
				{
					result = system->addDSP(dspchorus, 0);
					ERRCHECK(result);
				}
				lineCount++;
				break;
				
			}
			
			if (line.compare("parameq") == 0){
				bool active;
				
				result = dspparameq->getActive(&active);
				ERRCHECK(result);
				
				if (active)
				{
					result = dspparameq->remove();
					ERRCHECK(result);
				}
				else
				{
					result = system->addDSP(dspparameq, 0);
					ERRCHECK(result);
					
					result = dspparameq->setParameter(FMOD_DSP_PARAMEQ_CENTER, 5000.0f);
					ERRCHECK(result);
					result = dspparameq->setParameter(FMOD_DSP_PARAMEQ_GAIN, 0.0f);
					ERRCHECK(result);
				}
				lineCount++;
				break;					
			}
			
			if (line.compare("iTempo") == 0){
				
				//44100
				//62366.816406
				
				channel[0]->getFrequency(&frequency);
				
				float base = 2.0f;
				float newTempo = frequency * pow(base,(6.0f/12.0f));
				
				float pitchf = 1.0f;
				pitchf = pow(.9438f,6);
				
				
				result = dsppitch->remove();
				ERRCHECK(result);
				
				result = system->addDSP(dsppitch, 0);
				ERRCHECK(result);
				
				channel[0]->setFrequency(newTempo);
				
				
				result = dsppitch->setParameter(FMOD_DSP_PITCHSHIFT_PITCH, pitchf);
				ERRCHECK(result);
				
				channel[0]->getFrequency(&frequency);
				
				lineCount++;
				break;
			}
			
			if (line.compare("dTempo") == 0){
				
				channel[0]->getFrequency(&frequency);
				
				float base = 2.0f;
				float newTempo = frequency * pow(base,(-6.0f/12.0f));
				
				float pitchf = 1.0f;
				
				result = dsppitch->remove();
				ERRCHECK(result);
				
				result = system->addDSP(dsppitch, 0);
				ERRCHECK(result);
				
				channel[0]->setFrequency(newTempo);
				
				
				result = dsppitch->setParameter(FMOD_DSP_PITCHSHIFT_PITCH, pitchf);
				ERRCHECK(result);
				
				channel[0]->getFrequency(&frequency);
				
				lineCount++;
				break;
			}
			
			if (line.compare("quit") == 0){
				key = 27;
				lineCount++;
				//printf("Line Count: %d \n", lineCount);
				break;
			}
			
		}
		myfile.close();
		
		
        if (kbhit())
        {
            key = getch();
			
            switch (key)
            {
				case 's' :
                {
					bool paused;
					
					channel[0]->getPaused(&paused);
					ERRCHECK(result);
					
					paused = !paused;
					
					result = channel[0]->setPaused(paused);
					ERRCHECK(result);
					break;
				}
				case '1' :
                {
					//	Play Sound From Left Speakers
					for (count = 0; count < numSounds; count++)
					{
						channel[count]->getVolume(&volume);
						result = channel[count]->setSpeakerMix(1.0f, 0, 0, 0, 0, 0, 0, 0);
						ERRCHECK(result);
					}
                    break;
                }
                case '2' :
                {
					//	Play Sound From Right Speakers
					for (count = 0; count < numSounds; count++)
					{
					channel[count]->getVolume(&volume);
					printf("Volume: %f\n", volume);
                    result = channel[count]->setSpeakerMix(0, 1.0f, 0, 0, 0, 0, 0, 0);
                    ERRCHECK(result);
					}
                    break;
                }
				case '3' :
                {
					//	Play Sound From Both Speakers
					for (count = 0; count < numSounds; count++)
					{
					channel[count]->getVolume(&volume);
                    result = channel[count]->setSpeakerMix(1.0f, 1.0f, 0, 0, 0, 0, 0, 0);
                    ERRCHECK(result);
					}
                    break;
                }	
                case '4' : 
                {
					//	Decrmenent Tempo
					//if(frequencyCount > -12){
					//if (tempoChange + pitchCount > -12 && abs(pitchCount) < 12 && tempoChange > -12) {
					if (pitchf < 1.98f) {
						frequencyCount--;
						tempoChange++;
						printf("Pitch Count: %d\n", pitchCount);
						printf("Tempo Count: %d\n", tempoChange);
						
						channel[0]->getFrequency(&frequency);
						//printf("Z Initial Frequency: %f \n", frequency);
						
						float base = 2.0f;
						
						//printf("inc by: %f \n", pow(base,(-1.0f/12.0f)));
						float newTempo = frequency * pow(base,(-1.0f/12.0f));

						if (pitchf == 1.0f) {
							pitchf = pow(1.059f,abs(tempoChange + pitchCount));
						}
						else if (pitchf > 1.0f) {
							pitchf = pow(1.059f,abs(tempoChange + pitchCount));
						}
						else if (pitchf < 1.0f) {
							pitchf = pow(.9438f,abs(tempoChange + pitchCount));
						}
						
						printf("pitchf: %f\n", pitchf);
						printf("Frequency: %f \n", newTempo);
						
						result = dsppitch->remove();
						ERRCHECK(result);
						
						result = system->addDSP(dsppitch, 0);
						ERRCHECK(result);
						
						channel[0]->setFrequency(newTempo);
						
						result = dsppitch->setParameter(FMOD_DSP_PITCHSHIFT_PITCH, pitchf);
						ERRCHECK(result);

					}
					else {
						printf("Reached Min Tempo\n");
					}
					
					
                    break;
                }
				case '5' : 
                {
					//	Incrmenent Tempo

					if (pitchf > 0.52f) {
						frequencyCount++;
						tempoChange--;
						printf("Pitch Count: %d\n", pitchCount);
						printf("Tempo Count: %d\n", tempoChange);
						
						channel[0]->getFrequency(&frequency);
						//printf("Z Initial Frequency: %f \n", frequency);
						
						float base = 2.0f;
						
						float newTempo = frequency * pow(base,(1.0f/12.0f));

						if (pitchf == 1.0f) {
							printf("tempo one\n");
							pitchf = pow(.9438f,abs(tempoChange + pitchCount));
						}
						else if (pitchf > 1.0f) {
							printf("tempo inc\n");
							pitchf = pow(1.059f,abs(tempoChange + pitchCount));
						}
						else if (pitchf < 1.0f) {
							printf("tempo dec\n");
							pitchf = pow(.9438f,abs(tempoChange + pitchCount));
						}

						printf("pitchf: %f\n", pitchf);
						
						printf("Frequency: %f \n", newTempo);

						result = dsppitch->remove();
						ERRCHECK(result);
						
						result = system->addDSP(dsppitch, 0);
						ERRCHECK(result);
						
						channel[0]->setFrequency(newTempo);
											
						result = dsppitch->setParameter(FMOD_DSP_PITCHSHIFT_PITCH, pitchf);
						ERRCHECK(result);
						
					}
					else {
						printf("Reached Max Tempo\n");
					}
					
					break;
                }	
					
				case '6' : 
                {
					//	Pitch Shift Down Octave
                    bool active;
					
                    result = dsppitch->getActive(&active);
                    ERRCHECK(result);
					
                    if (active)
                    {
                        result = dsppitch->remove();
                        ERRCHECK(result);
                    }
                    else
                    {
                        result = system->addDSP(dsppitch, 0);
                        ERRCHECK(result);
						
						result = dsppitch->setParameter(FMOD_DSP_PITCHSHIFT_PITCH, 0.5f);
                        ERRCHECK(result);
						
                    }
                    break;
                }
					
				case '7' :
                {
					//	Pitch Shift Down One Note
                    bool active;
					
                    result = dsppitch->getActive(&active);
                    ERRCHECK(result);
					
                    if (active)
                    {
                        result = dsppitch->remove();
                        ERRCHECK(result);
                    }
                    else
                    {
                        result = system->addDSP(dsppitch, 0);
                        ERRCHECK(result);
						
						result = dsppitch->setParameter(FMOD_DSP_PITCHSHIFT_PITCH, .9438f);
                        ERRCHECK(result);
						
                    }
                    break;
                }
					
				case '8' :
                {
					//	Pitch Shift Up One Note
                    bool active;
					
                    result = dsppitch->getActive(&active);
                    ERRCHECK(result);
					
                    if (active)
                    {
                        result = dsppitch->remove();
                        ERRCHECK(result);
                    }
                    else
                    {
                        result = system->addDSP(dsppitch, 0);
                        ERRCHECK(result);
						
						result = dsppitch->setParameter(FMOD_DSP_PITCHSHIFT_PITCH, 1.059f);
                        ERRCHECK(result);
						
                    }
                    break;
                }
					
				case '9' :
                {
					//	Pitch Shift Up Octave
                    bool active;
					
                    result = dsppitch->getActive(&active);
                    ERRCHECK(result);
					
                    if (active)
                    {
                        result = dsppitch->remove();
                        ERRCHECK(result);
                    }
                    else
                    {
                        result = system->addDSP(dsppitch, 0);
                        ERRCHECK(result);
						
						result = dsppitch->setParameter(FMOD_DSP_PITCHSHIFT_PITCH, 2.0f);
                        ERRCHECK(result);
						
                    }
                    break;
                }
					
					
				case 'v' : 
                case 'V' : 
                {
					//	Increment Volume
					for (count = 0; count < numSounds; count++)
					{
					channel[count]->getVolume(&volume);
                    volume += 0.1f;
                    if (volume > 1)
                    {
						volume = 1;
                    }
					//printf("Volume: %f \n", volume);
                    channel[count]->setVolume(volume);
					}
                    break;
                }
				case 'd' : 
                case 'D' : 
                {
					//	Decrement Volume
					for (count = 0; count < numSounds; count++)
					{
					channel[count]->getVolume(&volume);
                    volume -= 0.1f;
                    if (volume < 0)
                    {
						volume = 0;
                    }
					//printf("Volume: %f \n", volume);
                    channel[count]->setVolume(volume);
					}
                    break;
                }
                case '[' :
                {
					// Pan Left
					for (count = 0; count < numSounds; count++)
					{
                    channel[count]->getPan(&pan);
                    pan -= 0.1f;
                    if (pan < -1)
                    {
                        pan = -1;
                    }
                    channel[count]->setPan(pan);
					}
                    break;
                }
                case ']' :
                {
					//	Pan Right
					for (count = 0; count < numSounds; count++)
					{
                    channel[count]->getPan(&pan);
                    pan += 0.1f;
                    if (pan > 1)
                    {
                        pan = 1;
                    }
                    channel[count]->setPan(pan);
					}
                    break;
                }
					
				case 'n' :
                {
					//	Decremental Pitch
					if (pitchf > 0.52f) {
						frequencyCount--;
						pitchCount--;
						printf("Pitch Count: %d\n", pitchCount);
						printf("Tempo Count: %d\n", tempoChange);
						
						if (pitchf == 1.0f) {
							pitchf = pow(.9438f,abs(tempoChange + pitchCount));
						}
						else if (pitchf > 1.0f) {
							pitchf = pow(1.059f,tempoChange + pitchCount);
						}
						else if (pitchf < 1.0f) {
							pitchf = pow(.9438f,abs(tempoChange + pitchCount));
						}						
						
						printf("pitchf: %f\n", pitchf);
						
						result = dsppitch->remove();
						ERRCHECK(result);
						
						result = system->addDSP(dsppitch, 0);
						ERRCHECK(result);
						
						result = dsppitch->setParameter(FMOD_DSP_PITCHSHIFT_PITCH, pitchf);
						ERRCHECK(result);
					}
					else {
						printf("Reached Min Pitch\n");
					}
					
					
					
                    break;
                }
					
					
				case 'm' :
                {
					//	Incremental Pitch
					if (pitchf < 1.98f) {
						frequencyCount++;
						pitchCount++;
						printf("Pitch Count: %d\n", pitchCount);
						printf("Tempo Count: %d\n", tempoChange);
						
						if (pitchf == 1.0f) {
							pitchf = pow(1.059f,abs(tempoChange + pitchCount));
						}
						else if (pitchf > 1.0f) {
							pitchf = pow(1.059f,tempoChange + pitchCount);
						}
						else if (pitchf < 1.0f) {
							pitchf = pow(.9438f,abs(tempoChange + pitchCount));
						}
						
						printf("pitchf: %f\n", pitchf);
						
						result = dsppitch->remove();
						ERRCHECK(result);
						
						result = system->addDSP(dsppitch, 0);
						ERRCHECK(result);
						
						
						result = dsppitch->setParameter(FMOD_DSP_PITCHSHIFT_PITCH, pitchf);
						ERRCHECK(result);
					}
					else {
						printf("Reached Max Pitch\n");
					}
					
                    break;
                }
					
				case 'r' :
                {
					//	Lowpass
					bool active;
					
                    result = dsplowpass->getActive(&active);
                    ERRCHECK(result);
					
                    if (active)
                    {
                        result = dsplowpass->remove();
                        ERRCHECK(result);
                    }
                    else
                    {
                        result = system->addDSP(dsplowpass, 0);
                        ERRCHECK(result);
                    }
                    break;
					
				}
					
				case 't' :
                {
					// Highpass
					bool active;
					
                    result = dsphighpass->getActive(&active);
                    ERRCHECK(result);
					
                    if (active)
                    {
                        result = dsphighpass->remove();
                        ERRCHECK(result);
                    }
                    else
                    {
                        result = system->addDSP(dsphighpass, 0);
                        ERRCHECK(result);
                    }
                    break;
					
				}
					
				case 'y' :
                {
					//	Echo
					bool active;
					
                    result = dspecho->getActive(&active);
                    ERRCHECK(result);
					
                    if (active)
                    {
                        result = dspecho->remove();
                        ERRCHECK(result);
                    }
                    else
                    {
                        result = system->addDSP(dspecho, 0);
                        ERRCHECK(result);
						
                        result = dspecho->setParameter(FMOD_DSP_ECHO_DELAY, 100.0f);
                        ERRCHECK(result);
                    }
                    break;
					
				}
					
				case 'u' :
                {
					//	Flange
					bool active;
					
                    result = dspflange->getActive(&active);
                    ERRCHECK(result);
					
                    if (active)
                    {
                        result = dspflange->remove();
                        ERRCHECK(result);
                    }
                    else
                    {
                        result = system->addDSP(dspflange, 0);
                        ERRCHECK(result);
                    }
                    break;
					
				}
					
				case 'i' :
                {
					//	Distortion
					bool active;
					
                    result = dspdistortion->getActive(&active);
                    ERRCHECK(result);
					
                    if (active)
                    {
                        result = dspdistortion->remove();
                        ERRCHECK(result);
                    }
                    else
                    {
                        result = system->addDSP(dspdistortion, 0);
                        ERRCHECK(result);
						
                        result = dspdistortion->setParameter(FMOD_DSP_DISTORTION_LEVEL, 0.8f);
                        ERRCHECK(result);
                    }
                    break;
					
				}
					
				case 'o' :
                {
					//	Chorus
					bool active;
					
                    result = dspchorus->getActive(&active);
                    ERRCHECK(result);
					
                    if (active)
                    {
                        result = dspchorus->remove();
                        ERRCHECK(result);
                    }
                    else
                    {
                        result = system->addDSP(dspchorus, 0);
                        ERRCHECK(result);
                    }
                    break;
					
				}
					
				case 'p' :
                {
					//	Parameq
					bool active;
					
                    result = dspparameq->getActive(&active);
                    ERRCHECK(result);
					
                    if (active)
                    {
                        result = dspparameq->remove();
                        ERRCHECK(result);
                    }
                    else
                    {
                        result = system->addDSP(dspparameq, 0);
                        ERRCHECK(result);
						
                        result = dspparameq->setParameter(FMOD_DSP_PARAMEQ_CENTER, 5000.0f);
                        ERRCHECK(result);
                        result = dspparameq->setParameter(FMOD_DSP_PARAMEQ_GAIN, 0.0f);
                        ERRCHECK(result);
                    }
                    break;
					
				}
            }
        }
		
        system->update();
		
        {
            int  channelsplaying = 0;
			
			bool dsppitch_active;
			dsppitch	 ->getActive(&dsppitch_active);
			
            system->getChannelsPlaying(&channelsplaying);
			
        }
		
        fflush(stdout);
        Sleep(10);
		
    } while (key != 27);
	
	{
		for (count = 0; count < numSounds; count++)
		{
		channel[count]->getFrequency(&frequency);
		channel[count]->getVolume(&volume);
		int count;
		float initFreq = frequency;
		float initVol = volume;
		
        for (count = 0; count < 200; count++)
        {
			//printf("End Frequency: %f \n", frequency);
            channel[count]->setFrequency(frequency);
            channel[count]->setVolume(volume);
			
            volume   -= (initVol / 200.0f);
            frequency -= (initFreq / 200.0f);
			
            Sleep(2);
        }
		}
	}
	
    printf("\n");
	
    /*
	 Shut down
	 */
	for (count = 0; count < numSounds; count++)
	{
		result = sound[count]->release();
		ERRCHECK(result);
	}
    result = system->close();
    ERRCHECK(result);
    result = system->release();
    ERRCHECK(result);
	
    return 0;
}


