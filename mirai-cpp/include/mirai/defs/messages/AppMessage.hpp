#pragma once
#ifndef mirai_cpp_defs_messages_app_message_hpp_H_
#define mirai_cpp_defs_messages_app_message_hpp_H_

#include "mirai/defs/message_interface.hpp"

namespace Cyan
{
	class AppMessage : public IMessage
	{
	public:
		AppMessage() : content_() {}
		AppMessage(const string& content) : content_(content) {}
		virtual const string& GetType() const override
		{
			return type_;
		}
		virtual bool operator==(const IMessage& m) const override
		{
			if (auto m_ptr = dynamic_cast<const AppMessage*>(&m))
			{
				return m_ptr->content_ == this->content_;
			}
			return false;
		}
		virtual bool operator!=(const IMessage& m) const override
		{
			return !(*this == m);
		}
		virtual bool Set(const json& json) override
		{
			if (json["type"].is_null() || json["type"].get<string>() != this->GetType())
				throw std::runtime_error("给定的json不正确");
			content_ = json["content"].get<string>();
			return true;
		}
		virtual json ToJson() const override
		{
			return
			{
				{ "type", type_ },
				{ "content", content_ }
			};
		}
		virtual ~AppMessage() {}

		const string& Content() const { return content_; }
		void Content(const string& content) { this->content_ = content; }

	private:
		string type_ = "App";
		string content_;
	};

}
#endif

