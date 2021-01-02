#pragma once
#ifndef mirai_cpp_defs_qq_types_hpp_H_
#define mirai_cpp_defs_qq_types_hpp_H_

#include <exception>
#include "serializable.hpp"
#include "mirai/exported.h"

namespace Cyan
{
	// QQ 号码类型
	class QQ_t
	{
	public:
		QQ_t() :QQ(-1) {}
		explicit  QQ_t(int64_t qq) :QQ(qq) {}
		operator int64_t() const { return QQ; }
		int64_t ToInt64() const { return QQ; }
	private:
		int64_t QQ;
	};

	inline QQ_t operator "" _qq(unsigned long long int v)
	{
		return QQ_t(int64_t(v));
	}

	// 群号码类型
	class GID_t
	{
	public:
		GID_t() :GID(-1) {}
		explicit GID_t(int64_t gid) :GID(gid) {}
		operator int64_t() const { return GID; }
		int64_t ToInt64() const { return GID; }
	private:
		int64_t GID;
	};

	inline GID_t operator "" _gid(unsigned long long int v)
	{
		return GID_t(int64_t(v));
	}

	// 消息 ID
	typedef int64_t MessageId_t;

	// 群成员权限
	enum class GroupPermission
	{
		Member,
		Administrator,
		Owner
	};

	inline string GroupPermissionStr(GroupPermission gp)
	{
		string result;
		switch (gp)
		{
		case Cyan::GroupPermission::Member:
			result = "MEMBER";
			break;
		case Cyan::GroupPermission::Administrator:
			result = "ADMINISTRATOR";
			break;
		case Cyan::GroupPermission::Owner:
			result = "OWNER";
			break;
		}
		return result;
	}

	inline GroupPermission GroupPermissionStr(const string& gp)
	{
		if (gp == "MEMBER") return GroupPermission::Member;
		if (gp == "ADMINISTRATOR") return GroupPermission::Administrator;
		if (gp == "OWNER") return GroupPermission::Owner;
		throw std::runtime_error("错误的 GroupPermissionStr");
	}

	struct MiraiImage
	{
		string ID;
		string Url;
		string Path;
	};

	typedef MiraiImage FriendImage;

	typedef MiraiImage GroupImage;

	typedef MiraiImage TempImage;

	struct MiraiVoice
	{
		string Id;
		string Url;
		string Path;
	};

	// 预先声明 MiraiBot 类
	class EXPORTED MiraiBot;

}

#endif // !mirai_cpp_defs_qq_types_hpp_H_