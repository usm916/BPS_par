//
//  Spring.h
//  audioInputExample
//
//  Created by Takuto Usami on 11/11/05.
//  Copyright (c) 2011年 __MyCompanyName__. All rights reserved.
//

#ifndef Spring_h
#define Spring_h

#define _PRAMNUM 12

#include "ofMain.h"
#include <iostream>
#include <vector>
#include <sstream>
#include <math.h>

class SpPointObj {//: public ofBaseApp {
    
public: 
    float x, y, z, vX, vY, vZ, trgtX, trgtY, trgtZ, acX, acY, acZ, spX, spY, spZ;
//    float pointObj[_PRAMNUM * _ParNUM];// pos, v, def, trg
    float vx[45], vy[45], vz[45];
    float rx, ry, rz;
    float pvx, pvy, pvz;
    float px,py,pz;
    float ppx,ppy,ppz;
        
    SpPointObj() {};
    SpPointObj(float posX, float posY, float posZ, float tvx, float tvy, float tvz) {
        x = posX;
        y = posY;
        z = posZ;
        vx[0] = tvx;
        vy[0] = tvy;
        vz[0] = tvz;
        vX = .0;
        vY = .0;
        vZ = .0;
        trgtX = .0;
        trgtY = .0;
        trgtZ = .0;
        acX = .0;
        acY = .0;
        acZ = .0;
        spX = .0;
        spY = .0;
        spZ = .0;
    };
    
};

class Spring : public ofBaseApp {

    public :
        float k;
        float posx, posy, posz, fricX, fricY, fricZ;
        float vx, vy, vz;
        float orx, ory, orz, rndXX;
        float pmX, pmY;
        float rangZ;
        vector <SpPointObj> stPos;
        SpPointObj tpoint;
        ofImage imgTemp;
        vector <ofImage> pLight;
        int ParticleNUM;
    
    Spring() {};
    Spring(int NUM) {
        pmX = 0.0;
        pmY = 0.0;
        orx = 400;
        ory = 300;
        orz = 0;
        ParticleNUM = NUM;
        rangZ = ofGetWidth();
        
        for(int i=0;i<ParticleNUM;i++){
            tpoint = SpPointObj( ofRandom(-ofGetWidth(),ofGetWidth()), ofRandom(-ofGetWidth(),ofGetWidth()), ofRandom(-ofGetWidth(),ofGetWidth()),  0.0f, 0.0f, 0.0f );
            stPos.push_back(tpoint);
        }
    };
    Spring(int NUM, float _z) {
        pmX = 0.0;
        pmY = 0.0;
        orx = 400;
        ory = 300;
        orz = 0;
        ParticleNUM = NUM;
        rangZ = _z;
        
        for(int i=0;i<ParticleNUM;i++){
            tpoint = SpPointObj( ofRandom(-ofGetWidth(),ofGetWidth()), ofRandom(-ofGetWidth(),ofGetWidth()), ofRandom(-ofGetWidth(),ofGetWidth()),  0.0f, 0.0f, 0.0f );
            stPos.push_back(tpoint);
        }
    };
    
    void setup();
    float dist(SpPointObj, SpPointObj);
    float checkNum(float);
    float checkNumZ(float);

    void calConf();
    void connect();
    void drawLine3f(SpPointObj, SpPointObj);
    void calSpringToTarget();
    void calMove(vector<float>);
    void calMoveF(vector<float>);

    void drawOwn(float,ofColor);
    void drawLine(ofColor);
    void setPos();
};

#endif
