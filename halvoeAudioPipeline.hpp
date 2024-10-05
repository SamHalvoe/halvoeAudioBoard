#pragma once

#include <AudioTools.h>
#include <AudioLibs/I2SCodecStream.h>

#include "halvoeAudioDriver.hpp"

namespace halvoe::audioPipeline
{
  AudioInfo audioInfo(44100, 2, 16); // sampling rate, # channels, bit depth
  I2SCodecStream i2sOutStream(halvoe::audioDriver::audioBoard);
  VolumeStream volumeStream;
  Pipeline audioPipeline;
  StreamCopy audioCopier(4096);
  
  float volumeStep = 0.05;
  float volume = volumeStep;

  bool setup()
  {
    // setup volume stream
    auto volumeConfig = volumeStream.defaultConfig();
    volumeConfig.copyFrom(audioInfo);
    volumeConfig.allow_boost = true;
    if (not volumeStream.begin(volumeConfig)) { return false; }
    volumeStream.setVolume(0.0);

    // setup i2s
    auto i2sConfig = i2sOutStream.defaultConfig();
    i2sConfig.copyFrom(audioInfo);
    i2sConfig.buffer_size = 2048;
    if (not i2sOutStream.begin(i2sConfig)) { return false; }
    i2sOutStream.setVolume(1.0);

    // setup audio pipeline
    audioPipeline.add(volumeStream);
    audioPipeline.setOutput(i2sOutStream);
    if (not audioPipeline.isOK()) { return false; }

    return true;
  }

  void begin(Stream& io_audioSource)
  {
    audioCopier.begin(audioPipeline, io_audioSource);
  }

  void end()
  {
    audioCopier.end();
  }
}
