#pragma once

#include "SerialInterface.hpp"

namespace halvoe
{
	class SerialPeriphial : public SerialInterface
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
			bool doHandleData(Deserializer<c_bufferSize>&& in_deserializer, DataCode in_code)
			{
				return false;
			}

			bool doHandleCommand(Deserializer<c_bufferSize>&& in_deserializer, CommandCode in_code)
			{
				switch (in_code)
				{
					case CommandCode::playFile:
						StringSizeType filenameSize = 0;
						auto filenameRaw = in_deserializer.read<StringSizeType>(64, filenameSize);
						String filename(filenameRaw.get(), filenameSize);
						return playFile(filename);
				}

				return false;
			}
	};
}