// NonVol.cpp
// Store/Load User Preference to/from Non-Volatile Memory

#include <Arduino.h>
#include <avr/eeprom.h>
#include "A1Main.h"
#include "HamBands.h"
#include "ButtonUtil.h"
#include "MorseCode.h"
#include "Menus.h"
#include "NonVol.h"
#include "debug.h"

#define ID_FLAG (1409141141L)  // YYMMDDHHMM, Used for EEPROM Structure Revision Flag


// Local Varibles
long idFlag = ID_FLAG;


// ###############################################################################
void eePromIO(int mode) {
#define DEBUG(x ...)  // Default to NO debug    
//#define DEBUG(x ...) debugUnique(x)    // UnComment for Debug
    
   DEBUG(P("\nFunc: %s %d"), __func__, __LINE__);
   
   struct config_t {
        long idFlag;
        unsigned long frequency;
        int editIfMode;
        unsigned long iFreqUSB;
        unsigned long iFreqLSB;
        unsigned long vfoA;
        unsigned long vfoB;
        boolean isLSB;
        byte vfoActive;
        unsigned long freqCache[BANDS*2];
        byte sideBandMode;
        byte sideBandModeCache[BANDS*2];
        byte cw_wpm;
        unsigned int qrssDitTime;
        unsigned long blinkTimeOut;
        unsigned long blinkPeriod;
        byte blinkRatio;
        unsigned long menuIdleTimeOut;
        byte checkSum;
    } E;
    byte checkSum = 0;
    byte *pb = (byte *)&E;
    
    DEBUG(P("\nFunc: %s %d, editIfMode= %d"), __func__, __LINE__, editIfMode);
    if (editIfMode) return; // Do Nothing if in Edit-IF-Mode   
    
    cursorOff();
   
    DEBUG(P("\nFunc: %s %d, Mode= %d"), __func__, __LINE__, mode);
    switch(mode) {
    case EEP_LOAD:
        DEBUG(P("Loading EEPROM"));
        // Read from Non-Volatile Memory and check for the correct ID
        eeprom_read_block(&E, 0, sizeof(E));
        if (E.idFlag != ID_FLAG) { sprintf(c, P("Load Failed ID")); break; }
        
        // Compute and Check the CheckSum
        for (int i = 0; i < sizeof(E); i++) checkSum += *pb++;
        if (checkSum != 0) { sprintf(c, P("Load Failed CSum")); break; }
        
        // Assign Values to Working Variables from eeProm Structure
        idFlag = E.idFlag; 
        frequency = E.frequency;
        editIfMode = E.editIfMode;
        iFreqUSB = E.iFreqUSB;
        iFreqLSB = E.iFreqLSB;
        vfoA = E.vfoA;
        vfoB = E.vfoB;
        isLSB = E.isLSB;
        vfoActive = E.vfoActive;
        memcpy(freqCache, E.freqCache, sizeof(E.freqCache));
        sideBandMode = E.sideBandMode;
        memcpy(sideBandModeCache, E.sideBandModeCache, sizeof(E.sideBandModeCache));
        cw_wpm = E.cw_wpm;
        qrssDitTime = E.qrssDitTime;
        blinkTimeOut = E.blinkTimeOut;
        blinkPeriod = E.blinkPeriod;
        blinkRatio = E.blinkRatio;
        menuIdleTimeOut = E.menuIdleTimeOut;
        checkSum = E.checkSum;
       
        sprintf(c, P("Loading %dB"), sizeof(E));      
        break;
        
    case EEP_SAVE :
        // Assign Working Variables to the eeProm Structure
        E.idFlag = ID_FLAG;
        E.frequency = frequency;
        E.editIfMode = editIfMode;
        E.iFreqUSB = iFreqUSB;
        E.iFreqLSB = iFreqLSB;
        E.vfoA = vfoA;
        E.vfoB = vfoB;
        E.isLSB = isLSB;
        E.vfoActive = vfoActive;
        memcpy(E.freqCache, freqCache, sizeof(E.freqCache));
        E.sideBandMode = sideBandMode;
        memcpy(E.sideBandModeCache, sideBandModeCache, sizeof(E.sideBandModeCache));
        E.cw_wpm = cw_wpm;
        E.qrssDitTime = qrssDitTime;
        E.blinkTimeOut = blinkTimeOut;
        E.blinkPeriod = blinkPeriod;
        E.blinkRatio = blinkRatio;
        E.menuIdleTimeOut = menuIdleTimeOut;
        E.checkSum = checkSum;   // Not necessary, used here as an Optical Place Holder
        
        // Compute and save the new Checksum of eeProm Struture
        for (int i = 0; i < sizeof(E) - sizeof(E.checkSum); i++) checkSum += *pb++;
        E.checkSum = -checkSum; // Apply CheckSum to Structure
        
        // Write the eeProm Strcture to Non-Volatile Memory
        eeprom_write_block(&E, 0, sizeof(E));
        
        sprintf(c, P("Storing %dB"), sizeof(E));
        break;
    }
   
    printLineCEL(STATUS_LINE, c);
    delay(500);
    deDounceBtnRelease(); // Wait for Release
} 


// ###############################################################################
void loadUserPreferences() {
#define DEBUG(x ...)  // Default to NO debug    
//#define DEBUG(x ...) debugUnique(x)    // UnComment for Debug
    
    DEBUG(P("\nFunc: %s %d"), __func__, __LINE__);
      
    // Check EEPROM for User Saved Preference, Load if available
    // Hold FN Button at Power-ON or Processor Reset does a "Factory Reset" to Default Values
    printLineCEL(FIRST_LINE, P("User Pref:"));
    if (btnDown() != FN_BTN) eePromIO(EEP_LOAD);
    else {
        printLineCEL(STATUS_LINE, P("Factory Reset"));
        #ifdef USE_AUTOSAVE_FACTORY_RESET
            eePromIO(EEP_SAVE);
        #endif // USE_AUTOSAVE_FACTORY_RESET
     }
    delay(500);
    deDounceBtnRelease(); // Wait for Button Release 
}


// End
