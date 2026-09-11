#include "zh_ui.h"
#include "catalogGUI.hpp"
#include "khicas_gb18030.h"
#include <fxcg/display.h>
#include <string.h>

struct zh_ui_entry { const char *source; const char *translated; };
static const zh_ui_entry zh_ui_entries[] = {
  {"Config","\x01""""\xc9""""\xe8""""\xd6""""\xc3"""},
  {"Shortcuts","\x01""""\xbf""""\xec""""\xbd""""\xdd""""\xbc""""\xfc"""},
  {"About","\x01""""\xb9""""\xd8""""\xd3""""\xda"""},
  {"A propos","\x01""""\xb9""""\xd8""""\xd3""""\xda"""},
  {"Quit","\x01""""\xcd""""\xcb""""\xb3""""\xf6"""},
  {"Python heap ","\x01""Python ""\xb6""""\xd1"" "},
  {"Python not available","\x01""Python ""\xb2""""\xbb""""\xbf""""\xc9""""\xd3""""\xc3"""},
  {"Digits (9, 19, 28)?","\x01""""\xca""""\xfd""""\xd7""""\xd6""""\xce""""\xbb""""\xca""""\xfd""""\xa3""""\xa8""9""\xa1""""\xa2""19""\xa1""""\xa2""28""\xa3""""\xa9""""\xa3""""\xbf"""},
  {"Heap size in K","\x01""""\xb6""""\xd1""""\xb4""""\xf3""""\xd0""""\xa1""""\xa3""""\xa8""K""\xa3""""\xa9"""},
  {"No Data","\x01""""\xce""""\xde""""\xca""""\xfd""""\xbe""""\xdd"""},
  {"Variables","\x01""""\xb1""""\xe4""""\xc1""""\xbf"""},
  {"Last result","\x01""""\xc9""""\xcf""""\xb4""""\xce""""\xbd""""\xe1""""\xb9""""\xfb"""},
  {"Current command","\x01""""\xb5""""\xb1""""\xc7""""\xb0""""\xc3""""\xfc""""\xc1""""\xee"""},
  {"Clip on:","\x01""""\xbc""""\xf4""""\xcc""""\xf9""""\xb0""""\xe5""""\xc4""""\xda""""\xc8""""\xdd""""\xa3""""\xba"""},
  {"Save session ","\x01""""\xb1""""\xa3""""\xb4""""\xe6""""\xbb""""\xe1""""\xbb""""\xb0"""},
  {"Save session as","\x01""""\xbb""""\xe1""""\xbb""""\xb0""""\xc1""""\xed""""\xb4""""\xe6""""\xce""""\xaa"""},
  {"Load session","\x01""""\xbc""""\xd3""""\xd4""""\xd8""""\xbb""""\xe1""""\xbb""""\xb0"""},
  {"New session","\x01""""\xd0""""\xc2""""\xbd""""\xa8""""\xbb""""\xe1""""\xbb""""\xb0"""},
  {"New script","\x01""""\xd0""""\xc2""""\xbd""""\xa8""""\xbd""""\xc5""""\xb1""""\xbe"""},
  {"Edit script","\x01""""\xb1""""\xe0""""\xbc""""\xad""""\xbd""""\xc5""""\xb1""""\xbe"""},
  {"Run script","\x01""""\xd4""""\xcb""""\xd0""""\xd0""""\xbd""""\xc5""""\xb1""""\xbe"""},
  {"Matrix editor","\x01""""\xbe""""\xd8""""\xd5""""\xf3""""\xb1""""\xe0""""\xbc""""\xad""""\xc6""""\xf7"""},
  {"Clear","\x01""""\xc7""""\xe5""""\xb3""""\xfd"""},
  {"Clear script","\x01""""\xc7""""\xe5""""\xb3""""\xfd""""\xbd""""\xc5""""\xb1""""\xbe"""},
  {"Radians","\x01""""\xbb""""\xa1""""\xb6""""\xc8"""},
  {"Auto save","\x01""""\xd7""""\xd4""""\xb6""""\xaf""""\xb1""""\xa3""""\xb4""""\xe6"""},
  {"Function Catalog","\x01""""\xba""""\xaf""""\xca""""\xfd""""\xc4""""\xbf""""\xc2""""\xbc"""},
  {"  INPUT","\x01""""\xca""""\xe4""""\xc8""""\xeb"""},
  {" EXAMPL1","\x01""""\xca""""\xbe""""\xc0""""\xfd"" 1"},
  {" EXAMPL2","\x01""""\xca""""\xbe""""\xc0""""\xfd"" 2"},
  {"  HELP","\x01""""\xb0""""\xef""""\xd6""""\xfa"""},
  {" QRHELP ","\x01""""\xb6""""\xfe""""\xce""""\xac""""\xc2""""\xeb""""\xb0""""\xef""""\xd6""""\xfa"""},
  {"Help on command","\x01""""\xc3""""\xfc""""\xc1""""\xee""""\xb0""""\xef""""\xd6""""\xfa"""},
  {"Sorry, no help available...","\x01""""\xb1""""\xa7""""\xc7""""\xb8""""\xa3""""\xac""""\xc3""""\xbb""""\xd3""""\xd0""""\xbf""""\xc9""""\xd3""""\xc3""""\xb5""""\xc4""""\xb0""""\xef""""\xd6""""\xfa""""\xa1""""\xa3"""},
  {"display option: ","\x01""""\xcf""""\xd4""""\xca""""\xbe""""\xd1""""\xa1""""\xcf""""\xee""""\xa3""""\xba"""},
  {"color option: ","\x01""""\xd1""""\xd5""""\xc9""""\xab""""\xd1""""\xa1""""\xcf""""\xee""""\xa3""""\xba"""},
  {"fsolve option: ","\x01""""\xca""""\xfd""""\xd6""""\xb5""""\xc7""""\xf3""""\xbd""""\xe2""""\xd1""""\xa1""""\xcf""""\xee""""\xa3""""\xba"""},
  {"Object type: ","\x01""""\xb6""""\xd4""""\xcf""""\xf3""""\xc0""""\xe0""""\xd0""""\xcd""""\xa3""""\xba"""},
  {"EXE/EXIT: OK","\x01""EXE/EXIT""\xa3""""\xba""""\xc8""""\xb7""""\xb6""""\xa8"""},
  {"Xcas doc qrcode","\x01""Xcas ""\xce""""\xc4""""\xb5""""\xb5""""\xb6""""\xfe""""\xce""""\xac""""\xc2""""\xeb"""},
  {"Line copied to clipboard","\x01""""\xd2""""\xd1""""\xbd""""\xab""""\xd0""""\xd0""""\xb8""""\xb4""""\xd6""""\xc6""""\xb5""""\xbd""""\xbc""""\xf4""""\xcc""""\xf9""""\xb0""""\xe5"""},
  {"Line cut and copied to clipboard","\x01""""\xd2""""\xd1""""\xbc""""\xf4""""\xc7""""\xd0""""\xb2""""\xa2""""\xb8""""\xb4""""\xd6""""\xc6""""\xd0""""\xd0"""},
  {"CATALOG","\x01""""\xc4""""\xbf""""\xc2""""\xbc"""},
  {"File,Cfg","\x01""""\xce""""\xc4""""\xbc""""\xfe""""\xa1""""\xa2""""\xc9""""\xe8""""\xd6""""\xc3"""},
  {"Edit","\x01""""\xb1""""\xe0""""\xbc""""\xad"""},
  {"simp","\x01""""\xbb""""\xaf""""\xbc""""\xf2"""},
  {"Syntax","\x01""""\xd3""""\xef""""\xb7""""\xa8"""},
  {"Xcas interpreter","\x01""Xcas ""\xbd""""\xe2""""\xca""""\xcd""""\xc6""""\xf7"""},
  {"XcasPy ^=**","\x01""XcasPy ^=**"},
  {"XcasPy ^=xor","\x01""XcasPy ^=xor"},
  {"Clear variables?","\x01""""\xc7""""\xe5""""\xb3""""\xfd""""\xb1""""\xe4""""\xc1""""\xbf""""\xa3""""\xbf"""},
  {"F1: cancel,  F6: confirm","\x01""F1""\xa3""""\xba""""\xc8""""\xa1""""\xcf""""\xfb""""\xa3""""\xac""F6""\xa3""""\xba""""\xc8""""\xb7""""\xc8""""\xcf"""},
  {"Keep variables?","\x01""""\xb1""""\xa3""""\xc1""""\xf4""""\xb1""""\xe4""""\xc1""""\xbf""""\xa3""""\xbf"""},
  {"F1: keep,   F6: erase","\x01""F1""\xa3""""\xba""""\xb1""""\xa3""""\xc1""""\xf4""""\xa3""""\xac""F6""\xa3""""\xba""""\xc7""""\xe5""""\xb3""""\xfd"""},
  {"F1: yes,     F6: cancel","\x01""F1""\xa3""""\xba""""\xca""""\xc7""""\xa3""""\xac""F6""\xa3""""\xba""""\xc8""""\xa1""""\xcf""""\xfb"""},
  {"F1/F6: ok","\x01""F1/F6""\xa3""""\xba""""\xc8""""\xb7""""\xb6""""\xa8"""},
  {"Press F1 or F6","\x01""""\xb0""""\xb4"" F1 ""\xbb""""\xf2"" F6"},
  {"Invalid value","\x01""""\xca""""\xfd""""\xd6""""\xb5""""\xce""""\xde""""\xd0""""\xa7"""},
  {"Interrupted","\x01""""\xd2""""\xd1""""\xd6""""\xd0""""\xb6""""\xcf"""},
  {"Screen frozen. Press any key.","\x01""""\xc6""""\xc1""""\xc4""""\xbb""""\xd2""""\xd1""""\xb6""""\xb3""""\xbd""""\xe1""""\xa1""""\xa3""""\xb0""""\xb4""""\xc8""""\xce""""\xd2""""\xe2""""\xbc""""\xfc""""\xbc""""\xcc""""\xd0""""\xf8""""\xa1""""\xa3"""},
  {"Type MENU key","\x01""""\xc7""""\xeb""""\xb0""""\xb4"" MENU ""\xbc""""\xfc"""},
  {"Memory full","\x01""""\xc4""""\xda""""\xb4""""\xe6""""\xb2""""\xbb""""\xd7""""\xe3"""},
  {"Purge variable","\x01""""\xc7""""\xe5""""\xb3""""\xfd""""\xb1""""\xe4""""\xc1""""\xbf"""},
  {"Saving in progress","\x01""""\xd5""""\xfd""""\xd4""""\xda""""\xb1""""\xa3""""\xb4""""\xe6"""},
  {"Auto-save disabled","\x01""""\xd7""""\xd4""""\xb6""""\xaf""""\xb1""""\xa3""""\xb4""""\xe6""""\xd2""""\xd1""""\xbd""""\xfb""""\xd3""""\xc3"""},
  {"Current session will be lost","\x01""""\xb5""""\xb1""""\xc7""""\xb0""""\xbb""""\xe1""""\xbb""""\xb0""""\xbd""""\xab""""\xb6""""\xaa""""\xca""""\xa7"""},
  {"Clear history?","\x01""""\xc7""""\xe5""""\xb3""""\xfd""""\xc0""""\xfa""""\xca""""\xb7""""\xbc""""\xc7""""\xc2""""\xbc""""\xa3""""\xbf"""},
  {"Really erase?","\x01""""\xc8""""\xb7""""\xb6""""\xa8""""\xd2""""\xaa""""\xc7""""\xe5""""\xb3""""\xfd""""\xc2""""\xf0""""\xa3""""\xbf"""},
  {"Quit?","\x01""""\xcd""""\xcb""""\xb3""""\xf6""""\xa3""""\xbf"""},
  {"Save current sheet?","\x01""""\xb1""""\xa3""""\xb4""""\xe6""""\xb5""""\xb1""""\xc7""""\xb0""""\xb1""""\xed""""\xb8""""\xf1""""\xa3""""\xbf"""},
  {"Error reading file","\x01""""\xb6""""\xc1""""\xc8""""\xa1""""\xce""""\xc4""""\xbc""""\xfe""""\xb3""""\xf6""""\xb4""""\xed"""},
  {"Could not find an empty cell","\x01""""\xd5""""\xd2""""\xb2""""\xbb""""\xb5""""\xbd""""\xbf""""\xd5""""\xb5""""\xa5""""\xd4""""\xaa""""\xb8""""\xf1"""},
  {"Unable to convert","\x01""""\xce""""\xde""""\xb7""""\xa8""""\xd7""""\xaa""""\xbb""""\xbb"""},
  {"Unable to find polynomial roots","\x01""""\xd5""""\xd2""""\xb2""""\xbb""""\xb5""""\xbd""""\xb6""""\xe0""""\xcf""""\xee""""\xca""""\xbd""""\xb8""""\xf9"""},
  {"Not a polynomial or deg<2 or deg>8","\x01""""\xb2""""\xbb""""\xca""""\xc7""""\xb6""""\xe0""""\xcf""""\xee""""\xca""""\xbd""""\xa3""""\xac""""\xbb""""\xf2""""\xb4""""\xce""""\xca""""\xfd""""\xb2""""\xbb""""\xd4""""\xda"" 2 ""\xb5""""\xbd"" 8 ""\xd6""""\xae""""\xbc""""\xe4"""},
  {"Numerical Integration Error","\x01""""\xca""""\xfd""""\xd6""""\xb5""""\xbb""""\xfd""""\xb7""""\xd6""""\xb4""""\xed""""\xce""""\xf3"""},
  {"No more free variable available","\x01""""\xc3""""\xbb""""\xd3""""\xd0""""\xbf""""\xc9""""\xd3""""\xc3""""\xb5""""\xc4""""\xd7""""\xd4""""\xd3""""\xc9""""\xb1""""\xe4""""\xc1""""\xbf"""},
  {"Root at","\x01""""\xb8""""\xf9""""\xce""""\xbb""""\xd3""""\xda"""},
  {"Screen frozen. Press EXIT.","\x01""""\xc6""""\xc1""""\xc4""""\xbb""""\xd2""""\xd1""""\xb6""""\xb3""""\xbd""""\xe1""""\xa1""""\xa3""""\xb0""""\xb4"" EXIT ""\xbc""""\xcc""""\xd0""""\xf8""""\xa1""""\xa3"""},
  {"F1: back to editor,  F6: confirm","\x01""F1""\xa3""""\xba""""\xb7""""\xb5""""\xbb""""\xd8""""\xb1""""\xe0""""\xbc""""\xad""""\xc6""""\xf7""""\xa3""""\xac""F6""\xa3""""\xba""""\xc8""""\xb7""""\xc8""""\xcf"""},
  {"EXE: quit, result stored in last","\x01""EXE""\xa3""""\xba""""\xcd""""\xcb""""\xb3""""\xf6""""\xa3""""\xac""""\xbd""""\xe1""""\xb9""""\xfb""""\xb1""""\xa3""""\xb4""""\xe6""""\xb5""""\xbd"" last"},
  {"+-: zoom, pad: move, EXIT: quit","\x01""+/-""\xa3""""\xba""""\xcb""""\xf5""""\xb7""""\xc5""""\xa3""""\xac""""\xb7""""\xbd""""\xcf""""\xf2""""\xbc""""\xfc""""\xa3""""\xba""""\xd2""""\xc6""""\xb6""""\xaf""""\xa3""""\xac""EXIT""\xa3""""\xba""""\xcd""""\xcb""""\xb3""""\xf6"""},
  {"Type MENU key, open another application","\x01""""\xb0""""\xb4"" MENU ""\xbc""""\xfc""""\xa3""""\xac""""\xc8""""\xbb""""\xba""""\xf3""""\xb4""""\xf2""""\xbf""""\xaa""""\xc6""""\xe4""""\xcb""""\xfb""""\xd3""""\xa6""""\xd3""""\xc3"""},
  {"File exists!","\x01""""\xce""""\xc4""""\xbc""""\xfe""""\xd2""""\xd1""""\xb4""""\xe6""""\xd4""""\xda""""\xa3""""\xa1"""},
  {"F1:overwrite,           F6: cancel","\x01""F1""\xa3""""\xba""""\xb8""""\xb2""""\xb8""""\xc7""""\xa3""""\xac""F6""\xa3""""\xba""""\xc8""""\xa1""""\xcf""""\xfb"""},
  {"F1: overwrite,           F6: cancel","\x01""F1""\xa3""""\xba""""\xb8""""\xb2""""\xb8""""\xc7""""\xa3""""\xac""F6""\xa3""""\xba""""\xc8""""\xa1""""\xcf""""\xfb"""},
  {"F1: save, F6: discard changes","\x01""F1""\xa3""""\xba""""\xb1""""\xa3""""\xb4""""\xe6""""\xa3""""\xac""F6""\xa3""""\xba""""\xb7""""\xc5""""\xc6""""\xfa""""\xd0""""\xde""""\xb8""""\xc4"""},
  {"Changes will be lost","\x01""""\xd0""""\xde""""\xb8""""\xc4""""\xbd""""\xab""""\xb6""""\xaa""""\xca""""\xa7"""},
  {"Replace? EXE: Yes, 8 or N: No","\x01""""\xcc""""\xe6""""\xbb""""\xbb""""\xa3""""\xbf""EXE""\xa3""""\xba""""\xca""""\xc7""""\xa3""""\xac""8 ""\xbb""""\xf2"" N""\xa3""""\xba""""\xb7""""\xf1"""},
  {"EXE: next, AC: cancel","\x01""EXE""\xa3""""\xba""""\xcf""""\xc2""""\xd2""""\xbb""""\xb8""""\xf6""""\xa3""""\xac""AC""\xa3""""\xba""""\xc8""""\xa1""""\xcf""""\xfb"""},
  {"PAD: select, CLIP: copy, AC: cancel","\x01""""\xb7""""\xbd""""\xcf""""\xf2""""\xbc""""\xfc""""\xa3""""\xba""""\xd1""""\xa1""""\xd4""""\xf1""""\xa3""""\xac""CLIP""\xa3""""\xba""""\xb8""""\xb4""""\xd6""""\xc6""""\xa3""""\xac""AC""\xa3""""\xba""""\xc8""""\xa1""""\xcf""""\xfb"""},
  {"Search, replace","\x01""""\xb2""""\xe9""""\xd5""""\xd2""""\xa1""""\xa2""""\xcc""""\xe6""""\xbb""""\xbb"""},
  {"Goto line","\x01""""\xcc""""\xf8""""\xd7""""\xaa""""\xb5""""\xbd""""\xd0""""\xd0"""},
  {"Insert","\x01""""\xb2""""\xe5""""\xc8""""\xeb"""},
  {"Check syntax","\x01""""\xbc""""\xec""""\xb2""""\xe9""""\xd3""""\xef""""\xb7""""\xa8"""},
  {"Scripts","\x01""""\xbd""""\xc5""""\xb1""""\xbe"""},
  {"File","\x01""""\xce""""\xc4""""\xbc""""\xfe"""},
  {"Help","\x01""""\xb0""""\xef""""\xd6""""\xfa"""},
  {"EXE: ok, EXIT: cancel","\x01""EXE""\xa3""""\xba""""\xc8""""\xb7""""\xb6""""\xa8""""\xa3""""\xac""EXIT""\xa3""""\xba""""\xc8""""\xa1""""\xcf""""\xfb"""},
  {"MicroPython 1.12 (c) D. George et al","\x01""MicroPython 1.12""\xa3""""\xa8""""\xb0""""\xe6""""\xc8""""\xa8""""\xcb""""\xf9""""\xd3""""\xd0"" D. George ""\xb5""""\xc8""""\xa3""""\xa9"""},
  {"License GPL2 (KhiCAS), MIT (mPython)","\x01""""\xd0""""\xed""""\xbf""""\xc9""""\xd6""""\xa4""""\xa3""""\xba""GPL2""\xa3""""\xa8""KhiCAS""\xa3""""\xa9""""\xa3""""\xac""MIT""\xa3""""\xa8""mPython""\xa3""""\xa9"""},
  {"  License GPL 2","\x01""  ""\xd0""""\xed""""\xbf""""\xc9""""\xd6""""\xa4""""\xa3""""\xba""GPL 2"},
  {"  Do not use if CAS is forbidden","\x01""  ""\xc8""""\xe7""""\xb9""""\xfb""""\xbf""""\xbc""""\xca""""\xd4""""\xbd""""\xfb""""\xd6""""\xb9""""\xca""""\xb9""""\xd3""""\xc3"" CAS""\xa3""""\xac""""\xc7""""\xeb""""\xce""""\xf0""""\xca""""\xb9""""\xd3""""\xc3"""},
  {"Screen frozen. Press esc.","\x01""""\xc6""""\xc1""""\xc4""""\xbb""""\xd2""""\xd1""""\xb6""""\xb3""""\xbd""""\xe1""""\xa1""""\xa3""""\xb0""""\xb4"" esc ""\xbc""""\xcc""""\xd0""""\xf8""""\xa1""""\xa3"""},
  {"Python/Xcas","\x01""Python/Xcas"},
  {"Sqrt","\x01""""\xc6""""\xbd""""\xb7""""\xbd""""\xb8""""\xf9"""},
  {"Step/step","\x01""""\xb5""""\xa5""""\xb2""""\xbd""/""\xb2""""\xbd""""\xbd""""\xf8"""},
  {"Save","\x01""""\xb1""""\xa3""""\xb4""""\xe6"""},
  {"Save as","\x01""""\xc1""""\xed""""\xb4""""\xe6""""\xce""""\xaa"""},
  {"Applications","\x01""""\xd3""""\xa6""""\xd3""""\xc3"""},
  {"Run session","\x01""""\xd4""""\xcb""""\xd0""""\xd0""""\xbb""""\xe1""""\xbb""""\xb0"""},
  {"Script editor","\x01""""\xbd""""\xc5""""\xb1""""\xbe""""\xb1""""\xe0""""\xbc""""\xad""""\xc6""""\xf7"""},
  {"Open script","\x01""""\xb4""""\xf2""""\xbf""""\xaa""""\xbd""""\xc5""""\xb1""""\xbe"""},
  {"Clear history","\x01""""\xc7""""\xe5""""\xb3""""\xfd""""\xc0""""\xfa""""\xca""""\xb7""""\xbc""""\xc7""""\xc2""""\xbc"""},
  {"Parameter","\x01""""\xb2""""\xce""""\xca""""\xfd"""},
  {"Config shift-SETUP","\x01""""\xc9""""\xe8""""\xd6""""\xc3""""\xa3""""\xa8""SHIFT-SETUP""\xa3""""\xa9"""},
  {"Index","\x01""""\xcb""""\xf7""""\xd2""""\xfd"""},
  {"New value?","\x01""""\xd0""""\xc2""""\xd6""""\xb5""""\xa3""""\xbf"""},
  {"New value? ","\x01""""\xd0""""\xc2""""\xd6""""\xb5""""\xa3""""\xbf"" "},
  {"Invalid variable name","\x01""""\xb1""""\xe4""""\xc1""""\xbf""""\xc3""""\xfb""""\xce""""\xde""""\xd0""""\xa7"""},
  {"F1 or F6: ok","\x01""F1 ""\xbb""""\xf2"" F6""\xa3""""\xba""""\xc8""""\xb7""""\xb6""""\xa8"""},
  {"Clear Xcas variables?","\x01""""\xc7""""\xe5""""\xb3""""\xfd"" Xcas ""\xb1""""\xe4""""\xc1""""\xbf""""\xa3""""\xbf"""},
  {"Clear MicroPython heap?","\x01""""\xc7""""\xe5""""\xb3""""\xfd"" MicroPython ""\xb6""""\xd1""""\xa3""""\xbf"""},
  {"Clear QuickJS heap?","\x01""""\xc7""""\xe5""""\xb3""""\xfd"" QuickJS ""\xb6""""\xd1""""\xa3""""\xbf"""},
  {"Variable name:","\x01""""\xb1""""\xe4""""\xc1""""\xbf""""\xc3""""\xfb""""\xa3""""\xba"""},
  {"Line number","\x01""""\xd0""""\xd0""""\xba""""\xc5"""},
  {"Columns:","\x01""""\xc1""""\xd0""""\xca""""\xfd""""\xa3""""\xba"""},
  {"Filename:","\x01""""\xce""""\xc4""""\xbc""""\xfe""""\xc3""""\xfb""""\xa3""""\xba"""},
  {"Search:","\x01""""\xb2""""\xe9""""\xd5""""\xd2""""\xa3""""\xba"""},
  {"Replace by:","\x01""""\xcc""""\xe6""""\xbb""""\xbb""""\xce""""\xaa""""\xa3""""\xba"""},
  {"EXIT or empty string: cancel","\x01""EXIT ""\xbb""""\xf2""""\xbf""""\xd5""""\xd7""""\xd6""""\xb7""""\xfb""""\xb4""""\xae""""\xa3""""\xba""""\xc8""""\xa1""""\xcf""""\xfb"""},
  {"EXIT: search only ","\x01""EXIT""\xa3""""\xba""""\xbd""""\xf6""""\xb2""""\xe9""""\xd5""""\xd2"" "},
  {"Save selection in","\x01""""\xbd""""\xab""""\xd1""""\xa1""""\xd4""""\xf1""""\xb1""""\xa3""""\xb4""""\xe6""""\xb5""""\xbd"""},
  {"Really leave","\x01""""\xc8""""\xb7""""\xb6""""\xa8""""\xd2""""\xaa""""\xcd""""\xcb""""\xb3""""\xf6""""\xc2""""\xf0"""},
  {"Auto shutdown","\x01""""\xd7""""\xd4""""\xb6""""\xaf""""\xb9""""\xd8""""\xbb""""\xfa"""},
  {"Exam mode","\x01""""\xbf""""\xbc""""\xca""""\xd4""""\xc4""""\xa3""""\xca""""\xbd"""},
  {"Session saved","\x01""""\xbb""""\xe1""""\xbb""""\xb0""""\xd2""""\xd1""""\xb1""""\xa3""""\xb4""""\xe6"""},
  {"BROAD LEAPS","\x01""""\xbf""""\xec""""\xcb""""\xd9""""\xcc""""\xf8""""\xd7""""\xaa"""},
  {"unable to read","\x01""""\xce""""\xde""""\xb7""""\xa8""""\xb6""""\xc1""""\xc8""""\xa1"""},
  {"Fatal: unable to load ram part","\x01""""\xd6""""\xc2""""\xc3""""\xfc""""\xb4""""\xed""""\xce""""\xf3""""\xa3""""\xba""""\xce""""\xde""""\xb7""""\xa8""""\xbc""""\xd3""""\xd4""""\xd8"" RAM ""\xb2""""\xbf""""\xb7""""\xd6"""},
  {"Fatal: unable to check ram part","\x01""""\xd6""""\xc2""""\xc3""""\xfc""""\xb4""""\xed""""\xce""""\xf3""""\xa3""""\xba""""\xce""""\xde""""\xb7""""\xa8""""\xbc""""\xec""""\xb2""""\xe9"" RAM ""\xb2""""\xbf""""\xb7""""\xd6"""},
  {"Please type any key","\x01""""\xc7""""\xeb""""\xb0""""\xb4""""\xc8""""\xce""""\xd2""""\xe2""""\xbc""""\xfc"""},
  {"To restart KhiCAS, type keys","\x01""""\xd2""""\xaa""""\xd6""""\xd8""""\xc6""""\xf4"" KhiCAS""\xa3""""\xac""""\xc7""""\xeb""""\xb0""""\xb4""""\xd2""""\xd4""""\xcf""""\xc2""""\xb0""""\xb4""""\xbc""""\xfc"""},
  {"To shutdown, type the keys","\x01""""\xd2""""\xaa""""\xb9""""\xd8""""\xbb""""\xfa""""\xa3""""\xac""""\xc7""""\xeb""""\xb0""""\xb4""""\xd2""""\xd4""""\xcf""""\xc2""""\xb0""""\xb4""""\xbc""""\xfc"""},
  {"Back to home menu. To shutdown","\x01""""\xb7""""\xb5""""\xbb""""\xd8""""\xd6""""\xf7""""\xb2""""\xcb""""\xb5""""\xa5""""\xa1""""\xa3""""\xd2""""\xaa""""\xb9""""\xd8""""\xbb""""\xfa""""\xa3""""\xac""""\xc7""""\xeb""""\xd4""""\xd9""""\xb4""""\xce""""\xb0""""\xb4"" SHIFT AC/ON"},
  {"Press MENU key","\x01""""\xc7""""\xeb""""\xb0""""\xb4"" MENU ""\xbc""""\xfc"""},
};

