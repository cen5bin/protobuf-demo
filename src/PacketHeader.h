#pragma once
#include <stdint.h>
#include "datatransfer.pb.h"

using namespace hadoop::hdfs;

namespace native {
	namespace libhdfs {

		class PacketHeader {
			public:
				PacketHeader();
				PacketHeader(int packetLen, int64_t offsetInBlock, int64_t seqno,
						bool lastPacketInBlock, int dataLen);
				bool isLastPacketInBlock();
				bool sanityCheck(int64_t lastSeqNo);
				int getDataLen();
				int getPacketLen();
				int64_t getOffsetInBlock();
				int64_t getSeqno();
				void readFields(const char * buf, size_t size);
				/**
				 * Write the header into the buffer.
				 * This requires that PKT_HEADER_LEN bytes are available.
				 */
				void writeInBuffer(char * buf, size_t size);

			public:
				static int GetPkgHeaderSize();
				static int CalcPkgHeaderSize();

			private:
				static int PkgHeaderSize;
			private:
				int32_t packetLen;
				PacketHeaderProto proto;
		};

	}
}

