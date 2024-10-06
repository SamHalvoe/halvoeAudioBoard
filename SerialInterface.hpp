#pragma once

#include <BasicSerializer.hpp>

namespace halvoe
{
  class SerialInterface
  {
    public:
      // This is the type, which holds the size in bytes of the serial message.
      // "The type of the size, which is transfered over the 'Serial' wire."
      using MessageSizeType = uint32_t;
      // This is the type, which holds the size in bytes of the strings, contained in the serial message.
      // "The type of the size, which is transfered over the 'Serial' wire."
      using StringSizeType = uint16_t;

      enum class MessageType : uint8_t
      {
        invalid = 0,
        data,
        command
      };

      enum class DataCode : uint16_t
      {
        invalid = 0,
        fileList
      };

      enum class CommandCode : uint16_t
      {
        invalid = 0,
        playFile
      };

      static const size_t c_bufferSize = 1024;

    private:
      std::array<uint8_t, c_bufferSize> m_serializerBuffer;
      std::array<uint8_t, c_bufferSize> m_deserializerBuffer;

    private:
      virtual bool doHandleData(Deserializer<c_bufferSize>&& in_deserializer, DataCode in_code) = 0;
      virtual bool doHandleCommand(Deserializer<c_bufferSize>&& in_deserializer, CommandCode in_code) = 0;

      bool handleData(Deserializer<c_bufferSize>&& in_deserializer)
      {
        auto code = DataCode{ in_deserializer.read<std::underlying_type<DataCode>::type>() };
        return doHandleData(std::move(in_deserializer), code);
      }

      bool handleCommand(Deserializer<c_bufferSize>&& in_deserializer)
      {
        auto code = CommandCode{ in_deserializer.read<std::underlying_type<CommandCode>::type>() };
        return doHandleCommand(std::move(in_deserializer), code);
      }
      
    public:
      SerialInterface() = default;

      // Implementer has to config and "begin" Serial port.
      virtual bool setup() = 0;

      void send()
      {
        Serializer<c_bufferSize> serialzer(m_serializerBuffer);

      }

      bool receive()
      {
        if (Serial1.available() < sizeof(MessageSizeType)) { return false; }
        
        Deserializer<c_bufferSize> deserializer(m_deserializerBuffer);
        Serial1.readBytes(m_deserializerBuffer.data(), sizeof(MessageSizeType));
        MessageSizeType messageSize = deserializer.read<MessageSizeType>();
        size_t bytesReceived = Serial1.readBytes(m_deserializerBuffer.data() + deserializer.getBytesRead(),
                                                 messageSize - deserializer.getBytesRead());
        
        if (bytesReceived != messageSize) { return false; }

        auto interfaceTag = deserializer.read<uint16_t>();
        // ToDo: Check tag!
        auto interfaceVersion = deserializer.read<uint16_t>();
        // ToDo: Check version!
        auto messageType = MessageType{ deserializer.read<std::underlying_type<MessageType>::type>() };

        switch (messageType)
        {
          case MessageType::invalid: return false;
          case MessageType::data: return handleData(std::move(deserializer));
          case MessageType::command: return handleCommand(std::move(deserializer));
        }

        return true;
      }
  };
}
