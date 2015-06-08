#include "FileSystem.h"
#include "ClientNamenodeProtocol.pb.h"
#include "hdfs.pb.h"
#include <cstdio>
#include <cstring>

using namespace native::libhdfs;
using namespace hadoop::hdfs;

FileSystem::~FileSystem()
{
	if (m_client) delete m_client;
}

FileSystem::FileSystem(const char *host, int port)
{
	m_client = new RpcClient(host, port);
}

bool FileSystem::exists(const char *path)
{
	GetFileInfoResponseProto response;
	m_client->getFileInfo(path, &response);
	return response.has_fs();
}

bool FileSystem::remove(const char *path, bool recursive)
{
	DeleteResponseProto response;
	m_client->rm(path, recursive, &response);
	return response.result();
}

bool FileSystem::mkdir(const char *path, int perm)
{
	MkdirsResponseProto response;
	m_client->mkdir(path, perm, &response);
	return response.result();
}

bool FileSystem::rename(const char *src, const char *dest)
{
	RenameResponseProto response;
	m_client->mv(src, dest, &response);
	return response.result();
}

bool FileSystem::isDirectory(const char *path)
{
	GetFileInfoResponseProto response;
	m_client->getFileInfo(path, &response);
	return response.fs().filetype() == 1;
}

bool FileSystem::isFile(const char *path)
{
	GetFileInfoResponseProto response;
	m_client->getFileInfo(path, &response);
	return response.fs().filetype() == 2;
}

void FileSystem::copyToLocalFile(const char *src, const char *dest)
{

}
