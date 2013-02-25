#pragma once

#include "ParticleSystem.h"
#include "ofMain.h"
#include "ofxOSC.h"
#include "ofxFX.h"
//#include "OSCvalue.h"
#include "cloudImage.h"
//#include "midiImport.h"
#include "objLoader.h"
#include "Spring.h"


#define PORT 54321
#define NUM_MSG_STRINGS 20
#define IDnum 4
#define MEM 40

class testApp : public ofBaseApp{
public:
	void setup();
	void update();
	void draw();

	void keyPressed  (int key);
	void mousePressed(int x, int y, int button);
    void mouseDragged(int x, int y, int button);
	void mouseReleased(int x, int y, int button);
    void glSetting();

    //declar value to Particle system
	float timeStep;
	float lineOpacity, pointOpacity;
	float particleNeighborhood, particleRepulsion;
	float centerAttraction;
	float tarTimeStep;
	float tarLineOpacity, tarPointOpacity;
	float tarParticleNeighborhood, tarParticleRepulsion;
	float tarCenterAttraction;
    float r, g, b, tarR, tarG, tarB;
    float edgeR, edgeG, edgeB, tarEdgeR, tarEdgeG, tarEdgeB;
    float fluidFboOpacity, tarFluidFboOpacity;
    
	int kParticles;
	ParticleSystem particleSystem;
	bool isMousePressed, slowMotion;
    
    ofxOscReceiver receiver;
    
    int current_msg_string;
    string msg_strings[NUM_MSG_STRINGS];
    float timers[NUM_MSG_STRINGS];
    
    void setForces();
    
    void drawParameters();
    void setMode(int);
    float avgValue(float &v, int num);
    int avgValue(int &v, int nu);
    
    ofEasyCam cam3D;
    const int GridNumX = 20;
    const int GridNumY = 12;
    const int GridNumZ = 20;
    
    //---------------------MIDI
//    MidiImportToOSCvalue midiImport1, midiImport2;
    
    //-------------------------------------------------------------------------
    void linearTrasition(float* volume, float _step, float* _target){
        float delta = *_target-*volume;
        if( abs((*volume - *_target)) >= _step ){
            if(delta > 0){
                *volume += _step;
                if(*volume > *_target) *volume = *_target;
            }else if(delta < 0){
                *volume -= _step;
                if(*volume < *_target) *volume = *_target;
            }
        }
    }
    void colorTrasition(float* volume, float _step, float* _target){
        float delta = *_target-*volume;
        if( abs((*volume - *_target)) >= _step ){
            if(delta > 0){
                *volume += _step;
                if(*volume > *_target) *volume = *_target;
            }else if(delta < 0){
                *volume -= _step;
                if(*volume < *_target) *volume = *_target;
            }
        }
    }
    ofImage cloud;
    CloudsImage clouds[2];
    ofFbo scrFbo;
    
    ofVbo vbo;
    ofVboMesh scrVboMesh;
    ofMesh Mesh;
    ofShader shader;
    ofTexture tex;
    ofTexture site;
    ofTexture* scr;
    ofTexture toShaderTex;
    bool isMouseDragged;
    ofImage starMap;
    
    ofShader bloomShader;       //Bloom shader (from OF forums)
    ofShader blurShader;        //So-so Gaussian Blur
    ofShader brcosaShader;      //Brightness, Contrast, Saturation adjustments
    ofShader chromaShader;      //Poor mans chromatic abberation
    ofShader fishShader;        //Fisheye
    ofShader rotaShader;        //Shader from jit.rota allowing you to zoom, rotate, offset a texture
    ofShader carToPolShader;    //Cartesian to polar shader
    ofShader wobbleShader;      //Wobbulation displacement
    ofShader lumaShader;        //Luma Keying
    ofShader radialShader;      //RadialBlur
    float chromeNoise, chromeMove;
    
