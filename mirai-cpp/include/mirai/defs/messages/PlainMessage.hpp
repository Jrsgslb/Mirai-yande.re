#pragma once
#ifndef mirai_cpp_defs_messages_plain_message_hpp_H_
#define mirai_cpp_defs_messages_plain_message_hpp_H_

#include "mirai/defs/message_interface.hpp"

namespace Cyan
{
	class PlainMessage : public IMessage
	{
	public:
		PlainMessage() : text_() {}
		PlainMessage(const string& text) : text_(text) {}
		virtual const string& GetType() const override
		{
			return type_;
		}
		virtual bool operator==(const IMessage& m) const override
		{
			if (auto m_ptr = dynamic_cast<const PlainMessage*>(&m))
			{
				return m_ptr->text_ == this->text_;
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
			text_ = json["text"].get<string>();
			return true;
		}
		virtual json ToJson() const override
		{
			return
			{
				{ "type", type_ },
				{ "text", text_ }
			};
		}
		virtual ~PlainMessage() {}

		const string& Text() const { return text_; }
		void Text(const string& text) { this->text_ = text; }

	private:
		string type_ = "Plain";
		string text_;
	};

}
#endif

