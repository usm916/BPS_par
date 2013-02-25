//
//  Spring.cpp
//  audioInputExample
//
#include "Spring.h"

static inline void billboard();

void Spring::setup(){
    imgTemp.loadImage("images/emitter_.png");
    pLight.push_back(imgTemp);
    imgTemp.loadImage("images/1.png");
    pLight.push_back(imgTemp);
    imgTemp.loadImage("images/4.png");
    pLight.push_back(imgTemp);
    imgTemp.loadImage("images/2.png");
    
//    fish = ObjLoader("fish.txt", 2000.0f);
    
}
// -----------------------------------------------------------------------------     
float Spring::dist(SpPointObj _p1, SpPointObj _p2){
    float dist2f, dist3f;
    dist2f = sqrt( pow(abs(_p1.x - _p2.x),2) + pow(abs(_p1.y - _p2.y),2));
    dist3f = sqrt( pow(dist2f,2) + pow(abs(_p1.z - _p2.z),2));
    cout << (float)dist3f << endl;
    return dist3f;

}

// -----------------------------------------------------------------------------     
void Spring::drawLine3f(SpPointObj _p1, SpPointObj _p2){
        ofLine(_p1.x, _p1.y, _p1.z, _p1.x, _p1.y, _p1.z);
}

//-----------------------------------------------------------------------------
void Spring::calConf(){
//    float cc = 10;
//    for(int i=0;i<ParticleNUM;i++){
//        for(int j=i+1;j<ParticleNUM;j++){
//            if(!i==j)continue;
//            stPos[i].acX = cc/(stPos[j].x - stPos[i].x);
//            stPos[i].acY = cc/(stPos[j].y - stPos[i].y);
//            stPos[i].acZ = cc/(stPos[j].z - stPos[i].z);
//            stPos[j].acX = cc/(stPos[i].x - stPos[j].x);
//            stPos[j].acY = cc/(stPos[i].y - stPos[j].y);
//            stPos[j].acZ = cc/(stPos[i].z - stPos[j].z);
//        }
//    }
}

//-----------------------------------------------------------------------------
void Spring::connect(){
    float cc = 150;
    ofNoFill();
    ofSetColor(255, 255, 255, 200);
    for(int i=0;i<ParticleNUM;i++){
        for(int j=i+1;j<ParticleNUM;j++){
            if(dist(stPos[i], stPos[j])<cc) drawLine3f(stPos[i], stPos[j]);
        }
    }
}

//-----------------------------------------------------------------------------
void Spring::calSpringToTarget(){
//    float k = 0.2;
    float sx = 1;
    float scal = 20;
    for(int i=0;i<ParticleNUM;i++){
//        stPos[i].spX = (stPos[i].trgtX - stPos[i].x)*k;
//        stPos[i].spY = (stPos[i].trgtY - stPos[i].y)*k;
//        stPos[i].spZ = (stPos[i].trgtZ - stPos[i].z)*k;
//        stPos[i].spX = scal * (ofNoise(stPos[i].spX/sx, stPos[i].spY/sx, stPos[i].spZ/sx)-0.5);
//        stPos[i].spY = scal * (ofNoise(stPos[i].spY/sx, stPos[i].spZ/sx, stPos[i].spY/sx)-0.5);
//        stPos[i].spZ = scal * (ofNoise(stPos[i].spZ/sx, stPos[i].spX/sx, stPos[i].spY/sx)-0.5);
//        cout << (float)ofNoise(stPos[i].spX/sx, stPos[i].spY/sx, stPos[i].spZ/sx) << endl;
    }
}

