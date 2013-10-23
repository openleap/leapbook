#include <iostream>
#include <CoreGraphics/CoreGraphics.h>
#include <Leap.h>

class MouseController : public Leap::Listener {
public:
    MouseController();
    virtual void onFrame(const Leap::Controller &);
    
protected:
    bool clickActive;
    bool leftHanded;
};

MouseController::MouseController()
  : clickActive(false), leftHanded(false) {}

void MouseController::onFrame(const Leap::Controller &controller) {
    // get list of detected screens
    const Leap::ScreenList screens = controller.calibratedScreens();
    
    // make sure we have a detected screen
    if (screens.empty()) return;
    const Leap::Screen screen = screens[0];

    // get hands
    const Leap::Frame frame = controller.frame();
    const Leap::HandList hands = frame.hands();
    if (hands.empty()) return;
    
    // find the first finger or tool
    const Leap::PointableList pointables = hands[0].pointables();
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
}

int main(int argc, const char * argv[]) {
    MouseController listener;
    Leap::Controller controller;
    controller.addListener(listener);
    
    std::cout << "Press any key to exit" << std::endl;
    std::cin.get();
    
    controller.removeListener(listener);
    
    return 0;
}

