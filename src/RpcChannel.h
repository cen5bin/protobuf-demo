#pragma once
#include "common.h"
#include <stdint.h>
#include "RpcHeader.pb.h"
#include "IpcConnectionContext.pb.h"
#include "message.h"
using namespace hadoop::common;
using namespace google::protobuf;


class RpcChannel
{
    private:
        int m_sockfd;

    public:
        RpcChannel(){}
        ~RpcChannel();
        RpcChannel(const char *ip, const uint16_t port);
        int sendMessage(const void *msg, int msg_len) const;
        int sendMessageWidthLength(const void *msg, int msg_len) const;
		int sendMessage(const char *msg) const;
		int sendMessage(const int8_t &msg) const;
		int sendMessage(const int32_t &msg) const;
		inline int sendProtobufMessage(const Message *msg) const;
		int sendProtobufMessageWithLength(const Message *msg) const;
        int receiveMessage(void *buf, int buf_size);
		void getConnection();
	
	private:
		int m_callId;
		RpcRequestHeaderProto *createRpcRequestHeader();
		IpcConnectionContextProto *createIpcConnectionContext();

};
