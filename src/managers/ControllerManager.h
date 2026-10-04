#pragma once
#include <Bluepad32.h>

// Callback functions for Bluepad32
void onConnectedController(ControllerPtr ctl);
void onDisconnectedController(ControllerPtr ctl);

// Main processing loop function
void processControllers();