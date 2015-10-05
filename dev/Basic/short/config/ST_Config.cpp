#include "ST_Config.hpp"
#include "conf/ConfigManager.hpp"

namespace sim_mob
{

VehicleType::VehicleType() : name(""), length(-1.0), width(-1.0), capacity(-1)
{}

bool VehicleType::isValidRole(const std::string &role) const
{
    //return (associatedRoles.find(role) != associatedRoles.end());
	return true;
}

bool VehicleType::operator==(const VehicleType &rhs) const
{
    return (this->name == rhs.name);
}

bool VehicleType::operator!=(const VehicleType &rhs) const
{
    return (this->name != rhs.name);
}

EntityTemplate::EntityTemplate() : startTimeMs(0), laneIndex(0),originNode(-1),
    destNode(-1),initSegId(-1),initDis(-1),initSpeed(0),agentId(-1), tripId(-1),
    vehicleType("")
{}

ST_Config* ST_Config::instance = nullptr;

ST_Config& ST_Config::getInstance()
{
    if(!instance)
    {
        instance = new ST_Config();
    }
    return *instance;
}

void ST_Config::deleteIntance()
{
    safe_delete_item(instance);
}

unsigned int& ST_Config::personWorkGroupSize()
{
    return workers.person.count;
}
const unsigned int& ST_Config::personWorkGroupSize() const
{
    return workers.person.count;
}

unsigned int& ST_Config::signalWorkGroupSize()
{
    return workers.signal.count;
}
const unsigned int& ST_Config::signalWorkGroupSize() const
{
    return workers.signal.count;
}

unsigned int& ST_Config::intMgrWorkGroupSize()
{
    return workers.intersectionMgr.count;
}
const unsigned int& ST_Config::intMgrWorkGroupSize() const
{
    return workers.intersectionMgr.count;
}

unsigned int& ST_Config::commWorkGroupSize()
{
    return workers.communication.count;
}
const unsigned int& ST_Config::commWorkGroupSize() const
{
    return workers.communication.count;
}

NetworkSource& ST_Config::networkSource()
{
    return networkSource;
}
const NetworkSource& ST_Config::networkSource() const
{
    return networkSource;
}

std::string& ST_Config::getNetworkXmlInputFile()
{
    return networkXmlInputFile;
}
const std::string& ST_Config::getNetworkXmlInputFile() const
{
    return networkXmlInputFile;
}

std::string& ST_Config::getNetworkXmlOutputFile()
{
    return networkXmlOutputFile;
}
const std::string& ST_Config::getNetworkXmlOutputFile() const
{
    return networkXmlOutputFile;
}

std::string& ST_Config::getRoadNetworkXsdSchemaFile()
{
    return roadNetworkXsdSchemaFile;
}
void ST_Config::setRoadNetworkXsdSchemaFile(std::string& name)
{
    roadNetworkXsdSchemaFile = name;
}
const std::string& ST_Config::getRoadNetworkXsdSchemaFile() const
{
    return roadNetworkXsdSchemaFile;
}

AuraManager::AuraManagerImplementation& ST_Config::aura_manager_impl()
{
    return auraManagerImplementation;
}
const AuraManager::AuraManagerImplementation& ST_Config::aura_manager_impl() const
{
    return auraManagerImplementation;
}

bool ST_Config::commSimEnabled() const
{
    return commsim.enabled;
}

unsigned int ST_Config::personTimeStepInMilliSeconds() const
{
    return workers.person.granularityMs;
}

unsigned int ST_Config::signalTimeStepInMilliSeconds() const
{
    return workers.signal.granularityMs;
}

unsigned int ST_Config::intMgrTimeStepInMilliSeconds() const
{
    return workers.intersectionMgr.granularityMs;
}

unsigned int ST_Config::communicationTimeStepInMilliSeconds() const
{
    return workers.communication.granularityMs;
}

}