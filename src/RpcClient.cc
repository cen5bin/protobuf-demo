#include "RpcClient.h"
#include "SocketDataChannel.h"
#include <string>

#define DEBUG
#include "Log.h"

using namespace native::libhdfs;

RpcClient::~RpcClient()
{
	if (m_channel) delete m_channel;
	if (m_client) delete m_client;
	if (m_controller) delete m_controller;
}

RpcClient::RpcClient(const char *host, const uint16_t port)
{
	m_channel = new SocketRpcChannel(host, port);
	m_channel->getConnection();
	m_client = new ClientNamenodeProtocol::Stub(m_channel);
	m_controller = new RpcController();
}

int RpcClient::ls(const char *path, GetListingResponseProto *response)
{
	GetListingRequestProto request;
	request.set_src(path);
	request.set_startafter("");
	request.set_needlocation(false);
	m_client->getListing(m_controller, &request, response, NULL);
	return m_controller->status();
}

int RpcClient::rm(const char *path, bool recursive, DeleteResponseProto *response)
{
	DeleteRequestProto request;
	request.set_src(path);
	request.set_recursive(recursive);
	m_client->delete1(m_controller, &request, response, NULL);
	return m_controller->status();
}

int RpcClient::mv(const char *src, const char *dst, RenameResponseProto *response)
{
	RenameRequestProto request;
	request.set_src(src);
	request.set_dst(dst);
	m_client->rename(m_controller, &request, response, NULL);
	return m_controller->status();
}

int RpcClient::mkdir(const char *path, int perm, MkdirsResponseProto *response)
{
	MkdirsRequestProto request;
	request.set_src(path);
	request.set_createparent(false);
	request.mutable_masked()->set_perm(perm);
	m_client->mkdirs(m_controller, &request, response, NULL);
	return m_controller->status();
}

int RpcClient::chmod(const char *path, int mode, SetPermissionResponseProto *response)
{
	SetPermissionRequestProto request;
	request.set_src(path);
	request.mutable_permission()->set_perm(mode);
	m_client->setPermission(m_controller, &request, response, NULL);
	return m_controller->status();
}

int RpcClient::chown(const char *path, const char *owner, const char *group, SetOwnerResponseProto *response)
{
	SetOwnerRequestProto request;
	request.set_src(path);
	request.set_username(owner);
	request.set_groupname(group);
	m_client->setOwner(m_controller, &request, response, NULL);
	return m_controller->status();
}

int RpcClient::create(const char *path, int replication, int blocksize, bool overwrite, CreateResponseProto *response)
{
	CreateRequestProto request;
	request.set_src(path);
	int flag = overwrite ? 0x02 : 0x01;
	request.set_createflag(flag);
	request.set_createparent(false);
	request.set_clientname("native libhdfs");
	request.set_replication(replication);
	request.set_blocksize(blocksize);
	FsPermissionProto *perm = request.mutable_masked();
	perm->set_perm(0664);
	m_client->create(m_controller, &request, response, NULL);
	return m_controller->status();
}

int RpcClient::create(const char *path, CreateResponseProto *response)
{
	return this->create(path, 1, 64 * 1024 * 1024, false, response);
}

int RpcClient::getFileInfo(const char *path, GetFileInfoResponseProto *response)
{
	GetFileInfoRequestProto request;
	request.set_src(path);
	m_client->getFileInfo(m_controller, &request, response, NULL);
	return m_controller->status();
}

int RpcClient::getBlockLocations(const char *path, const HdfsFileStatusProto &fs, GetBlockLocationsResponseProto *response)
{
	GetBlockLocationsRequestProto request;
	request.set_src(path);
	request.set_length(fs.length());
	request.set_offset(0);
	m_client->getBlockLocations(m_controller, &request, response, NULL);
	return m_controller->status();
}

void RpcClient::write(const char *path)
{
	AddBlockRequestProto request;
	AddBlockResponseProto response;
	request.set_src(path);
	request.set_clientname("libhdfs");
	m_client->addBlock(m_controller, &request, &response, NULL);
	GetFileInfoResponseProto fileInfo;
	this->getFileInfo(path, &fileInfo);
	GetBlockLocationsResponseProto blockLocation;
	this->getBlockLocations(path, fileInfo.fs(), &blockLocation);
	_D("%d", blockLocation.locations().blocks_size());
	//LocatedBlockProto block = blockLocation.locations().blocks(0);
	
}

void RpcClient::read(const char *path, char *buf, uint32_t size)
{
	GetFileInfoResponseProto fileInfo;
	this->getFileInfo(path, &fileInfo);
	GetBlockLocationsResponseProto blockLocation;
	this->getBlockLocations(path, fileInfo.fs(), &blockLocation);
	_D("%d", blockLocation.locations().blocks_size());
	_D("%s", blockLocation.locations().blocks(0).locs(0).id().ipaddr().c_str());

	if (blockLocation.locations().filelength() == 0) return;	
	
	for (int i = 0; i < blockLocation.locations().blocks_size(); ++i)
	{
		LocatedBlockProto block = blockLocation.locations().blocks(i);
		uint64_t length = block.b().numbytes();
		const char *pool_id = block.b().poolid().c_str();
		uint64_t offset_in_block = 0;
		TokenProto token = block.blocktoken();
		DatanodeInfoProto location = block.locs(0);
		const char *host = location.id().ipaddr().c_str();
		uint32_t port = location.id().xferport();

		SocketDataChannel channel(host, port);
		//channel.readBlock(length, pool_id, block.b().blockid(), block.b().generationstamp(), offset_in_block, token, false);
		channel.closeSocket();
	}
}

void RpcClient::copyToLocalFile(const char *path, const char *dest)
{
	static uint8_t block_buf[64 * 1024 * 1024];
	FILE *fp = fopen(dest, "wb");
	if (fp == NULL)
	{
		_E("can't open file %s", dest);
		return;
	}
	GetFileInfoResponseProto fileInfo;
	this->getFileInfo(path, &fileInfo);
	GetBlockLocationsResponseProto blockLocation;
	this->getBlockLocations(path, fileInfo.fs(), &blockLocation);
	_D("%d", blockLocation.locations().blocks_size());
	_D("%s", blockLocation.locations().blocks(0).locs(0).id().ipaddr().c_str());

	if (blockLocation.locations().filelength() == 0) return;	
	
	for (int i = 0; i < blockLocation.locations().blocks_size(); ++i)
	{
		LocatedBlockProto block = blockLocation.locations().blocks(i);
		uint64_t length = block.b().numbytes();
		const char *pool_id = block.b().poolid().c_str();
		uint64_t offset_in_block = 0;
		TokenProto token = block.blocktoken();
		DatanodeInfoProto location = block.locs(0);
		const char *host = location.id().ipaddr().c_str();
		uint32_t port = location.id().xferport();

		SocketDataChannel channel(host, port);
		uint64_t data_len = channel.readBlock(length, pool_id, block.b().blockid(), block.b().generationstamp(), offset_in_block, token, false, block_buf);
		_D("%d", data_len);
		fwrite(block_buf, data_len, 1, fp);
		channel.closeSocket();
	}
	fclose(fp);
}

bool RpcClient::isDir(const HdfsFileStatusProto &fs) const
{
	return fs.filetype() == 1;
}

bool RpcClient::isFile(const HdfsFileStatusProto &fs) const
{
	return fs.filetype() == 2;
}
