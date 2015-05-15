#pragma once
#include <windows.h>
#include <fstream>
class OpenFileWindow
{
public:
	OpenFileWindow(void);
	TCHAR * open();
	~OpenFileWindow(void);
};

