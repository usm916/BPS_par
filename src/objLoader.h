//
//  objLoader.h
//  emptyExample
//
//  Created by Takuto Usami on 12/04/03.
//  Copyright (c) 2012年 __MyCompanyName__. All rights reserved.
//

#ifndef emptyExample_objLoader_h
#define emptyExample_objLoader_h

#include "ofMain.h"
#include <fstream>

using namespace std;

class ObjLoader {
    
//    vector<ofColor> lineColor;
    ofColor lineColorH;
    ofColor objColor;
    vector<vector<ofVec3f> > vtxLine;
    string objFileName;
    
    ofImage* img;
    int countNum;
    float objScale, objScaleX, objScaleY, objScaleZ;
    float alpha;
    
    
    public:
        ObjLoader(){};
        ObjLoader(string _filename, float _scale){
            objFileName = _filename;
            objScaleX = _scale;
            objScaleY = _scale;
            objScaleZ = _scale;
        };
        ObjLoader(string _filename, float _sx, float _sy, float _sz){
            objFileName = _filename;
            objScaleX = _sx;
            objScaleY = _sy;
            objScaleZ = _sz;
        };
        
        void loadObjFile();
        void drawObj(ofVec3f, float, float,float,float);
        void drawParticlee(ofVec3f);
        void objSetColor(ofColor);
};

#endif
