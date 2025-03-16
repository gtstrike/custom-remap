#include <ApplicationServices/ApplicationServices.h>
#include <CoreFoundation/CoreFoundation.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>

// Function to get the active application's name
char* get_active_app() {
    ProcessSerialNumber psn;
    CFStringRef appName;
    GetFrontProcess(&psn);
    CopyProcessName(&psn, &appName);
    static char appNameCStr[256];
    CFStringGetCString(appName, appNameCStr, sizeof(appNameCStr), kCFStringEncodingUTF8);
    CFRelease(appName);
    return appNameCStr;
}

// Function to simulate a key press
void press_key(CGKeyCode key) {
    CGEventRef keyDown = CGEventCreateKeyboardEvent(NULL, key, true);
    CGEventRef keyUp = CGEventCreateKeyboardEvent(NULL, key, false);
    CGEventPost(kCGHIDEventTap, keyDown);
    CGEventPost(kCGHIDEventTap, keyUp);
    CFRelease(keyDown);
    CFRelease(keyUp);
}

// Callback function to intercept mouse clicks
CGEventRef callback(CGEventTapProxy proxy, CGEventType type, CGEventRef event, void *refcon) {
    if (type == kCGEventOtherMouseDown) {
        int button = CGEventGetIntegerValueField(event, kCGMouseEventButtonNumber);
        if (strcmp(get_active_app(), "Adobe Photoshop 2022") == 0) { // Check if Photoshop is active
            switch (button) {
                case 2:
                    press_key(98);  // F7
                    return NULL;
                case 3:
                    press_key(100); // F8
                    return NULL;
                case 4:
                    press_key(109); // F10
                    return NULL;
            }
        }
    }
    return event;
}

int main() {
    CGEventMask mask = (1 << kCGEventOtherMouseDown);
    CFMachPortRef eventTap = CGEventTapCreate(kCGHIDEventTap, kCGHeadInsertEventTap, 0, mask, callback, NULL);
    if (!eventTap) {
        fprintf(stderr, "Failed to create event tap. Check permissions!\n");
        return 1;
    }
    
    CFRunLoopSourceRef runLoopSource = CFMachPortCreateRunLoopSource(kCFAllocatorDefault, eventTap, 0);
    CFRunLoopAddSource(CFRunLoopGetCurrent(), runLoopSource, kCFRunLoopCommonModes);
    CGEventTapEnable(eventTap, true);
    printf("✅ Listening for mouse clicks in Adobe Photoshop 2022...\n");
    CFRunLoopRun();
    return 0;
}
