#include "SocketRpcChannel.h"
#include "ClientNamenodeProtocol.pb.h"
#include "hdfs.pb.h"
#include <cstdio>
#define LOG
#include "Log.h"
#include "RpcClient.h"
#include <uuid/uuid.h>
using namespace hadoop::hdfs;
using namespace native::libhdfs;

int main()
{

	RpcClient client("127.0.0.1", 9000);
	GetFileInfoResponseProto res;
	client.getFileInfo("/bb", &res);
	//client.getFileInfo("/user/hadoop/input.txt", &res);

	client.read("/bb", NULL, 0);
	//client.read("/user/hadoop/input.txt", NULL, 0);

	GetListingResponseProto rep;
	//client.ls("/", &rep);
	//DeleteResponseProto rr;
	//client.rm("/asd11", &rr);
	//RenameResponseProto rr1;
	//client.mv("/asd1", "/asd11", &rr1);
	//CreateResponseProto rr2;
	//_D("%d", client.create("/xx1", &rr2));
	return 0;

	uuid_t uuid;
	uuid_generate_random ( uuid );
	char s[37];
	uuid_unparse ( uuid, s );
	puts(s);

	SocketRpcChannel channel("127.0.0.1", 9000);
	channel.getConnection();

	ClientNamenodeProtocol::Stub stub(&channel);
	RenameRequestProto request;
	RenameResponseProto response;
	DeleteRequestProto request1;
	request.set_src("/asd");
	request.set_dst("/asdaa");
	request1.set_src("/asd1");
	request1.set_recursive(false);
	DeleteResponseProto response1;
	if (request.IsInitialized()) _D("yes");
	else _D("no");


	GetListingRequestProto req;
	req.set_src("/");
	req.set_startafter("");
	req.set_needlocation(false);

	stub.getListing(NULL, &req, &rep, NULL);
	DirectoryListingProto dirlist = rep.dirlist();
	_D("%d", dirlist.remainingentries());
	_D("%d", dirlist.partiallisting_size());
	//_D()
//	stub.delete1(NULL, &request1, &response1, NULL);
	return 0;
}
