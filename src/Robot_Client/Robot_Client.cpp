#include <Platform.h>

#include <iostream>
#include <list>
#include <vector>
#include <map>
#include <cstring>
#include <string>
#include <memory>
#include <algorithm>
#include <cstdint>

#include <Aris_Socket.h>
#include <Aris_XML.h>
#include <Aris_Core.h>

using namespace std;

namespace Robots
{
	int SendRequest(int argc, char *argv[], const char *xmlFileName)
	{
		Aris::Core::MSG msg;
		/*����msg��������Ҫ��copy�������ƣ�Ȼ������copy��������*/

		/*copy�������ƣ���Ҫȥ��·��������չ��*/
		std::string cmdName(argv[0]);
		
#ifdef PLATFORM_IS_WINDOWS
		if (cmdName.rfind('\\'))
		{
			cmdName = cmdName.substr(cmdName.rfind('\\') + 1, cmdName.npos);
		}
#endif
#ifdef PLATFORM_IS_LINUX
		if (cmdName.rfind('/'))
		{
			cmdName = cmdName.substr(cmdName.rfind('/') + 1, cmdName.npos);
		}
#endif
		
		if (cmdName.rfind('.'))
		{
			cmdName = cmdName.substr(0, cmdName.rfind('.'));
		}
		msg.CopyMore(cmdName.c_str(), cmdName.length()+1);
		
		/*copy�������*/
		for (int i = 1; i < argc; ++i)
		{
			msg.CopyMore(argv[i], std::strlen(argv[i])+1);
		}

		/*β�����һ��������*/
		char endChar = '\0';
		msg.CopyMore(&endChar, 1);

		/*���Ӳ�����msg*/
		Aris::Core::DOCUMENT doc;

		if (doc.LoadFile(xmlFileName) != 0)
			throw std::logic_error("failed to read configuration xml file");

		std::string ip = doc.RootElement()->FirstChildElement("Server")->FirstChildElement("Connection")->Attribute("IP");
		std::string port = doc.RootElement()->FirstChildElement("Server")->FirstChildElement("Connection")->Attribute("Port");

		Aris::Core::CONN conn;

		conn.Connect(ip.c_str(), port.c_str());
		Aris::Core::MSG ret = conn.SendRequest(msg);

		/*������*/
		if (ret.GetLength() > 0)
		{
			cout << "cmd has fault, please regard to following information:" << endl;
			cout << "    " << ret.GetDataAddress();
		}
		else
		{
			cout << "send command successful";
		}

		return 0;
	}
}

