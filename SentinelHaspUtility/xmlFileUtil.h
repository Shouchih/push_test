#pragma once

#include "tinyxml2\tinyxml2.h"


//----------------------------------------------------------------------------------------------------------------------------------
void saveXMLConfigParam(const char * filePath,struct ConfigParam param);
bool readXMLConfigParam(const char * filePath,struct ConfigParam &param);
