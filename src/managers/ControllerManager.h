#pragma once
#include <Bluepad32.h> // It's best to include the main header here

// Forward declaration
void processGamepad(ControllerPtr ctl);

// Callback functions for Bluepad32
void onConnectedController(ControllerPtr ctl);
void onDisconnectedController(ControllerPtr ctl);

// Main processing loop function
void processControllers();