#include "SocketDataChannel.h"
#include "common.h"
#include <cstring>
#include <cstdlib>

#define DEBUG
#include "Log.h"

using namespace native::libhdfs;


//op codes
#define WRITE_BLOCK 80
#define READ_BLOCK 81
#define READ_METADATA 82
#define REPLACE_BLCOK 83
#define COPY_BLOCK 84
#define BLOCK_CHECKSUM 85
#define TRANSFER_BLOCK 86


SocketDataChannel::~SocketDataChannel()
{
	close(m_sockfd);
}

SocketDataChannel::SocketDataChannel(const char *host, const uint16_t port)
{
	m_sockfd = socket(AF_INET, SOCK_STREAM, 0);		
	if (m_sockfd == -1)
	{
		_E("create socket failed");
		perror("socket");
		exit(-1);
	}
	sockaddr_in dest_addr;
	dest_addr.sin_family = AF_INET;
	dest_addr.sin_port = htons(port);
	dest_addr.sin_addr.s_addr = inet_addr(host);
	memset(dest_addr.sin_zero, 0, sizeof(dest_addr.sin_zero));

	int ret = connect(m_sockfd, (sockaddr *)&dest_addr, sizeof(sockaddr));
	if (ret == -1)
	{
		perror("connect");
		exit(-1);
	}
}

void SocketDataChannel::readBlock(const uint64_t length, const char *pool_id, const uint64_t block_id,	const uint64_t generation_stamp, const uint64_t offset, const TokenProto &block_token, bool check_crc)
{
	
}
