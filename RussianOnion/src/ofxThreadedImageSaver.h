//
//  ofxThreadedImageSaver.h
//  RussianOnion
//
//  Created by Jeffrey Crouse on 8/30/13.
//
//

#pragma once

#include "ofMain.h"

class ofxThreadedImageSaver : public ofThread, public ofImage {
public:
    string fileName;
    
    void threadedFunction() {
        if(lock()) {
            saveImage(fileName);
            unlock();
        } else {
            printf("ofxThreadedImageSaver - cannot save %s cos I'm locked", fileName.c_str());
        }
        stopThread();
    }
    
    void saveThreaded(string fileName) {
        this->fileName = fileName;
        startThread(false, false);   // blocking, verbose
    }
};