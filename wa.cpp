#include <windows.h>
#include "wave.h"

void __start__(void) {ExitProcess(WinMain(GetModuleHandle(NULL), 0, NULL, 0));}

class WavePlayer kick;
class WavePlayer kick2;
class WavePlayer snare;
class WavePlayer snare2;
class WavePlayer bell;
class WavePlayer none;

inline void k2p(BYTE *keys, BYTE k, BYTE *f, WavePlayer *p) {
  if (keys[k] & 0x80) {
    if (f[k] == 0) {
      f[k] = 1;
      p->play();
    }
  } else f[k] = 0;
}
static BYTE flags[256];
DWORD WINAPI thfun(LPVOID some) {
  BYTE *keys = (BYTE *)some;
  k2p(keys, 'K', flags, &kick);
  k2p(keys, 'J', flags, &snare);
  k2p(keys, 'I', flags, &bell);
  k2p(keys, 'D', flags, &kick2);
  k2p(keys, 'F', flags, &snare2);
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
  /*case WM_KEYDOWN: // flow to follow
    if (lp & 0x40000000) return 0;
  case WM_KEYUP: {
    HDC hdc = GetDC(hwnd);
    SelectObject(hdc, GetStockObject(msg == WM_KEYUP ? BLACK_BRUSH : WHITE_BRUSH));
    PatBlt(hdc, (wp - 'A') * 50, 0, 50, 50, PATCOPY);
    ReleaseDC(hwnd, hdc);
    return 0;
  }*/
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

  if (1) {
    kick.init(MAKEINTRESOURCE(1), TEXT("WAVE"));
    kick2.init(&kick);
    snare.init(MAKEINTRESOURCE(2), TEXT("WAVE"));
    snare2.init(&snare);
    none.init(MAKEINTRESOURCE(3), TEXT("WAVE"));
    bell.init(MAKEINTRESOURCE(4), TEXT("WAVE"));
  } else {
    kick.init(TEXT("kick.wav"), NULL);
    kick2.init(TEXT("kick.wav"), NULL);
    snare.init(TEXT("snare.wav"), NULL);
    snare2.init(TEXT("snare.wav"), NULL);
    none.init(TEXT("none.wav"), NULL);
    bell.init(TEXT("bell.wav"), NULL);
  }
  SecureZeroMemory(&flags, 256);

  MSG msg;
  BYTE keys[256];
  while (GetMessage(&msg, NULL, 0, 0) > 0) {
    GetKeyboardState(keys);
    thfun(keys);
    TranslateMessage(&msg);
    DispatchMessage(&msg);
  }
  return msg.wParam;
}
