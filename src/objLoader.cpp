//  
//  objLoader.cpp
//  Sound Reactor
//
//  Created by Takuto Usami on 12/04/03.
//

#include "objLoader.h"
#include <iostream>

//--------------------------------------------------------
void ObjLoader::loadObjFile(){
    ifstream fin; //declare a file stream  
    fin.open( ofToDataPath(objFileName).c_str() ); //open your text file  
    if(!fin.is_open()){
        cout << "Faled to import objfile" << endl;
    }
    
    vector<string> data; //declare a vector of strings to store data
    vector<ofVec3f> vtx;
    vtx.clear();
    int numLine = 0;
    
    while(fin!=NULL) //as long as theres still text to be read  
    {  
        string str; //declare a string for storage  
        stringstream sstr;
        sstr.str("");
        getline(fin, str); //get a line from the file, put it in the string 
        sstr << str;
        string typ;
        float x,y,z;
        char c;
        sstr >> typ >> x >> y >> z; 
        if(typ=="v"){
            vtx.push_back( ofVec3f(x*objScaleX, y*objScaleY, z*objScaleZ) ); //push the string onto a vector of strings
            cout << x << "," << y << "," << z << endl;
        }else if(typ=="g"){
            //            vtxLine.assign(numLine, vtx);
            vtxLine.push_back(vtx);
            vtx.clear();
            cout << "push_back line " << numLine << " size() " << vtxLine.size() << endl;
            numLine++;
        }
    }
//    for(int j=0;j<vtxLine.size();j++){
//        lineColor.push_back( ofColor(ofRandom(80,255), ofRandom(80,255), ofRandom(80,255), 200.0) );
//        lineColor.push_back( ofColor(160,180,255,180) );
//    }
    cout << "setup: .obj was Loaded" << endl;
    alpha = 1.0;
}

//--------------------------------------------------------
void ObjLoader::drawObj(ofVec3f _origin, float _rx, float _ry, float _rz, float _alpha){
    float rid = 1.2f;
    objColor = ofColor(255,255,225);
    lineColorH = ofColor( objColor, _alpha );
//    lineColorH = ofColor( 160, 255, 255, _alpha );
//    lineColor[0] = ofColor(160,180,255,alpha);

    ofPushMatrix();
    ofTranslate(_origin);
    ofRotateZ(_rz);
    ofRotateY(_ry);
    ofRotateX(_rx);
    for(int j=0;j<vtxLine.size();j++){
//        ofSetColor(lineColor[j]);
        ofSetColor( lineColorH );
        glBegin(GL_LINE_STRIP);
        for(int i=0;i<vtxLine[j].size();i++){
            glVertex3f(vtxLine[j][i].x+ofRandom(-rid,rid), vtxLine[j][i].y+ofRandom(-rid,rid), vtxLine[j][i].z+ofRandom(-rid,rid));
        }
        glEnd();
    }
    ofPopMatrix();
}

//--------------------------------------------------------
void ObjLoader::drawParticlee(ofVec3f _origin){
    img -> draw(_origin);
}

//----------------------------------------------------------
void ObjLoader::objSetColor(ofColor _color){
    objColor = _color;
}