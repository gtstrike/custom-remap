#import <ApplicationServices/ApplicationServices.h>
#import <CoreFoundation/CoreFoundation.h>
#import <AppKit/AppKit.h>
#import <Cocoa/Cocoa.h>
#include <stdio.h>
#include <unistd.h>

// Function to simulate key press
void press_key(CGKeyCode key_code) {
    CGEventRef keyDown = CGEventCreateKeyboardEvent(NULL, key_code, true);
    CGEventRef keyUp = CGEventCreateKeyboardEvent(NULL, key_code, false);

    CGEventPost(kCGHIDEventTap, keyDown);
    CGEventPost(kCGHIDEventTap, keyUp);

    CFRelease(keyDown);
    CFRelease(keyUp);
}

// Function to check if Chrome is active
bool is_chrome_active() {
    NSRunningApplication *frontmostApp = [[NSWorkspace sharedWorkspace] frontmostApplication];
    NSString *appNameStr = [frontmostApp localizedName];

    printf("Active App: %s\n", [appNameStr UTF8String]);

    return [appNameStr isEqualToString:@"Google Chrome"];
}

// Mouse button callback
CGEventRef mouse_callback(CGEventTapProxy proxy, CGEventType type, CGEventRef event, void *refcon) {
    if (type == kCGEventOtherMouseDown) {
        int button_id = (int)CGEventGetIntegerValueField(event, kCGMouseEventButtonNumber);
        printf("Mouse Button ID: %d\n", button_id);
        fflush(stdout);

        if (button_id == 2) {
            printf("🟢 Middle Mouse → F7\n");
            press_key((CGKeyCode)98); // F7
        } else if (button_id == 3) {
            printf("🔵 Button 3 → F8\n");
            press_key((CGKeyCode)100); // F8
        } else if (button_id == 4) {
            printf("🟣 Button 4 → F10\n");
            press_key((CGKeyCode)109); // F10
        }
    }
    return event;
}

// App focus change handler
void app_focus_changed(NSNotification *notification) {
    if (is_chrome_active()) {
        printf("🔴 Chrome is active. Running AppleScript...\n");
        system("osascript /Users/gemma/Documents/work/custom-remap/saveonotherapp.scpt"); // Adjust path to your actual .scpt file
    }
}

int main() {
    printf("🔑 Grant Full Disk Access + Accessibility permissions to this binary.\n");

    // Set up Chrome watcher
    NSWorkspace *workspace = [NSWorkspace sharedWorkspace];
    [[workspace notificationCenter] addObserverForName:NSWorkspaceDidActivateApplicationNotification
                                               object:nil
                                                queue:nil
                                           usingBlock:^(NSNotification *note) {
        app_focus_changed(note);
    }];

    // Set up mouse hook
    CFMachPortRef eventTap = CGEventTapCreate(
        kCGHIDEventTap,
        kCGHeadInsertEventTap,
        kCGEventTapOptionDefault,
        CGEventMaskBit(kCGEventOtherMouseDown),
        mouse_callback,
        NULL
    );

    if (!eventTap) {
        fprintf(stderr, "❌ Failed to create event tap. Check permissions.\n");
        return 1;
    }

    CFRunLoopSourceRef runLoopSource = CFMachPortCreateRunLoopSource(kCFAllocatorDefault, eventTap, 0);
    CFRunLoopAddSource(CFRunLoopGetCurrent(), runLoopSource, kCFRunLoopCommonModes);
    CGEventTapEnable(eventTap, true);

    printf("✅ Running daemon: monitoring mouse + Chrome focus...\n");
    fflush(stdout);

    CFRunLoopRun();
    return 0;
}
