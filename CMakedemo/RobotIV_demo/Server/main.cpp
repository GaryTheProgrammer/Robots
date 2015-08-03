﻿#include <iostream>
#include <cstring>
#include <iomanip> 
#include <bitset>
#include <cstring>
#include <map>
#include <string>

using namespace std;

#include <stdlib.h>

#include <Aris_Core.h>
#include <Aris_Message.h>
#include <Aris_Control.h>
#include <Robot_Server.h>

using namespace Aris::Core;

#include "trajectory_generator.h"


extern int HEXBOT_HOME_OFFSETS_RESOLVER[18];




int walk(Robots::ROBOT_BASE *pRobot, const Robots::GAIT_PARAM_BASE *pParam)
{
	static double lastPee[18];
	static double lastPbody[6];

	const Robots::WALK_PARAM *pWP=static_cast<const Robots::WALK_PARAM *>(pParam);


	if(pParam->count < pWP->totalCount)
	{
		walkAcc(pRobot,pParam);
	}
	else
	{
		Robots::WALK_PARAM param2=*pWP;
		param2.count=pWP->count - pWP->totalCount;

		memcpy(param2.beginPee,lastPee,sizeof(lastPee));
		memcpy(param2.beginBodyPE,lastPbody,sizeof(lastPbody));

		walkDec(pRobot,&param2);
	}

	if(pParam->count%100==0)
	{
		double pEE[18];
		double pBody[18];
		pRobot->GetPee(pEE,"G");
				pRobot->GetBodyPe(pBody,"313");
	}


	if(pParam->count==pWP->totalCount-1)
	{
		pRobot->GetPee(lastPee,"G");
		pRobot->GetBodyPe(lastPbody,"313");

		double *pEE=lastPee;
		double *pBody=lastPbody;
	}
	return 2* pWP->totalCount - pWP->count-1;
}





int copyClient()
{
	const int TASK_NAME_LEN =1024;

	std::int32_t count = 0;
	std::int32_t nIndex = 0;
	char path[TASK_NAME_LEN] = {0};
	char cParam[100] = {0};
	char *proName = path;
	std::int32_t tmp_len;

	pid_t pId = getpid();
	sprintf(cParam,"/proc/%d/exe",pId);
	count = readlink(cParam, path, TASK_NAME_LEN);

	if (count < 0 || count >= TASK_NAME_LEN)
    {
        throw std::logic_error("Current System Not Surport Proc.\n");
    }
	else
	{
		nIndex = count - 1;

		for( ; nIndex >= 0; nIndex--)
		{
			if( path[nIndex] == '/' )//筛选出进程名
		    {
				nIndex++;
				proName += nIndex;
				break;
		    }
		}
	}

	std::string pwd(path,nIndex);


	Aris::Core::DOCUMENT doc;

	/*get all cmd names*/
	if(doc.LoadFile("/usr/Robots/CMakeDemo/Robot_III/resource/client.xml")!=0)
		throw std::logic_error("failed to read client.xml");

	auto pCmds=doc.FirstChildElement("Commands");

	if(pCmds==nullptr)
		throw std::logic_error("invalid client.xml");

	for (auto pChild = pCmds->FirstChildElement();
		pChild != nullptr;
		pChild = pChild->NextSiblingElement())
	{
		std::string fullpath=std::string("cp ")+pwd+std::string("Client ")+pwd+pChild->Name();

		//cout<<fullpath<<endl;
		system(fullpath.c_str());

	}

	return 0;
};

Aris::Core::MSG parse(const std::string &cmd, const map<std::string, std::string> &params)
{
	Robots::WALK_PARAM  param;
	param.cmdType=RUN_GAIT;
	param.cmdID=1;

	param.motorNum=18;
	int id[18]={0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17};
	std::memcpy(param.motorID,id,sizeof(id));

	param.legNum=6;
	int legid[6]={0,1,2,3,4,5};
	std::memcpy(param.legID,legid,sizeof(legid));


	for(auto &i:params)
	{
		if(i.first=="totalCount")
		{
			param.totalCount=std::stoi(i.second);
		}
		else if(i.first=="n")
		{
			param.n=stoi(i.second);
		}
		else if(i.first=="walkDirection")
		{
			param.walkDirection=stoi(i.second);
		}
		else if(i.first=="upDirection")
		{
			param.upDirection=stoi(i.second);
		}
		else if(i.first=="distance")
		{
			param.d=stod(i.second);
		}
		else if(i.first=="height")
		{
			param.h=stod(i.second);
		}
		else if(i.first=="alpha")
		{
			param.alpha=stod(i.second);
		}
		else if(i.first=="beta")
		{
			param.beta=stod(i.second);
		}
	}

	Aris::Core::MSG msg;

	msg.CopyStruct(param);

	std::cout<<"finished parse"<<std::endl;

	return msg;
}


int main()
{
	copyClient();

	auto rs = Robots::ROBOT_SERVER::GetInstance();
	rs->CreateRobot<Robots::ROBOT_III>("/usr/Robots/resource/HexapodIII/HexapodIII.xml");
	rs->AddGait("wk",walk,parse);
	rs->Start();
	/**/
	std::cout<<"finished"<<std::endl;


	Aris::Core::RunMsgLoop();

	return 0;
}
