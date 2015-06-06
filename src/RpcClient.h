#pragma once
#include <stdint.h>
#include "SocketRpcChannel.h"
#include "RpcController.h"
#include "ClientNamenodeProtocol.pb.h"

using namespace hadoop::hdfs;


namespace native
{
	namespace libhdfs
	{
		class RpcClient
		{
			public:
				RpcClient();
				~RpcClient();
				RpcClient(const char *host, const uint16_t port);

				//APIs:
				int ls(const char *path, GetListingResponseProto *response);
				int rm(const char *path, DeleteResponseProto *response);	
				int mv(const char *src, const char *dst, RenameResponseProto *response);
				int mkdir(const char *path, MkdirsResponseProto *response);

			private:
				SocketRpcChannel *m_channel;
				RpcController *m_controller;
				ClientNamenodeProtocol::Stub *m_client;
		};
	}
}
