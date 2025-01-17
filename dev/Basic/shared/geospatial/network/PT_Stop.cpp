//Copyright (c) 2013 Singapore-MIT Alliance for Research and Technology
//Licensed under the terms of the MIT License, as described in the file:
//   license.txt   (http://opensource.org/licenses/MIT)

#include "PT_Stop.hpp"
#include "Node.hpp"
#include "RoadSegment.hpp"
#include "Lane.hpp"
#include "Point.hpp"
#include "util/DynamicVector.hpp"
#include "util/LangHelpers.hpp"
#include "util/Utils.hpp"
#include "RoadNetwork.hpp"

using namespace sim_mob;

std::map<std::string, BusStop *> BusStop::mapOfCodevsBusStops;

void BusStop::registerBusStop(BusStop* stop)
{
    std::string code = stop->getStopCode();
    if(mapOfCodevsBusStops.find(code)==mapOfCodevsBusStops.end()){
        mapOfCodevsBusStops[code] = stop;
    }
}
BusStop* BusStop::findBusStop(const std::string& code)
{
    BusStop* stop = nullptr;
    std::map<std::string, BusStop *>::iterator it = mapOfCodevsBusStops.find(code);
    if(it!=mapOfCodevsBusStops.end()){
        stop = it->second;
    }
    return stop;
}

BusStop::BusStop() :
        terminusType(NOT_A_TERMINUS), length(0.0), twinStop(nullptr), virtualStop(false), offset(0.0),
        reverseSectionId(0), terminalNodeId(0), roadSegment(NULL), stopId(0), stopCode(std::string())
{
}

BusStop::~BusStop()
{
    //busLines.clear();
}

const Point& BusStop::getStopLocation() const
{
    return location;
}

void BusStop::setStopLocation(Point loc)
{
    location = loc;
}

const RoadSegment* BusStop::getParentSegment() const
{
    return roadSegment;
}

void BusStop::setParentSegment(RoadSegment *roadSegment)
{
    this->roadSegment = roadSegment;
}

const TerminusType& BusStop::getTerminusType() const
{
    return terminusType;
}

void BusStop::setTerminusType(TerminusType type)
{
    terminusType = type;
}

double BusStop::getLength() const
{
    return length;
}

void BusStop::setLength(double len)
{
    length = len;
}

const BusStop* BusStop::getTwinStop() const
{
    return twinStop;
}

void BusStop::setTwinStop(const BusStop* stop)
{
    twinStop = stop;
}

bool BusStop::isVirtualStop() const
{
    return virtualStop;
}

void BusStop::setVirtualStop()
{
    virtualStop = true;
}

double BusStop::getOffset() const
{
    return offset;
}

void BusStop::setOffset(double val)
{
    offset = val;
}

const std::string& BusStop::getStopName() const
{
    return stopName;
}

void BusStop::setStopName(const std::string& name)
{
    stopName = name;
}

unsigned int BusStop::getStopId() const
{
    return stopId;
}

void BusStop::setStopId(unsigned int id)
{
    stopId = id;
    setRoadItemId(id);
}

const std::string& BusStop::getStopCode() const
{
    return stopCode;
}

void BusStop::setStopCode(const std::string& code)
{
    stopCode = code;
}

const std::string& BusStop::getStopStatus() const
{
    return stopStatus;
}

void BusStop::setStopStatus(const std::string& status)
{
    stopStatus = status;
}

unsigned int BusStop::getReverseSectionId() const
{
    return reverseSectionId;
}

void BusStop::setReverseSectionId(unsigned int reverseSectionId)
{
    this->reverseSectionId = reverseSectionId;
}

unsigned int BusStop::getTerminalNodeId() const
{
    return terminalNodeId;
}

void BusStop::setTerminalNodeId(unsigned int terminalNodeId)
{
    this->terminalNodeId = terminalNodeId;
}

TrainStop::TrainStop(std::string stopIds)
{
    stopName = stopIds;
    std::stringstream ss(stopIds);
    std::string singleMrtStopId;
    while (std::getline(ss, singleMrtStopId, '/'))
    {
        trainStopIds.push_back(singleMrtStopId);
    }
}

const std::string& TrainStop::getStopName() const
{
    return stopName;
}

TrainStop::~TrainStop()
{
    trainStopIds.clear();
    roadSegments.clear();
}

const RoadSegment* TrainStop::getStationSegmentForNode(const Node* nd) const
{
    const RoadSegment* res = nullptr;
    double minDis = std::numeric_limits<double>::max();
    for (std::vector<const RoadSegment*>::const_iterator segIt = roadSegments.begin(); segIt != roadSegments.end(); segIt++)
    {
        const RoadSegment* segment = *segIt;
        const Point& firstPoint = segment->getPolyLine()->getFirstPoint();
        DynamicVector estimateDistVector(nd->getLocation().getX(),nd->getLocation().getY(), firstPoint.getX(), firstPoint.getY());
        double actualDistanceStart = estimateDistVector.getMagnitude();
        if (minDis > actualDistanceStart)
        {
            minDis = actualDistanceStart;
            res = segment;
        }
    }
    return res;
}

const RoadSegment* TrainStop::getRandomStationSegment() const
{
    int random = Utils::generateInt(0, roadSegments.size()-1);
    std::vector<const RoadSegment*>::const_iterator segIt = roadSegments.begin();
    std::advance(segIt, random);
    return (*segIt);
}

Station::Station(const std::string& id):TrainStop(id),stationAgent(nullptr)
{

}
void Station::addPlatform(const std::string& lineId, Platform* platform)
{
    lineToPlatform[lineId] = platform;
}

void Station::setAssociatedAgent(Agent* agent)
{
    stationAgent = agent;
}
const std::map<std::string, Platform*>& Station::getPlatforms() const
{
    return lineToPlatform;
}

Platform* Station::getPlatform(const std::string& lineId) const
{
    Platform* platform = nullptr;
    std::map<std::string, Platform*>::const_iterator it = lineToPlatform.find(lineId);
    if(it!=lineToPlatform.end()){
        platform = it->second;
    }
    return platform;
}
void TrainStop::addAccessRoadSegment(unsigned int segmentId)
{
    const RoadNetwork* rn = RoadNetwork::getInstance();
    const RoadSegment* accessSegment = rn->getById(rn->getMapOfIdVsRoadSegments(), segmentId);
    if(accessSegment)
    {
        this->roadSegments.push_back(accessSegment);
    }
    else
    {
        throw std::runtime_error("invalid segment id for train stop access");
    }
}
