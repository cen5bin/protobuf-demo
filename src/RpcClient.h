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

				//hdfs APIs:
				int ls(const char *path, GetListingResponseProto *response);
				int rm(const char *path, bool recursive, DeleteResponseProto *response);	
				int mv(const char *src, const char *dst, RenameResponseProto *response);
				int mkdir(const char *path, int perm, MkdirsResponseProto *response);
				int chmod(const char *path, int mode, SetPermissionResponseProto *response);
				int chown(const char *path, const char *owner, const char *group, SetOwnerResponseProto *response);

				//create file
				int create(const char *path, int replication, int blocksize, bool overwrite, CreateResponseProto *response);
				int create(const char *path, CreateResponseProto *response);
				int getBlockLocations(const char *path, const HdfsFileStatusProto &fs, GetBlockLocationsResponseProto *response);


				int getFileInfo(const char *path, GetFileInfoResponseProto *response);

				//----------------------------
				inline bool isDir(const HdfsFileStatusProto &fs) const;
				inline bool isFile(const HdfsFileStatusProto &fs) const;
				void read(const char *path, char *buf, uint32_t size);

				void copyToLocalFile(const char *src, const char *dest);
			private:

				//int getBlockLocation(const char *path, )



				SocketRpcChannel *m_channel;
				RpcController *m_controller;
				ClientNamenodeProtocol::Stub *m_client;
		};
	}
}
