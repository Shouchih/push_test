use Sentinel Hasp API Lib

all file need put at path \SentinelHaspApiLib

at project properties

-->Linker
-->Input
-->Additional Dependencies


--> add:

SentinelHaspApiLib/libhasp_windows_103339.lib


----------------------------------------------------------------------------
for use of strcpy function  --  ®ø°£ warning 


Project properties->Configuration Properties->C/C++->Preprocessor->Preprocessor Definitions

Click on the elipses (...)
type 

_CRT_SECURE_NO_WARNINGS