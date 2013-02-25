//
//  File.cpp
//  emptyExample
//
//  Created by usm on 11/16/12.
//
//

#include "cloudImage.h"

void Cloud::drawCloud(float* _x, float* _y){
    ofPushMatrix();
    spd = 1.5;
    s1 = ofGetElapsedTimeMillis()/time0/spd;
    s2 = ofGetElapsedTimeMillis()/time1/spd;
    s3 = ofGetElapsedTimeMillis()/time2/spd;
    s4 = ofGetElapsedTimeMillis()/time3/spd;

    nx = ofNoise(1.8,s1) + dy/6;
    ny = ofNoise(2.3,s1) + dx/6;
    ofTranslate(*_x + dx * ofNoise(cood2,s2/4,s4), *_y + dy * ofNoise(cood1,s1/4,s3));
    ofRotateZ(s1*10+s2*9);
    sizeX, sizeY;
    sizeX = 65 + 1.23*scale2 * ofNoise(cood1,s1);
    sizeY = 67 + 1.36*scale2 * ofNoise(cood2,s2);
    cloud->draw(nx*3, ny*3, sizeX, sizeY);
    ofPopMatrix();
}

void CloudsImage::draw(float* _x, float* _y){
//    ofEnableAlphaBlending();
    ofSetColor(255,255,255,*fluidOpacity);
    ofSetRectMode(OF_RECTMODE_CENTER);
    for(int i=0;i<numCloud;i++){
        clouds[i].drawCloud(_x, _y);
    }
    ofSetRectMode(OF_RECTMODE_CORNER);
//    ofDisableAlphaBlending();
}