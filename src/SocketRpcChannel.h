#pragma once
#include "common.h"
#include <stdint.h>
#include "RpcHeader.pb.h"
#include "IpcConnectionContext.pb.h"
#include "google/protobuf/message.h"
#include "google/protobuf/service.h"
using namespace hadoop::common;
using namespace google::protobuf;


class SocketRpcChannel : public RpcChannel
{
    private:
        int m_sockfd;

    public:
        SocketRpcChannel(){}
        ~SocketRpcChannel();
        SocketRpcChannel(const char *ip, const uint16_t port);
		
		void getConnection();
		//void send_rpc_message()
		
		virtual void CallMethod(const MethodDescriptor* method, RpcController* controller, const Message *request, Message *response, Closure *done);

	private:
		int m_callId;
		RpcRequestHeaderProto *createRpcRequestHeader();
		IpcConnectionContextProto *createIpcConnectionContext();
        int sendMessage(const void *msg, int msg_len) const;
        int sendMessageWidthLength(const void *msg, int msg_len) const;
		int sendMessage(const char *msg) const;
		int sendMessage(const int8_t &msg) const;
		int sendMessage(const int32_t &msg) const;
		inline int sendProtobufMessage(const Message *msg) const;
		int sendProtobufMessageWithLength(const Message *msg) const;

        int receiveMessage(void *buf, int buf_size);

};