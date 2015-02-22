#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){

	//	PD SETUP -----
	int numOutChannels = 2;
	int numInChannels = 1;
	int sampleRate = 44100;
	int ticksPerBuffer = 8;
	
	pd.init(numOutChannels, numInChannels, sampleRate, ticksPerBuffer);
	
	pd.subscribe("pdFiddle");
	pd.addReceiver(*this);
	
	pd.start();
	pd.openPatch("simon.pd");
	
	ofSoundStreamSetup(numOutChannels, numInChannels, this, sampleRate, ofxPd::blockSize() * ticksPerBuffer, 1);
	
	//  OF SETUP -----
	ofBackground(0, 255, 255);
	
	ofTrueTypeFont::setGlobalDpi(72);
	font.loadFont("verdana.ttf", 30, true, true);
	font.setLineHeight(34.0f);
	font.setLetterSpacing(1.035);

}

//--------------------------------------------------------------
void ofApp::audioOut(float *output, int bufferSize, int numChannels) {
	pd.audioOut(output, bufferSize, numChannels);
}

//--------------------------------------------------------------
void ofApp::audioIn(float *input, int bufferSize, int numChannels) {
	pd.audioIn(input, bufferSize, numChannels);
}

//--------------------------------------------------------------
void ofApp::update(){

}

//--------------------------------------------------------------
void ofApp::draw(){

}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){

}
