//
//  FlingerMac.h
//  flinger
//
//  Created by Mischa Spiegelmock on 3/11/13.
//
//

#ifndef __flinger__MacDriver__
#define __flinger__MacDriver__

#include <iostream>
#include <map>
#include <set>
#include <CoreFoundation/CoreFoundation.h>
#include <CoreGraphics/CoreGraphics.h>
#include <ApplicationServices/ApplicationServices.h>
#include <Security/Security.h>
#include "FlingerDriver.h"

namespace flinger {
    
class MacDriver : public Driver {
public:
    MacDriver();
    virtual ~MacDriver() {};
    
    virtual void releaseWinRef(flingerWinRef win);

    virtual Leap::Vector getWindowSize(const flingerWinRef win);
    virtual const flingerWinRef getWindowAt(double x, double y);
    virtual const Leap::Vector getWindowPosition(const flingerWinRef win);
    virtual void setWindowPosition(const flingerWinRef win, Leap::Vector &pos);
    virtual void setWindowCenter(const flingerWinRef win, Leap::Vector &pos);
    virtual void setWindowSize(const flingerWinRef win, Leap::Vector &size);
    virtual void scaleWindow(const flingerWinRef win, double dx, double dy);
    
protected:
    // passed to CGWindowListCopyWindowInfo
    // to control which windows we are testing
    CGWindowListOption listOptions;
    
    // last retrieved set of window information
    CFArrayRef windowList;
    
    // update windowList with current window layer attrs
    void updateWindowInfo();
    
    // search for a window containing (x,y) at depth winIdx
    // belonging to process pid
    const flingerWinRef findWindowForPID(int pid, int winIdx, double x, double y);
    
    CGSize _getWindowSize(const flingerWinRef win);
    CGPoint _getWindowPosition(const flingerWinRef win);
//    bool enableAXTrust();
    void needAXAccess();
};
    
}

#endif /* defined(__flinger__FlingerMac__) */
