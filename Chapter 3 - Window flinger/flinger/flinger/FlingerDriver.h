//
//  FlingerDriver.h
//  flinger
//
//  Created by Mischa Spiegelmock on 3/11/13.
//
//

#ifndef __flinger__FlingerDriver__
#define __flinger__FlingerDriver__

#include <iostream>
#include <Leap.h>

namespace flinger {
    
typedef void* flingerWinRef;

class Driver {
public:
    virtual ~Driver() {};
    
    virtual void releaseWinRef(flingerWinRef win) = 0;
    virtual Leap::Vector getWindowSize(const flingerWinRef win) = 0;
    virtual const flingerWinRef getWindowAt(double x, double y) = 0;
    virtual const Leap::Vector getWindowPosition(const flingerWinRef win) = 0;
    virtual void setWindowPosition(const flingerWinRef win, Leap::Vector &pos) = 0;
    virtual void setWindowCenter(const flingerWinRef win, Leap::Vector &pos) = 0;
    virtual void setWindowSize(const flingerWinRef win, Leap::Vector &size) = 0;
    virtual void scaleWindow(const flingerWinRef win, double dx, double dy) = 0;
};

}

#endif /* defined(__flinger__FlingerDriver__) */
