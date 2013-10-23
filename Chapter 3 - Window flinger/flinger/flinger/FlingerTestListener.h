//
//  FlingerTestListener.h
//  flinger
//
//  Created by Mischa Spiegelmock on 3/11/13.
//
//

#ifndef __flinger__FlingerTestListener__
#define __flinger__FlingerTestListener__

#include <iostream>
#include <Leap.h>
#include "FlingerListener.h"

namespace flinger {

class TestListener : public Listener {
public:
    virtual void onWindowMovedBy(const Leap::Controller&, const flingerWinRef &win, double dx, double dy);
    virtual const flingerWinRef getWindowAt(double x, double y) { return NULL; };
    virtual const Leap::Vector getWindowCoords(const flingerWinRef win) { return Leap::Vector(); };
};

}

#endif /* defined(__flinger__FlingerTestListener__) */