const char *zh_ui_translate(const char *text) {
  if (!text || !zh_ui_enabled || (unsigned char)text[0] == 0x01) return text;
  for (unsigned i=0; i<sizeof(zh_ui_entries)/sizeof(zh_ui_entries[0]); ++i)
    if (!strcmp(text,zh_ui_entries[i].source)) return zh_ui_entries[i].translated;
  return text;
}

void zh_ui_define_status(const char *text) {
  const char *source = zh_ui_translate(text);
  const char *display = source;
  const int gb = khicas_gb_strip(source, &display);
  if (gb) khicas_enable_gb18030();
  DefineStatusMessage((char *)display, 1, 0, 0);
  if (gb) khicas_disable_gb18030();
}

void zh_ui_bdisp_mmprint(int x, int y, const char *text, int mode,
                         unsigned int xlimit, int p6, int p7, int color,
                         int background, int writeflag, int p11) {
  const char *source = zh_ui_translate(text);
  const char *display = source;
  const int gb = khicas_gb_strip(source, &display);
  if (gb) khicas_enable_gb18030();
  Bdisp_MMPrint(x, y, display, mode, xlimit, p6, p7, color, background,
                writeflag, p11);
  if (gb) khicas_disable_gb18030();
}

void zh_ui_printmini(int *x, int *y, const char *text, int mode,
                     unsigned int xlimit, int p6, int p7, int color,
                     int background, int writeflag, int p11) {
  const char *source = zh_ui_translate(text);
  const char *display = source;
  const int gb = khicas_gb_strip(source, &display);
  if (gb) khicas_enable_gb18030();
  PrintMini(x, y, (unsigned char *)display, mode, xlimit, p6, p7, color,
            background, writeflag, p11);
  if (gb) khicas_disable_gb18030();
}
