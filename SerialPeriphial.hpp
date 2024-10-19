#pragma once

#include "SerialInterface.hpp"

namespace halvoe
{
	static constexpr const size_t c_serializerBufferSize = 1024;
	static constexpr const size_t c_deserializerBufferSize = 1024;

	class SerialPeriphial : public SerialInterface<c_serializerBufferSize, c_deserializerBufferSize>
	{
		public:
			SerialPeriphial() = default;

			bool setup()
			{
				Serial1.setPins(33, 32);
				Serial1.begin(19200);

				return Serial1;
			}
			
		private:
			bool doHandleData(Deserializer<c_deserializerBufferSize>&& in_deserializer, SerialDataCode in_code)
			{
				return false;
			}

			bool doHandleCommand(Deserializer<c_deserializerBufferSize>&& in_deserializer, SerialCommandCode in_code)
			{
				switch (in_code)
				{
					case SerialCommandCode::playFile:
						SerialStringSizeType filenameSize = 0;
						auto filenamePointer = in_deserializer.read<SerialStringSizeType>(64, filenameSize);
						String filename(filenamePointer.get(), filenameSize);
						return playFile(filename);
				}

				return false;
			}
	};
}