// Plays the sounds assigned to "Mouse Click Down" / "Mouse Click Up" in the Windows Sound control panel.
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <mmsystem.h>

#define SCHEME L"AppEvents\\Schemes\\Apps\\.Default"

typedef struct { WCHAR path[MAX_PATH]; BYTE *data; } Sound;
static Sound down, up;

static void play(const Sound *s)
{
    if (s->data) PlaySoundW((LPCWSTR)s->data, NULL, SND_ASYNC | SND_MEMORY | SND_NODEFAULT);
}

static LRESULT CALLBACK hook(int n, WPARAM w, LPARAM l)
{
    if (n >= 0) {
        if (w == WM_LBUTTONDOWN || w == WM_RBUTTONDOWN) play(&down);
        else if (w == WM_LBUTTONUP || w == WM_RBUTTONUP) play(&up);
    }
    return CallNextHookEx(NULL, n, w, l);
}

static void load(Sound *s, const WCHAR *evt)
{
    WCHAR key[256], raw[MAX_PATH] = L"", path[MAX_PATH] = L"";
    DWORD size = sizeof raw;
    wsprintfW(key, SCHEME L"\\%s\\.Current", evt);
    if (RegGetValueW(HKEY_CURRENT_USER, key, NULL, RRF_RT_REG_SZ | RRF_RT_REG_EXPAND_SZ | RRF_NOEXPAND, NULL, raw, &size) == ERROR_SUCCESS)
        ExpandEnvironmentStringsW(raw, path, MAX_PATH);
    if (lstrcmpiW(path, s->path) == 0) return;
    lstrcpyW(s->path, path);

    // stop playback before freeing a buffer PlaySound may still be reading
    PlaySoundW(NULL, NULL, 0);
    if (s->data) { HeapFree(GetProcessHeap(), 0, s->data); s->data = NULL; }
    if (!path[0]) return;

    HANDLE f = CreateFileW(path, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
    if (f == INVALID_HANDLE_VALUE) return;
    DWORD len = GetFileSize(f, NULL), got = 0;
    if (len != INVALID_FILE_SIZE && len < 4 * 1024 * 1024) {
        s->data = HeapAlloc(GetProcessHeap(), 0, len);
        if (s->data && (!ReadFile(f, s->data, len, &got, NULL) || got != len)) {
            HeapFree(GetProcessHeap(), 0, s->data);
            s->data = NULL;
        }
    }
    CloseHandle(f);
}

static void refresh(void)
{
    load(&down, L"MouseClickDown");
    load(&up, L"MouseClickUp");
    SetProcessWorkingSetSize(GetCurrentProcess(), (SIZE_T)-1, (SIZE_T)-1);
}

int WINAPI wWinMain(HINSTANCE inst, HINSTANCE prev, PWSTR cmd, int show)
{
    CreateMutexW(NULL, TRUE, L"ClickSound_single_instance");
    if (GetLastError() == ERROR_ALREADY_EXISTS) return 0;

    HKEY scheme;
    HANDLE changed = CreateEventW(NULL, FALSE, FALSE, NULL);
    if (RegOpenKeyExW(HKEY_CURRENT_USER, SCHEME, 0, KEY_NOTIFY, &scheme) != ERROR_SUCCESS) return 1;

    refresh();
    RegNotifyChangeKeyValue(scheme, TRUE, REG_NOTIFY_CHANGE_LAST_SET | REG_NOTIFY_CHANGE_NAME, changed, TRUE);
    SetWindowsHookExW(WH_MOUSE_LL, hook, inst, 0);

    for (;;) {
        DWORD r = MsgWaitForMultipleObjects(1, &changed, FALSE, INFINITE, QS_ALLINPUT);
        if (r == WAIT_OBJECT_0) {
            refresh();
            RegNotifyChangeKeyValue(scheme, TRUE, REG_NOTIFY_CHANGE_LAST_SET | REG_NOTIFY_CHANGE_NAME, changed, TRUE);
        } else {
            MSG m;
            while (PeekMessageW(&m, NULL, 0, 0, PM_REMOVE)) {
                if (m.message == WM_QUIT) return 0;
                DispatchMessageW(&m);
            }
        }
    }
}
