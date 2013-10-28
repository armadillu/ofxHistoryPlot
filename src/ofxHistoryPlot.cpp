/*
 *  ofxHistoryPlot.cpp
 *  emptyExample
 *
 *  Created by Oriol Ferrer Mesià on 28/01/11.
 *  Copyright 2011 uri.cat. All rights reserved.
 *
 */

#include "ofxHistoryPlot.h"
#include <float.h>

ofxHistoryPlot::ofxHistoryPlot(float * val, string varName, float maxHistory, bool autoUpdate_){
	
	autoUpdate = autoUpdate_;
	this->varName = varName;
	valf = val;
	if (val != NULL){
		lowest = *valf * 0.99;
		highest = *valf * 1.01;
	}else{
		lowest = -1;
		highest = 1;
	}
	MAX_HISTORY = maxHistory;
	manualRange = false;
	onlyLowestIsFixed = false;
	count = 1;
	precision = 2;
	colorSet = false;
	lineWidth = 1.0f;
	drawSkip = 1;
	showNumericalInfo = true;
	respectBorders = true;
	drawBackground = true;
	bgColor = ofColor(0);
	gridColor = ofColor(255,16);
	drawGrid = true;
	shrinkBackInAutoRange = false;
	plotNeedsRefresh = true;
	gridMesh.setMode(OF_PRIMITIVE_LINES);
	plotMesh.setMode(OF_PRIMITIVE_LINE_STRIP);
	gridUnit = 40;
}

void ofxHistoryPlot::setMaxHistory(int max){
	MAX_HISTORY = max;
}

void ofxHistoryPlot::reset(){
	values.clear();
	count = 0;
}


void ofxHistoryPlot::update(float newVal){
	
	count++;
	
	if (newVal != newVal && valf != NULL)	//if no value is supplied (default value NAN), use the float* we were given..
		newVal = *valf;	
	
	if ( ( manualRange && onlyLowestIsFixed ) || !manualRange ){	//update graph range every now and then
		int skip = 3;
		if(shrinkBackInAutoRange){
			//if (!autoUpdate) skip = 1;	//if not doing this too fast, no need to skip range processing
			if ( count%skip == 0 ){			
				if (!onlyLowestIsFixed) lowest = FLT_MAX;
				highest = -FLT_MIN;
				for (int i = 0; i < values.size(); i+=skip){
					float val = values[i];
					if (val > highest) highest = val;
					if (!onlyLowestIsFixed) if (val < lowest) lowest = val;
				}	
				if (lowest == FLT_MAX) lowest = -1;
				if (highest == -FLT_MIN) highest = 1;
			}
		}
		if ( newVal > highest) highest = newVal;
		if ( newVal < lowest && !onlyLowestIsFixed) lowest = newVal;
	}
	
	values.push_back( float( newVal) );
	if (values.size() > MAX_HISTORY){
		values.erase( values.begin() );
	}

	plotNeedsRefresh = true;
//	float adapt = 0.05;
//	lowest = lowest + adapt * (newVal - lowest);
//	highest = highest - adapt * (highest - newVal);
}

void ofxHistoryPlot::draw(float x, float y){
	draw(x,y,DEFAULT_WIDTH,DEFAULT_HEIGHT);
}

void ofxHistoryPlot::draw(int x, int y){
	draw((float)x,(float)y,DEFAULT_WIDTH,DEFAULT_HEIGHT);
}

void ofxHistoryPlot::refillGridMesh(float x, float y , float w, float h){

	gridMesh.clear();
	float gridH = gridUnit;
	float numLinesH = h / gridH;
	gridMesh.setMode(OF_PRIMITIVE_LINES);
	for(int i = 0; i < numLinesH; i++){
		gridMesh.addVertex( ofVec2f(x,  y + gridH * i) );
		gridMesh.addVertex( ofVec2f(x + w,  y + gridH * i) );
	}
	float numLinesW = w / gridH;
	for(int i = 0; i < numLinesW; i++){
		gridMesh.addVertex( ofVec2f( gridH/2 + x + gridH * i, y ) );
		gridMesh.addVertex( ofVec2f( gridH/2 + x + gridH * i, y + h) );
	}
}


