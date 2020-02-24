#include <windows.h>

class WavePlayer {
private:
  HWAVEOUT waveout;
  WAVEFORMATEX wfx;
  WAVEHDR whdr;
  BOOL loadWave(LPTSTR filename, MMIOINFO *minfo) {
    HMMIO file;
    if (filename) {
      file = mmioOpen(filename, NULL, MMIO_READ);
    } else {
      file = mmioOpen(NULL, minfo, MMIO_READ);
    }
    // init vars
    MMCKINFO root, head, body;
    SecureZeroMemory(&root, sizeof(MMCKINFO));
    SecureZeroMemory(&head, sizeof(MMCKINFO));
    SecureZeroMemory(&body, sizeof(MMCKINFO));
    root.fccType = mmioStringToFOURCC(TEXT("WAVE"), 0);
    head.ckid = mmioStringToFOURCC(TEXT("fmt "), 0);
    body.ckid = mmioStringToFOURCC(TEXT("data"), 0);
    // # Analyze RIFF file
    if (file) {
      // analyze
      if (mmioDescend(file, &root, NULL, MMIO_FINDRIFF) == 0) { //WAVE
        if (mmioDescend(file, &head, NULL, MMIO_FINDCHUNK) == 0) { //fmt
          mmioRead(file, (HPSTR)&wfx, head.cksize);
          mmioAscend(file, &head, 0);
          if (mmioDescend(file, &body, NULL, MMIO_FINDCHUNK) == 0) { //data
            whdr.dwBufferLength = body.cksize;
            whdr.lpData = (LPSTR)HeapAlloc(GetProcessHeap(), 0, body.cksize);
            mmioRead(file, (HPSTR)whdr.lpData, body.cksize);
            //mmioAscend(file, &body, 0);
          }
        }
        //mmioAscend(file, &root, 0);
      }
      mmioClose(file, 0);
    }
    return file && head.cksize && body.cksize;
  }
public:
  void init(LPTSTR filenameOrId, LPTSTR restype = NULL) {
    // fn("test.wav");
    // fn(MAKEINTRESOURCE(ID_TEST_WAV), "WAVE");
    SecureZeroMemory(&wfx, sizeof(WAVEFORMATEX));
    SecureZeroMemory(&whdr, sizeof(WAVEHDR));
    BOOL result = FALSE;
    if (restype) {
      HINSTANCE hi = GetModuleHandle(NULL);
      HRSRC fres = FindResource(hi, filenameOrId, restype);
      MMIOINFO minfo;
      SecureZeroMemory(&minfo, sizeof(MMIOINFO));
      minfo.pchBuffer = (HPSTR)LockResource(LoadResource(hi, fres));
      minfo.cchBuffer = SizeofResource(hi, fres);
      minfo.fccIOProc = FOURCC_MEM;
      result = loadWave(NULL, &minfo);
    } else {
      result = loadWave(filenameOrId, NULL);
    }
    if (result) {
      waveOutOpen(&waveout, WAVE_MAPPER, &wfx, 0, 0, CALLBACK_NULL);
      waveOutPrepareHeader(waveout, &whdr, sizeof(WAVEHDR));
    }
  }
  void play() {
    waveOutReset(waveout);
    waveOutWrite(waveout, &whdr, sizeof(WAVEHDR));
  }
};
