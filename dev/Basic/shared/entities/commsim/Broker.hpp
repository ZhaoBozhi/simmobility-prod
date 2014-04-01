//Copyright (c) 2013 Singapore-MIT Alliance for Research and Technology
//Licensed under the terms of the MIT License, as described in the file:
//   license.txt   (http://opensource.org/licenses/MIT)

#pragma once

#include <list>

#include <boost/thread/condition_variable.hpp>
#include <boost/unordered_map.hpp>

#include "conf/ConfigParams.hpp"

#include "entities/Agent.hpp"
#include "entities/commsim/client/ClientType.hpp"
#include "entities/commsim/client/base/ClientRegistration.hpp"
#include "entities/commsim/service/Services.hpp"
#include "entities/commsim/broker/base/Broker-util.hpp"
#include "entities/commsim/broker/base/Common.hpp"
#include "entities/commsim/message/Handlers.hpp"
#include "entities/commsim/message/ThreadSafeQueue.hpp"
#include "entities/commsim/serialization/CommsimSerializer.hpp"
#include "entities/commsim/wait/WaitForAgentRegistration.hpp"
#include "entities/commsim/wait/WaitForAndroidConnection.hpp"
#include "entities/commsim/wait/WaitForNS3Connection.hpp"
#include "entities/commsim/connection/ConnectionServer.hpp"

#include "util/OneTimeFlag.hpp"
#include "workers/Worker.hpp"

#include "event/SystemEvents.hpp"
#include "message/MessageBus.hpp"
#include "event/EventPublisher.hpp"

namespace sim_mob {

/**
 * This file contains Broker class as the main component in this file.
 * The rest of the structures are custom data types used in the broker class.
 * Find the documentation at the beginning of each structure/class.
 */

//Forward Declarations
template <class RET,class MSG>
class MessageFactory;

template<class T>
class Message;

class AgentCommUtilityBase;
class Publisher;
class ConnectionHandler;
class ConnectionServer;
class ClientHandler;
class BrokerBlocker;


namespace {
//TEMPORARY: I just need an easy way to disable output for now. This is *not* the ideal solution.
const bool EnableDebugOutput = false;
} //End unnamed namespace



///BrokerPublisher class. No documentation provided.
class BrokerPublisher : public sim_mob::event::EventPublisher {
public:
	virtual ~BrokerPublisher() {}
};

///ClientRegistrationPublisher class. No documentation provided.
class ClientRegistrationPublisher : public sim_mob::event::EventPublisher {
public:
	virtual ~ClientRegistrationPublisher() {}
};


 //since we have not created the original key/values, we wont use shared_ptr to avoid crashing
struct MessageElement{
	MessageElement(){}
	MessageElement(boost::shared_ptr<sim_mob::ConnectionHandler> cnnHandler, const MessageConglomerate& conglom) :
		cnnHandler(cnnHandler), conglom(conglom){}

	boost::shared_ptr<sim_mob::ConnectionHandler> cnnHandler;
	MessageConglomerate conglom;
};


/**
 * A typedef-container for our MessageFactories container type.
 */
/*struct MessageFactories {
	typedef unsigned int Key;
	typedef boost::shared_ptr<MessageFactory<std::vector<sim_mob::comm::MsgPtr>, std::string> > Value;

	typedef std::map<Key, Value> Type;
	typedef std::pair<Key, Value> Pair;
};*/


/**
 * A typedef-container for our PublisherList container type.
 */
struct PublisherList {
	typedef sim_mob::Services::SIM_MOB_SERVICE Key;
	typedef boost::shared_ptr<sim_mob::event::EventPublisher> Value;

	typedef std::map<Key, Value> Type;
	typedef std::pair<Key, Value> Pair;
};


/**
 * A typedef-container for our ClientList container type.
 */
struct ClientList {
	typedef sim_mob::comm::ClientType Key;
	typedef boost::unordered_map< std::string , boost::shared_ptr<sim_mob::ClientHandler> > Value;

	typedef std::map<Key, Value> Type;
	typedef std::pair<Key, Value> Pair;
	typedef std::pair<std::string , boost::shared_ptr<sim_mob::ClientHandler> > ValuePair;

};


/**
 * A typedef-container for our BrokerBlockers container type.
 * [Blocker type: simmobility agents, android emulator, ns3 simulator  etc]
 */
struct BrokerBlockers {
	typedef unsigned int Key;
	typedef boost::shared_ptr<sim_mob::BrokerBlocker> Value;

	typedef std::map<Key, Value> Type;
	typedef std::pair<Key, Value> Pair;
};


///Helper typedef: key for our SendBuffer
struct SendBuffer {
	typedef boost::shared_ptr<sim_mob::ClientHandler> Key;
};




///Isolates external Broker functionality.
class BrokerBase {
public:
	virtual ~BrokerBase() {}