//-----------------------------------------------------------------------------
void Spring::calMove(vector<float> _spd){
    float fric = 0.97;
    for(int i=0;i<ParticleNUM;i++){
//        stPos[i].vX = stPos[i].vX*fric + stPos[i].spX + (ofNoise(-1*_spd[0], (float)ofGetElapsedTimeMillis() * _spd[0]*0.1)-.5);
//        stPos[i].vY = stPos[i].vY*fric + stPos[i].spY + (ofNoise(-1*_spd[0], (float)ofGetElapsedTimeMillis() * _spd[0]*0.1)-.5);
//        stPos[i].vZ = stPos[i].vZ*fric + stPos[i].spZ + (ofNoise(-1*_spd[0], (float)ofGetElapsedTimeMillis() * _spd[0]*0.1)-.5);
        stPos[i].vX = stPos[i].vX*fric + stPos[i].spX + ofRandom(-1,1)*_spd[0];
        stPos[i].vY = stPos[i].vY*fric + stPos[i].spY + ofRandom(-1,1)*_spd[1];
        stPos[i].vZ = stPos[i].vZ*fric + stPos[i].spZ + ofRandom(-1,1)*_spd[2];
        stPos[i].x = checkNum( stPos[i].x + stPos[i].vX );
        if(rangZ==-70){
            stPos[i].y = checkNumZ( stPos[i].y + stPos[i].vY );
        }else{
            stPos[i].y = checkNum( stPos[i].y + stPos[i].vY );
        }
        
        float roll, pitch, yaw;
        ofVec3f m_vel, m_oldvel;
        m_vel=ofVec3f(stPos[i].vX, stPos[i].vY, stPos[i].vZ);
        m_oldvel.set(0,0,0);
        // Determine the direction of the lateral acceleration.
        ofVec3f lateralDir = (m_vel.crossed(m_vel - m_oldvel)).crossed(m_vel);
        
        //    ofVec3f lateralDir = (m_vel.crossed(m_vel - m_oldvel)).crossed(m_vel);
        
        lateralDir.normalize();
        // Set the lateral acceleration's magnitude. The magnitude is the vector
        // projection of the appliedAcceleration vector onto the direction of the
        // lateral acceleration).
        
        float lateralMag = (m_vel - m_oldvel).dot(lateralDir);
        // compute roll
        
        if (lateralMag == 0) {
            roll = 0.0f;  
        } else {
            roll = (float) -1 * atan2(9.8999999, (double)lateralMag) + HALF_PI;
        }
        
        // compute pitch
        pitch = (float) -atan(m_vel.y / sqrt((m_vel.z*m_vel.z) + (m_vel.x*m_vel.x)));
        
        // compute yaw
        yaw = (float) atan2(m_vel.x, m_vel.z);
        
        // store them
        stPos[i].rx = pitch;
        stPos[i].ry = yaw;
        stPos[i].rz = roll;
        
        stPos[i].z = checkNum( stPos[i].z + stPos[i].vZ );
        stPos[i].ppx = stPos[i].px;
        stPos[i].ppy = stPos[i].py;
        stPos[i].ppz = stPos[i].pz;
        stPos[i].px = stPos[i].x;
        stPos[i].py = stPos[i].y;
        stPos[i].pz = stPos[i].z;
    }    
}

