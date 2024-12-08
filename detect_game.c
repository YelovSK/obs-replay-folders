#include <windows.h>
#include <psapi.h>

static const char IGNORE_WINDOWS[] = "C:\\Windows\\";
static const char IGNORE_MICROSOFT_SHARED[] = "C:\\Program Files\\Common Files\\microsoft shared\\";

BOOL StartsWith(const char *str, const char *prefix) {
    if (!str || !prefix) {
        return FALSE;
    }

    size_t prefix_len = strlen(prefix);
    size_t str_len = strlen(str);
    if (prefix_len > str_len) {
        return FALSE;
    }

    return strncmp(str, prefix, prefix_len) == 0;
}

BOOL IsFullscreen(HWND hwnd) {
  WINDOWPLACEMENT wp;
  wp.length = sizeof(WINDOWPLACEMENT);

  if (!GetWindowPlacement(hwnd, &wp)) {
    return FALSE;
  }
  if (!IsWindowVisible(hwnd)) {
    return FALSE;
  }
  if (IsZoomed(hwnd)) {
    return TRUE;
  }

  RECT rcDesktop;
  GetClientRect(GetDesktopWindow(), &rcDesktop);
  return (wp.rcNormalPosition.left <= rcDesktop.left &&
          wp.rcNormalPosition.top <= rcDesktop.top &&
          wp.rcNormalPosition.right >= rcDesktop.right &&
          wp.rcNormalPosition.bottom >= rcDesktop.bottom);
}

void ConvertTCHARToChar(const TCHAR *source, char *dest, size_t destSize) {
#ifdef _UNICODE
  wcstombs_s(NULL, dest, destSize, source, _TRUNCATE);
#else
  strncpy_s(dest, destSize, source, _TRUNCATE);
#endif
}

__declspec(dllexport) int get_running_fullscreen_game_path(char *buffer,
                                                           int bufferSize) {
  HWND hwnd = NULL;
  while ((hwnd = FindWindowEx(NULL, hwnd, NULL, NULL)) != NULL) {
    if (!IsFullscreen(hwnd)) {
      continue;
    }

    DWORD processId;
    GetWindowThreadProcessId(hwnd, &processId);

    HANDLE hProcess = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, processId);
    if (hProcess == NULL) {
      return 1;
    }

    TCHAR executablePath[MAX_PATH];
    if (GetModuleFileNameEx(hProcess, NULL, executablePath, MAX_PATH) == 0) {
      CloseHandle(hProcess);
      return 1;
    }

    CloseHandle(hProcess);
    size_t exe_bufferSize = sizeof(executablePath) / sizeof(executablePath[0]);

    char* charPath = (char*)malloc(exe_bufferSize);

    ConvertTCHARToChar(executablePath, charPath, exe_bufferSize);

    if (StartsWith(charPath, IGNORE_WINDOWS)) continue;
    if (StartsWith(charPath, IGNORE_MICROSOFT_SHARED)) continue;

    // Use charPath as a regular char array
    strcpy_s(buffer, bufferSize, charPath);
    free(charPath);
    return 0;
  }

  return 1;
}
