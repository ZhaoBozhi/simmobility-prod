/* Copyright Singapore-MIT Alliance for Research and Technology */

#pragma once

#include <map>
#include <string>
#include <vector>

#include "GenConfig.h"

#include "entities/Agent.hpp"
#include "roles/Role.hpp"
#include "buffering/Shared.hpp"
#include "entities/UpdateParams.hpp"
#include "entities/misc/TripChain.hpp"

namespace sim_mob
{

class TripChainItem;
class SubTrip;

#ifndef SIMMOB_DISABLE_MPI
class PartitionManager;
class PackageUtils;
class UnPackageUtils;
#endif

/**
 * Basic Person class.
 *
 * \author Seth N. Hetu
 * \author Wang Xinyuan
 * \author Luo Linbo
 * \author Li Zhemin
 * \author Xu Yan
 * \author Harish Loganathan
 *
 * A person may perform one of several roles which
 *  change over time. For example: Drivers, Pedestrians, and Passengers are
 *  all roles which a Person may fulfill.
 */
class Person : public sim_mob::Agent {
public:
	bool tripchainInitialized;
	///The "src" variable is used to help flag how this person was created.
	explicit Person(const std::string& src, const MutexStrategy& mtxStrat, int id=-1);
	explicit Person(const std::string& src, const MutexStrategy& mtxStrat, std::vector<sim_mob::TripChainItem*> tc);
	virtual ~Person();
	void initTripChain();


	//Update Person behavior (old)
	//virtual Entity::UpdateStatus update(timeslice now);

	///Load a Person's config-specified properties, creating a placeholder trip chain if
	/// requested.
	virtual void load(const std::map<std::string, std::string>& configProps);

    ///Update a Person's subscription list.
    virtual void buildSubscriptionList(std::vector<BufferedBase*>& subsList);

    ///Change the role of this person: Driver, Passenger, Pedestrian
    void changeRole(sim_mob::Role* newRole);
    sim_mob::Role* getRole() const;
    bool updatePersonRole(sim_mob::Role* newRole = 0);
    ///Check if any role changing is required.
    /// "nextValidTimeMS" is the next valid time tick, which may be the same at this time tick.
    Entity::UpdateStatus checkTripChain(uint32_t currTimeMS);
    bool changeRoleRequired(sim_mob::Role & currRole,sim_mob::SubTrip &currSubTrip)const;//todo depricate later
    bool changeRoleRequired_Trip(/*sim_mob::Trip &trip*/) const;
    bool changeRoleRequired_Activity(/*sim_mob::Activity &activity*/) const;
    bool changeRoleRequired(sim_mob::TripChainItem &tripChinItem) const;
    //update origin and destination node based on the trip, subtrip or activity given
    bool updateOD(sim_mob::TripChainItem *tc ,const sim_mob::SubTrip *subtrip = 0);
    ///get this person's trip chain
    std::vector<TripChainItem*>& getTripChain()
    {
        return tripChain;
    }

    ///Set this person's trip chain
    void setTripChain(const std::vector<TripChainItem*>& tripChain)
    {
        this->tripChain = tripChain;
    }

/*	const sim_mob::Link* getCurrLink() const;
	void setCurrLink(sim_mob::Link* link);*/
	
	int laneID;
	const std::string& getAgentSrc() const { return agentSrc; }

    SubTrip* getNextSubTripInTrip();
    TripChainItem* findNextItemInTripChain();

    std::vector<TripChainItem*>::iterator currTripChainItem; // pointer to current item in trip chain
    std::vector<SubTrip>::const_iterator currSubTrip; //pointer to current subtrip in the current trip (if  current item is trip)

    //
    std::vector<TripChainItem*>::iterator nextTripChainItem; // pointer to next item in trip chain
    std::vector<SubTrip>::const_iterator nextSubTrip; //pointer to next subtrip in the current trip (if  current item is trip)

    //Used for passing various debug data. Do not rely on this for anything long-term.
    std::string specialStr;


protected:
	virtual bool frame_init(timeslice now);
	virtual Entity::UpdateStatus frame_tick(timeslice now);
	virtual void frame_output(timeslice now);


private:
	//Very risky:
	UpdateParams* curr_params;


	bool advanceCurrentTripChainItem();
	bool advanceCurrentSubTrip();
	bool findNextTripChainItem();
	bool findNextSubTrip();
	std::vector<sim_mob::SubTrip>::const_iterator resetCurrSubTrip();
	bool findNextPersonRole();

    //Properties
    sim_mob::Role* prevRole; ///< To be deleted on the next time tick.
    sim_mob::Role* currRole;
    sim_mob::Role* nextRole;//do not be misled. this variable is only temporary and will not be used to update the currRole

    //Can be helpful for debugging
    std::string agentSrc;

    int currTripChainSequenceNumber;
    std::vector<TripChainItem*> tripChain;

    //to mark the first call to update function
    bool first_update_tick;

    ///Determines if frame_init() has been done.
    friend class PartitionManager;
    friend class BoundaryProcessor;

#ifndef SIMMOB_DISABLE_MPI
public:
	virtual void pack(PackageUtils& packageUtil);
	virtual void unpack(UnPackageUtils& unpackageUtil);

	virtual void packProxy(PackageUtils& packageUtil);
	virtual void unpackProxy(UnPackageUtils& unpackageUtil);

#endif
};

}
