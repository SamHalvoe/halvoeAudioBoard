#include "halvoeAudioPipeline.hpp"

namespace halvoe
{
  AudioPipeline::AudioPipeline() :
    m_audioInfo(44100, 2, 16), // sampling rate, # channels, bit depth
    m_i2sOutStream(m_boardDriver.getBoard()),
    m_audioCopier(4096),
    m_silenceStream(m_silenceGenerator)
  {}

  bool AudioPipeline::setup()
  {
    // setup audio board (dríver)
    m_boardDriver.setup();

    // setup volume stream
    auto volumeConfig = m_volumeStream.defaultConfig();
    volumeConfig.copyFrom(m_audioInfo);
    volumeConfig.allow_boost = true;
    if (not m_volumeStream.begin(volumeConfig)) { return false; }
    m_volumeStream.setVolume(0.0);

    // setup i2s
    auto i2sConfig = m_i2sOutStream.defaultConfig();
    i2sConfig.copyFrom(m_audioInfo);
    i2sConfig.buffer_size = 2048;
    if (not m_i2sOutStream.begin(i2sConfig)) { return false; }
    m_i2sOutStream.setVolume(1.0);

    // setup audio pipeline
    m_audioPipeline.add(m_volumeStream);
    m_audioPipeline.setOutput(m_i2sOutStream);
    if (not m_audioPipeline.isOK()) { return false; }

    beginSilence();

    return true;
  }

  void AudioPipeline::begin(Stream& io_audioSource)
  {
    m_audioCopier.begin(m_audioPipeline, io_audioSource);
  }

  void AudioPipeline::end()
  {
    m_audioCopier.end();
  }

  void AudioPipeline::beginSilence()
  {
    m_silenceGenerator.begin();
    m_silenceStream.begin();
    m_audioCopier.begin(m_audioPipeline, m_silenceStream);
  }

  void AudioPipeline::endSilence()
  {
    m_audioCopier.end();
    m_silenceStream.end();
    m_silenceGenerator.end();
  }

  void AudioPipeline::run()
  {
    size_t copiedBytesCount = m_audioCopier.copy();

    if (m_isPlaybackActive)
    {
      if (m_volume < 1.0)
      {
        m_volumeStream.setVolume(m_volume);
        m_volume = m_volume + m_volumeStep;
      }

      if (copiedBytesCount == 0) // 0 bytes copied mean end of source
      {
        endPlayback();
      }
    }
  }

  bool AudioPipeline::playFile(const String& in_filename)
  {
    if (m_isPlaybackActive) { return false; }

    String path("/");
    path.concat(in_filename);
    m_soundFile = SD.open(path);

    if (not m_soundFile)
    {
      Serial.print("Could not open file: \"");
      Serial.print(path);
      Serial.println("\"");

      return false;
    }

    endSilence();
    begin(m_soundFile);
    m_isPlaybackActive = true;
    return true;
  }

  bool AudioPipeline::endPlayback()
  {
    if (not m_isPlaybackActive) { return false; }

    m_volumeStream.setVolume(0.0);
    m_volume = 0.0;
    end();
    beginSilence();
    m_isPlaybackActive = false;
    return true;
  }
}
