#pragma once
#include <string>
#include <vector>
#include <windows.h>

struct NeonCamera {
    std::string  id;
    std::string  name;
    int          slot = -1;
    int          width = 0;
    int          height = 0;
    int          fps = 0;
    std::string  loop;
    std::string  audio_device;
    int          volume = 100;
    bool         muted = false;
    bool         autoplay = true;
    bool         vcam_active = false;
    std::string  vcam_error;
    std::string  playState;       
    std::string  currentItem;
    double       positionSec = 0.0;
    double       durationSec = 0.0;
    std::string  lastError;
    std::vector<std::string> playlist;
};

struct NeonAudioDevice {
    std::string id;
    std::string name;
    bool        isDefault = false;
};

namespace NeonApi {

constexpr const wchar_t* kHost = L"127.0.0.1";
constexpr int kPort = 8686;
constexpr const wchar_t* kBasePath = L"/api/v1";

 
std::wstring GetDfvcamExePath();

 
bool StartService();
void StopService();

 
bool IsAlive();

 
std::vector<NeonCamera> ListCameras();
bool GetCamera(const std::string& id, NeonCamera& out);
std::string CreateCamera(const std::wstring& name,
                         int width, int height, int fps);
bool DeleteCamera(const std::string& id);

 
bool SetPlaylist(const std::string& id,
                 const std::vector<std::wstring>& files);
bool AddToPlaylist(const std::string& id, const std::wstring& file);

 
std::vector<NeonAudioDevice> ListAudioDevices();
bool SetAudioDevice(const std::string& id, const std::string& deviceId);

 
bool Play(const std::string& id);
bool Pause(const std::string& id);
bool Stop(const std::string& id);

 
std::string JsonEscape(const std::string& s);
std::string ExtractJsonString(const std::string& json,
                              const std::string& key,
                              size_t from = 0);
std::vector<std::string> ParsePlaylist(const std::string& json);

}  