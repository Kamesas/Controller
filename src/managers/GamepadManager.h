#pragma once

// By including the main Bluepad32 header here, we ensure that
// the ControllerPtr type AND all button definitions (like BUTTON_L1)
// are available to any file that includes this header.
#include <Bluepad32.h>

// Function to process gamepad inputs
void processGamepad(ControllerPtr ctl);

// Function to handle the blinking logic for turn signals
void handleBlinking();
