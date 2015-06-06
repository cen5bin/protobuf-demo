#include "RpcClient.h"

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
