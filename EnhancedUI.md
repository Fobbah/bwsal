Class header file: http://code.google.com/p/bwsal/source/browse/trunk/BWSAL/include/EnhancedUI.h

Not a manager.

The EnhancedUI class handles the UI additions to the Starcraft game screen which augment the original Starcraft UI system but are not directly related to debugging. While the EnhancedUI is not dependent on the BWSAL managers (excluding UnitGroupManager), it should provide functionality to assist in developing and debugging BWSAL.

  * [update](#update.md)

## update ##
void update();

Should be called from AIModule::onFrame. Displays the terrain markers, base locations, and progress bars for units being constructed, morphing, or training.