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

		ofxHistoryPlot(float * val, string varName, float maxHistory, bool autoUpdate_ = true);
		
		void update( float val = numeric_limits<float>::quiet_NaN() );	//adds in the plot current value of the specified var, 
											//usually you would call this once per frame if not autoUpdate_ == false for the graph to update
										
		void draw(float x, float y , float w = DEFAULT_WIDTH, float h = DEFAULT_HEIGHT);	//draws a box
		void draw(float x, float y );	//draws a box
		void draw(int x, int y );
		void setRange(float low, float high);	//range is manual
		void setLowerRange(float low);	//low is fixed, high is auto

		void addHorizontalGuide(float yval, ofColor c);
		void clearHorizontalGuides();

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
		void setDrawGrid(bool d) { drawGrid = d;}
		void setGridUnit(float g){gridUnit = g;} //pixels
		void setAutoRangeShrinksBack(bool shrink){shrinkBackInAutoRange = shrink;};
		void setDrawFromRight(bool val); //begin drawing graph from right instead of left
		void reset();
		float getLowerRange();
		float getHigerRange();
		void setShowSmoothedCurve(bool show){showSmoothedPlot = show;}
		void setSmoothFilter(float filter){smoothFactor = filter;};
		vector<float> getValues(){ return values; }

	private:

		void refillGridMesh(float x, float y , float w, float h);
		void refillPlotMesh(ofVboMesh& mesh, vector<float> & vals, float x, float y , float w, float h);

		string			varName;
		float *			valf;
	
		vector<float>	values;
		vector<float>	smoothValues;

		vector<float>	horizontalGuides;
		vector<ofColor>	horizontalGuideColors;

		float			lowest, highest;
		bool			manualRange;
		bool			onlyLowestIsFixed;
	
		bool			drawBackground;

		bool			autoUpdate;
		bool			showNumericalInfo;
		bool			respectBorders;
		bool			drawGrid;
		bool			shrinkBackInAutoRange;
		bool			drawFromRight; //begin drawing graph from right instead of left

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
