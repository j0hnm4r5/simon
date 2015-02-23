#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){

	//	MIDI MAP -----
	{
		midiTable["C"]  = 24;
		midiTable["C#"] = 25;
		midiTable["Db"] = 25;
		midiTable["D"]  = 26;
		midiTable["D#"] = 27;
		midiTable["Eb"] = 27;
		midiTable["E"]  = 28;
		midiTable["F"]  = 29;
		midiTable["F#"] = 30;
		midiTable["Gb"] = 30;
		midiTable["G"]  = 31;
		midiTable["G#"] = 32;
		midiTable["Ab"] = 32;
		midiTable["A"]  = 33;
		midiTable["A#"] = 34;
		midiTable["Bb"] = 34;
		midiTable["B"]  = 35;
	}

	//	SCALE -----
	{
		scale[0] = "C";
		scale[1] = "D";
		scale[2] = "E";
		scale[3] = "G";
		scale[4] = "A";
	}

	//	COLOR MAP -----
	{
		colorMap["C"] = simonPink;
		colorMap["D"] = simonLightBlue;
		colorMap["E"] = simonGreen;
		colorMap["G"] = simonPurple;
		colorMap["A"] = simonYellow;
	}

	//	PD SETUP -----
	int numOutChannels = 2;
	int numInChannels = 1;
	int sampleRate = 44100;
	int ticksPerBuffer = 8;

	pd.init(numOutChannels, numInChannels, sampleRate, ticksPerBuffer);

	pd.subscribe("pdFiddle");
	pd.subscribe("pdFiddleBang");
	pd.addReceiver(*this);

	pd.start();
	pd.openPatch("simon.pd");

	ofSoundStreamSetup(numOutChannels, numInChannels, this, sampleRate, ofxPd::blockSize() * ticksPerBuffer, 1);

	// make the song
	songLength = 10;
	makeSong(songLength);
//	for (int i = 0; i < songLength; i ++) {
//		ofLog() << song[i];
//	}

	//  OF SETUP -----
	ofBackground(simonDarkBlue);

	ofTrueTypeFont::setGlobalDpi(72);
	font.loadFont("verdana.ttf", 90);
	font.setLineHeight(34.0f);
	font.setLetterSpacing(1.035);
	
	// VARIABLES -----
	fromFiddle = 0;
	octave = 2;
	
	currentNote = 0;
	counter = 0;
	
	noteLength = 500;
	breathLength = 100;
	playbackLength = 750;
}

//--------------------------------------------------------------
void ofApp::makeSong(int length){
	for (int i = 0; i < length; i++) {
		song.push_back(scale[(int)ofRandom(sizeof(scale) / sizeof(*scale))]);
	}
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
void ofApp::receiveFloat(const std::string& dest, float value) {

	if (dest == "pdFiddle") {
		fromFiddle = (int)value;
	}

}

//--------------------------------------------------------------
void ofApp::receiveBang(const std::string& dest) {

	if (dest == "pdFiddleBang") {
		fromFiddleBang = true;
	}

}


//--------------------------------------------------------------
void ofApp::play(){
		
	pd.sendFloat("pitch", midiTable[song[counter]] * octave);

	pd.sendFloat("volume", .5);
	ofSleepMillis(noteLength);

	pd.sendFloat("volume", 0);
	ofSleepMillis(breathLength);

}

//--------------------------------------------------------------
void ofApp::listen(){

	vector<int> playback;

	int time = ofGetElapsedTimeMillis();
//	ofLog() << time;
	
	int numNotes = 0;
	
	while (ofGetElapsedTimeMillis() < time + (currentNote + 1) * playbackLength) {
		ofLog() << fromFiddleBang;
		if (fromFiddleBang) {
			playback.push_back(fromFiddle);
			numNotes++;
		}
		fromFiddleBang = false;
	}
	
	
	for (int i = 0; i < numNotes; i ++) {
		ofLog() << playback[i];
	}
	
	
	
}

//--------------------------------------------------------------
void ofApp::update(){

	if (counter <= currentNote) {
    play();
		ofLog() << counter;
	}
//	listen();

	if (counter == currentNote) {
		currentNote++;
		counter = 0;
		ofSleepMillis(1000);
	} else {
		counter++;
	}
	
}

//--------------------------------------------------------------
void ofApp::draw(){

//	ofLog() << note;
	ofBackground(colorMap[song[counter]]);
	ofSetColor(simonDarkBlue);
	font.drawString(song[counter],
		ofGetWidth() / 2 - font.stringWidth(song[counter]) / 2,
		ofGetHeight() / 2 - font.stringHeight(song[counter]) / 2);

}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){

}
