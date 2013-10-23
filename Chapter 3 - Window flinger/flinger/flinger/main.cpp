//
//  main.cpp
//  flinger
//
//  Created by Mischa Spiegelmock on 3/10/13.
//
//

#include <iostream>
#include <Leap.h>
#include "FlingerTestListener.h"

using namespace std;

int main(int argc, const char * argv[]) {
    Leap::Controller controller;
    flinger::TestListener testListener;
    controller.addListener(testListener);
    
    cin.get();
    
    controller.removeListener(testListener);
    
    return 0;
}

