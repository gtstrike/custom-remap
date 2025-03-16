#include <ApplicationServices/ApplicationServices.h>
#include <CoreFoundation/CoreFoundation.h>
#include <stdio.h>
#include <string.h>

#define TARGET_APP "Adobe Photoshop"

void pressKey(CGKeyCode keyCode) {
    CGEventRef keyDown = CGEventCreateKeyboardEvent(NULL, keyCode, true);
    CGEventPost(kCGHIDEventTap, keyDown);
    CFRelease(keyDown);
    
    CGEventRef keyUp = CGEventCreateKeyboardEvent(NULL, keyCode, false);
    CGEventPost(kCGHIDEventTap, keyUp);
    CFRelease(keyUp);
}

bool isPhotoshopActive() {
    CFArrayRef windowList = CGWindowListCopyWindowInfo(kCGWindowListOptionOnScreenOnly, kCGNullWindowID);
    CFIndex count = CFArrayGetCount(windowList);
    bool result = false;
    
    for (CFIndex i = 0; i < count; i++) {
        CFDictionaryRef windowInfo = CFArrayGetValueAtIndex(windowList, i);
        CFStringRef ownerName = CFDictionaryGetValue(windowInfo, kCGWindowOwnerName);
        
        if (ownerName && CFStringCompare(ownerName, CFSTR(TARGET_APP), 0) == kCFCompareEqualTo) {
            result = true;
            break;
        }
    }
    
    CFRelease(windowList);
    return result;
}

CGEventRef callback(CGEventTapProxy proxy, CGEventType type, CGEventRef event, void *refcon) {
    if (type != kCGEventOtherMouseDown) return event;
    
    if (!isPhotoshopActive()) return event;
    
    int button = CGEventGetIntegerValueField(event, kCGMouseEventButtonNumber);
    printf("Mouse Button Pressed: %d\n", button);
    
    switch (button) {
        case 2: pressKey(98);  break; // F7
        case 3: pressKey(100); break; // F8
        case 4: pressKey(109); break; // F10
        default: return event;
    }
    
    return NULL; // Suppress event to prevent double input
}

int main() {
    CFMachPortRef tap = CGEventTapCreate(kCGHIDEventTap, kCGHeadInsertEventTap, kCGEventTapOptionDefault, 1 << kCGEventOtherMouseDown, callback, NULL);
    if (!tap) {
        printf("Failed to create event tap! Check permissions.\n");
        return 1;
    }
    
    CFRunLoopSourceRef runLoopSource = CFMachPortCreateRunLoopSource(NULL, tap, 0);
    CFRunLoopAddSource(CFRunLoopGetCurrent(), runLoopSource, kCFRunLoopDefaultMode);
    CGEventTapEnable(tap, true);
    
    printf("Daemon running: remapping mouse clicks in Photoshop.\n");
    CFRunLoopRun();
    
    return 0;
}
