#pragma once

#include <SD.h>
#include <AudioTools.h>
#include <AudioTools/AudioLibs/I2SCodecStream.h>
#include "halvoeAudioDriver.hpp"

namespace halvoe
{
  class AudioPipeline
  {
    private:
      AudioBoardDriver m_boardDriver;
      AudioInfo m_audioInfo;
      I2SCodecStream m_i2sOutStream;
      VolumeStream m_volumeStream;
      Pipeline m_audioPipeline;
      StreamCopy m_audioCopier;
      SilenceGenerator<uint8_t> m_silenceGenerator;
      GeneratedSoundStream<uint8_t> m_silenceStream;
      
      float m_volumeStep = 0.05;
      float m_volume = m_volumeStep;
      bool m_isPlaybackActive = false;

      File m_soundFile;

    private:
      void begin(Stream& io_audioSource);
      void end();
      void beginSilence();
      void endSilence();

    public:
      AudioPipeline();
      bool setup();
      void run();

      bool playFile(const String& in_filename);
      bool endPlayback();
  };
}
