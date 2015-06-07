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

int RpcClient::rm(const char *path, DeleteResponseProto *response)
{
	DeleteRequestProto request;
	request.set_src(path);
	request.set_recursive(false);
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

int RpcClient::mkdir(const char *path, MkdirsResponseProto *response)
{
	MkdirsRequestProto request;

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
		channel.readBlock(length, pool_id, block.b().blockid(), block.b().generationstamp(), offset_in_block, token, false);
	}
}

bool RpcClient::isDir(const HdfsFileStatusProto &fs) const
{
	return fs.filetype() == 1;
}

bool RpcClient::isFile(const HdfsFileStatusProto &fs) const
{
	return fs.filetype() == 2;
}