	//Used by: TODO
	virtual void onMessageReceived(boost::shared_ptr<ConnectionHandler>cnnHadler, const BundleHeader& header, std::string message) = 0;

	//Used by the WhoAreYouProtocol when clients connect.
	virtual void insertIntoWaitingOnWHOAMI(const std::string& token, boost::shared_ptr<sim_mob::ConnectionHandler> newConn) = 0;

	//Used by the BrokerBlocker subclasses. Hopefully we can further abstract these.
	virtual size_t getRegisteredAgentsSize() const = 0;
	virtual const ClientList::Type & getClientList() const = 0;
};



/**
 * This class is the heart of communication simulator
 * It is responsible for configuring and managing communication simulator.
 * the update method of broker performs the following:
 * - managing the synchronization(with internal and external entities)
 * - Processing the incoming requests for registration(internal and external)
 * - Processing the incoming/outgoing messages
 * - clean up
 * - other application specific settings
 * It is advisable to subclass from broker to make customized configuration/implementation
 */
class Broker : public sim_mob::Agent, public sim_mob::BrokerBase {
public:
	explicit Broker(const MutexStrategy& mtxStrat, int id=-1, std::string commElement_ = "", std::string commMode_ = "");
	virtual ~Broker();

private:
	///List of all known tokens and their associated ConnectionHandlers.
	///This list is built over time, as new connections/Agents are added (ConnectionHandlers should never be removed).
	/// THREADING: This data structure is modified by parallel threads.
	std::map<std::string, boost::shared_ptr<sim_mob::ConnectionHandler> > tokenConnectionLookup;
	boost::mutex mutex_token_lookup; ///<Mutex to lock tokenConnectionLookup.

protected:
	struct ClientWaiting {
		ClientRegistrationRequest request;
		boost::shared_ptr<sim_mob::ConnectionHandler> existingConn;
		ClientWaiting(ClientRegistrationRequest request, boost::shared_ptr<sim_mob::ConnectionHandler> existingConn) : request(request), existingConn(existingConn) {}
	};

	//clientType => ClientWaiting
	typedef std::multimap<std::string, ClientWaiting> ClientWaitList;

	///Lookup for message handlers by type.
	///TODO: We need to register custom handlers for different clients, similar to how we use "overrides" for RoadRunner.
	HandlerLookup handleLookup;

	///the external communication entity that is using this broker as interface to from simmobility
	std::string commElement;//"roadrunner", "stk",...etc
	std::string commMode; //android-only, android-ns3,...etc

	//	list of the registered agents and their corresponding communication equipment
	AgentsList REGISTERED_AGENTS;
	///	waiting list for external clients willing to communication with simmobility
	ClientWaitList clientRegistrationWaitingList; //<client type, requestform>

	///	list of authorized clients who have passed the registration process
	ClientList::Type clientList; //key note: there can be one agent associated with multiple clients in this list. why? : coz clients of any type are i this list. and any one has associated itself to this agent for its specific type's reason


	///	connection point to outside simmobility
	ConnectionServer connection;

	///Broker's Publisher
	BrokerPublisher publisher;

	///services to be published by the broker's publisher
	std::vector<sim_mob::Services::SIM_MOB_SERVICE> serviceList;

	///	place to gather outgoing data for each tick
	//SendBuffer::Type sendBuffer;
	std::map<SendBuffer::Key, OngoingSerialization> sendBuffer;

	//Lock the send buffer (note: I am not entirely sure how thread-safe the sendBuffer is, so I am locking it just in case).
	//TODO: In the future, all clients on the same Broker should share a non-locked list of OngoingSerializations,
	//      and the Broker can just collate these.
	boost::mutex mutex_send_buffer;

	///	incoming data(from clients to broker) is saved here in the form of messages
	sim_mob::ThreadSafeQueue<sim_mob::MessageElement> receiveQueue;

	//todo this will be configured in the configure() method and
	//replace the above "clientRegistrationFactory" member for simplicity
	std::map<comm::ClientType, boost::shared_ptr<sim_mob::ClientRegistrationHandler> > ClientRegistrationHandlerMap;

	//	publishes an event when a client is registered with the broker
	sim_mob::ClientRegistrationPublisher registrationPublisher;

	///	internal controlling container
	std::set<const sim_mob::Agent*> duplicateEntityDoneChecker ;

	///Helper struct for client checking (below)
	struct ConnClientStatus {
		int total;
		int done;
		ConnClientStatus() : total(0), done(0) {}
	};