//-----------------------------------------------------------------------------
void Spring::calMoveF(vector<float> _spd){
    float fric = 0.96;
    float res = 0.005;
//    cout << "p_"<<  ofVec3f(stPos[0].x, stPos[0].y, stPos[0].z) << endl;
//    cout << "v_ "<< ofVec3f(stPos[0].vX, stPos[0].vY, stPos[0].vZ) << endl;

    for(int i=0;i<ParticleNUM;i++){
        stPos[i].vX = stPos[i].vX*fric + (ofNoise(stPos[i].x*res + PI, stPos[i].y*res, (float)ofGetElapsedTimeMillis()*_spd[0]*.1) -0.5);
        stPos[i].vY = stPos[i].vY*fric + (ofNoise(stPos[i].x*res - PI, stPos[i].y*res, (float)ofGetElapsedTimeMillis()*_spd[0]*.1) -0.5)*0.1;
        stPos[i].vZ = stPos[i].vZ*fric + (ofNoise(stPos[i].y*res, stPos[i].x*res + PI, (float)ofGetElapsedTimeMillis()*_spd[0]*.1) -0.5);
        
        stPos[i].x = checkNum( stPos[i].x + stPos[i].vX );
        if(rangZ==-70){
            stPos[i].y = checkNumZ( stPos[i].y + stPos[i].vY );
        }else{
            stPos[i].y = checkNum( stPos[i].y + stPos[i].vY );
        }
        stPos[i].z = checkNum( stPos[i].z + stPos[i].vZ );

        float roll, pitch, yaw;
        ofVec3f m_vel, m_oldvel;
        m_vel=ofVec3f(stPos[i].vX, stPos[i].vY, stPos[i].vZ);
        m_oldvel.set(0,0,0);
        // Determine the direction of the lateral acceleration.
        ofVec3f lateralDir = (m_vel.crossed(m_vel - m_oldvel)).crossed(m_vel);
        
        //    ofVec3f lateralDir = (m_vel.crossed(m_vel - m_oldvel)).crossed(m_vel);
        
        lateralDir.normalize();
        // Set the lateral acceleration's magnitude. The magnitude is the vector
        // projection of the appliedAcceleration vector onto the direction of the
        // lateral acceleration).
        
        float lateralMag = (m_vel - m_oldvel).dot(lateralDir);
        // compute roll
        
        if (lateralMag == 0) {
            roll = 0.0f;  
        } else {
            roll = (float) -1 * atan2(9.8999999, (double)lateralMag) + HALF_PI;
        }
        
        // compute pitch
        pitch = (float) -atan(m_vel.y / sqrt((m_vel.z*m_vel.z) + (m_vel.x*m_vel.x)));
        
        // compute yaw
        yaw = (float) atan2(m_vel.x, m_vel.z);
        
        // store them
        stPos[i].rx = pitch;
        stPos[i].ry = yaw;
        stPos[i].rz = roll;
        
        stPos[i].ppx = stPos[i].px;
        stPos[i].ppy = stPos[i].py;
        stPos[i].ppz = stPos[i].pz;
        stPos[i].px = stPos[i].x;
        stPos[i].py = stPos[i].y;
        stPos[i].pz = stPos[i].z;
    }    
}

//-----------------------------------------------------------------------------
float Spring::checkNum(float _value){
    if(_value < -ofGetWidth()*1.5){
        _value = _value + 3*ofGetWidth();
    }else if(_value > ofGetWidth()){
        _value = _value - 3*ofGetWidth();
    }
    return _value;
}

//-----------------------------------------------------------------------------
float Spring::checkNumZ(float _value){
    if(_value < -ofGetWidth()){
        _value = _value + ofGetWidth() + rangZ;
    }else if(_value > rangZ){
        _value = _value - ofGetWidth() + rangZ;
    }
    return _value;
}
// -----------------------------------------------------------------------------
void Spring::drawOwn(float ps, ofColor clr){
        ofFill();
        ofSetColor(clr);
    for(int i=0;i<ParticleNUM;i++){
        ofPushMatrix();
        ofTranslate(stPos[i].x, stPos[i].y, stPos[i].z);
        billboard();
        pLight[0].draw(0,0,0, ps, ps);
        ofPopMatrix();
    }
}

// -----------------------------------------------------------------------------
void Spring::drawLine(ofColor clr){
    ofFill();
    for(int i=0;i<ParticleNUM;i++){
        ofSetColor(clr);
        ofLine(stPos[i].x, stPos[i].y, stPos[i].z,stPos[i].px, stPos[i].py, stPos[i].pz);
        ofSetColor(clr);
        ofLine(stPos[i].px, stPos[i].py, stPos[i].pz,stPos[i].ppx, stPos[i].ppy, stPos[i].ppz);
    }
}

//-----------------------------
static inline void billboard()
{
	GLfloat m[16];
	glGetFloatv(GL_MODELVIEW_MATRIX, m);
	
	float inv_len;
	
	m[8] = -m[12];
	m[9] = -m[13];
	m[10] = -m[14];
	inv_len = 1. / sqrt(m[8] * m[8] + m[9] * m[9] + m[10] * m[10]);
	m[8] *= inv_len;
	m[9] *= inv_len;
	m[10] *= inv_len;
	
	m[0] = -m[14];
	m[1] = 0.0;
	m[2] = m[12];
	inv_len = 1. / sqrt(m[0] * m[0] + m[1] * m[1] + m[2] * m[2]);
	m[0] *= inv_len;
	m[1] *= inv_len;
	m[2] *= inv_len;
	
	m[4] = m[9] * m[2] - m[10] * m[1];
	m[5] = m[10] * m[0] - m[8] * m[2];
	m[6] = m[8] * m[1] - m[9] * m[0];
	
	glLoadMatrixf(m);
}
