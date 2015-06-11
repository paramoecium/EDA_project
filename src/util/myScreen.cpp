#include "util.h"
#include <iostream>

using namespace std;

const char CSI[3] = {27, '['};

void cursorToPrevLine(){
   string str(CSI);
   str += "F";
   cout << str;
}

void cursorClearAfter(){
   string str(CSI);
   str += "0K";
   cout << str;
}