	///This map tracks the number of ClientHandlers connected per ConnectionHandler, as well as the number (per tick)
	///  that have already sent the CLIENT_MESSAGES_DONE message. It is locked by its associated mutex.
	std::map<boost::shared_ptr<sim_mob::ConnectionHandler>, ConnClientStatus> clientDoneChecklist;
	boost::mutex mutex_client_done_chk;

	///	some control members(//todo: no need to be static as there can be multiple brokers with different requirements)
	static const unsigned int MIN_CLIENTS = 1; //minimum number of registered clients(not waiting list)
	static const unsigned int MIN_AGENTS = 1; //minimum number of registered agents

	///	list of all brokers
	static std::map<std::string, sim_mob::Broker*> externalCommunicators;

	WaitForAndroidConnection waitAndroidBlocker;
	WaitForNS3Connection waitNs3Blocker;

	///	container for classes who evaluate wait-for-connection criteria for simmobility agents
	WaitForAgentRegistration waitAgentBlocker;

	//various controlling mutexes and condition variables
	boost::mutex mutex_client_request;
	boost::mutex mutex_clientList;
	boost::mutex mutex_clientDone;
	boost::mutex mutex_agentDone;
	boost::condition_variable COND_VAR_CLIENT_REQUEST;
	boost::condition_variable COND_VAR_CLIENT_DONE;

	//Number of connected Agents (entities which respond with a WHOAMI).
	size_t numAgents;


	///Set of clients that need their enableRegionSupport() function called. This can only be done once their time tick is over,
	///  so we pend them on this list. The extra weak_ptr shouldn't be a problem; if the object is destroyed before its
	///  call to enableRegionSupport(), it will just be silently dropped.
	std::set< boost::weak_ptr<sim_mob::ClientHandler> > newClientsWaitingOnRegionEnabling;


	/*
	 *
	 */
	boost::condition_variable COND_VAR_AGENT_DONE;

	/**
	 * There Can be different types of External Entities(clients)
	 * that are configure to be working with Broker. Each one of
	 * them can have a different criteria to Block the broker until
	 * some condition is met(like client's connection to simmobility).
	 * The following method will examine all those Conditions and reports
	 * back if any client is not connected when it was supposed to be connected.
	 */
	bool checkAllBrokerBlockers();

	/**
	 * checks wether an agent(entity) is dead or alive.
	 * Note: this function is not used any more.
	 */
	bool deadEntityCheck(sim_mob::AgentCommUtilityBase * info);
	/**
	 * Returns true if enough clients exist to allow the broker to update.
	 * Note: this function is not used any more.
	 */
	bool clientsQualify() const;

	///Return the number of clients that have completed the registration process.
	size_t getNumConnectedAgents() const;

	/**
	 * The Following two methods revise the registration of the registered agents.
	 * Note: these functions are not used any more.
	 */
	void refineSubscriptionList();
	void refineSubscriptionList(sim_mob::Agent * target);
	/**
	 * processes clients requests to be registered with the broker
	 */
	virtual void processClientRegistrationRequests();


	void setNewClientProps();

	/**
	 * checks to see if a client has sent all what it had to send during the current tick
	 * Note: The client will send an explicit message stating it is 'done' with whatever
	 * it had to send for the current tick.
	 */
	void waitForClientsDone();


	/**
	 * internal cleanup at each tick
	 */
	void cleanup();

	/**
	 * 	handlers executed when an agent is going out of simulation(die)
	 * The number and order of arguments are as per EventPublisher guidelines.
	 * To avoid comment duplication, refer to the wiki and the corresponding source code for more information
	 */
	virtual void onEvent(event::EventId eventId, sim_mob::event::Context ctxId, event::EventPublisher* sender, const event::EventArgs& args);
	/**
	 * to be called and identify the agent who has just updated
	 * The number and order of arguments are as per EventPublisher guidelines.
	 * To avoid comment duplication, refer to the wiki and the corresponding source code for more information
	 */
	virtual void onAgentUpdate(sim_mob::event::EventId id, sim_mob::event::Context context, sim_mob::event::EventPublisher* sender, const UpdateEventArgs& argums);
	/**
	 * 	actuall implementation of onAgentUpdate
	 */
	void agentUpdated(const Agent* target );
	/**
	 * 	is called when a new client is registered with the broker
	 */
	virtual void onClientRegister(sim_mob::event::EventId id, sim_mob::event::Context context, sim_mob::event::EventPublisher* sender, const ClientRegistrationEventArgs& argums);

	/**
	 * 	publish various data the broker has subscibed to
	 */
	void processPublishers(timeslice now);
	/**
	 * 	sends a signal to clients(through send buffer) telling them broker is ready to receive their data for the current tick
	 */
	void sendReadyToReceive();
	/**
	 * 	sends out data accumulated in the send buffer
	 */
	void processOutgoingData(timeslice now);
	/**
	 * 	handles messages received in the current tick
	 */
	void processIncomingData(timeslice);

public:


