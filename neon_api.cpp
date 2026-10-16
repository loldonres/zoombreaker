#include "neon_api.h"
#include "http_client.h"
#include "utf8_utils.h"
#include <shlobj.h>
#include <sstream>
#include <cstdio>
#include <cstdlib>

namespace NeonApi {

std::wstring GetDfvcamExePath() {
    wchar_t localAppData[MAX_PATH] = {0};
    if (FAILED(SHGetFolderPathW(nullptr, CSIDL_LOCAL_APPDATA | CSIDL_FLAG_CREATE,
                                nullptr, SHGFP_TYPE_CURRENT, localAppData)))
        return {};
    return std::wstring(localAppData) +
           L"\\ZoomBreaker\\Bundled\\Neon\\binaries\\dfvcam.exe";
}

bool StartService() {
    std::wstring exe = GetDfvcamExePath();
    if (GetFileAttributesW(exe.c_str()) == INVALID_FILE_ATTRIBUTES)
        return false;

    std::wstring cwd = exe;
    size_t slash = cwd.find_last_of(L"\\/");
    if (slash != std::wstring::npos) cwd = cwd.substr(0, slash);

    std::wstring cmdLine = L"\"" + exe + L"\" serve";

    STARTUPINFOW si{};
    si.cb = sizeof(si);
    si.dwFlags = STARTF_USESHOWWINDOW;
    si.wShowWindow = SW_HIDE;

    PROCESS_INFORMATION pi{};
    std::vector<wchar_t> cmdBuf(cmdLine.begin(), cmdLine.end());
    cmdBuf.push_back(0);

    BOOL ok = CreateProcessW(nullptr, cmdBuf.data(),
        nullptr, nullptr, FALSE, CREATE_NO_WINDOW,
        nullptr, cwd.c_str(), &si, &pi);
    if (!ok) return false;

    CloseHandle(pi.hThread);
    CloseHandle(pi.hProcess);
    return true;
}

void StopService() {
    HttpPost(kHost, kPort, L"/api/v1/shutdown", "");
}

bool IsAlive() {
    HttpResponse r = HttpGet(kHost, kPort, L"/api/v1/status");
    if (!r.ok || r.status != 200) return false;
    return r.body.find("\"ok\":true") != std::string::npos;
}

std::string JsonEscape(const std::string& s) {
    std::string out;
    out.reserve(s.size() + 8);
    for (char c : s) {
        switch (c) {
        case '\\': out += "\\\\"; break;
        case '"':  out += "\\\""; break;
        case '\n': out += "\\n";  break;
        case '\r': out += "\\r";  break;
        case '\t': out += "\\t";  break;
        default:   out += c;      break;
        }
    }
    return out;
}

 
std::string ExtractJsonString(const std::string& json,
                              const std::string& key,
                              size_t from) {
    std::string pattern = "\"" + key + "\":";
    size_t pos = json.find(pattern, from);
    if (pos == std::string::npos) return {};
    size_t start = pos + pattern.size();

    while (start < json.size() &&
           (json[start] == ' ' || json[start] == '\t')) ++start;

    if (start >= json.size() || json[start] != '"') return {};
    ++start;

    std::string value;
    size_t end = start;
    while (end < json.size()) {
        char c = json[end];
        if (c == '\\' && end + 1 < json.size()) {
            char esc = json[end + 1];
            switch (esc) {
            case '"':  value += '"';  break;
            case '\\': value += '\\'; break;
            case 'n':  value += '\n'; break;
            case 'r':  value += '\r'; break;
            case 't':  value += '\t'; break;
            default:   value += esc;  break;
            }
            end += 2;
            continue;
        }
        if (c == '"') break;
        value += c;
        ++end;
    }
    return value;
}

 
static double ExtractJsonNumber(const std::string& json,
                                const std::string& key) {
    std::string pattern = "\"" + key + "\":";
    size_t pos = json.find(pattern);
    if (pos == std::string::npos) return 0.0;
    pos += pattern.size();
    while (pos < json.size() &&
           (json[pos] == ' ' || json[pos] == '\t')) ++pos;
    return std::atof(json.c_str() + pos);
}

static bool ExtractJsonBool(const std::string& json,
                            const std::string& key) {
    std::string pattern = "\"" + key + "\":";
    size_t pos = json.find(pattern);
    if (pos == std::string::npos) return false;
    pos += pattern.size();
    while (pos < json.size() &&
           (json[pos] == ' ' || json[pos] == '\t')) ++pos;
    return json.compare(pos, 4, "true") == 0;
}

 
std::vector<std::string> ParsePlaylist(const std::string& json) {
    std::vector<std::string> out;

    size_t p = json.find("\"playlist\":");
    if (p == std::string::npos) return out;
    size_t arrStart = json.find('[', p);
    if (arrStart == std::string::npos) return out;
    size_t arrEnd = json.find(']', arrStart);
    if (arrEnd == std::string::npos) return out;

    size_t i = arrStart + 1;
    while (i < arrEnd) {
        while (i < arrEnd && (json[i] == ' ' || json[i] == ',')) ++i;
        if (i >= arrEnd) break;
        if (json[i] != '"') { ++i; continue; }

        ++i;
        std::string item;
        while (i < arrEnd) {
            char c = json[i];
            if (c == '\\' && i + 1 < arrEnd) {
                char esc = json[i + 1];
                switch (esc) {
                case '"':  item += '"';  break;
                case '\\': item += '\\'; break;
                case 'n':  item += '\n'; break;
                case 'r':  item += '\r'; break;
                case 't':  item += '\t'; break;
                default:   item += esc;  break;
                }
                i += 2;
                continue;
            }
            if (c == '"') { ++i; break; }
            item += c;
            ++i;
        }
        out.push_back(item);
    }
    return out;
}

static NeonCamera ParseCamera(const std::string& j) {
    NeonCamera c;
    c.id           = ExtractJsonString(j, "id");
    c.name         = ExtractJsonString(j, "name");
    c.loop         = ExtractJsonString(j, "loop");
    c.audio_device = ExtractJsonString(j, "audio_device");
    c.vcam_error   = ExtractJsonString(j, "vcam_error");
    c.playState    = ExtractJsonString(j, "state");
    c.currentItem  = ExtractJsonString(j, "current_item");
    c.lastError    = ExtractJsonString(j, "last_error");
    c.playlist     = ParsePlaylist(j);

    c.slot         = (int)ExtractJsonNumber(j, "slot");
    c.width        = (int)ExtractJsonNumber(j, "width");
    c.height       = (int)ExtractJsonNumber(j, "height");
    c.fps          = (int)ExtractJsonNumber(j, "fps");
    c.volume       = (int)ExtractJsonNumber(j, "volume");
    c.muted        = ExtractJsonBool(j, "muted");
    c.autoplay     = ExtractJsonBool(j, "autoplay");
    c.vcam_active  = ExtractJsonBool(j, "vcam_active");
    c.positionSec  = ExtractJsonNumber(j, "position_sec");
    c.durationSec  = ExtractJsonNumber(j, "duration_sec");
    return c;
}

 
static bool FindObjectBounds(const std::string& s, size_t objStart,
                             size_t& objEnd) {
    if (objStart >= s.size() || s[objStart] != '{') return false;
    int depth = 0;
    bool inStr = false;
    for (size_t i = objStart; i < s.size(); ++i) {
        char ch = s[i];
        if (ch == '"' && (i == 0 || s[i-1] != '\\')) inStr = !inStr;
        if (inStr) continue;
        if (ch == '{') depth++;
        else if (ch == '}') {
            depth--;
            if (depth == 0) { objEnd = i; return true; }
        }
    }
    return false;
}

std::vector<NeonCamera> ListCameras() {
    std::vector<NeonCamera> out;
    HttpResponse r = HttpGet(kHost, kPort, L"/api/v1/cameras");
    if (!r.ok || r.status != 200) return out;

    size_t pos = 0;
    while (pos < r.body.size()) {
        size_t objStart = r.body.find('{', pos);
        if (objStart == std::string::npos) break;

        size_t objEnd = 0;
        if (!FindObjectBounds(r.body, objStart, objEnd)) break;

        std::string obj = r.body.substr(objStart, objEnd - objStart + 1);
         
        if (obj.find("\"id\":") != std::string::npos)
            out.push_back(ParseCamera(obj));

        pos = objEnd + 1;
    }
    return out;
}

bool GetCamera(const std::string& id, NeonCamera& out) {
    std::wstring path = std::wstring(kBasePath) + L"/cameras/" +
                        Utf8ToWString(id);
    HttpResponse r = HttpGet(kHost, kPort, path);
    if (!r.ok || r.status != 200) return false;
    out = ParseCamera(r.body);
    return true;
}

std::string CreateCamera(const std::wstring& name,
                         int width, int height, int fps) {
    char buf[512];
    snprintf(buf, sizeof(buf),
        "{\"name\":\"%s\",\"width\":%d,\"height\":%d,\"fps\":%d,"
        "\"autoplay\":true,\"loop\":\"all\"}",
        JsonEscape(WStringToUtf8(name)).c_str(),
        width, height, fps);

    HttpResponse r = HttpPost(kHost, kPort, L"/api/v1/cameras", buf);
    if (!r.ok || r.status != 201) return {};
    return ExtractJsonString(r.body, "id");
}

bool DeleteCamera(const std::string& id) {
    std::wstring path = std::wstring(kBasePath) + L"/cameras/" +
                        Utf8ToWString(id);
    HttpResponse r = HttpDelete(kHost, kPort, path);
    return r.ok && r.status == 200;
}

bool SetPlaylist(const std::string& id,
                 const std::vector<std::wstring>& files) {
    std::string body = "{\"items\":[";
    for (size_t i = 0; i < files.size(); ++i) {
        if (i) body += ",";
        body += "\"" + JsonEscape(WStringToUtf8(files[i])) + "\"";
    }
    body += "]}";

    std::wstring path = std::wstring(kBasePath) + L"/cameras/" +
                        Utf8ToWString(id) + L"/playlist";
    HttpResponse r = HttpPut(kHost, kPort, path, body);
    return r.ok && r.status == 200;
}

bool AddToPlaylist(const std::string& id, const std::wstring& file) {
    NeonCamera cam;
    if (!GetCamera(id, cam)) return false;

    std::vector<std::wstring> files;
    for (const auto& s : cam.playlist)
        files.push_back(Utf8ToWString(s));
    files.push_back(file);

    return SetPlaylist(id, files);
}

std::vector<NeonAudioDevice> ListAudioDevices() {
    std::vector<NeonAudioDevice> out;
    HttpResponse r = HttpGet(kHost, kPort, L"/api/v1/audio-devices");
    if (!r.ok || r.status != 200) return out;

    size_t pos = 0;
    while (pos < r.body.size()) {
        size_t objStart = r.body.find('{', pos);
        if (objStart == std::string::npos) break;

        size_t objEnd = 0;
        if (!FindObjectBounds(r.body, objStart, objEnd)) break;

        std::string obj = r.body.substr(objStart, objEnd - objStart + 1);

        NeonAudioDevice d;
        d.id   = ExtractJsonString(obj, "id");
        d.name = ExtractJsonString(obj, "name");
        d.isDefault = obj.find("\"default\":true") != std::string::npos;

         
        if (d.name.empty()) d.name = ExtractJsonString(obj, "friendlyName");
        if (d.name.empty()) d.name = ExtractJsonString(obj, "deviceName");
         
        if (d.name.empty()) d.name = d.id;

        out.push_back(d);
        pos = objEnd + 1;
    }
    return out;
}

bool SetAudioDevice(const std::string& id, const std::string& deviceId) {
    std::string body = "{\"id\":\"" + JsonEscape(deviceId) + "\"}";
    std::wstring path = std::wstring(kBasePath) + L"/cameras/" +
                        Utf8ToWString(id) + L"/audio-device";
    HttpResponse r = HttpPut(kHost, kPort, path, body);
    return r.ok && r.status == 200;
}

bool Play(const std::string& id) {
    std::wstring path = std::wstring(kBasePath) + L"/cameras/" +
                        Utf8ToWString(id) + L"/play";
    HttpResponse r = HttpPost(kHost, kPort, path, "");
    return r.ok && r.status == 200;
}

bool Pause(const std::string& id) {
    std::wstring path = std::wstring(kBasePath) + L"/cameras/" +
                        Utf8ToWString(id) + L"/pause";
    HttpResponse r = HttpPost(kHost, kPort, path, "");
    return r.ok && r.status == 200;
}

bool Stop(const std::string& id) {
    std::wstring path = std::wstring(kBasePath) + L"/cameras/" +
                        Utf8ToWString(id) + L"/stop";
    HttpResponse r = HttpPost(kHost, kPort, path, "");
    return r.ok && r.status == 200;
}

}  
