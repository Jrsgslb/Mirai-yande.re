#pragma once
#ifndef mirai_cpp_events_bot_online_event_hpp_H_
#define mirai_cpp_events_bot_online_event_hpp_H_

#include "mirai/third-party/nlohmann/json.hpp"
#include "mirai/defs/qq_types.hpp"
#include "event_interface.hpp"

namespace Cyan
{
	/**
	 * \brief bot 登录成功事件
	 */
	class BotOnlineEvent : public EventBase
	{
	public:
		QQ_t QQ;

		static MiraiEvent GetMiraiEvent()
		{
			return MiraiEvent::BotOnlineEvent;
		}

		virtual bool Set(const json& j) override
		{
			this->QQ = (QQ_t)(j["qq"].get<int64_t>());
			return true;
		}
		virtual json ToJson() const override
		{
			json j = json::object();
			j["type"] = "BotOnlineEvent";
			j["qq"] = (int64_t)this->QQ;
			return j;
		}

	};

}

#endif // !mirai_cpp_events_bot_online_event_hpp_H_