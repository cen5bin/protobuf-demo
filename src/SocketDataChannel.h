#pragma once
#include <stdint.h>
#include "Security.pb.h"
#include "SocketChannel.h"
#include "ClientNamenodeProtocol.pb.h"

using namespace hadoop::common;
using namespace hadoop::hdfs;

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


				uint64_t readBlock(const uint64_t length, const char *pool_id, const uint64_t block_id,	const uint64_t generation_stamp, const uint64_t offset, const TokenProto &block_token, bool check_crc, uint8_t *block_buf);

void writeBlock(const LocatedBlockProto &block, const uint64_t length, const uint64_t offset, const TokenProto &block_token, const uint64_t latestgenerationstamp, const uint64_t minBytesRcvd, const uint64_t maxBytesRcv, const uint32_t pipelinesize, int stage);
			private:
				//void sendRequest(const uint64_t length, const char *pool_id, const uint64_t block_id,	const uint64_t generation_stamp, const uint64_t offset, const tokenproto &block_token);
		};
	}
}
