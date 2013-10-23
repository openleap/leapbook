//
//  FlingerListener.cpp
//  flinger
//
//  Created by Mischa Spiegelmock on 3/10/13.
//
//

#include "FlingerListener.h"

using namespace std;

namespace flinger {
    
Listener::Listener() {
    currentWin = NULL;
    
#ifdef FLINGER_MAC
    driver = new MacDriver();
#else
    #error "Platform undefined"
#endif
}
    
Listener::~Listener() {
    setCurrentWin(NULL);
    delete(driver);
}

void Listener::setCurrentWin(flingerWinRef win) {
    if (currentWin)
        driver->releaseWinRef(currentWin);
    currentWin = win;
}

    
void Listener::onInit(const Leap::Controller &controller) {
    controller.enableGesture(Leap::Gesture::TYPE_SCREEN_TAP);
    controller.enableGesture(Leap::Gesture::TYPE_SWIPE);

    //    controller.enableGesture(Leap::Gesture::TYPE_KEY_TAP);
    //    controller.enableGesture(Leap::Gesture::TYPE_CIRCLE);

    // make ScreenTap a little more tolerent (at the expense of latency?)
    Leap::Config config = controller.config();
    bool setConfigSuccess = config.setFloat("Gesture.ScreenTap.HistorySeconds", 0.3);
    config.save();
    
    // ask to receive frames even if this application is not in the foreground
    Leap::Controller::PolicyFlag policyFlags = Leap::Controller::POLICY_BACKGROUND_FRAMES;
    controller.setPolicyFlags(policyFlags);
}
    
// where is this pointable pointing (on the screen, not in space)?
static Leap::Vector pointableScreenPos(const Leap::Pointable &pointable, const Leap::ScreenList &screens) {
    // need to have screen info
    if (screens.empty())
        return Leap::Vector();
    
    // get screen associated with gesture
    Leap::Screen screen = screens.closestScreenHit(pointable);
    if (! screen.isValid())
        return Leap::Vector();
    
    // get point location
    Leap::Vector cursorLoc = screen.intersect(pointable, true);
    if (! cursorLoc.isValid())
        return Leap::Vector();
    
    double screenX = cursorLoc.x * screen.widthPixels();
    double screenY = (1.0 - cursorLoc.y) * screen.heightPixels();

    return Leap::Vector(screenX, screenY, 0);
}
    
    
void Listener::onFrame(const Leap::Controller &controller) {
    Leap::Frame latestFrame = controller.frame();
    Leap::Frame refFrame = controller.frame(handGestureFrameInterval);
    
    if (refFrame.isValid() && latestFrame.hands().count() == 1) {
//        Leap::Hand hand = latestFrame.hands()[0];
//        double scaleFactor = hand.scaleFactor(refFrame);
//        cout << "Scale: " << scaleFactor << endl;
    }
    
    Leap::ScreenList screens = controller.calibratedScreens();
    Leap::GestureList gestures = latestFrame.gestures();

    // process gestures
    DockPosition dockPos = FLINGER_DOCK_NONE;
    for (Leap::GestureList::const_iterator it = gestures.begin(); it != gestures.end(); ++it) {
        Leap::Gesture gesture = *it;
        
        switch (gesture.type()) {
            case Leap::Gesture::TYPE_SCREEN_TAP: {
                auto tap = Leap::ScreenTapGesture(gesture);
                
                // select new current window, or deselect current window
                if (currentWin) {
                    setCurrentWin(NULL);
                    cout << "Deselected window\n";
                    break;
                }
                
                // get tap location in screen coords
                if (gesture.pointables().empty()) continue;
                Leap::Vector screenLoc = pointableScreenPos(tap.pointable(), screens);
                if (! screenLoc.isValid())
                    continue;
                
                // find window at tap location
                flingerWinRef win = driver->getWindowAt(screenLoc.x, screenLoc.y);
                if (win == NULL)
                    continue;
                
                // set new current window
                currentWin = win;
                currentWinOrigPosition = driver->getWindowPosition(win);
            } break;
                
            case Leap::Gesture::TYPE_SWIPE: {
                auto swipe = Leap::SwipeGesture(gesture);
                auto direction = swipe.direction();

                switch (swipe.state()) {
                    case Leap::Gesture::STATE_UPDATE:
                        // scoot window according to swipe direction to give a little visual feedback
                        if (currentWin) {
                            auto magnified = direction * 100;
                            auto curPos = driver->getWindowPosition(currentWin);
                            if (curPos.isValid()) {
                                curPos.x += magnified.x;
                                curPos.y += magnified.y;
                                driver->setWindowPosition(currentWin, curPos);
                            }
                        }
//                                cout << (direction * 100).toString() << endl;
                        break;
                        
                    case Leap::Gesture::STATE_STOP:
                        // swipe finished
    //                    cout << direction << endl;
                        
                        // what major direction did they swipe in? left, top, right, bottom?
                        if (direction.x > 0 && direction.x > direction.y)
                            dockPos = FLINGER_DOCK_RIGHT;
                        else if (direction.x < 0 && direction.x < direction.y)
                            dockPos = FLINGER_DOCK_LEFT;
                        else if (direction.y < 0 && direction.y < direction.x)
                            dockPos = FLINGER_DOCK_BOTTOM;
                        else if (direction.y > direction.x)
                            dockPos = FLINGER_DOCK_TOP;
                        break;
                }
            } break;
        
            default:
                break;
        }
    }
    
    // trying to dock a window to a section of the screen with a swipe gesture?
    if (currentWin && dockPos != FLINGER_DOCK_NONE && ! screens.isEmpty()) {
        auto dockScreen = screens[0];
        auto width = dockScreen.widthPixels();
        auto height = dockScreen.heightPixels();
        
        // figure out window coords
        float destX, destY, destWidth = 0, destHeight = 0;
        switch (dockPos) {
            case FLINGER_DOCK_LEFT:
                destX = 0;
                destY = 0;
                destWidth = width / 2;
                destHeight = height;
                break;
            case FLINGER_DOCK_RIGHT:
                destX = width / 2;
                destY = 0;
                destWidth = width / 2;
                destHeight = height;
                break;
            case FLINGER_DOCK_TOP:
                destX = 0;
                destY = 0;
                destWidth = width;
                destHeight = height / 2;
                break;
            case FLINGER_DOCK_BOTTOM:
                destX = 0;
                destY = height / 2;
                destWidth = width;
                destHeight = height / 2;
                break;
            default:
                cerr << "Got dockPos set to unknown enum value\n";
                break;
        }
        
        if (destWidth && destHeight) {
            // position window in docked area
            // would be pretty neat to animate this stuff
            Leap::Vector pos = Leap::Vector(destX, destY, 0);
            driver->setWindowPosition(currentWin, pos);
            Leap::Vector size = Leap::Vector(destWidth, destHeight, 0);
            driver->setWindowSize(currentWin, size);
            
            // stop dragging/focusing window
            setCurrentWin(NULL);
            return;
        }
    }
    
    // one-finger pointing
    if (currentWin && latestFrame.pointables().count() == 1) {
        Leap::Vector hitPoint = pointableScreenPos(latestFrame.pointables()[0], screens);
        if (hitPoint.isValid())
            driver->setWindowCenter(currentWin, hitPoint);
        return;
    }
    
    // scale currently selected window with Motion scale detection
    if (currentWin && refFrame.isValid()) {
        float scaleProbability = latestFrame.scaleProbability(refFrame);
        std::cout << "Probability: " << scaleProbability << endl;
        if (scaleProbability > handGestureMinScaleProbability) {
            float scaleFactor = latestFrame.scaleFactor(refFrame);
            if (scaleFactor < 1.0)
                scaleFactor = -scaleFactor;
            scaleFactor *= 50;
            cout << scaleFactor << endl;
            driver->scaleWindow(currentWin, scaleFactor, scaleFactor);
        }
    }
    
    return;
    // scale currently selected window with two-hand finger pointing
    Leap::HandList hands = latestFrame.hands();
    if (currentWin && hands.count() == 2 && hands[0].pointables().count() >= 1 && hands[1].pointables().count() >= 1) {
//        cout << "distance: " << pointDistance << endl;
        
        // get pointables from this frame and last frame, find delta
        Leap::Frame lastFrame = controller.frame(1);
        if (lastFrame.isValid()) {
            Leap::Pointable latestPointer1 = hands[0].pointables()[0];
            Leap::Pointable latestPointer2 = hands[1].pointables()[0];
            
            Leap::Pointable lastPointer1 = lastFrame.finger(latestPointer1.id());
            Leap::Pointable lastPointer2 = lastFrame.finger(latestPointer2.id());
            if (lastPointer1.isValid() && lastPointer2.isValid()) {
                Leap::Vector latestHitPoint1 = pointableScreenPos(latestFrame.pointables()[0], screens);
                Leap::Vector latestHitPoint2 = pointableScreenPos(latestFrame.pointables()[1], screens);
                Leap::Vector lastHitPoint1 = pointableScreenPos(lastFrame.pointables()[0], screens);
                Leap::Vector lastHitPoint2 = pointableScreenPos(lastFrame.pointables()[1], screens);
                
                // use movement of fingers to determine scale factor
                // "pinching" gesture:
                // if left finger moves left, scale increases
                // if left finger moves right, scale decreases
                // if right finger moves right, scale increases
                // if right finger moves left, scale decereases
                //  <----- -----> = expand
                //  -----> <----- = contact
                
                if (latestHitPoint1.isValid() && latestHitPoint2.isValid() && lastHitPoint1.isValid() && lastHitPoint2.isValid()) {
                    
                                        
                    // use primary hand as scale, secondary hand as center anchor point
                    double dx, dy;
                    if (latestPointer1.tipPosition().x > latestPointer2.tipPosition().x) {
                        // latestPointer1 == right hand

                        dx = latestPointer1.tipPosition().x - lastPointer1.tipPosition().x;
                        dx += lastPointer2.tipPosition().x - latestPointer2.tipPosition().x;
                    } else {
                        dx = latestPointer2.tipPosition().x - lastPointer2.tipPosition().x;
                        dx += lastPointer1.tipPosition().x - latestPointer1.tipPosition().x;
                    }
                    
                    // calculate screen aspect ratio
                    dy = dx;
                    
                    // distance
                    double distance = latestPointer1.tipPosition().distanceTo(latestPointer2.tipPosition());
                    // mod dx/dy by distance, closer together = greater scaling
                    double distanceScale = MIN(distance, 300) / 300;
                    distanceScale = 1 - distanceScale;
                    if (distanceScale < 0.5) distanceScale = 0.5;
                    dx *= distanceScale;
                    dy *= distanceScale;
                    
                    dx *= 5;
                    dy *= 5;

                    cout << "distance: " << distanceScale << " dx: " << dx << ", dy: " << dy << endl;
                    
                    // average x/y
//                    Leap::Vector centerPoint(x, y, 0);
                    
                    // scale window
                    //driver->setWindowCenter(currentWin, centerPoint);
                    driver->scaleWindow(currentWin, dx * 2, dy * 2);
                }
            }
        }
    }
}
    
}