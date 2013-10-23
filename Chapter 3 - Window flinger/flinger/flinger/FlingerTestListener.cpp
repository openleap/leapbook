//
//  FlingerTestListener.cpp
//  flinger
//
//  Created by Mischa Spiegelmock on 3/11/13.
//
//

#include "FlingerTestListener.h"

using namespace std;
    
namespace flinger {

    void TestListener::onWindowMovedBy(const Leap::Controller&, const flingerWinRef &win, double dx, double dy) {
        cout << "Moved window by " << dx << ", " << dy << endl;
    }

}
