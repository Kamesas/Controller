#ifndef CONTROLLER_MANAGER_H
#define CONTROLLER_MANAGER_H

#include <Bluepad32.h>

// Callback functions for controller connections
void onConnectedController(ControllerPtr ctl);
void onDisconnectedController(ControllerPtr ctl);

// Main function to process controller data
void processControllers();

#endif // CONTROLLER_MANAGER_H