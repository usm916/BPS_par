//
//  cloudImage.h
//  emptyExample
//
//  Created by usm on 11/16/12.
//
//

#ifndef emptyExample_cloudImage_h
#define emptyExample_cloudImage_h

#include "ofMain.h"

class Cloud {
public:
    void drawCloud(float*, float*);
    
    ofImage* cloud;
    float time0, time1, time2, time3;
    float spd = 1.5;
    float s1,s2,s3,s4, nx, ny, sizeX, sizeY;
    float dx, dy;
    float scale1, scale2, cood1, cood2;
    
    Cloud(){}
    Cloud(ofImage* _image){
        cloud = _image;
        dx = ofRandom(30);
        dy = ofRandom(40);
        time0 = ofRandom(600,700);
        time1 = ofRandom(700,800);
        time2 = ofRandom(500,600);
        time3 = ofRandom(550,750);
        scale1 = ofRandom(10,20);
        scale2 = ofRandom(20,30);
        cood1 = ofRandom(10);
        cood2 = ofRandom(20);
    }
};

int const numCloud = 10;

class CloudsImage {
public:
    void draw(float*, float*);
    ofImage* image;
    float* fluidOpacity;
    Cloud clouds[numCloud];

    CloudsImage(){}
    CloudsImage(ofImage* _image, float* _fluidOpacity){
        image = _image;
        fluidOpacity = _fluidOpacity;
        for(int i=0;i<numCloud;i++){
            clouds[i] = Cloud(image);
        }
    }
};

#endif
