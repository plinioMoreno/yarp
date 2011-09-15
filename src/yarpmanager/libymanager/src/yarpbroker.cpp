/*
 *  Yarp Modules Manager
 *  Copyright: 2011 (C) Robotics, Brain and Cognitive Sciences - Italian Institute of Technology (IIT)
 *  Authors: Ali Paikan <ali.paikan@iit.it>
 * 
 *  Copy Policy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#include "yarpbroker.h"

#include <signal.h>
#include <string.h>

#define YARPRUN_ERROR 		-1
#define RUN_TIMEOUT 		3 		//times of 100ms + running() delay = 3s
#define STOP_TIMEOUT 		10
#define KILL_TIMEOUT 		3

using namespace yarp::os;


YarpBroker::YarpBroker()
{
	bOnlyConnector = bInitialized = false;	
	//yarp.setVerbosity(-1);
}


YarpBroker::~YarpBroker()
{
}


bool YarpBroker::init(void)
{
	if(bInitialized)
		return true; 

	if(!NetworkBase::checkNetwork())
	{
		strError = "Yarp network server is not up.";
		return false;
	}
	bInitialized = true;
	bOnlyConnector = true;
	return true;
}

bool YarpBroker::init(const char* szcmd, const char* szparam,
			const char* szhost, const char* szstdio,
			const char* szworkdir, const char* szenv )
{
	if(bInitialized)
		return true; 

	if(!NetworkBase::checkNetwork())
	{
		strError = "Yarp network server is not up.";
		return false;
	}
	
	if(!szcmd)
	{
		strError = "command is not specified.";
		return false;
	}

	if(!szhost)
	{
		strError = "remote host port is not specified.";
		return false;
	}

	if(szhost[0] != '/')
		strHost = string("/") + string(szhost);
	else
		strHost = szhost;

	if(!exists(strHost.c_str()))
	{
		strError = szhost;
		strError += " dose not exist. check yarprun is running as server.";
		return false;		
	}
	
	strCmd = szcmd;	
	if(strlen(szparam))
		strParam = szparam;
	if(strlen(szworkdir))
		strWorkdir = szworkdir;

	if(strlen(szstdio))
	{
		if(szstdio[0] != '/')
			strStdio = string("/") + string(szstdio);
		else
			strStdio = szstdio;
	}

	if(strlen(szenv))
		strEnv = szenv;
	
	ostringstream sstrID;
	sstrID<<generateID();
	strTag = strHost + strCmd + sstrID.str();

	//cout<<strCmd<<" has tag "<<strTag<<endl;

	bInitialized = true;			
	return true;
}


bool YarpBroker::run()
{	
	if(!bInitialized) return false;
	if(bOnlyConnector) return false;
	
	strError.clear();
	int ret = requestServer(runProperty());
	if( ret == -1)
	{
		strError = "cannot ask ";
		strError += strHost;
		strError += " to run ";
		strError += strCmd;
		return false;
	}
	
	int w = 0;
	while(w++ < RUN_TIMEOUT)
	{
		if(running())
			return true;
		Time::delay(0.1); 
	}

	strError = "cannot run ";
	strError += strCmd;
	strError += " on ";
	strError += strHost;	
	return false;
}

bool YarpBroker::stop()
{
	if(!bInitialized) return true;
	if(bOnlyConnector) return false;
	
	strError.clear();
	yarp::os::Bottle msg,grp,response;
	
	grp.clear();
	grp.addString("on");
	grp.addString(strHost.c_str());
	msg.addList()=grp;
	grp.clear();
	grp.addString("sigterm");
	grp.addString(strTag.c_str());
	msg.addList()=grp;
    response=SendMsg(msg, strHost.c_str());
	int ret = response.get(0).asString()=="sigterm OK"?0:YARPRUN_ERROR;
	
	if( ret == -1)
	{
		strError = "cannot ask ";
		strError += strHost;
		strError += " to stop ";
		strError += strCmd;
		return false;
	}
	
	int w = 0;
	while(w++ < STOP_TIMEOUT)
	{
		if(!running())
			return true;
		Time::delay(0.1); 
	}

	strError = "Timeout! cannot stop ";
	strError += strCmd;
	strError += " on ";
	strError += strHost;
	return false;
}

bool YarpBroker::kill()
{
	if(!bInitialized) return true;
	if(bOnlyConnector) return false;
	
	strError.clear();
	
	yarp::os::Bottle msg,grp,response;	
	grp.clear();
	grp.addString("on");
	grp.addString(strHost.c_str());
	msg.addList() = grp;
	grp.clear();
	grp.addString("kill");
	grp.addString(strTag.c_str());
	grp.addInt(SIGKILL);
	msg.addList() = grp;
    response = SendMsg(msg, strHost.c_str());
	int ret = response.get(0).asString()=="kill OK"?0:YARPRUN_ERROR;
	if( ret == -1)
	{
		strError = "cannot ask ";
		strError += strHost;
		strError += " to kill ";
		strError += strCmd;
		return false;
	}
	
	int w = 0;
	while(w++ < KILL_TIMEOUT)
	{
		if(!running())
			return true;
		Time::delay(0.1); 
	}

	strError = "cannot kill ";
	strError += strCmd;
	strError += " on ";
	strError += strHost;
	return false;	
}


bool YarpBroker::running(void)
{
	if(!bInitialized) return false;
	if(bOnlyConnector) return false;
	
	strError.clear();
	yarp::os::Bottle msg,grp,response;
	
	grp.clear();
	grp.addString("on");
	grp.addString(strHost.c_str());
	msg.addList()=grp;

	grp.clear();
	grp.addString("isrunning");
	grp.addString(strTag.c_str());
	msg.addList()=grp;
	
    response = SendMsg(msg, strHost.c_str());	
	if (!response.size()) 
		return false;
	return response.get(0).asString()=="running";
}


Property& YarpBroker::runProperty(void)
{
	command.clear();
	string cmd = strCmd + string(" ") + strParam;
	command.put("cmd", cmd.c_str());
	command.put("on", strHost.c_str());
	command.put("as", strTag.c_str());
	if(!strWorkdir.empty())
		command.put("workdir", strWorkdir.c_str());
	if(!strStdio.empty())
		command.put("stdio", strStdio.c_str());
	if(!strEnv.empty())
		command.put("env", strEnv.c_str());
	return command;
}


/**
 *  connecttion broker
 */ 
