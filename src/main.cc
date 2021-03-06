#include "SocketRpcChannel.h"
#include "ClientNamenodeProtocol.pb.h"
#include "hdfs.pb.h"
#include <cstdio>
#define LOG
#include "Log.h"
#include "RpcClient.h"
#include <uuid/uuid.h>
#include "FileSystem.h"
using namespace hadoop::hdfs;
using namespace native::libhdfs;

int main()
{

	//FILE *fp = fopen("xx", "wb");
	//if (!fp)
	//{
	//	_E("can't open file");
	//	return -1;
	//}
	//return 0;
	RpcClient client("127.0.0.1", 9000);
	GetFileInfoResponseProto res;
	//client.getFileInfo("/bb", &res);
	//client.getFileInfo("/user/hadoop/input.txt", &res);

	//MkdirsResponseProto rr;
	//client.mkdir("/pp", &rr);

	//SetPermissionResponseProto rr1;
	//client.chmod("/xx", 0777, &rr1);

	//SetOwnerResponseProto rr2;
	//client.chown("/xx", "kk", "kk", &rr2);

	FileSystem fs("127.0.0.1", 9000);
	//fs.rename("/xx1", "/xx2");
	//fs.mkdir("/asd123");
	bool xxx = fs.exists("/xx2");
	if (xxx) _D("exists");
	else _D("not exists");

	fs.copyToLocalFile("/bb", "xx");
	fs.copyToLocalFile("/user/hadoop/input.txt", "xx1");
	//fs.remove("/asd123", true);
	//client.read("/bb", NULL, 0);
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
