/*
    Nudelta Console
    Copyright (C) 2022 Mohamed Gaber

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/
#include "access.hpp"

#if defined(__APPLE_CC__) || defined(__APPLE__)
    #include <IOKit/hidsystem/IOHIDLib.h>

std::optional< bool > checkHIDAccess() {
    switch (IOHIDCheckAccess(kIOHIDRequestTypeListenEvent)) {
        case kIOHIDAccessTypeGranted:
            return true;
            break;
        case kIOHIDAccessTypeDenied:
            return false;
            break;
        default:
            return std::nullopt;
            break;
    }
}
bool requestHIDAccess() {
    return IOHIDRequestAccess(kIOHIDRequestTypeListenEvent);
}

const char *hidAccessFailureMessage =
    "Please grant Nudelta input monitoring permissions in your System Preferences.";

#elif defined(__gnu_linux__)
std::optional< bool > checkHIDAccess() {
    return true;
}

bool requestHIDAccess() {
    return true;
}

const char *hidAccessFailureMessage =
    "Unable to read HID devices. Try running these commands then restarting your computer: \n\n"
    "echo 'KERNEL==\"hidraw*\", SUBSYSTEM==\"hidraw\", TAG+=\"uaccess\"' | sudo tee /etc/udev/rules.d/70-nudelta.rules &&"
    "sudo udevadm control --reload-rules && sudo udevadm trigger";
#else
// Not implemented/Can't check

std::optional< bool > checkHIDAccess() {
    return true;
}

bool requestHIDAccess() {
    return true;
}

const char *hidAccessFailureMessage =
    "Unable to read HID devices. Consider running Nudelta as a superuser or checking your operating system's HID access permissions.";

#endif