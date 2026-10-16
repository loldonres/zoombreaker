#pragma once
#include <windows.h>
#include <string>
#include <vector>

 
 
std::vector<std::wstring> GetZoomParticipants();

 
HWND FindZoomParticipantsWindow();

 
 
void ScrollParticipantsDown(HWND participantsWnd);

 
void ScrollParticipantsUp(HWND participantsWnd);

 
 
std::vector<std::wstring> CollectAllZoomParticipants();