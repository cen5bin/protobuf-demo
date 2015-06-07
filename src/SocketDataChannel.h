#pragma once
#include <stdint.h>
#include "Security.pb.h"
#include "SocketChannel.h"

using namespace hadoop::common;

namespace native 
{
	namespace libhdfs
	{
		class SocketDataChannel: public SocketChannel
		{
			public:
				~SocketDataChannel();
				SocketDataChannel(){}
				SocketDataChannel(const char *host, const uint32_t port);


				void readBlock(const uint64_t length, const char *pool_id, const uint64_t block_id,	const uint64_t generation_stamp, const uint64_t offset, const TokenProto &block_token, bool check_crc);

		};
	}
}