void ofxHistoryPlot::refillPlotMesh(float x, float y , float w, float h){

	plotMesh.clear();
	float border = respectBorders ? 12 : 0;
	int start = 0;
	if (count >= MAX_HISTORY){
		start = drawSkip - (count) % (drawSkip);
	}

	for (int i =  start; i < values.size(); i+= drawSkip){
		float xx = ofMap(i, 0, MAX_HISTORY, 0, w, false);
		float yy = ofMap( values[i], lowest-0.001, highest+0.001, border, h - border, true);
		plotMesh.addVertex(ofVec3f(x + xx, y + h - yy));
	}
}


void ofxHistoryPlot::draw(float x, float y , float w, float h){

	bool needsMesh = false;
	ofRectangle r = ofRectangle(x,y,w,h);
	if ( r != prevRect || autoUpdate || plotNeedsRefresh){
		needsMesh = true;
		plotNeedsRefresh = false;
	}

	if (autoUpdate) update();
	bool haveData = true;
	if (values.size() == 0 ) haveData = false;
	
	#ifndef TARGET_OPENGLES	
	glPushAttrib(GL_CURRENT_BIT);
	#endif
		if (drawBackground){
			ofSetColor(bgColor);
			ofRect(x, y, w, h);
			if (drawGrid){
				if(needsMesh){
					refillGridMesh(x, y, w, h);
				}
				ofSetColor(gridColor);
				#ifndef TARGET_OPENGLES
				glPushAttrib(GL_ENABLE_BIT);
				#endif
				ofSetLineWidth(1);
				gridMesh.draw();
				#ifndef TARGET_OPENGLES
				glPopAttrib();
				#endif
			}
		}
		if ( showNumericalInfo && haveData){
			glColor4f(0.7,0.7,0.7,1);
			float cVal = values[values.size()-1];
			string text = varName + " " + ofToString(cVal, precision);
			ofDrawBitmapString(text, x + w - (text.length()) * 8  , y + 10);
		}
		if ( showNumericalInfo ){
			glColor4f(0.33,0.33,0.33, 1);
			ofDrawBitmapString(ofToString(highest, precision), 1 + x , y + 10);
			ofDrawBitmapString(ofToString(lowest, precision), 1 + x , y + h - 1);
		}
		
	#ifndef TARGET_OPENGLES	
	glPopAttrib();
	#endif
	
	if (haveData){
		ofNoFill();

		#ifndef TARGET_OPENGLES	
		glPushAttrib(GL_ENABLE_BIT);
		#endif
		glEnable(GL_LINE_SMOOTH);
		ofSetLineWidth(lineWidth);
		glHint (GL_LINE_SMOOTH_HINT, GL_FASTEST);

			if (colorSet){
				#ifndef TARGET_OPENGLES	
				glPushAttrib(GL_CURRENT_BIT);
				#endif
				glColor4ub(lineColor.r, lineColor.g, lineColor.b, lineColor.a);
			}

			if(needsMesh){
				refillPlotMesh(x, y, w, h);
			}

			plotMesh.draw();

		#ifndef TARGET_OPENGLES
			if (colorSet) glPopAttrib();
		glPopAttrib();
		#endif

		ofFill();
	}
	prevRect = r;
}


void ofxHistoryPlot::setRange(float low, float high){

	manualRange = true;
	onlyLowestIsFixed = false;
	lowest = low;
	highest = high;
}

float ofxHistoryPlot::getLowerRange(){
	return lowest;
}


float ofxHistoryPlot::getHigerRange(){
	return highest;
}


void ofxHistoryPlot::setLowerRange(float low){
	
	manualRange = true;
	onlyLowestIsFixed = true;
	lowest = low;
}