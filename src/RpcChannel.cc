#include "RpcChannel.h"
#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <string>

#include "coded_stream.h"

#define LOG
#include "Log.h"

using std::string;
using namespace google::protobuf::io;

static const char *RPC_HEADER = "hrpc";
static const int8_t VERSON = 9;
static const int8_t RPC_SERVICE_CLASS = 0x00;
static const int8_t AUTH_PROTOCOL_NONE = 0x00;
static const int8_t RPC_PROTOCOL_BUFFER = 0x02;

static const char *HDFS_PROTOCOL = "org.apache.hadoop.hdfs.protocol.ClientProtocol";


RpcChannel::~RpcChannel()
{
	close(m_sockfd);
}

RpcChannel::RpcChannel(const char *ip, const unsigned short port) 
{
	m_callId = -3;
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
	_D("sendMessage int8_t");
	return send(m_sockfd, (void *)&msg, sizeof(int8_t), 0);
}

int RpcChannel::sendMessage(const int32_t &msg) const
{
	_D("sendMessage int32_t");
	return send(m_sockfd, (void *)&msg, sizeof(int32_t), 0);
}

int RpcChannel::receiveMessage(void *buf, int buf_size)
{
	return recv(m_sockfd, buf, buf_size, 0);
}

int RpcChannel::sendMessageWidthLength(const void *msg, int msg_len) const 
{
	static uint8_t buf[10];
	_F_IN_();
	CodedOutputStream::WriteVarint32ToArray(msg_len, buf);
	_D("msg_len: %d", msg_len);
	for (int i = 0; i < CodedOutputStream::VarintSize32(msg_len); ++i) _D("%d", buf[i]);
	int ret = send(m_sockfd, (void *)buf, CodedOutputStream::VarintSize32(msg_len), 0);
	ret += send(m_sockfd, msg, msg_len, 0);
	_F_OUT_();
	return ret;
}

inline int RpcChannel::sendProtobufMessage(const Message *msg) const 
{
	static string buf;
	msg->SerializeToString(&buf);
	_D("%d", buf.length());
	return send(m_sockfd, (void *)buf.c_str(), buf.length(), 0);
}

int RpcChannel::sendProtobufMessageWithLength(const Message *msg) const
{
	static uint8_t buf[10];
	int msg_len = msg->ByteSize();
	_D("msg_len: %d", msg_len);
	CodedOutputStream::WriteVarint32ToArray(msg_len, buf);
	for (int i = 0; i < CodedOutputStream::VarintSize32(msg_len); ++i) _D("%d", buf[i]);
	int ret = send(m_sockfd, (void *)buf, CodedOutputStream::VarintSize32(msg_len), 0);
	return ret + this->sendProtobufMessage(msg);
}

RpcRequestHeaderProto *RpcChannel::createRpcRequestHeader()
{
	static RpcRequestHeaderProto request_header = RpcRequestHeaderProto();
	request_header.set_rpckind((RpcKindProto)2);
	request_header.set_rpcop((RpcRequestHeaderProto_OperationProto)0);
	request_header.set_callid(this->m_callId);
	request_header.set_retrycount(-1);
	request_header.set_clientid("asdasdada");	

	if (this->m_callId == -3) this->m_sockfd = 0;
	else this->m_callId++;

	return &request_header;
}

IpcConnectionContextProto *RpcChannel::createIpcConnectionContext()
{
	static IpcConnectionContextProto context = IpcConnectionContextProto();
	context.set_protocol(HDFS_PROTOCOL);	
	UserInformationProto *userInfo =context.mutable_userinfo();
	userInfo->set_effectiveuser("asdasda");
	
	return &context;
}

void RpcChannel::getConnection()
{
	this->sendMessage(RPC_HEADER);
	this->sendMessage(VERSON);
	this->sendMessage(RPC_SERVICE_CLASS);
	this->sendMessage(AUTH_PROTOCOL_NONE);

	RpcRequestHeaderProto *request_header = this->createRpcRequestHeader();
	_D("%d", request_header->ByteSize());
	IpcConnectionContextProto *context = this->createIpcConnectionContext();
	_D("%d", context->ByteSize());
	
	int32_t header_len = request_header->ByteSize() + CodedOutputStream::VarintSize32(request_header->ByteSize()) + context->ByteSize() + CodedOutputStream::VarintSize32(context->ByteSize());

	this->sendMessage(header_len);
	this->sendProtobufMessageWithLength(request_header);
	this->sendProtobufMessageWithLength(context);
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
