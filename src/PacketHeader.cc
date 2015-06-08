#include "PacketHeader.h"
#include "BigEndian.h"

namespace native {
	namespace libhdfs {
		int PacketHeader::PkgHeaderSize = PacketHeader::CalcPkgHeaderSize();

		int PacketHeader::CalcPkgHeaderSize() {
			PacketHeaderProto header;
			header.set_offsetinblock(0);
			header.set_datalen(0);
			header.set_lastpacketinblock(false);
			header.set_seqno(0);
			return header.ByteSize() + sizeof(int32_t) /*packet length*/ + sizeof(int16_t)/* proto length */;
		}

		int PacketHeader::GetPkgHeaderSize() {
			return PkgHeaderSize;
		}

		PacketHeader::PacketHeader() :
			packetLen(0) {
			}

		PacketHeader::PacketHeader(int packetLen, int64_t offsetInBlock, int64_t seqno,
				bool lastPacketInBlock, int dataLen) :
			packetLen(packetLen) {
				proto.set_offsetinblock(offsetInBlock);
				proto.set_seqno(seqno);
				proto.set_lastpacketinblock(lastPacketInBlock);
				proto.set_datalen(dataLen);
			}

		int PacketHeader::getDataLen() {
			return proto.datalen();
		}

		bool PacketHeader::isLastPacketInBlock() {
			return proto.lastpacketinblock();
		}

		bool PacketHeader::sanityCheck(int64_t lastSeqNo) {
			// We should only have a non-positive data length for the last packet
			if (proto.datalen() <= 0 && !proto.lastpacketinblock())
				return false;

			// The last packet should not contain data
			if (proto.lastpacketinblock() && proto.datalen() != 0)
				return false;

			// Seqnos should always increase by 1 with each packet received
			if (proto.seqno() != lastSeqNo + 1)
				return false;

			return true;
		}

		int64_t PacketHeader::getSeqno() {
			return proto.seqno();
		}

		int64_t PacketHeader::getOffsetInBlock() {
			return proto.offsetinblock();
		}

		int PacketHeader::getPacketLen() {
			return packetLen;
		}

		void PacketHeader::readFields(const char * buf, size_t size) {
			int16_t protoLen;
			assert(size > sizeof(packetLen) + sizeof(protoLen));
			packetLen = ReadBigEndian32FromArray(buf);
			protoLen = ReadBigEndian16FromArray(buf + sizeof(packetLen));

			if (packetLen < static_cast<int>(sizeof(int32_t)) || protoLen < 0
					|| static_cast<int>(sizeof(packetLen) + sizeof(protoLen)) + protoLen > static_cast<int>(size)) {
			}

			if (!proto.ParseFromArray(buf + sizeof(packetLen) + sizeof(protoLen),
						protoLen)) {
		}
		}

		void PacketHeader::writeInBuffer(char * buf, size_t size) {
			buf = WriteBigEndian32ToArray(packetLen, buf);
			buf = WriteBigEndian16ToArray(proto.ByteSize(), buf);
			proto.SerializeToArray(buf, size - sizeof(int32_t) - sizeof(int16_t));
		}

	}
}

