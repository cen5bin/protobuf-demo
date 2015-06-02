#include "SocketRpcChannel.h"
#include "ClientNamenodeProtocol.pb.h"
#include <cstdio>
#define LOG
#include "Log.h"

#include <uuid/uuid.h>
using namespace hadoop::hdfs;

int main()
{

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
	GetListingResponseProto rep;
	req.set_src("/");
	req.set_startafter("");
	req.set_needlocation(false);

//	stub.getListing(NULL, &req, &rep, NULL);
	stub.delete1(NULL, &request1, &response1, NULL);
	return 0;
}
