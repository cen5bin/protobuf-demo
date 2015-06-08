#include "SocketDataChannel.h"
#include "common.h"
#include <cstring>
#include <cstdlib>
#include "datatransfer.pb.h"

#define INFO
#include "Log.h"

using namespace native::libhdfs;
using namespace hadoop::hdfs;
using namespace hadoop::common;

static const uint32_t BUFFER_SIZE = 1024;
static const uint32_t BLOCK_SIZE = 64 * 1024 * 1024;

//op codes
static const int8_t WRITE_BLOCK = 80;
static const int8_t READ_BLOCK = 81;
static const int8_t READ_METADATA = 82;
static const int8_t REPLACE_BLCOK = 83;
static const int8_t COPY_BLOCK = 84;
static const int8_t BLOCK_CHECKSUM = 85;
static const int8_t TRANSFER_BLOCK = 86;



SocketDataChannel::~SocketDataChannel()
{
	close(m_sockfd);
}

SocketDataChannel::SocketDataChannel(const char *host, const uint32_t port)
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
	static uint8_t buf[BUFFER_SIZE];
	static uint8_t block_buf[BLOCK_SIZE];
	this->sendMessage((int16_t)htons(28));
	this->sendMessage(READ_BLOCK);

	OpReadBlockProto request;
	request.set_offset(offset);
	request.set_len(length - offset);

	ClientOperationHeaderProto *header = request.mutable_header();
	header->set_clientname("libhdfs");
	BaseHeaderProto *baseheader = header->mutable_baseheader();

	//设置tokenproto
	TokenProto *token = baseheader->mutable_token();
	token->set_identifier(block_token.identifier());
	token->set_password(block_token.password());
	token->set_kind(block_token.kind());
	token->set_service(block_token.service());

	//设置extendedblockproto
	ExtendedBlockProto *block = baseheader->mutable_block();
	block->set_poolid(pool_id);
	block->set_blockid(block_id);
	block->set_generationstamp(generation_stamp);

	this->sendProtobufMessageWithLength(&request);

	int ll = this->receiveMessage(buf, BUFFER_SIZE);
	_D("%d", ll);
	uint32_t len;
	const uint8_t *ptr = readVarint32FromArray(buf, &len);
	_D("%u", len);
	BlockOpResponseProto block_op_response;
	block_op_response.ParseFromArray(ptr, len);
	ChecksumTypeProto checksum_type = block_op_response.readopchecksuminfo().checksum().type();
	int checksum_len;
	if (checksum_type == CHECKSUM_CRC32 || checksum_type == CHECKSUM_CRC32C)
		checksum_len = 4;
	else 
	{
		_E("checksum type not implemented");
		exit(-1);
	}

	uint32_t bytes_per_chunk = block_op_response.readopchecksuminfo().checksum().bytesperchecksum();
	_D("bytesperchunk %d", bytes_per_chunk);
	uint64_t tol_read = 0;
	if (block_op_response.status() == 0)
	{
		_D("tol_read %llu, length %llu", tol_read, length);
		uint8_t bbb[1000000];
		while (tol_read < length)
		{
			uint32_t packet_len;
			this->receiveMessage((void *)&packet_len, sizeof(packet_len));
			packet_len = ntohl(packet_len);
			_D("packet_len %d", packet_len);
			uint16_t head_size;
			this->receiveMessage((void *)&head_size, sizeof(head_size));
			head_size = ntohs(head_size);
			this->receiveMessage(buf, head_size);
			PacketHeaderProto packet_header;
			packet_header.ParseFromArray(buf, head_size);
			int32_t data_len = packet_header.datalen();
			int chunks_per_packet = (data_len + bytes_per_chunk - 1) / bytes_per_chunk;
			_D("chunks_per_packet %d", chunks_per_packet);
			data_len = data_len - 4 - checksum_len * chunks_per_packet;

			if (check_crc)
			{

			}
			else 
			{
				uint32_t checksum_tol = checksum_len * chunks_per_packet;
				while (checksum_tol > 0)
				{
					uint32_t len = this->receiveMessage(buf, BUFFER_SIZE);
					checksum_tol -= len;
				}
			}

			int32_t read_len = 0;
			while (read_len < data_len)
			{
				int32_t len = this->receiveMessage(bbb + tol_read, BUFFER_SIZE);
				read_len += len;
				tol_read += len;
			}
		}
		bbb[length] = '\0';
		puts((char *)bbb);
	}

	_I("total read %llu", tol_read);

	ClientReadStatusProto rs;
	rs.set_status((Status)0);
	this->sendProtobufMessageWithLength(&rs);
}

void SocketDataChannel::writeBlock(const LocatedBlockProto &block, const uint64_t length, const uint64_t offset, const TokenProto &block_token, const uint64_t latestgenerationstamp, const uint64_t minBytesRcvd, const uint64_t maxBytesRcv, const uint32_t pipelinesize, int stage)
{
	static uint8_t buf[BUFFER_SIZE];
	this->sendMessage((int16_t)htons(28));
	this->sendMessage(WRITE_BLOCK);

	OpWriteBlockProto request;
	request.set_latestgenerationstamp(latestgenerationstamp);
	request.set_minbytesrcvd(minBytesRcvd);
	request.set_maxbytesrcvd(maxBytesRcv);
	request.set_pipelinesize(pipelinesize);
	request.set_stage((OpWriteBlockProto_BlockConstructionStage)stage);

	ClientOperationHeaderProto *header = request.mutable_header();
	header->set_clientname("libhdfs");
	BaseHeaderProto *baseheader = header->mutable_baseheader();
	ExtendedBlockProto *eb = baseheader->mutable_block();
	eb->set_blockid(block.b().blockid());
	eb->set_generationstamp(block.b().generationstamp());
	eb->set_numbytes(block.b().numbytes());
	eb->set_poolid(block.b().poolid());
	TokenProto *token = baseheader->mutable_token();
	token->set_identifier(block_token.identifier());
	token->set_password(block_token.password());
	token->set_kind(block_token.kind());
	token->set_service(block_token.service());

	ChecksumProto *cs = request.mutable_requestedchecksum();
	cs->set_type(CHECKSUM_CRC32);
	cs->set_bytesperchecksum(4);

	for (int i = 0; i < block.locs_size(); ++i)
	{
		DatanodeInfoProto location = block.locs(i);
		DatanodeInfoProto *ll = request.add_targets();
		DatanodeIDProto *id = ll->mutable_id();
		id->set_hostname(location.id().hostname());
		id->set_infoport(location.id().infoport());
		id->set_ipaddr(location.id().ipaddr());
		id->set_ipcport(location.id().ipcport());
		id->set_datanodeuuid(location.id().datanodeuuid());
		id->set_xferport(location.id().xferport());
		ll->set_location(location.location());
	}

	this->sendProtobufMessageWithLength(&request);
}
