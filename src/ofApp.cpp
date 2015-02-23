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
	fontBig.loadFont("Arvo-Bold.ttf", 200);
	fontMedium.loadFont("Arvo-Bold.ttf", 72);
	fontSmall.loadFont("Arvo-Bold.ttf", 36);
	
	// VARIABLES -----
	fromFiddle = 0;
	octave = 2;
	
	currentNote = 0;
	counter = 0;
	
	noteLength = 500;
	breathLength = 100;
	playbackLength = 750;
	
	bIsTitle = true;
	bIsPlaying = false;
	bIsLost = false;
	bIsWon = false;
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
void ofApp::drawTitle(){

	ofBackground(simonDarkBlue);
	
	int marginX = ofGetWidth() * 0.1;
	int marginY = ofGetHeight() * 0.1;
	
	// play button
	ofSetColor(simonRed);
	ofRectangle bigButton = ofRectangle(marginX, marginY, ofGetWidth() * 0.8, ofGetWidth() * 0.8);
	ofRect(bigButton);
	
	// options buttons
	ofSetColor(simonYellow);
	ofRectangle notesButton = ofRectangle(marginX, ofGetWidth() * 0.8 + marginY * 2, ofGetWidth() * 0.35, marginY);
	ofRectangle colorsButton = ofRectangle(marginX * 2 + ofGetWidth() * 0.35, ofGetWidth() * 0.8 + marginY * 2, ofGetWidth() * 0.35, marginY);
	ofRect(notesButton);
	ofRect(colorsButton);
	
	// text
	ofSetColor(simonDarkBlue);
	string playString = "PLAY";
	fontMedium.drawString(playString, bigButton.getCenter()[0]	- fontMedium.stringWidth(playString) / 2, bigButton.getCenter()[1]	 + fontMedium.stringHeight(playString) / 2);
	
	string notesString = "NOTES";
	string colorsString = "COLORS";
	fontSmall.drawString(notesString, notesButton.getCenter()[0]	- fontSmall.stringWidth(notesString) / 2, notesButton.getCenter()[1]	 + fontSmall.stringHeight(notesString) / 2);
	fontSmall.drawString(colorsString, colorsButton.getCenter()[0]	- fontSmall.stringWidth(colorsString) / 2, colorsButton.getCenter()[1] + fontSmall.stringHeight(colorsString) / 2);

}

//--------------------------------------------------------------
void ofApp::drawLose(){

	ofBackground(simonDarkBlue);
	
	int marginX = ofGetWidth() * 0.1;
	int marginY = ofGetHeight() * 0.1;
	
	// play button
	ofSetColor(simonRed);
	ofRectangle bigButton = ofRectangle(marginX, marginY, ofGetWidth() * 0.8, ofGetWidth() * 0.8);
	ofRect(bigButton);
	
	// options buttons
	ofSetColor(simonYellow);
	ofRectangle tryButton = ofRectangle(marginX, ofGetWidth() * 0.8 + marginY * 2, ofGetWidth() * 0.8, marginY);
	ofRect(tryButton);
	
	// text
	ofSetColor(simonDarkBlue);
	string playString = "YOU LOSE";
	fontMedium.drawString(playString, bigButton.getCenter()[0]	- fontMedium.stringWidth(playString) / 2, bigButton.getCenter()[1]	 + fontMedium.stringHeight(playString) / 2);
	
	string tryString = "TRY AGAIN";
	fontSmall.drawString(tryString, tryButton.getCenter()[0]	- fontSmall.stringWidth(tryString) / 2, tryButton.getCenter()[1] + fontSmall.stringHeight(tryString) / 2);
	
}

//--------------------------------------------------------------
void ofApp::drawWin(){

	ofBackground(simonDarkBlue);
	
	int marginX = ofGetWidth() * 0.1;
	int marginY = ofGetHeight() * 0.1;
	
	// play button
	ofSetColor(simonRed);
	ofRectangle bigButton = ofRectangle(marginX, marginY, ofGetWidth() * 0.8, ofGetWidth() * 0.8);
	ofRect(bigButton);
	
	// options buttons
	ofSetColor(simonYellow);
	ofRectangle tryButton = ofRectangle(marginX, ofGetWidth() * 0.8 + marginY * 2, ofGetWidth() * 0.8, marginY);
	ofRect(tryButton);
	
	// text
	ofSetColor(simonDarkBlue);
	string playString = "YOU WIN!";
	fontMedium.drawString(playString, bigButton.getCenter()[0]	- fontMedium.stringWidth(playString) / 2, bigButton.getCenter()[1]	 + fontMedium.stringHeight(playString) / 2);
	
	string tryString = "TRY AGAIN";
	fontSmall.drawString(tryString, tryButton.getCenter()[0]	- fontSmall.stringWidth(tryString) / 2, tryButton.getCenter()[1] + fontSmall.stringHeight(tryString) / 2);
	
}

//--------------------------------------------------------------
void ofApp::drawCard(){

	ofBackground(colorMap[song[counter]]);
	ofSetColor(simonDarkBlue);
	fontBig.drawString(song[counter], ofGetWidth() / 2 - fontBig.stringWidth(song[counter]) / 2, ofGetHeight() / 2);

}

//--------------------------------------------------------------
void ofApp::draw(){

	if (bIsTitle) {
		drawTitle();
	} else if (bIsLost) {
		drawLose();
	} else if (bIsWon) {
		drawWin();
	} else {
		drawCard();
	}

}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){

}
