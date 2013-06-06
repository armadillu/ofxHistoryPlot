/*
 *  ofxHistoryPlot.h
 *  emptyExample
 *
 *  Created by Oriol Ferrer Mesià on 28/01/11.
 *  Copyright 2011 uri.cat. All rights reserved.
 *
 */

#pragma once

#include "ofMain.h"
#include <limits>


#define DEFAULT_WIDTH	160.0f
#define DEFAULT_HEIGHT	120.0f

class ofxHistoryPlot: public ofBaseDraws{

	public :

		ofxHistoryPlot(float * val, string varName, float maxHistory, bool autoUpdate_ = true);
		
		void update( float val = numeric_limits<float>::quiet_NaN() );	//adds in the plot current value of the specified var, 
											//usually you would call this once per frame if not autoUpdate_ == false for the graph to update
										
		void draw(float x, float y , float w = DEFAULT_WIDTH, float h = DEFAULT_HEIGHT);	//draws a box
		void draw(float x, float y );	//draws a box
		void draw(int x, int y );
		void setRange(float low, float high);	//range is manual
		void setLowerRange(float low);	//low is fixed, high is auto
		void setMaxHistory(int max);
		void setPrecision(int prec){ precision = ofClamp(precision, 0, 15); }	//number of decimals to show
		float getHeight(){ return DEFAULT_HEIGHT;}
		float getWidth(){ return DEFAULT_WIDTH;}
		void setColor(ofColor c){ colorSet = true; lineColor = c;}
		void setBackgroundColor(ofColor c){ bgColor = c;}
		void setShowNumericalInfo(bool show){ showNumericalInfo = show;}	
		void setRespectBorders(bool respect){respectBorders = respect;}
		void setDrawSkipVal(int skip){ drawSkip = skip; if (drawSkip <1) drawSkip = 1;} //draw evey n samples, might speed up drawing 
		void setLineWidth(float w){ lineWidth = w;}
		void setDrawBackground(bool d) { drawBackground = d;}
		void reset();
		
	private:
	
		string			varName;
		float *			valf;
	
		vector<float>	values;
		float			lowest, highest;
		bool			manualRange;
		bool			onlyLowestIsFixed;
	
		bool			drawBackground;

		bool			autoUpdate;
		bool			showNumericalInfo;
		bool			respectBorders;
	
		int				MAX_HISTORY;
		int				index;
		int				addedCounter;
		
		int				precision;
	
		unsigned int	count;
	
		bool			colorSet;
		ofColor			lineColor;
		ofColor			bgColor;
			
		int				drawSkip;
		float			lineWidth;
};