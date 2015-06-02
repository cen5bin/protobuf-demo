#include "RpcChannel.h"
#include <cstdio>
#include <cstring>
#include <cstdlib>

#define LOG
#include "Log.h"

static const char *RPC_HEADER = "hrpc";
static const int8_t VERSON = 9;
static const int8_t RPC_SERVICE_CLASS = 0x00;
static const int8_t AUTH_PROTOCOL_NONE = 0x00;
static const int8_t RPC_PROTOCOL_BUFFER = 0x02;


RpcChannel::~RpcChannel()
{
	close(m_sockfd);
}

RpcChannel::RpcChannel(const char *ip, const unsigned short port) 
{
	m_sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (m_sockfd == -1)
	{
		_E("创建socket失败");
		perror("socket");
		exit(-1);
	}
	
	//目标地址
	sockaddr_in dest_addr;
	dest_addr.sin_family = AF_INET;
	dest_addr.sin_port = htons(port);
	dest_addr.sin_addr.s_addr = inet_addr(ip);
	memset(dest_addr.sin_zero, 0, sizeof(dest_addr.sin_zero));
	
	int ret = connect(m_sockfd, (sockaddr*)&dest_addr, sizeof(sockaddr));
	if (ret == -1)
	{
		perror("connect");
		exit(-1);
	}
}

int RpcChannel::sendMessage(const void *msg, int msg_len) const
{
	return send(m_sockfd, msg, msg_len, 0);
}

int RpcChannel::sendMessage(const char *msg) const
{
	return send(m_sockfd, (void *)msg, strlen(msg), 0);
}

int RpcChannel::sendMessage(const int8_t &msg) const
{
	return send(m_sockfd, (void *)&msg, sizeof(int8_t), 0);
}

int RpcChannel::receiveMessage(void *buf, int buf_size)
{
	return recv(m_sockfd, buf, buf_size, 0);
}

void RpcChannel::getConnection()
{
	int ret = this->sendMessage(RPC_HEADER);
	_D("%d", ret);
	ret = this->sendMessage(VERSON);
	_D("%d", ret);
}


int main()
{
	RpcChannel channel("127.0.0.1", 9000);
//	printf("%d\n", channel.sendMessage((void *)"asd^]", 5));
	//printf("%d\n", channel.sendMessage((void *)"asd", 3));
//	char buf[1024];
//	int ret = channel.receiveMessage((void *)buf, 1024);
//	printf("%d\n", ret);
	channel.getConnection();
	return 0;
}
