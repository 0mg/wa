#include <windows.h>
#include "wave.h"

void __start__(void) {ExitProcess(WinMain(GetModuleHandle(NULL), 0, NULL, 0));}

class WavePlayer kick;
class WavePlayer snare;
class WavePlayer bell;
class WavePlayer none;

DWORD WINAPI thfun(LPVOID some) {
  static BOOL j = 0, k = 0, f = 0, d = 0;
  do {
    if (GetKeyState('K') < 0) {
      if (k == 0) {
        k = 1;
        kick.play();
      }
    } else k = 0;
    if (GetKeyState('J') < 0) {
      if (j == 0) {
        j = 1;
        snare.play();
      }
    } else j = 0;
    if (GetKeyState('D') < 0) {
      if (d == 0) {
        d = 1;
        kick.play();
      }
    } else d = 0;
    if (GetKeyState('I') < 0) {
      if (f == 0) {
        f = 1;
        bell.play();
      }
    } else f = 0;
  } while (some);
  return 0;
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) {
  switch (msg) {
  case WM_CREATE: {
    SetTimer(hwnd, 1, 3000, NULL);
    return 0;
  }
  case WM_TIMER: {
    none.play();
    return 0;
  }
  case WM_DESTROY: {
    PostQuitMessage(0);
    return 0;
  }
  }
  return DefWindowProc(hwnd, msg, wp, lp);
}

int WINAPI WinMain(HINSTANCE hi, HINSTANCE hp, LPSTR cl, int cs) {
  WNDCLASS wc;
  SecureZeroMemory(&wc, sizeof(WNDCLASS));
  wc.lpfnWndProc = WindowProc;
  wc.hInstance = hi;
  wc.hCursor = LoadCursor(NULL, IDC_ARROW);
  wc.hbrBackground = (HBRUSH)(COLOR_MENU + 1);
  wc.lpszClassName = TEXT("class");
  RegisterClass(&wc);

  HWND hwnd = CreateWindow(wc.lpszClassName, TEXT("play sound [J][K]"),
    WS_OVERLAPPEDWINDOW | WS_VISIBLE,
    CW_USEDEFAULT, CW_USEDEFAULT,
    640, 480,
    NULL, NULL, hi, NULL);
  if (hwnd == NULL) return 0;

  kick.init(MAKEINTRESOURCE(1), TEXT("WAVE"));
  snare.init(MAKEINTRESOURCE(2), TEXT("WAVE"));
  none.init(MAKEINTRESOURCE(3), TEXT("WAVE"));
  bell.init(MAKEINTRESOURCE(4), TEXT("WAVE"));
  //kick.init(TEXT("kick.wav")), snare.init(TEXT("snare.wav"));

  MSG msg;
  while (GetMessage(&msg, NULL, 0, 0) > 0) {
    thfun(NULL);
    TranslateMessage(&msg);
    DispatchMessage(&msg);
  }
  return msg.wParam;
}
