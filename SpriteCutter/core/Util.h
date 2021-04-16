#pragma once

#define CREATE_FILE_STRUCT() OPENFILENAME ofn;\
char szFile[260];\
HANDLE hf;\
ZeroMemory(&ofn, sizeof(ofn));\
ofn.lStructSize = sizeof(ofn);\
ofn.hwndOwner = NULL;\
ofn.lpstrFile = szFile;\
ofn.lpstrFile[0] = '\0';\
ofn.nMaxFile = sizeof(szFile);\
ofn.nFilterIndex = 1;\
ofn.lpstrFileTitle = NULL;\
ofn.nMaxFileTitle = 0;\
ofn.lpstrInitialDir = NULL;\
ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;\

inline void CleanPointer(void* ptr) { delete ptr; ptr = nullptr; }