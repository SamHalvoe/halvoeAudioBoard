#pragma once

#include "halvoeAudioPipeline.hpp"
#include "halvoeSDHandler.hpp"

namespace halvoe
{
  //const char* soundFilename = "ddd4416.wav";
  File soundFile;
  bool isPlaybackActive = false;

  bool playFile(const String& in_filename)
  {
    if (isPlaybackActive) { return false; }

    String path("/");
    path.concat(in_filename);
    soundFile = SD.open(path);

    if (not soundFile)
    {
      Serial.print("Could not open file: \"");
      Serial.print(path);
      Serial.println("\"");

      return false;
    }

    audioPipeline::endSilence();
    audioPipeline::begin(soundFile);
    isPlaybackActive = true;

    return true;
  }

  bool endPlayback()
  {
    if (not isPlaybackActive) { return false; }

    audioPipeline::volumeStream.setVolume(0.0);
    audioPipeline::volume = 0.0;
    audioPipeline::end();
    audioPipeline::beginSilence();
    isPlaybackActive = false;
    return true;
  }
}
