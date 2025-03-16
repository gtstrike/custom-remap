#import <ApplicationServices/ApplicationServices.h>
#import <CoreFoundation/CoreFoundation.h>
#import <AppKit/AppKit.h>
#import <Cocoa/Cocoa.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

void press_key(CGKeyCode key_code) {
    CGEventRef keyDown = CGEventCreateKeyboardEvent(NULL, key_code, true);
    CGEventRef keyUp = CGEventCreateKeyboardEvent(NULL, key_code, false);
    
    CGEventPost(kCGHIDEventTap, keyDown);
    CGEventPost(kCGHIDEventTap, keyUp);
    
    CFRelease(keyDown);
    CFRelease(keyUp);
}

bool is_photoshop_active() {
    NSRunningApplication *frontmostApp = [[NSWorkspace sharedWorkspace] frontmostApplication];
    NSString *appNameStr = [frontmostApp localizedName];

    return [appNameStr isEqualToString:@"Adobe Photoshop 2022"];
}

CGEventRef callback(CGEventTapProxy proxy, CGEventType type, CGEventRef event, void *refcon) {
    if (type == kCGEventOtherMouseDown) {
        int button_id = (int)CGEventGetIntegerValueField(event, kCGMouseEventButtonNumber);

        if (is_photoshop_active()) {
            if (button_id == 2) {
                printf("🟢 Middle Mouse Button Clicked! Mapping to F7...\n");
                press_key((CGKeyCode)98); // F7
            } else if (button_id == 3) {
                printf("🔵 Button 3 Clicked! Mapping to F8...\n");
                press_key((CGKeyCode)100); // F8
            } else if (button_id == 4) {
                printf("🟣 Button 4 Clicked! Mapping to F10...\n");
                press_key((CGKeyCode)109); // F10
            }
            fflush(stdout);
            return NULL;
        }
    }
    return event;
}

int main() {
    printf("🔑 Ensure you have granted Terminal or your app Full Disk Access and Accessibility permissions.\n");

    CFMachPortRef eventTap = CGEventTapCreate(
        kCGHIDEventTap,
        kCGHeadInsertEventTap,
        kCGEventTapOptionDefault,
        1 << kCGEventOtherMouseDown,
        callback,
        NULL
    );

    if (!eventTap) {
        printf("❌ Failed to create event tap. Check Accessibility permissions!\n");
        return 1;
    }

    CFRunLoopSourceRef runLoopSource = CFMachPortCreateRunLoopSource(kCFAllocatorDefault, eventTap, 0);
    CFRunLoopAddSource(CFRunLoopGetCurrent(), runLoopSource, kCFRunLoopCommonModes);
    
    CGEventTapEnable(eventTap, true);

    printf("✅ Listening for mouse button clicks... (Press Ctrl+C to stop)\n");
    fflush(stdout);

    CFRunLoopRun();

    return 0;
}
