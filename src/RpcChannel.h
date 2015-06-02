#pragma once
#include "common.h"
#include <stdint.h>

class RpcChannel
{
    private:
        int m_sockfd;

    public:
        RpcChannel(){}
        ~RpcChannel();
        RpcChannel(const char *ip, const uint16_t port);
        int sendMessage(const void *msg, int msg_len) const;
		int sendMessage(const char *msg) const;
		int sendMessage(const int8_t &msg) const;
        int receiveMessage(void *buf, int buf_size);
		void getConnection();
};
