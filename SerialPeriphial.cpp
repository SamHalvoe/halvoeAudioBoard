#include "SerialPeriphial.hpp"
#include "halvoeAudioPipeline.hpp"

namespace halvoe
{
  SerialPeriphial::SerialPeriphial(HardwareSerial& in_serial, AudioPipeline& io_audioPipeline) :
		SerialInterface<c_serializerBufferSize, c_deserializerBufferSize>(in_serial),
		m_audioPipeline(io_audioPipeline)
  {}

	bool SerialPeriphial::setup()
	{
		m_serial.setPins(33, 32);
		m_serial.begin(19200);

		return m_serial;
	}

	bool SerialPeriphial::handlePlayFile(Deserializer<c_deserializerBufferSize>& io_deserializer)
	{
		SerialStringSizeType filenameSize = 0;
		auto filenamePointer = io_deserializer.read<SerialStringSizeType>(64, filenameSize);
		String filename(filenamePointer.get(), filenameSize);
		return m_audioPipeline.playFile(filename);
	}

	bool SerialPeriphial::handleEndPlayback()
	{
		return m_audioPipeline.endPlayback();
	}

	bool SerialPeriphial::doHandleData(Deserializer<c_deserializerBufferSize>& io_deserializer, SerialDataCode in_code)
	{
		return false;
	}

	bool SerialPeriphial::doHandleCommand(Deserializer<c_deserializerBufferSize>& io_deserializer, SerialCommandCode in_code)
	{
		switch (in_code)
		{
			case SerialCommandCode::playFile:		 return handlePlayFile(io_deserializer);
			case SerialCommandCode::endPlayback: return handleEndPlayback();
		}

		return false;
	}
}
