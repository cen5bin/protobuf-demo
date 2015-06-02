#include "SocketRpcChannel.h"
#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <string>
#include <uuid/uuid.h>

#include "google/protobuf/io/coded_stream.h"

#define LOG
#include "Log.h"

using std::string;
using namespace google::protobuf::io;

static const char *RPC_HEADER = "hrpc";
static const int8_t VERSON = 9;
static const int8_t RPC_SERVICE_CLASS = 0x00;
static const int8_t AUTH_PROTOCOL_NONE = 0x00;
//static const int8_t RPC_PROTOCOL_BUFFER = 0x02;

static const char *HDFS_PROTOCOL = "org.apache.hadoop.hdfs.protocol.ClientProtocol";


SocketRpcChannel::~SocketRpcChannel()
{
	close(m_sockfd);
}

SocketRpcChannel::SocketRpcChannel(const char *ip, const unsigned short port) 
{
	uuid_t uuid;
	uuid_generate_random(uuid);
	char s[37];
	uuid_unparse(uuid, m_clientId);
	m_callId = -3;
	m_sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (m_sockfd == -1)
	{
		_E("创建socket失败");
		perror("socket");
		exit(-1);
	}
	_D("sockfd: %d", m_sockfd);	
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

void SocketRpcChannel::CallMethod(const MethodDescriptor* method, RpcController* controller, const Message *request, Message *response, Closure *done)
{
	_F_IN_();
	_D("%s", method->full_name().c_str());
	//this->sendRpcMessage(method->full_name().c_str(), request);
	//this->getConnection();
	this->sendRpcMessage(method->name().c_str(), request);
	//this->sendRpcMessage(method->name().c_str(), request);
	//this->sendRpcMessage(method->name().c_str(), request);
	//this->sendRpcMessage(method->name().c_str(), request);
	//this->sendRpcMessage(method->name().c_str(), request);
	//_D("%d", ret);
	char *buf[1024];
	int ret = this->receiveMessage((void *)buf, 1024);
	_D("%d", ret);
	_F_OUT_();
}

int SocketRpcChannel::sendMessage(const void *msg, int msg_len) const
{
	return send(m_sockfd, msg, msg_len, 0);
}

int SocketRpcChannel::sendMessage(const char *msg) const
{
	return send(m_sockfd, (void *)msg, strlen(msg), 0);
}

int SocketRpcChannel::sendMessage(const int8_t &msg) const
{
	_D("sendMessage int8_t");
	return send(m_sockfd, (void *)&msg, sizeof(int8_t), 0);
}

int SocketRpcChannel::sendMessage(const int32_t msg) const
{
	_D("sendMessage int32_t");
	//int32_t tmp = htons(88);
	//for (int i = 0; i < 4; ++i, tmp/=256) printf("%2x ", tmp % 256);
	return send(m_sockfd, (void *)&msg, sizeof(int32_t), 0);
}

int SocketRpcChannel::receiveMessage(void *buf, int buf_size)
{
	_D("%d", m_sockfd);
	return recv(m_sockfd, buf, buf_size, 0);
}

inline int SocketRpcChannel::sendProtobufMessage(const Message *msg) const 
{
	static string buf;
	msg->SerializeToString(&buf);
	_D("%d", buf.length());
	return send(m_sockfd, (void *)buf.c_str(), buf.length(), 0);
}

int SocketRpcChannel::sendProtobufMessageWithLength(const Message *msg) const
{
	static uint8_t buf[10];
	int msg_len = msg->ByteSize();
	_D("msg_len: %d", msg_len);
	CodedOutputStream::WriteVarint32ToArray(msg_len, buf);
	for (int i = 0; i < CodedOutputStream::VarintSize32(msg_len); ++i) _D("%d", buf[i]);
	int ret = send(m_sockfd, (void *)buf, CodedOutputStream::VarintSize32(msg_len), 0);
	return ret + this->sendProtobufMessage(msg);
}

RpcRequestHeaderProto *SocketRpcChannel::createRpcRequestHeader()
{
	static RpcRequestHeaderProto request_header = RpcRequestHeaderProto();
	request_header.set_rpckind(RPC_PROTOCOL_BUFFER);
	request_header.set_rpcop((RpcRequestHeaderProto_OperationProto)0);
	request_header.set_callid(this->m_callId);
	request_header.set_retrycount(-1);
	char tmp = m_clientId[16];
	m_clientId[16] = '\0';
	request_header.set_clientid(m_clientId);
	m_clientId[16] = tmp;

	if (this->m_callId == -3) this->m_callId = 0;
	else this->m_callId++;

	return &request_header;
}

IpcConnectionContextProto *SocketRpcChannel::createIpcConnectionContext()
{
	static IpcConnectionContextProto context = IpcConnectionContextProto();
	context.set_protocol(HDFS_PROTOCOL);	
	UserInformationProto *userInfo =context.mutable_userinfo();
	userInfo->set_effectiveuser("hadoop");
	
	return &context;
}

RequestHeaderProto *SocketRpcChannel::createRequestHeader(const char *method)
{
	static RequestHeaderProto header = RequestHeaderProto();
	header.set_methodname(method);
	header.set_declaringclassprotocolname(HDFS_PROTOCOL);
	header.set_clientprotocolversion(1);
	return &header;
}

void SocketRpcChannel::getConnection()
{
	_F_IN_();
	int ret = this->sendMessage(RPC_HEADER);
	_D("%d", ret);
	ret = this->sendMessage(VERSON);
	_D("%d", ret);
	ret = this->sendMessage(RPC_SERVICE_CLASS);
	_D("%d", ret);
	ret = this->sendMessage(AUTH_PROTOCOL_NONE);
	_D("%d", ret);

	RpcRequestHeaderProto *request_header = this->createRpcRequestHeader();
	IpcConnectionContextProto *context = this->createIpcConnectionContext();
	
	int32_t header_len = request_header->ByteSize() + CodedOutputStream::VarintSize32(request_header->ByteSize()) + context->ByteSize() + CodedOutputStream::VarintSize32(context->ByteSize());
	_D("header_len: %d", header_len);

	ret = this->sendMessage((int32_t)htonl(header_len));
	_D("%d", ret); 
	ret = this->sendProtobufMessageWithLength(request_header);
	_D("%d", ret); 
	ret = this->sendProtobufMessageWithLength(context);

	_D("%d", ret);
	_F_OUT_();
}

int SocketRpcChannel::sendRpcMessage(const char *method, const Message *request)
{
	RpcRequestHeaderProto *rpc_request_header = this->createRpcRequestHeader();
	RequestHeaderProto *request_header = this->createRequestHeader(method);
	
	int len = rpc_request_header->ByteSize() + CodedOutputStream::VarintSize32(rpc_request_header->ByteSize()) +
		request_header->ByteSize() + CodedOutputStream::VarintSize32(request_header->ByteSize()) 
		+ request->ByteSize() + CodedOutputStream::VarintSize32(request->ByteSize());

	this->sendMessage((int32_t)htonl(len));
	this->sendProtobufMessageWithLength(rpc_request_header);
	this->sendProtobufMessageWithLength(request_header);
	this->sendProtobufMessageWithLength(request);
}

