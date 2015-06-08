#include "Packet.h"
#include <cstdlib>
#include <cstring>
#include "PacketHeader.h"


#define DEBUG
#include "Log.h"

//#include "datatransfer.pb.h"


using namespace native::libhdfs;

Packet::Packet(int pktSize, int chunksPerPkt, int64_t offsetInBlock, int64_t seqno, int checksumSize): checksumSize(checksumSize), offsetInBlock(offsetInBlock)
{
	lastPacketInBlock = syncBlock = 0;
	headStart = 0;
	maxChunks = chunksPerPkt;
	numChunks = 0;
	checksumPos = checksumStart = PacketHeader::GetPkgHeaderSize();
	dataPos = dataStart = checksumStart + chunksPerPkt * checksumSize; 

}

void Packet::setSyncFlag(bool sync)
{
	syncBlock = sync;
}
void Packet::setLastPacketInBlock(bool lastPacket)
{
	lastPacketInBlock = lastPacket;
}
char *Packet::getBuffer(int *size)
{
	int dataLen = dataPos - dataStart;
	int checksumLen = checksumPos - checksumStart;
	int pktLen = dataLen + checksumLen;

	PacketHeader header(pktLen + sizeof(int32_t), offsetInBlock, seqno, lastPacketInBlock, dataLen);
	header.writeInBuffer(buf+headStart, PacketHeader::GetPkgHeaderSize());
	*size = pktLen + PacketHeader::GetPkgHeaderSize();
	return buf;
}

void Packet::addCheckSum(uint32_t checksum)
{
	memcpy(&buf[checksumPos], &checksum, sizeof(checksum));
	checksumPos += sizeof(checksum);
}

void Packet::addData(const char *data, uint32_t size)
{
	memcpy(buf + dataPos, data, size);
	dataPos += size;
}
