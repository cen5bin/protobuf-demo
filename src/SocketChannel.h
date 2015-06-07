#pragma once
#include <stdint.h>
#include "google/protobuf/message.h"

using namespace google::protobuf;

namespace native
{
	namespace libhdfs
	{

		class SocketChannel
		{
			public:
				~SocketChannel();
				SocketChannel(){}
				void closeSocket();
				
			protected:
				int sendMessage(const void *msg, int msg_len) const;
				int sendMessage(const char *msg) const;
				int sendMessage(const int8_t &msg) const;
				int sendMessage(const int16_t &msg) const;
				int sendMessage(const int32_t &msg) const;
				int m_sockfd;
				int receiveMessage(void *buf, int buf_size);
				int receiveMessage(uint8_t *buf, int buf_size);

				int sendProtobufMessage(const Message *msg) const;
				int sendProtobufMessageWithLength(const Message *msg) const;
				const uint8_t *readVarint32FromArray(const uint8_t *buf, uint32_t *value);
		};

	}
}
