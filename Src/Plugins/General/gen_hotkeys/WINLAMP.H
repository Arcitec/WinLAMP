#ifndef _WAFE_H_
#define _WAFE_H_

#define IPC_FF_FIRST 2000
#define IPC_FF_NOTIFYHOTKEY IPC_FF_FIRST + 6 // data = const char * to hotkey description

#define WINLAMP_OPTIONS_EQ               40036 // toggles the EQ window
#define WINLAMP_OPTIONS_PLEDIT           40040 // toggles the playlist window
#define WINLAMP_VOLUMEUP                 40058 // turns the volume up a little
#define WINLAMP_VOLUMEDOWN               40059 // turns the volume down a little
#define WINLAMP_FFWD5S                   40060 // fast forwards 5 seconds
#define WINLAMP_REW5S                    40061 // rewinds 5 seconds

// the following are the five main control buttons, with optionally shift 
// or control pressed
// (for the exact functions of each, just try it out)
#define WINLAMP_BUTTON1                  40044
#define WINLAMP_BUTTON2                  40045
#define WINLAMP_BUTTON3                  40046
#define WINLAMP_BUTTON4                  40047
#define WINLAMP_BUTTON5                  40048
#define WINLAMP_BUTTON1_SHIFT            40144
#define WINLAMP_BUTTON2_SHIFT            40145
#define WINLAMP_BUTTON3_SHIFT            40146
#define WINLAMP_BUTTON4_SHIFT            40147
#define WINLAMP_BUTTON5_SHIFT            40148
#define WINLAMP_BUTTON1_CTRL             40154
#define WINLAMP_BUTTON2_CTRL             40155
#define WINLAMP_BUTTON3_CTRL             40156
#define WINLAMP_BUTTON4_CTRL             40157
#define WINLAMP_BUTTON5_CTRL             40158

#define WINLAMP_FILE_PLAY                40029 // pops up the load file(s) box
#define WINLAMP_OPTIONS_PREFS            40012 // pops up the preferences
#define WINLAMP_OPTIONS_AOT              40019 // toggles always on top
#define WINLAMP_HELP_ABOUT               40041 // pops up the about box :)

#endif