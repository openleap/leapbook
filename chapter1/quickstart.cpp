#include <iostream>
#include <Leap.h>

using namespace std;

class Quickstart : public Leap::Listener {
public:
    virtual void onInit(const Leap::Controller &);
    virtual void onFrame(const Leap::Controller &);
};

void Quickstart::onInit(const Leap::Controller &controller) {
    if (! controller.isConnected()) {
        // controller is not connected or the driver software is not started.
        // give the user a friendly reminder that we can't do anything yet
        cout << "Please connect your Leap Motion and run the Leap application" << endl;
    }
}

void Quickstart::onFrame(const Leap::Controller &controller) {
    // returns the most recent frame. older frames can be accessed by passing in 
    // a "history" parameter to retrieve an older frame, up to about 60
    // (exact number subject to change)
    const Leap::Frame frame = controller.frame();

    // do nothing unless hands are detected
    if (frame.hands().empty())
        return;
    
    // retrieve first pointable object (finger or tool)
    // from the frame
    const Leap::PointableList pointables = frame.hands()[0].pointables();
    if (pointables.empty())
        return;
    const Leap::Pointable firstPointable = pointables[0];
    
    // print velocity on the X axis
    cout << "Pointable X velocity: " << firstPointable.tipVelocity()[0] << endl;
}

int main() {
    // create instance of our Listener subclass
    Quickstart listener;
    
    // create generic Controller to interface with the Leap device
    Leap::Controller controller;
    
    // tell the Controller to start sending events to our Listener
    controller.addListener(listener);
    
    // run until user presses a key
    cin.get();
    
    // clean up
    controller.removeListener(listener);
    
    return 0;
}
