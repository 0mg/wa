#include <windows.h>

class WavePlayer {
private:
  HWAVEOUT wavout;
  WAVEFORMATEX wavfmt;
  WAVEHDR wavhdr;
  BOOL loadWave(LPTSTR filename, MMIOINFO *minfo) {
    // # ready vars to analyze
    MMCKINFO root, head, body;
    SecureZeroMemory(&root, sizeof(MMCKINFO));
    SecureZeroMemory(&head, sizeof(MMCKINFO));
    SecureZeroMemory(&body, sizeof(MMCKINFO));
    root.fccType = mmioStringToFOURCC(TEXT("WAVE"), 0);
    head.ckid = mmioStringToFOURCC(TEXT("fmt "), 0);
    body.ckid = mmioStringToFOURCC(TEXT("data"), 0);
    // # Analyze RIFF file
    HMMIO file = mmioOpen(filename, minfo, MMIO_READ);
    if (file) {
      // analyze
      if (mmioDescend(file, &root, NULL, MMIO_FINDRIFF) == 0) { //WAVE
        if (mmioDescend(file, &head, &root, MMIO_FINDCHUNK) == 0) { //fmt
          mmioRead(file, (HPSTR)&wavfmt, head.cksize);
          mmioAscend(file, &head, 0);
          if (mmioDescend(file, &body, &root, MMIO_FINDCHUNK) == 0) { //data
            wavhdr.dwBufferLength = body.cksize;
            if (minfo) {
              wavhdr.lpData = minfo->pchBuffer + body.dwDataOffset;
            } else {
              wavhdr.lpData = (LPSTR)HeapAlloc(GetProcessHeap(), 0, body.cksize);
              mmioRead(file, (HPSTR)wavhdr.lpData, body.cksize);
            }
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
  void init(LPTSTR filenameOrId, LPTSTR restype) {
    // fn("test.wav");
    // fn(MAKEINTRESOURCE(ID_TEST_WAV), "WAVE");
    // # init instance
    SecureZeroMemory(&wavfmt, sizeof(WAVEFORMATEX));
    SecureZeroMemory(&wavhdr, sizeof(WAVEHDR));
    // # load sound (file or resource)
    LPTSTR filename = filenameOrId;
    MMIOINFO minfo, *pinfo = NULL;
    if (restype) {
      filename = NULL;
      pinfo = &minfo;
      SecureZeroMemory(&minfo, sizeof(MMIOINFO));
      HRSRC fres = FindResource(NULL, filenameOrId, restype);
      minfo.pchBuffer = (HPSTR)LockResource(LoadResource(NULL, fres));
      minfo.cchBuffer = SizeofResource(NULL, fres);
      minfo.fccIOProc = FOURCC_MEM;
    }
    // # ready to play sound
    if (loadWave(filename, pinfo)) {
      waveOutOpen(&wavout, WAVE_MAPPER, &wavfmt, 0, 0, CALLBACK_NULL);
      waveOutPrepareHeader(wavout, &wavhdr, sizeof(WAVEHDR));
    }
  }
  void init(WavePlayer *wp) { // copy from other instance
    wavfmt = wp->wavfmt;
    wavhdr = wp->wavhdr;
    waveOutOpen(&wavout, WAVE_MAPPER, &wavfmt, 0, 0, CALLBACK_NULL);
    waveOutPrepareHeader(wavout, &wavhdr, sizeof(WAVEHDR));
  }
  void play() {
    waveOutReset(wavout);
    waveOutWrite(wavout, &wavhdr, sizeof(WAVEHDR));
  }
  /*void uninit() {
    waveOutReset(wavout);
    waveOutUnprepareHeader(wavout, &wavhdr, sizeof(WAVEHDR));
    waveOutClose(wavout);
    HeapFree(GetProcessHeap(), 0, wavhdr.lpData);
  }*/
};
