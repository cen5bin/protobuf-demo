#pragma once
//#include "RpcClient.h"

namespace native
{
	namespace libhdfs
	{
		class RpcClient;
		class FileSystem
		{
			public:
				FileSystem(){}
				FileSystem(const char *host, int port);
				~FileSystem();
				bool exists(const char *path);
				bool remove(const char *path, bool recursive=false);
				bool mkdir(const char *path, int perm=0755);
				bool rename(const char *src, const char *dest);
				bool isFile(const char *path);
				bool isDirectory(const char *path);
				void copyToLocalFile(const char *src, const char *dest);
			

			private:
				RpcClient *m_client;
		};
	}
}
