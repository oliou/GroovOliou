/*
  ==============================================================================

    This file was auto-generated by the Jucer!

    It contains the basic startup code for a Juce application.

  ==============================================================================
*/

#include "utils.h"

using namespace std;

const Boolean debugMode = true;

void debugLog(String debugTxt, bool separator){

    String debugTxt2 = "\n ----------------------" + debugTxt +"--------------------------- \n";
    
    debugLog(debugTxt2);
}


void debugLog(String debugTxt){
    //creating log file
    ofstream myfile;
    myfile.open ("/Users/oliou/Desktop/debug.txt", std::ios::app);
    myfile << debugTxt << "\n";
    myfile.close();
}