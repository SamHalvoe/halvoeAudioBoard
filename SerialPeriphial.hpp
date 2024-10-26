#pragma once

#include <HardwareSerial.h>
#include <SerialInterface.hpp>
#include "halvoeAudioPipeline.hpp"

namespace halvoe
{
	constexpr const size_t c_serializerBufferSize = 1024;
	constexpr const size_t c_deserializerBufferSize = 1024;

	class SerialPeriphial : public SerialInterface<c_serializerBufferSize, c_deserializerBufferSize>
	{
		private:
			AudioPipeline& m_audioPipeline;

		public:
			SerialPeriphial(HardwareSerial& in_serial, AudioPipeline& io_audioPipeline);
			bool setup();
			
		private:
			bool handlePlayFile(Deserializer<c_deserializerBufferSize>& io_deserializer);
			bool handleEndPlayback();
			bool doHandleData(Deserializer<c_deserializerBufferSize>& io_deserializer, SerialDataCode in_code);
			bool doHandleCommand(Deserializer<c_deserializerBufferSize>& io_deserializer, SerialCommandCode in_code);
	};
}