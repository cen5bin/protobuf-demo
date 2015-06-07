#pragma once
#include "common.h"
#include <stdint.h>
#include "RpcHeader.pb.h"
#include "IpcConnectionContext.pb.h"
#include "google/protobuf/message.h"
#include "google/protobuf/service.h"
#include "ProtobufRpcEngine.pb.h"
#include "SocketChannel.h"

using namespace hadoop::common;
using namespace google::protobuf;


namespace native
{
	namespace libhdfs
	{
		class SocketRpcChannel : public RpcChannel, public SocketChannel
		{
			public:
				SocketRpcChannel(){}
				~SocketRpcChannel();
				SocketRpcChannel(const char *ip, const uint16_t port);
				
				void getConnection();
				
				virtual void CallMethod(const MethodDescriptor* method, google::protobuf::RpcController* controller, const Message *request, Message *response, Closure *done);

			private:
				int m_callId;
				char m_clientId[37];
				//int sendMessage(const void *msg, int msg_len) const;
				//int sendMessage(const char *msg) const;
				//int sendMessage(const int8_t &msg) const;
				//int sendMessage(const int32_t msg) const;
				//int m_sockfd;
				RpcRequestHeaderProto *createRpcRequestHeader();
				IpcConnectionContextProto *createIpcConnectionContext();
				RequestHeaderProto *createRequestHeader(const char *method);


				inline int sendProtobufMessage(const Message *msg) const;
				int sendProtobufMessageWithLength(const Message *msg) const;

				int sendRpcMessage(const char *method, const Message *request);

//				inline int receiveMessage(void *buf, int buf_size);
//				inline int receiveMessage(uint8_t *buf, int buf_size);
				int receiveRpcResponse(Message *response);
				void parseRpcResponse(Message *response, const uint8_t *buf, const uint32_t len);
				inline const uint8_t *readVarint32FromArray(const uint8_t *buf, uint32_t *value);

		};

	}
}
