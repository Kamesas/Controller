#ifndef GAMEPAD_LOGGER_H
#define GAMEPAD_LOGGER_H

#include <Bluepad32.h>

// This function will check for state changes and log them.
void logGamepadOnStateChange(ControllerPtr ctl);

#endif // GAMEPAD_LOGGER_H