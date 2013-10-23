#include <iostream>
#include <CoreGraphics/CoreGraphics.h>
#include <Leap.h>

using namespace std;

class MouseListener : public Leap::Listener {
public:
    MouseListener();
    const float clickActivationDistance = 40;
    virtual void onFrame(const Leap::Controller &);
    virtual void postMouseDown(unsigned x, unsigned y);
    virtual void postMouseUp(unsigned x, unsigned y);
    
protected:
    bool clickActive; // currently clicking?
    bool leftHanded;  // user setting
    int32_t clickerHandID, pointerHandID; // last recognized
};

MouseListener::MouseListener()
  : clickActive(false), leftHanded(false),
    clickerHandID(0), pointerHandID(0) {}

void MouseListener::onFrame(const Leap::Controller &controller) {
    // get list of detected screens
    const Leap::ScreenList screens = controller.calibratedScreens();
    
    // make sure we have a detected screen
    if (screens.empty()) return;
    const Leap::Screen screen = screens[0];

    // get hands
    const Leap::Frame frame = controller.frame();
    const Leap::HandList hands = frame.hands();
    if (hands.empty()) return;
    
    // which is the pointer hand and which is the clicker hand?
    // if there's only one hand it's the pointer hand. if there's two
    // then we'll use the user's primary hand. if ambidextrous, bail.
    Leap::Hand pointerHand, clickerHand;
    
    if (pointerHandID) {
        pointerHand = frame.hand(pointerHandID);
        if (! pointerHand.isValid())
            pointerHand = hands[0];
    }
    
    if (clickerHandID)
        clickerHand = frame.hand(clickerHandID);

    if (! clickerHand.isValid() && hands.count() == 2) {
        // figure out clicker and pointer hand
                
        // which hand is on the left and which is on the right?
        Leap::Hand leftHand, rightHand;
        if (hands[0].palmPosition()[0] <= hands[1].palmPosition()[0]) {
            leftHand = hands[0];
            rightHand = hands[1];
        } else {
            leftHand = hands[1];
            rightHand = hands[0];
        }
        
        // now that we know the left and right hands, determine
        // which is the clicker and which is the pointer
        if (leftHanded) {
            pointerHand = leftHand;
            clickerHand = rightHand;
        } else {
            pointerHand = rightHand;
            clickerHand = leftHand;
        }
        
        // keep track of which hands we decided on
        clickerHandID = clickerHand.id();
        pointerHandID = pointerHand.id();
    }
    
    // find the first finger or tool
    const Leap::PointableList pointables = pointerHand.pointables();
    if (pointables.empty()) return;
    const Leap::Pointable firstPointable = pointables[0];
    
    // get x, y coordinates on the first screen
    const Leap::Vector intersection = screen.intersect(
                                                       firstPointable,
                                                       true,  // normalize
                                                       1.0f   // clampRatio
                                                       );
    
    // if the user is not pointing at the screen all components of
    // the returned vector will be Not A Number (NaN)
    // isValid() returns true only if all components are finite
    if (! intersection.isValid()) return;
    
    unsigned int x = screen.widthPixels() * intersection.x;
    // flip y coordinate to standard top-left origin
    unsigned int y = screen.heightPixels() * (1.0f - intersection.y);
    
    // move cursor to location pointed at
    CGPoint destPoint = CGPointMake(x, y);
    CGDisplayMoveCursorToPoint(kCGDirectMainDisplay, destPoint);
    
    // check for click gesture
    if (! clickerHand.isValid()) return;
    
    // click = two fingers on the clicker hand touching together

    const Leap::PointableList clickerFingers = clickerHand.pointables();
    if (clickerFingers.count() != 2) return;
    
    // get finger distance
    float clickFingerDistance = clickerFingers[0].tipPosition().distanceTo(
                                                                           clickerFingers[1].tipPosition()
                                                                           );
    cout << "distance: " << clickFingerDistance << endl;
    if (! clickActive && clickFingerDistance < clickActivationDistance) {
        clickActive = true;
        cout << "mouseDown\n";
        postMouseDown(x, y);
    } else if (clickActive && clickFingerDistance > clickActivationDistance) {
        cout << "mouseUp\n";
        clickActive = false;
        postMouseUp(x, y);
    }
}

void MouseListener::postMouseDown(unsigned x, unsigned y) {
    CGEventRef mouseDownEvent = CGEventCreateMouseEvent(
                                                        NULL, kCGEventLeftMouseDown,
                                                        CGPointMake(x, y),
                                                        kCGMouseButtonLeft
                                                        );
    CGEventPost(kCGHIDEventTap, mouseDownEvent);
    CFRelease(mouseDownEvent);
}

void MouseListener::postMouseUp(unsigned x, unsigned y) {
    CGEventRef mouseUpEvent = CGEventCreateMouseEvent(
                                                      NULL, kCGEventLeftMouseUp,
                                                      CGPointMake(x, y),
                                                      kCGMouseButtonLeft
                                                      );
    CGEventPost(kCGHIDEventTap, mouseUpEvent);
    CFRelease(mouseUpEvent);
}


int main(int argc, const char * argv[]) {
    MouseListener listener;
    Leap::Controller controller;
    controller.addListener(listener);
    
    std::cout << "Press any key to exit" << std::endl;
    std::cin.get();
    
    controller.removeListener(listener);
    
    return 0;
}

