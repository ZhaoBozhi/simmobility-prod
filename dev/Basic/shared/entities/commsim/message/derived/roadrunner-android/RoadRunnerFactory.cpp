//Copyright (c) 2013 Singapore-MIT Alliance for Research and Technology
//Licensed under the terms of the MIT License, as described in the file:
//   license.txt   (http://opensource.org/licenses/MIT)

#include "RoadRunnerFactory.hpp"
#include "logging/Log.hpp"
#include "entities/commsim/message/derived/roadrunner-android/RemoteLogMessage.hpp"
#include "entities/commsim/message/derived/roadrunner-android/RerouteRequestMessage.hpp"
#include "entities/commsim/message/derived/roadrunner-android/RemoteLogHandler.hpp"
#include "entities/commsim/message/derived/roadrunner-android/RerouteRequestHandler.hpp"

using namespace sim_mob;


sim_mob::roadrunner::RoadRunnerFactory::RoadRunnerFactory(bool useNs3) : useNs3(useNs3)
{
	//Doing it manually; C++1 doesn't like the boost assignment.
	MessageMap.clear();
	MessageMap["MULTICAST"] = MULTICAST;
	MessageMap["UNICAST"] = UNICAST;
	MessageMap["CLIENT_MESSAGES_DONE"] = CLIENT_MESSAGES_DONE;
	MessageMap["REMOTE_LOG"] = REMOTE_LOG;
	MessageMap["REROUTE_REQUEST"] = REROUTE_REQUEST;

	//MessageMap = boost::assign::map_list_of("MULTICAST", MULTICAST)("UNICAST", UNICAST)("CLIENT_MESSAGES_DONE",CLIENT_MESSAGES_DONE)/*("ANNOUNCE",ANNOUNCE)("KEY_REQUEST", KEY_REQUEST)("KEY_SEND",KEY_SEND)*/;
}

sim_mob::roadrunner::RoadRunnerFactory::~RoadRunnerFactory()
{}



boost::shared_ptr<sim_mob::Handler>  sim_mob::roadrunner::RoadRunnerFactory::getHandler(MessageType type)
{
	boost::shared_ptr<sim_mob::Handler> handler;
	//if handler is already registered && the registered handler is not null
	typename std::map<MessageType, boost::shared_ptr<sim_mob::Handler> >::iterator it = HandlerMap.find(type);
	if((it != HandlerMap.end())&&((*it).second!= 0))
	{
		//get the handler ...
		handler = (*it).second;
	}
	else
	{
		//else, create a cache entry ...
		bool typeFound = true;
		switch(type)
		{
		case MULTICAST:
			handler.reset(new sim_mob::roadrunner::MulticastHandler(useNs3));
			break;
		case UNICAST:
			handler.reset(new sim_mob::roadrunner::UnicastHandler(useNs3));
			break;
		case REMOTE_LOG:
			handler.reset(new sim_mob::roadrunner::RemoteLogHandler());
			break;
		case REROUTE_REQUEST:
			handler.reset(new sim_mob::roadrunner::RerouteRequestHandler());
			break;
		default:
			typeFound = false;
			break;
		}
		//register this baby
		if(typeFound)
		{
			HandlerMap[type] = handler;
		}
	}

	return handler;
}


void sim_mob::roadrunner::RoadRunnerFactory::createMessage(const std::string &input, std::vector<sim_mob::comm::MsgPtr>& output)
{
	Json::Value root;
	sim_mob::pckt_header packetHeader;
	if(!sim_mob::JsonParser::parsePacketHeader(input, packetHeader, root))
	{
		return;
	}
	if(!sim_mob::JsonParser::getPacketMessages(input,root))
	{
		return;
	}
	for (int index = 0; index < root.size(); index++) {
		msg_header messageHeader;
		if (!sim_mob::JsonParser::parseMessageHeader(root[index], messageHeader)) {
			continue;
		}

		//Convert the message type:
		std::map<std::string, RoadRunnerFactory::MessageType>::const_iterator it = MessageMap.find(messageHeader.msg_type);
		if (it==MessageMap.end()) {
			Warn() <<"RoadRunnerFactory::createMessage() - Unknown message type: " <<messageHeader.msg_type <<"\n";
			continue;
		}

		const Json::Value& curr_json = root[index];
		switch (it->second) {
		case MULTICAST:{
			//create a message
			sim_mob::comm::MsgPtr msg(new MulticastMessage(curr_json, useNs3));
			//... and then assign the handler pointer to message's member
			msg->setHandler(getHandler(MULTICAST));
			output.push_back(msg);
			break;
		}
		case UNICAST:{
			//create a message
			sim_mob::comm::MsgPtr msg(new UnicastMessage(curr_json, useNs3));
			//... and then assign the handler pointer to message's member
			msg->setHandler(getHandler(UNICAST));
			output.push_back(msg);
			break;
		}
		case CLIENT_MESSAGES_DONE:{
			//create a message
			sim_mob::comm::MsgPtr msg(new ClientDoneMessage(curr_json));
			//... and then assign the handler pointer to message's member
//			msg->setHandler(getHandler()); no handler!
			output.push_back(msg);
			break;
		}
		case REMOTE_LOG: {
			sim_mob::comm::MsgPtr msg(new sim_mob::roadrunner::RemoteLogMessage(curr_json));
			msg->setHandler(getHandler(REMOTE_LOG));
			output.push_back(msg);
			break;
		}
		case REROUTE_REQUEST: {
			sim_mob::comm::MsgPtr msg(new sim_mob::roadrunner::RerouteRequestMessage(curr_json));
			msg->setHandler(getHandler(REROUTE_REQUEST));
			output.push_back(msg);
			break;
		}

		default:
			Warn() <<"RoadRunnerFactory::createMessage() - Unhandled message type: " <<messageHeader.msg_type <<"\n";
			break;
		}
	}		//for loop
}

