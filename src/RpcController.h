#pragma once

#include <string>
#include "google/protobuf/service.h"

namespace native
{
	namespace libhdfs
	{
		class RpcController : public google::protobuf::RpcController
		{
			public:
				~RpcController(){}
				RpcController(){m_status = 0;}
				int status(){return m_status;}
				void set_status(int status) {m_status = status;	}
				
				virtual bool Failed() const;
				virtual void Reset();
				virtual std::string ErrorText() const;
				virtual void StartCancel();
				virtual void SetFailed(const std::string &text);
				virtual bool IsCanceled() const;
				virtual void NotifyOnCancel(google::protobuf::Closure *callback);

			private:
				int m_status;
		};
	}
}
