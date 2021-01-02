#include <iostream>
// 使用静态库必须要在引入 mirai.h 前定义这个宏
#define MIRAICPP_STATICLIB
#include <mirai.h>

int main()
{
	using namespace std;
	using namespace Cyan;
	system("chcp 65001");
	MiraiBot bot("127.0.0.1", 539);
	while (true)
	{
		try
		{
			bot.Auth("INITKEY7A3O1a9v", 1589588851_qq);
			break;
		}
		catch (const std::exception& ex)
		{
			cout << ex.what() << endl;
		}
		MiraiBot::SleepSeconds(1);
	}
	cout << "成功登录 bot。" << endl;

	bot.On<MemberJoinRequestEvent>(
		[&](MemberJoinRequestEvent newMember)
		{
			cout << newMember.Message << endl;
			newMember.Accept();
		});

	bot.On<MemberJoinEvent>(
		[&](MemberJoinEvent e)
		{
			string memberName = e.NewMember.MemberName;
			cout << memberName << endl;
			bot.SendMessage(e.NewMember.Group.GID,
				MessageChain().Plain("欢迎 " + memberName + " 加入本群!"));
		});




	bot.EventLoop();

	return 0;
}