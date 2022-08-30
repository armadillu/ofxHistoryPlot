/*
 *  ofxHistoryPlot.h
 *  emptyExample
 *
 *  Created by Oriol Ferrer Mesià on 28/01/11.
 *  Copyright 2011 uri.cat. All rights reserved.
 *
 */

#ifndef OFX_HISTORY_PLOT_DEFINED
#define OFX_HISTORY_PLOT_DEFINED

#include "ofMain.h"
#include <limits>

#define DEFAULT_WIDTH	160.0f
#define DEFAULT_HEIGHT	120.0f

class ofxHistoryPlot{

	public :

		ofxHistoryPlot(float * val, std::string varName, float maxHistory, bool autoUpdate_ = true);
		
		void update( float val = numeric_limits<float>::quiet_NaN() );	//adds in the plot current value of the specified var, 
											//usually you would call this once per frame if not autoUpdate_ == false for the graph to update
										
		void draw(float x, float y , float w = DEFAULT_WIDTH, float h = DEFAULT_HEIGHT);	//draws a box
		void draw(float x, float y );	//draws a box
		void draw(int x, int y );
		void setRange(float low, float high);	//range is manual
		void setLowerRange(float low);	//low is fixed, high is auto
		void setRangeAuto();

		void addHorizontalGuide(float yval, ofColor c);
		void clearHorizontalGuides();
		void setDrawGuideValues(bool doIt){drawGuideValues = doIt;}

		void setMaxHistory(int max);
		void setPrecision(int prec){ precision = ofClamp(prec, 0, 15); }	//number of decimals to show
		float getHeight(){ return DEFAULT_HEIGHT;}
		float getWidth(){ return DEFAULT_WIDTH;}

		void setColor(ofColor c){ lineColor = c;}
		ofColor getColor(){return lineColor;}

		void setBackgroundColor(ofColor c){ bgColor = c;}
		void setGridColor(ofColor c){ gridColor = c;}
		void setShowNumericalInfo(bool show){ showNumericalInfo = show;}	
		void setRespectBorders(bool respect){respectBorders = respect;}
		void setDrawSkipVal(int skip){ drawSkip = skip; if (drawSkip <1) drawSkip = 1;} //draw evey n samples, might speed up drawing 
		void setLineWidth(float w){ lineWidth = w;}
		void setDrawBackground(bool d) { drawBackground = d;}
		void setDrawTitle(bool doit){drawTitle = doit;}
		void setDrawGrid(bool d) { drawGrid = d;}
		void setGridUnit(float g){gridUnit = g;} //pixels
		void setAutoRangeShrinksBack(bool shrink){shrinkBackInAutoRange = shrink;};
		void setDrawFromRight(bool val); //begin drawing graph from right instead of left
		void reset();
		float getLowerRange();
		float getHigerRange();
		float getLowestValue();
		float getHigestValue(); //deprecated - typo!
		float getHighestValue(){return getHigestValue();};

		void setShowSmoothedCurve(bool show){showSmoothedPlot = show;}
		void setSmoothFilter(float filter){smoothFactor = filter;};
		void setCropToRect(bool s){scissor = s;}

		std::deque<float>& getValues(){ return values; }
    
        std::string getVariableName();
		void setVariableName(const std::string & var){varName = var;};

		float getSmoothValue(){
			return smoothValue;
		}

		ofColor getBackgroundColor() { return bgColor; }

	private:
		
		// custom font
		ofTrueTypeFont font;
		int fontSize = 6;

		enum RangeMode{
			RANGE_MANUAL,
			RANGE_LOWER_FIXED,
			RANGE_AUTOMATIC
		};

		void refillGridMesh(float x, float y , float w, float h);
		void refillPlotMesh(ofVboMesh& mesh, deque<float> & vals, float x, float y , float w, float h);

		std::string			varName;
		float *			valf;
	
		std::deque<float>	values;
		std::deque<float>	smoothValues;

		std::vector<float>	horizontalGuides;
		bool			drawGuideValues;
		std::vector<ofColor>	horizontalGuideColors;

		float			lowest, highest;
		float			manualLowest, manualHighest;

		RangeMode		rangeMode;

		bool			drawBackground;

		bool			autoUpdate;
		bool			showNumericalInfo;
		bool			respectBorders;
		bool			drawGrid;
		bool			shrinkBackInAutoRange;
		bool			drawFromRight; //begin drawing graph from right instead of left
		bool			drawTitle;
		bool			scissor;

		int				MAX_HISTORY;
		int				index;
		int				addedCounter;
		
		int				precision;
	
		unsigned int	count;
	
		ofColor			lineColor;
		ofColor			bgColor;
		ofColor			gridColor;
			
		int				drawSkip;
		float			lineWidth;
		float			gridUnit;

		bool			showSmoothedPlot;
		float			smoothValue; //average of the last plotted vals
		float			smoothFactor; //(0 1.0] >> 1.0 means no smoothing

		bool plotNeedsRefresh;

		ofVboMesh gridMesh;
		ofVboMesh plotMesh;
		ofVboMesh smoothPlotMesh;

		ofRectangle prevRect;
};

#endif
