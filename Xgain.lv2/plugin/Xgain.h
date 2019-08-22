
#pragma once

#ifndef XGAIN_H_
#define XGAIN_H_

#include <lv2.h>

#define PLUGIN_URI "urn:Xputty:Xgain_"
#define PLUGIN_UI_URI "urn:Xputty:Xgain_gui"


typedef enum
{
   EFFECTS_OUTPUT,
   EFFECTS_INPUT,
   BYPASS,
   GAIN, 
} PortIndex;

#endif //XGAIN_H_
