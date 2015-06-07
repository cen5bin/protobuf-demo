#include "SocketChannel.h"
#include "common.h"
#include <cstring>
#include <string>

#define DEBUG
#include "Log.h"

using namespace native::libhdfs;

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

int SocketChannel::sendMessage(const int32_t msg) const
{
	_D("sendMessage int32_t");
	//int32_t tmp = htons(88);
	//for (int i = 0; i < 4; ++i, tmp/=256) printf("%2x ", tmp % 256);
	return send(m_sockfd, (void *)&msg, sizeof(int32_t), 0);
}
