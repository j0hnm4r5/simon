#pragma once

#include "ofMain.h"
#include "ofxPd.h"
#include "NamedColors.h"

using namespace pd;

class ofApp : public ofBaseApp, public PdReceiver {

	public:
		void setup();
		void update();
		void draw();

		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
	
		void play();
		void listen();
	
		void drawTitle();
		void drawCard();
		void drawLose();
		void drawWin();
	
		// PD -----
		ofxPd pd;
		void audioOut(float* output, int bufferSize, int numChannels);
		void audioIn(float* input, int bufferSize, int numChannels);
		void receiveFloat(const std::string& dest, float value);
	
		// OF -----
		ofTrueTypeFont fontBig;
		ofTrueTypeFont fontMedium;
		ofTrueTypeFont fontSmall;
	
		vector<string> song;
		void makeSong(int length);
		map<string, ofColor> colorMap;
		map<string, int> midiTable;
		string scale[5];
	
		int songLength;
		int octave;
	
		float fromFiddle;
	
		int currentNote;
		int counter;

		int noteLength;
		int breathLength;
	
		bool bIsTitle;
		bool bIsPlaying;
		bool bIsLost;
		bool bIsWon;
	
		bool bHasNotes;
		bool bHasColors;
	
		bool bIsPressing;
	
		ofRectangle bigButton;
		ofRectangle tryButton;
		ofRectangle notesButton;
		ofRectangle colorsButton;
	
		int difficulty;
		
};
