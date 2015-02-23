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
		scale[3] = "F";
		scale[4] = "G";
	}

	//	COLOR MAP -----
	{
		colorMap["C"] = simonPink;
		colorMap["D"] = simonLightBlue;
		colorMap["E"] = simonGreen;
		colorMap["F"] = simonPurple;
		colorMap["G"] = simonYellow;
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

	//  OF SETUP -----
	ofBackground(simonDarkBlue);
	ofSetLineWidth(5);

	ofTrueTypeFont::setGlobalDpi(72);
	fontBig.loadFont("Arvo-Bold.ttf", 200);
	fontMedium.loadFont("Arvo-Bold.ttf", 72);
	fontSmall.loadFont("Arvo-Bold.ttf", 36);
	
	// VARIABLES -----
	fromFiddle = 0.0;
	
	octave = 2;
	
	noteLength = 750;
	breathLength = 200;
	
	bIsTitle = true;
	bIsPlaying = false;
	bIsLost = false;
	bIsWon = false;
	
	bHasNotes = true;
	bHasColors = true;
	
	bIsPressing = false;
	
	difficulty = 3;
	songLength = 20;
}

//--------------------------------------------------------------
void ofApp::makeSong(int length){
	int lastNum = 0;
	int rand = 0;
	
	song.clear();
	
	for (int i = 0; i < length; i++) {
		while (rand == lastNum) {
			rand = (int)ofRandom(sizeof(scale) / sizeof(*scale));
		}
		song.push_back(scale[rand]);
		lastNum = rand;
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
		fromFiddle = value;
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
	
	float lastFiddle = 0.0;
	
	for (int i = 0; i < currentNote; i++) {
		int start = ofGetElapsedTimeMillis();
		float runningTotal = 0;
		int numSamples = 0;
		while (ofGetElapsedTimeMillis() < start + noteLength) {
			
			if (lastFiddle != fromFiddle) {
				ofLog() << (int)fromFiddle;
				runningTotal += fromFiddle;
				numSamples++;
			}
			
			lastFiddle = fromFiddle;
		}
//		ofLog() << (int)(runningTotal / numSamples);
		playback.push_back((int)(runningTotal / numSamples));
		ofSleepMillis(breathLength);
	}
	
	
	for (int i = 0; i < currentNote; i ++) {
		
		ofLog() << playback[i] << " " << midiTable[song[i]] * octave;

		if (currentNote > 1) {
			if (playback[i] >= midiTable[song[i]] * octave + difficulty || playback[i] <= midiTable[song[i]] * octave - difficulty) {
				bIsPlaying = false;
				bIsLost = true;
			}
		}
		
	}
}

//--------------------------------------------------------------
void ofApp::update(){

	if (bIsPlaying) {
		if (counter <= currentNote) {
			play();
//			ofLog() << counter;
		}

		if (counter == currentNote) {
			currentNote++;
			counter = 0;
			
			listen();
			
			ofSleepMillis(1000);
		} else {
			counter++;
		}
		
		if (currentNote == songLength) {
			bIsPlaying = false;
			bIsWon = true;
		}
	}
	
}

//--------------------------------------------------------------
void ofApp::drawTitle(){

	ofBackground(simonDarkBlue);
	
	int marginX = ofGetWidth() * 0.1;
	int marginY = ofGetHeight() * 0.1;
	
	// play button
	bigButton = ofRectangle(marginX, marginY, ofGetWidth() * 0.8, ofGetWidth() * 0.8);
	
	ofSetColor(simonRed);
	if (bIsPressing) {
		ofNoFill();
	}
	ofRect(bigButton);
	ofFill();
	
	// options buttons
	notesButton = ofRectangle(marginX, ofGetWidth() * 0.8 + marginY * 2, ofGetWidth() * 0.35, marginY);
	colorsButton = ofRectangle(marginX * 2 + ofGetWidth() * 0.35, ofGetWidth() * 0.8 + marginY * 2, ofGetWidth() * 0.35, marginY);
	
	ofSetColor(simonYellow);
	if (!bHasNotes) {
		ofNoFill();
	}
	ofRect(notesButton);
	ofFill();
	
	if (!bHasColors) {
		ofNoFill();
	}
	ofRect(colorsButton);
	ofFill();
	
	// text
	string playString = "PLAY";
	string notesString = "NOTES";
	string colorsString = "COLORS";
	
	ofSetColor(simonDarkBlue);
	if (bIsPressing) {
		ofSetColor(simonRed);
	}
	fontMedium.drawString(playString, bigButton.getCenter()[0]	- fontMedium.stringWidth(playString) / 2, bigButton.getCenter()[1]	 + fontMedium.stringHeight(playString) / 2);
	
	ofSetColor(simonDarkBlue);
	if (!bHasNotes) {
		ofSetColor(simonYellow);
	}
	fontSmall.drawString(notesString, notesButton.getCenter()[0]	- fontSmall.stringWidth(notesString) / 2, notesButton.getCenter()[1]	 + fontSmall.stringHeight(notesString) / 2);

	ofSetColor(simonDarkBlue);
	if (!bHasColors) {
		ofSetColor(simonYellow);
	}
	fontSmall.drawString(colorsString, colorsButton.getCenter()[0]	- fontSmall.stringWidth(colorsString) / 2, colorsButton.getCenter()[1] + fontSmall.stringHeight(colorsString) / 2);

}

//--------------------------------------------------------------
void ofApp::drawLose(){

	ofBackground(simonDarkBlue);
	
	int marginX = ofGetWidth() * 0.1;
	int marginY = ofGetHeight() * 0.1;
	
	// play button
	ofSetColor(simonRed);
	bigButton = ofRectangle(marginX, marginY, ofGetWidth() * 0.8, ofGetWidth() * 0.8);
	ofRect(bigButton);
	
	// options buttons
	ofSetColor(simonYellow);
	tryButton = ofRectangle(marginX, ofGetWidth() * 0.8 + marginY * 2, ofGetWidth() * 0.8, marginY);
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
	bigButton = ofRectangle(marginX, marginY, ofGetWidth() * 0.8, ofGetWidth() * 0.8);
	ofRect(bigButton);
	
	// options buttons
	ofSetColor(simonYellow);
	tryButton = ofRectangle(marginX, ofGetWidth() * 0.8 + marginY * 2, ofGetWidth() * 0.8, marginY);
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
void ofApp::mousePressed(int x, int y, int button){

	if (bIsTitle) {
		if (bigButton.inside(x, y)) {
			bIsPressing = true;
		} else if (notesButton.inside(x, y)) {
			bHasNotes = !bHasNotes;
		} else if (colorsButton.inside(x, y)) {
			bHasColors = !bHasColors;
		}
	} else if (bIsLost) {
		if (tryButton.inside(x, y)) {
			bIsLost = false;
			bIsPlaying = true;
			
			makeSong(songLength);
			currentNote = 0;
			counter = 0;
		}
	} else if (bIsWon) {
		if (tryButton.inside(x, y)) {
			bIsWon = false;
			bIsPlaying = true;
			
			makeSong(songLength);
			currentNote = 0;
			counter = 0;
		}
	} else {
		
	}

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){
	if (bIsTitle) {
		if (bigButton.inside(x, y)) {
			bIsPressing = false;
			bIsTitle = false;
			bIsPlaying = true;
			
			makeSong(songLength);
			currentNote = 0;
			counter = 0;
		}
	}

}