#include <windows.h>

#include <psapi.h>

#define MAX_TITLE_LENGTH 256
static const char prefix[] = "C:\\Windows\\";
static const char windowsapps[] =
    "C:\\Program Files\\Common Files\\microsoft shared\\";
static const char gameBarExe[] = "GameBar.exe";

BOOL TestFullscreen(HWND hwnd) {
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
    if (TestFullscreen(hwnd)) {
      DWORD processId;
      GetWindowThreadProcessId(hwnd, &processId);

      HANDLE hProcess =
          OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, processId);
      if (hProcess == NULL) {
        return 1;
      }

      TCHAR executablePath[MAX_PATH];
      if (GetModuleFileNameEx(hProcess, NULL, executablePath, MAX_PATH) == 0) {
        CloseHandle(hProcess);
        return 1;
      }

      CloseHandle(hProcess);
      size_t exe_bufferSize =
          sizeof(executablePath) / sizeof(executablePath[0]);

      char *charPath = (char *)malloc(exe_bufferSize);

      ConvertTCHARToChar(executablePath, charPath, exe_bufferSize);
      int result = strncmp(charPath, prefix, 11);
      if (result == 0) {
        continue;
      }

      result = strncmp(charPath, windowsapps, 47);
      if (result == 0) {
        continue;
      }

      result = strcmp(charPath + strlen(charPath) - 11, gameBarExe);
      if (result == 0) {
        continue;
      }

      // Use charPath as a regular char array
      strcpy_s(buffer, bufferSize, charPath);
      free(charPath);
      return 0;
    }
  }
  return 1;
}
