#pragma once
#ifndef mirai_cpp_events_member_leave_quit_event_hpp_H_
#define mirai_cpp_events_member_leave_quit_event_hpp_H_

#include "mirai/third-party/nlohmann/json.hpp"
#include "mirai/defs/group_member.hpp"
#include "event_interface.hpp"

namespace Cyan
{
	/**
	 * \brief 群成员主动离群事件
	 */
	class MemberLeaveEventQuit : public EventBase
	{
	public:
		GroupMember_t Member;

		static MiraiEvent GetMiraiEvent()
		{
			return MiraiEvent::MemberLeaveEventQuit;
		}

		virtual bool Set(const json& j) override
		{
			this->Member.Set(j["member"]);
			return true;
		}
		virtual json ToJson() const override
		{
			json j = json::object();
			j["type"] = "MemberLeaveEventQuit";
			j["member"] = this->Member.ToJson();
			return j;
		}
	};

}

#endif // !mirai_cpp_events_member_leave_quit_event_hpp_H_