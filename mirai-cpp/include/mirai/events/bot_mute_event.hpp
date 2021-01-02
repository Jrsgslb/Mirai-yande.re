#pragma once
#ifndef mirai_cpp_events_bot_mute_event_hpp_H_
#define mirai_cpp_events_bot_mute_event_hpp_H_

#include "mirai/third-party/nlohmann/json.hpp"
#include "mirai/defs/group_member.hpp"
#include "event_interface.hpp"

namespace Cyan
{

	/**
	 * \brief bot 被禁言事件
	 */
	class BotMuteEvent : public EventBase
	{
	public:
		int DurationSeconds = 0;
		GroupMember_t Operator;

		static MiraiEvent GetMiraiEvent()
		{
			return MiraiEvent::BotMuteEvent;
		}

		virtual bool Set(const json& j) override
		{
			this->DurationSeconds = j["durationSeconds"].get<int>();
			this->Operator.Set(j["operator"]);
			return true;
		}
		virtual json ToJson() const override
		{
			json j = json::object();
			j["type"] = "BotMuteEvent";
			j["durationSeconds"] = this->DurationSeconds;
			j["operator"] = this->Operator.ToJson();
			return j;
		}

	};

}

#endif // !mirai_cpp_events_bot_mute_event_hpp_H_