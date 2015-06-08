#pragma once
#include <stdint.h>

#define MAX_PACKET_SIZE (64 * 1024)

namespace native
{
	namespace libhdfs
	{

		class Packet
		{
			public:
				Packet(){}
				~Packet(){}
				Packet(int pktSize, int chunksPerPkt, int64_t offsetInBlock, int64_t seqno, int checksumSize);
				void addCheckSum(uint32_t checksum);
				void addData(const char *data, uint32_t size);
				char* getBuffer(int *size);
				void setSyncFlag(bool sync);
				void setLastPacketInBlock(bool lastPacket);

			private:
				char buf[MAX_PACKET_SIZE];
				int checksumStart;
				int checksumPos;
				int checksumSize;

				int dataPos;
				int dataStart;

				int maxChunks;
				int numChunks;

				int headStart;

				bool lastPacketInBlock;
				bool syncBlock;
				int64_t offsetInBlock;
				int64_t seqno;
		};
	}
}
