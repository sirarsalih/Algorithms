#include "OpenFileWindow.h"


OpenFileWindow::OpenFileWindow(void)
{
	
}

TCHAR * OpenFileWindow::open()
{
	OPENFILENAME ofn;
	TCHAR szFile[260] = {0};
	HWND hwnd = NULL; 
	HANDLE hf = NULL;
 
	ZeroMemory(&ofn, sizeof(ofn));
	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = hwnd;
	ofn.lpstrFile = szFile;
	ofn.lpstrFile[0] = '\0';
	ofn.nMaxFile = sizeof(szFile);
	ofn.nFilterIndex = 1;
	ofn.lpstrFileTitle = NULL;
	ofn.nMaxFileTitle = 0;
	ofn.lpstrInitialDir = NULL;
	ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

	TCHAR * filePath;

	if (GetOpenFileName(&ofn)==TRUE) 
	{
		hf = CreateFile(ofn.lpstrFile, GENERIC_READ, 0, (LPSECURITY_ATTRIBUTES) NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, (HANDLE) NULL);
		
		filePath = ofn.lpstrFile;		

		CloseHandle(hf);

		return filePath;
	}
	else
		return NULL;
}



OpenFileWindow::~OpenFileWindow(void)
{
}