bool YarpBroker::connect(const char* from, const char* to, 
			const char* carrier)
{
	
	if(!from)
	{
		strError = "no source port is introduced.";
		return false;
	}

	if(!to)
	{
		strError = "no destination port is introduced.";
		return false;
	}

	if(!exists(from))
	{
		strError = from;
		strError += " does not exist.";
		return false;
	}

	if(!exists(to))
	{
		strError = to;
		strError += " does not exist.";
		return false;
	}
		
	if(connected(from, to))
		return true;
		
	if(!NetworkBase::connect(from, to, carrier))
	{
		strError = "cannot connect ";
		strError +=from;
		strError += " to " + string(to);
		return false;
	}
	return true;
}

bool YarpBroker::disconnect(const char* from, const char* to)
{
	
	if(!from)
	{
		strError = "no source port is introduced.";
		return false;
	}

	if(!to)
	{
		strError = "no destination port is introduced.";
		return false;
	}

	if(!exists(from))
	{
		strError = from;
		strError += " does not exist.";
		return false;
	}

	if(!exists(to))
	{
		strError = to;
		strError += " does not exist.";
		return false;
	}
	
	if(!connected(from, to))
		return true;

	if( !NetworkBase::disconnect(from, to))
	{
		strError = "cannot disconnect ";
		strError +=from;
		strError += " from " + string(to);		
		return false;
	}
	return true;
	
}

bool YarpBroker::exists(const char* port)
{
	return NetworkBase::exists(port);
}

bool YarpBroker::connected(const char* from, const char* to)
{
	return NetworkBase::isConnected(from, to);
}


const char* YarpBroker::error(void)
{
	return strError.c_str();
}


/**
 * 
 *  mimicing yarprun
 */ 
Bottle YarpBroker::SendMsg(Bottle& msg,ConstString target)
{
	Port port;
    port.open("...");
    for (int i=0; i<20; ++i)
	{
	    if (NetworkBase::connect(port.getName().c_str(),target.c_str())) break;
	    Time::delay(1.0);
	}
	Bottle response;
    port.write(msg,response);
    NetworkBase::disconnect(port.getName().c_str(),target.c_str());
    port.close();
	return response;
}


int YarpBroker::requestServer(Property& config)
{
	yarp::os::Bottle msg;

	// USE A YARP RUN SERVER TO MANAGE STDIO
	//
	// client -> stdio server -> cmd server
	//
	if (config.check("cmd") && config.check("stdio"))
	{
		if (config.find("stdio")=="") {return YARPRUN_ERROR; }
		if (config.find("cmd")=="")   {return YARPRUN_ERROR; }
		if (!config.check("as") || config.find("as")=="") { return YARPRUN_ERROR; }
		if (!config.check("on") || config.find("on")=="") { return YARPRUN_ERROR; }

		msg.addList()=config.findGroup("stdio");
		msg.addList()=config.findGroup("cmd");
		msg.addList()=config.findGroup("as");
		msg.addList()=config.findGroup("on");
		
		if (config.check("workdir")) msg.addList()=config.findGroup("workdir");
		if (config.check("geometry")) msg.addList()=config.findGroup("geometry");
		if (config.check("hold")) msg.addList()=config.findGroup("hold");
		if (config.check("env")) msg.addList()=config.findGroup("env");

		Bottle response=SendMsg(msg,config.find("stdio").asString());
		if (!response.size()) return YARPRUN_ERROR;
		return response.get(0).asInt()>0?1:0;
	}
	
	// DON'T USE A RUN SERVER TO MANAGE STDIO
	//
	// client -> cmd server
	//
	if (config.check("cmd"))
	{                
		if (config.find("cmd").asString()=="")   { return YARPRUN_ERROR; }
		if (!config.check("as") || config.find("as").asString()=="") {return YARPRUN_ERROR; }
		if (!config.check("on") || config.find("on").asString()=="") {return YARPRUN_ERROR; }

		msg.addList()=config.findGroup("cmd");
		msg.addList()=config.findGroup("as");

		if (config.check("workdir")) msg.addList()=config.findGroup("workdir");
		if (config.check("env")) msg.addList()=config.findGroup("env");

		Bottle response=SendMsg(msg,config.find("on").asString());
		if (!response.size()) return YARPRUN_ERROR;
		return response.get(0).asInt()>0?1:0;
	}
	return 0;
}
