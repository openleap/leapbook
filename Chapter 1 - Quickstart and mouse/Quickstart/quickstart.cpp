#include <iostream>
#include <Leap.h>

using namespace std;

class Quickstart : public Leap::Listener {
public:
    virtual void onConnect(const Leap::Controller &);
    virtual void onFrame(const Leap::Controller &);
};

void Quickstart::onConnect(const Leap::Controller &controller) {
    std::cout << "Hello, Leap user!\n";
}

void Quickstart::onFrame(const Leap::Controller &controller) {
    // returns the most recent frame. older frames can be accessed by passing in 
    // a "history" parameter to retrieve an older frame, up to about 60
    // (exact number subject to change)
    const Leap::Frame frame = controller.frame();

    // do nothing unless hands are detected
    if (frame.hands().empty())
        return;
    
    // first detected hand
    const Leap::Hand firstHand = frame.hands()[0];
    // first pointable object (finger or tool)
    const Leap::PointableList pointables = firstHand.pointables();
    if (pointables.empty()) return;
    const Leap::Pointable firstPointable = pointables[0];
    
    // print velocity on the X axis
    cout << "Pointable X velocity: " << firstPointable.tipVelocity()[0] << endl;
    
    const Leap::FingerList fingers = firstHand.fingers();
    if (fingers.empty()) return;
    
    for (int i = 0; i < fingers.count(); i++) {
        const Leap::Finger finger = fingers[i];
        
        std::cout << "Detected finger " << i << " at position (" <<
            finger.tipPosition().x << ", " <<
            finger.tipPosition().y << ", " <<
            finger.tipPosition().z << ")" << std::endl;
    }
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
