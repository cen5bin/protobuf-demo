#include "SocketChannel.h"
#include "common.h"
#include <cstring>
#include <string>
#include "google/protobuf/io/coded_stream.h"

#define ERROR
#include "Log.h"

using namespace native::libhdfs;
using namespace google::protobuf::io;

SocketChannel::~SocketChannel()
{
	close(m_sockfd);
}

int SocketChannel::sendMessage(const void *msg, int msg_len) const
{
	return send(m_sockfd, msg, msg_len, 0);
}

int SocketChannel::sendMessage(const char *msg) const
{
	return send(m_sockfd, (void *)msg, strlen(msg), 0);
}

int SocketChannel::sendMessage(const int8_t &msg) const
{
	_D("sendMessage int8_t");
	return send(m_sockfd, (void *)&msg, sizeof(int8_t), 0);
}

int SocketChannel::sendMessage(const int16_t &msg) const
{
	_D("sendMessage int16_t");
	return send(m_sockfd, (void *)&msg, sizeof(int16_t), 0);
}
int SocketChannel::sendMessage(const int32_t &msg) const
{
	_D("sendMessage int32_t");
	return send(m_sockfd, (void *)&msg, sizeof(int32_t), 0);
}

int SocketChannel::receiveMessage(void *buf, int buf_size)
{
	return recv(m_sockfd, buf, buf_size, 0);
}

int SocketChannel::receiveMessage(uint8_t *buf, int buf_size)
{
	return recv(m_sockfd, (void *)buf, buf_size, 0);
}

int SocketChannel::sendProtobufMessage(const Message *msg) const 
{
	static string buf;
	msg->SerializeToString(&buf);
	_D("%d", buf.length());
	return send(m_sockfd, (void *)buf.c_str(), buf.length(), 0);
}

int SocketChannel::sendProtobufMessageWithLength(const Message *msg) const
{
	static uint8_t buf[10];
	int msg_len = msg->ByteSize();
	_D("msg_len: %d", msg_len);
	CodedOutputStream::WriteVarint32ToArray(msg_len, buf);
	for (int i = 0; i < CodedOutputStream::VarintSize32(msg_len); ++i) _D("%d", buf[i]);
	int ret = send(m_sockfd, (void *)buf, CodedOutputStream::VarintSize32(msg_len), 0);
	return ret + this->sendProtobufMessage(msg);
}

//从数组获得一个varint
const uint8_t* SocketChannel::readVarint32FromArray(const uint8_t* buffer, uint32_t* value) {
	static const int kMaxVarintBytes = 10;
	static const int kMaxVarint32Bytes = 5;
	const uint8* ptr = buffer;
	uint32 b;
	uint32 result;

	b = *(ptr++); result = (b&0x7F); if(!(b&0x80)) goto done;
	b = *(ptr++); result|=(b&0x7F)<<7; if(!(b&0x80)) goto done;
	b = *(ptr++); result|=(b&0x7F)<<14; if(!(b&0x80)) goto done;
	b = *(ptr++); result|=(b&0x7F)<<21; if(!(b&0x80)) goto done;
	b = *(ptr++); result |=  b<<28; if (!(b & 0x80)) goto done;
	// If the input is larger than 32 bits, we still need to read it all
	// and discard the high-order bits.
	for (int i = 0; i < kMaxVarintBytes - kMaxVarint32Bytes; i++) 
	{
		b = *(ptr++); if (!(b & 0x80)) goto done;
	}
	return NULL;
done:
	*value = result;
	return ptr;
}

void SocketChannel::closeSocket()
{
	close(m_sockfd);
}
