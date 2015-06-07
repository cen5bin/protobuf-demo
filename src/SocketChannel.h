#pragma once
#include <stdint.h>

namespace native
{
	namespace libhdfs
	{

		class SocketChannel
		{
			public:
				~SocketChannel();
				SocketChannel(){}
				
			protected:
				int sendMessage(const void *msg, int msg_len) const;
				int sendMessage(const char *msg) const;
				int sendMessage(const int8_t &msg) const;
				int sendMessage(const int32_t msg) const;
				int m_sockfd;
				int receiveMessage(void *buf, int buf_size);
				int receiveMessage(uint8_t *buf, int buf_size);

		};

	}
}
