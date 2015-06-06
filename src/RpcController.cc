#include "RpcController.h"

using namespace native::libhdfs;

bool RpcController::Failed() const 
{
	return 0;
}


void RpcController::Reset()
{

}

std::string RpcController::ErrorText() const
{
	return "";
}

void RpcController::StartCancel()
{

}

void RpcController::SetFailed(const std::string &text)
{

}

bool RpcController::IsCanceled() const
{
	return 0;
}

void RpcController::NotifyOnCancel(google::protobuf::Closure *callback)
{

}