	/**
	 * 	configure publisher, message handlers and waiting criteria...
	 */
	virtual void configure();
	/**
	 * 	temporary function replacing onAgentUpdate
	 */
	void AgentUpdated(Agent *);

	/**
	 * 	list of registered agents
	 */
	AgentsList::type &getRegisteredAgents();

	virtual size_t getRegisteredAgentsSize() const;

	/**
	 * 	list of registered agents + mutex
	 */
	AgentsList::type &getRegisteredAgents(AgentsList::Mutex* mutex);
	/**
	 * 	register an agent
	 */
	void registerEntity(sim_mob::AgentCommUtilityBase* agent);
	/**
	 * 	unregister an agent
	 */
	void unRegisterEntity(sim_mob::AgentCommUtilityBase* agent);
	/**
	 * 	unregister an agent
	 */
	void unRegisterEntity(sim_mob::Agent * agent);


	///Returns the size of the list of clients waiting to be admitted as registered clients.
	size_t getClientWaitingListSize() const;

	/**
	 * 	returns list of registered clients
	 */
	virtual const ClientList::Type & getClientList() const;
	/**
	 *
	 * 	searches for a client of specific ID and Type
	 */
	bool getClientHandler(std::string clientId,std::string clientType, boost::shared_ptr<sim_mob::ClientHandler> &output) const;

	/**
	 * 	adds to the list of registered clients
	 */
	void insertClientList(std::string, comm::ClientType , boost::shared_ptr<sim_mob::ClientHandler>&);

	/**
	 * 	adds a client to the registration waiting list
	 */
	void insertClientWaitingList(std::string clientType, ClientRegistrationRequest request, boost::shared_ptr<sim_mob::ConnectionHandler> existingConn);


	/**
	 * When a new connection is registered (or a NEW_CLIENT message goes out), the
	 *   ConnectionServer sends out a WHOAREYOU message, and then waits for a response.
	 *   Since this response comes in asynchronously, the session pointer is
	 *   pushed to the Broker using this function. It is then paired up with an Agent
	 *   upon receiving a WHOAMI message through the normal channels.
	 * NOTE: For now, this pairing is arbitrary for agents on the same connector. You could
	 *       To ensure that ONLY the agent who received the WHOAREYOU can respond, one
	 *       might add a unique token PER AGENT to the WHOAREYOU message that is then relayed back
	 *       in the WHOAMI (but at the moment this is not necessary. Currently the token is only unique per connection.
	 * THREADING: This function is called directly by thread (via the WhoAreYouProtocol).
	 */
	virtual void insertIntoWaitingOnWHOAMI(const std::string& token, boost::shared_ptr<sim_mob::ConnectionHandler> newConn);

	sim_mob::event::EventPublisher & getPublisher();

	/**
	 * Accessor to client registration publisher
	 */
	sim_mob::ClientRegistrationPublisher &getRegistrationPublisher();

	/**
	 * 	request to insert into broker's send buffer
	 */
	bool insertSendBuffer(boost::shared_ptr<sim_mob::ClientHandler> client, const std::string& message);

	/**
	 * 	callback function executed upon message arrival
	 */
	virtual void onMessageReceived(boost::shared_ptr<ConnectionHandler>cnnHadler, const BundleHeader& header, std::string message);

	/**
	 * 	broker, as an agent, has an update function
	 */
	Entity::UpdateStatus update(timeslice now);
	static std::map<std::string, sim_mob::Broker*> & getExternalCommunicators() ;
	static sim_mob::Broker* getExternalCommunicator(const std::string & value) ;
	/**
	 * 	Add proper entries in simmobility's configuration class
	 */
	static void addExternalCommunicator(const std::string & name, sim_mob::Broker* broker);

	///Implements pure virtual Agent::isNonspatial. Returns true.
	bool isNonspatial();

protected:
	/**
	 * 	Wait for clients
	 */
	void waitAndAcceptConnections();
	/**
	 * 	wait for the registered agents to complete their tick
	 */
	void waitForAgentsUpdates();

	/**
	 * 	wait for a message from all of the registered the client stating that they are done sending messages for this tick
	 */
	bool allClientsAreDone();


public:
	//Unused, required overrides.
	void load(const std::map<std::string, std::string>& configProps); ///<Implements pure virtual Agent::load(). Unused.
	bool frame_init(timeslice now); ///<Implements pure virtual Agent::frame_init(). Unused.
	Entity::UpdateStatus frame_tick(timeslice now); ///<Implements pure virtual Agent::frame_tick(). Unused.
	void frame_output(timeslice now); ///<Implements pure virtual Agent::frame_output. Unused.

};

}
