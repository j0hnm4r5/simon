#pragma once

#include "ofMain.h"
#include "ofxPd.h"

using namespace pd;

class ofApp : public ofBaseApp{

	public:
		void setup();
		void update();
		void draw();

		void keyPressed(int key);
	
		// OF -----
		ofTrueTypeFont font;
	
		// PD -----
		ofxPd pd;
		void audioOut(float* output, int bufferSize, int numChannels);
		void audioIn(float* input, int bufferSize, int numChannels);
		void receiveFloat(const std::string& dest, float value);
	
		int fromFiddle;

};