    bool checkAreaRect(float, float, ofPoint, ofVec2f);
    int rtnAdrs(float, float);
    void callSound(bool, int);
    void addPoint(float x, float y, float z);
    void drawParticles();
    void setScreenMeshVertex();
    void drawNodeCursor(float, float, float, float, float);
    void drawControlWire(float, float);
    float average(float* num);
    void updateValue();
    void changeBG();
    
    // OSC
    int idNumber;
    int pMouseX[MEM], pMouseY[MEM];
    float handX[IDnum], handY[IDnum], scrHandX[IDnum], scrHandY[IDnum];
    float legX[IDnum], legY[IDnum], scrLegX[IDnum], scrLegY[IDnum];
    float shoulderX[IDnum], shoulderY[IDnum], scrShoulderX[IDnum], scrShoulderY[IDnum];
    float userCenterX[IDnum], userCenterY[IDnum], scrUserCenterX[IDnum], scrUserCenterY[IDnum];
    float elbowX[IDnum], elbowY[IDnum], scrElbowX[IDnum], scrElbowY[IDnum];
    float neck[IDnum*2], scrNeck[IDnum*2];
    ofPoint neckPos;
    float filterFactorHandPos;
    float paddingFrame;
    float pHandX[IDnum][MEM], pHandY[IDnum][MEM];
    float pLegX[IDnum][MEM], pLegY[IDnum][MEM];
    float pShoulderX[IDnum][MEM], pShoulderY[IDnum][MEM];
    float puCenterX[IDnum][MEM], puCenterY[IDnum][MEM];
    float pElbowX[IDnum][MEM], pElbowY[IDnum][MEM];
    float rotUser[IDnum], pRotUser[IDnum][MEM];
    float fmsx, fmsy;
    int behaviour;
    
    float soundT, soundL;
    int behaviaMode;
    bool backToNormal;
    bool mouseCheck, bInvert, bInvertBG, bPInvert;
    bool bSetVertex;
    int ra, ga, ba;
    // Sound
    ofSoundPlayer soundBase[9];
    ofSoundPlayer sound[9];
    int soundCount;
    // vector to store all values
    vector <ofVec3f> points;
    vector <ofVec3f> sizes;
    
    vector<float> spdSE;

    //-------------------------------- Load OBJECTS " ANIMAL "
    ObjLoader bird, whl, taiji, tree1, tree2, fish;
    ObjLoader logo, treeN1, treeN2;
    ObjLoader space01, room;
    vector<ofVec3f> treePos1, treePos2, whlPos, fishPos;
    vector<float> treeRot1, treeRot2, whlRot, fishRot;
    
    void setupScene();
    void drawSceneTrees();
    void drawSceneSpace();
    void drawSceneTulou();
    
    float forestOpacity, tarForestOpacity;
    float tulouOpacity, tarTulouOpacity;
    
    //------------------Rain
    ofColor rainColor;
    float psz;
    Spring spring0;
    Spring NodeForFishes;
    Spring NodeForWhl;
    Spring NodeForBird;
    vector<float> nfSpd;
    vector<float> nwSpd;
    vector<float> nbSpd;
    
    //--------------------------------------------for ofxFX
    void fluidSetup();
    void fluidDraw();
    void fluidUpdate();
    void fluidDrawForce(float, float, ofFloatColor, float, float);
    bool valueToBool(float _bool){
        if(_bool>0.5){
            return true;
        }else{
            return false;
        }
    }
    ofxFluid fluid;
    
    ofFbo fluidFbo;
    ofVboMesh fluidScrMesh;
    ofTexture* fluidTexture;
    
    int width;
    int height;
    
    ofFloatColor fluidColor;
    float fluidScrScale, fluidColorScale, fluidForce;
    int setDelta;
    ofVec2f oldM;
    ofPoint pmus;
    bool    bPaint, bObstacle, bBounding, bClear, bBlack, bTulou, bForest, bParticle, bFluid, bHideCur;
    int invertCount;
    float color, force;
};
