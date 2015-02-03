//Copyright (c) 2013 Singapore-MIT Alliance for Research and Technology
//Licensed under the terms of the MIT License, as described in the file:
//   license.txt   (http://opensource.org/licenses/MIT)

#pragma once

/// \file SOCI_Converters.hpp
///This file contains several Type converters for SOCI object-mapping
/// \author Seth N. Hetu
/// \author LIM Fung Chai
/// \author Matthew Bremer Bruchon

#include "soci.h"
#include "Node.hpp"
#include "Section.hpp"
#include "Lane.hpp"
#include "Crossing.hpp"
#include "Turning.hpp"
#include "Polyline.hpp"
#include "BusStop.hpp"
#include "./Signal.hpp"
#include "Phase.hpp"
#include "path/PathSetManager.hpp"
#include "entities/PersonLoader.hpp"
#include "geospatial/TurningSection.hpp"
#include "geospatial/TurningConflict.hpp"

//using namespace sim_mob::aimsun;
//using std::string;


namespace soci
{
class ERP_Section;
class ERP_Surcharge;
class ERP_Gantry_Zone;
class LinkTravelTime;
class PathSet;
class SinglePath;
class CBD_Pair;
template<> struct type_conversion<sim_mob::CBD_Pair>
{
    typedef values base_type;
    static void from_base(const soci::values& vals, soci::indicator& ind, sim_mob::CBD_Pair &res)
    {
    	res.from_section = vals.get<int>("from_section", 0);
    	res.to_section = vals.get<int>("to_section", 0);
    }
};
template<> struct type_conversion<sim_mob::aimsun::Node>
{
    typedef values base_type;
    static void from_base(const soci::values& vals, soci::indicator& ind, sim_mob::aimsun::Node &res)
    {
    	res.id = vals.get<int>("node_id", 0);
    	res.xPos = vals.get<double>("xpos", 0.0);
    	res.yPos = vals.get<double>("ypos", 0.0);
    	res.isIntersection = vals.get<int>("isintersection", 0);
    }
    static void to_base(const sim_mob::aimsun::Node& src, soci::values& vals, soci::indicator& ind)
    {
    	vals.set("node_id", src.id);
        vals.set("xpos", src.xPos);
        vals.set("ypos", src.yPos);
        vals.set("isintersection", src.isIntersection?1:0);
        ind = i_ok;
    }
};

template<> struct type_conversion<sim_mob::SinglePath>
{
    typedef values base_type;
    static void from_base(const soci::values& vals, soci::indicator& ind, sim_mob::SinglePath &res)
    {
    	res.id = vals.get<std::string>("id", "");
    	res.pathSetId = vals.get<std::string>("pathset_id", "");
    	res.partialUtility = vals.get<double>("partial_utility", 0.0);
    	res.pathSize = vals.get<double>("path_size", 0.0);
    	res.signalNumber = vals.get<int>("signal_number", 0);
    	res.rightTurnNumber = vals.get<int>("right_turn_number", 0);
    	res.scenario = vals.get<std::string>("scenario", "");
    	res.length = vals.get<double>("length",0.0);
    	res.highWayDistance = vals.get<double>("highway_distance",0.0);
		res.isMinDistance = vals.get<int>("min_distance",0);
		res.isMinSignal = vals.get<int>("min_signal",0);
		res.isMinRightTurn = vals.get<int>("min_right_turn",0);
		res.isMaxHighWayUsage = vals.get<int>("max_highway_usage",0);
		res.valid_path = vals.get<int>("valid_path",0);
		res.isShortestPath = vals.get<int>("shortest_path",0);
		res.index = vals.get<long long>("serial_id",0);
    }
    static void to_base(const sim_mob::SinglePath& src, soci::values& vals, soci::indicator& ind)
    {
    	vals.set("id", src.id);
        vals.set("pathset_id", src.pathSetId);
        vals.set("partial_utility", src.partialUtility);
        vals.set("path_size", src.pathSize);
        vals.set("signal_number", src.signalNumber);
        vals.set("right_turn_number", src.rightTurnNumber);
        vals.set("scenario", src.scenario);
        vals.set("length", src.length);
        vals.set("highway_distance", src.highWayDistance);
        vals.set("min_distance", (src.isMinDistance ? 1 : 0));
        vals.set("min_signal", (src.isMinSignal ? 1 : 0));
        vals.set("min_right_turn", (src.isMinRightTurn ? 1 : 0));
        vals.set("max_highway_usage", (src.isMaxHighWayUsage ? 1 : 0));
        vals.set("valid_path", (src.valid_path ? 1 : 0));
        vals.set("shortest_path", (src.isShortestPath ? 1 : 0));
        vals.set("serial_id", src.index);
        ind = i_ok;
    }
};
template<> struct type_conversion<sim_mob::ERP_Surcharge>
{
    typedef values base_type;
    static void from_base(const soci::values& vals, soci::indicator& ind, sim_mob::ERP_Surcharge &res)
    {
    	res.gantryNo = vals.get<std::string>("gantry_no", "");
    	res.startTime = vals.get<std::string>("start_time", "00:00:00");
    	res.endTime = vals.get<std::string>("end_time", "00:00:00");
    	res.rate = vals.get<double>("Rate", 0.0);
    	res.vehicleTypeId = vals.get<int>("Vehicle_Type_Id", 0);
    	res.vehicleTypeDesc = vals.get<std::string>("Vehicle_Type_Desc", "");
    	res.day = vals.get<std::string>("Day", "");
    }
    static void to_base(const sim_mob::ERP_Surcharge& src, soci::values& vals, soci::indicator& ind)
    {
    	vals.set("Gantry_No", src.gantryNo);
        vals.set("Start_Time", src.startTime);
        vals.set("End _Time", src.endTime);
        vals.set("Rate", src.rate);
        vals.set("Vehicle_Type_Id", src.vehicleTypeId);
        vals.set("Vehicle_Type_Desc", src.vehicleTypeDesc);
        vals.set("Day", src.day);
        ind = i_ok;
    }
};
template<> struct type_conversion<sim_mob::ERP_Section>
{
    typedef values base_type;
    static void from_base(const soci::values& vals, soci::indicator& ind, sim_mob::ERP_Section &res)
    {
    	res.ERP_Gantry_No = vals.get<int>("ERP_Gantry_No", 0);
    	res.section_id = vals.get<int>("section_id", 0);
    }
    static void to_base(const sim_mob::ERP_Section& src, soci::values& vals, soci::indicator& ind)
    {
    	vals.set("ERP_Gantry_No", src.ERP_Gantry_No);
        vals.set("section_id", src.section_id);
        ind = i_ok;
    }
};
template<> struct type_conversion<sim_mob::SegmentType>
{
    typedef values base_type;
    static void from_base(const soci::values& vals, soci::indicator& ind, sim_mob::SegmentType &res)
    {
    	res.id = vals.get<std::string>("id", "");
    	res.type = vals.get<int>("type", 0);
    }
    static void to_base(const sim_mob::SegmentType& src, soci::values& vals, soci::indicator& ind)
    {
    	vals.set("id", src.id);
        vals.set("type", src.type);
        ind = i_ok;
    }
};
template<> struct type_conversion<sim_mob::NodeType>
{
    typedef values base_type;
    static void from_base(const soci::values& vals, soci::indicator& ind, sim_mob::NodeType &res)
    {
    	res.id = vals.get<std::string>("id", "");
    	res.type = vals.get<int>("type", 0);
    }
    static void to_base(const sim_mob::NodeType& src, soci::values& vals, soci::indicator& ind)
    {
    	vals.set("id", src.id);
        vals.set("type", src.type);
        ind = i_ok;
    }
};
template<> struct type_conversion<sim_mob::ERP_Gantry_Zone>
{
    typedef values base_type;
    static void from_base(const soci::values& vals, soci::indicator& ind, sim_mob::ERP_Gantry_Zone &res)
    {
    	res.gantryNo = vals.get<std::string>("Gantry_no", "");
    	res.zoneId = vals.get<std::string>("Zone_Id", "");
    }
    static void to_base(const sim_mob::ERP_Gantry_Zone& src, soci::values& vals, soci::indicator& ind)
    {
    	vals.set("Gantry_no", src.gantryNo);
        vals.set("Zone_Id", src.zoneId);
        ind = i_ok;
    }
};
template<> struct type_conversion<sim_mob::LinkTravelTime>
{
    typedef values base_type;
    static void from_base(const soci::values& vals, soci::indicator& ind, sim_mob::LinkTravelTime &res)
    {
//    	res.gid = vals.get<int>("gid", 0);
    	res.linkId = vals.get<int>("link_id", 0);
    	res.startTime = vals.get<std::string>("start_time", "00:00:00");
    	res.endTime = vals.get<std::string>("end_time", "00:00:00");
    	res.travelTime = vals.get<double>("travel_time", 0.0);
    }
    static void to_base(const sim_mob::LinkTravelTime& src, soci::values& vals, soci::indicator& ind)
    {
    	vals.set("link_id", src.linkId);
        vals.set("start_time", src.startTime);
        vals.set("end_time", src.endTime);
        vals.set("travel_time", src.travelTime);
        ind = i_ok;
    }
};

template<> struct type_conversion<sim_mob::aimsun::Section>
{
    typedef values base_type;
    static void from_base(const soci::values& vals, soci::indicator& ind, sim_mob::aimsun::Section &res)
    {
    	res.id = vals.get<int>("id", 0);
    	res.roadName = vals.get<std::string>("road_name", "");
    	res.numLanes = vals.get<int>("nb_lanes", 1);
    	res.speed = vals.get<double>("speed", 0);
    	res.capacity = vals.get<double>("capacity", 0);
    	res.length = vals.get<double>("length", 0);
    	res.TMP_FromNodeID = vals.get<int>("fnode", 0);
    	res.TMP_ToNodeID = vals.get<int>("tnode", 0);
    }
    static void to_base(const sim_mob::aimsun::Section& src, soci::values& vals, soci::indicator& ind)
    {
    	vals.set("id", src.id);
        vals.set("road_name", src.roadName);
        vals.set("nb_lanes", src.numLanes);
        vals.set("speed", src.speed);
        vals.set("capacity", src.capacity);
        vals.set("length", src.length);
        vals.set("fnode", src.fromNode->id);
        vals.set("tnode", src.toNode->id);
        ind = i_ok;
    }
};

template<> struct type_conversion<sim_mob::aimsun::Phase>
{
    typedef values base_type;
    static void from_base(const soci::values& vals, soci::indicator& ind, sim_mob::aimsun::Phase &res)
    {
    	res.name = vals.get<std::string>("phases","");
    	res.nodeId = vals.get<int>("node_id",0);
    	res.sectionFrom = vals.get<int>("from_section",0);
    	res.sectionTo = vals.get<int>("to_section",0);
    	res.laneFrom_A = vals.get<int>("from_lane_a", 0);
    	res.laneTo_A = vals.get<int>("to_lane_a", 0);
    	res.laneFrom_B = vals.get<int>("from_lane_b", 0);
    	res.laneTo_B = vals.get<int>("to_lane_b", 0);
    }
    static void to_base(const sim_mob::aimsun::Phase& src, soci::values& vals, soci::indicator& ind)
    {
//    	vals.set("id", src.id);
        vals.set("phases", src.name);
        vals.set("node_id", src.nodeId);
        vals.set("from_section", src.sectionFrom);
        vals.set("to_section", src.sectionTo);
        vals.set("from_lane_a", src.laneFrom_A);
        vals.set("from_lane_b", src.laneFrom_B);
        vals.set("to_lane_a", src.laneTo_A);
        vals.set("to_lane_b", src.laneTo_B);
        ind = i_ok;
    }
};

template<> struct type_conversion<sim_mob::aimsun::Turning>
{
    typedef values base_type;
    static void from_base(const soci::values& vals, soci::indicator& ind, sim_mob::aimsun::Turning &res)
    {
    	res.id = vals.get<int>("turning_id", 0);
    	res.fromLane.first = vals.get<int>("from_lane_a", 0);
    	res.fromLane.second = vals.get<int>("from_lane_b", 0);
    	res.toLane.first = vals.get<int>("to_lane_a", 0);
    	res.toLane.second = vals.get<int>("to_lane_b", 0);
    	res.TMP_FromSection = vals.get<int>("from_section", 0);
    	res.TMP_ToSection = vals.get<int>("to_section", 0);
    }
    static void to_base(const sim_mob::aimsun::Turning& src, soci::values& vals, soci::indicator& ind)
    {
    	vals.set("turning_id", src.id);
    	vals.set("from_lane_a", src.fromLane.first);
    	vals.set("from_lane_b", src.fromLane.second);
    	vals.set("to_lane_a", src.toLane.first);
    	vals.set("to_lane_b", src.toLane.second);
    	vals.set("from_section", src.fromSection->id);
    	vals.set("to_section", src.toSection->id);
        ind = i_ok;
    }
};


template<> struct type_conversion<sim_mob::aimsun::Polyline>
{
    typedef values base_type;
    static void from_base(const soci::values& vals, soci::indicator& ind, sim_mob::aimsun::Polyline &res)
    {
    	res.xPos = vals.get<double>("xpos", 0.0);
    	res.yPos = vals.get<double>("ypos", 0.0);
    	res.TMP_SectionId = vals.get<int>("section_id", 0);
    }
    static void to_base(const sim_mob::aimsun::Polyline& src, soci::values& vals, soci::indicator& ind)
    {
    	vals.set("section_id", src.section->id);
        vals.set("xpos", src.xPos);
        vals.set("ypos", src.yPos);
        ind = i_ok;
    }
};

template<> struct type_conversion<sim_mob::aimsun::Crossing>
{
    typedef values base_type;
    static void from_base(const soci::values& vals, soci::indicator& ind, sim_mob::aimsun::Crossing &res)
    {
    	res.laneID = vals.get<int>("lane_id", 0);
    	res.laneType = vals.get<std::string>("lane_type", "");
    	res.TMP_AtSectionID = vals.get<int>("section", 0);
    	res.xPos = vals.get<double>("xpos", 0.0);
    	res.yPos = vals.get<double>("ypos", 0.0);
    }
    static void to_base(const sim_mob::aimsun::Crossing& src, soci::values& vals, soci::indicator& ind)
    {
    	vals.set("lane_id", src.laneID);
    	vals.set("lane_type", src.laneType);
    	vals.set("section", src.atSection->id);
    	vals.set("xpos", src.xPos);
    	vals.set("ypos", src.yPos);
        ind = i_ok;
    }
};


template<> struct type_conversion<sim_mob::aimsun::Lane>
{
    typedef values base_type;
    static void from_base(const soci::values& vals, soci::indicator& ind, sim_mob::aimsun::Lane &res)
    {
    	res.laneID = vals.get<int>("lane_id", 0);
    	res.laneType = vals.get<std::string>("lane_type", "");
    	res.TMP_AtSectionID = vals.get<int>("section", 0);
    	res.xPos = vals.get<double>("xpos", 0.0);
    	res.yPos = vals.get<double>("ypos", 0.0);
        res.rowNo = vals.get<int>("rowno", 0);
    }
    static void to_base(const sim_mob::aimsun::Lane& src, soci::values& vals, soci::indicator& ind)
    {
    	vals.set("lane_id", src.laneID);
    	vals.set("lane_type", src.laneType);
    	vals.set("section", src.atSection->id);
    	vals.set("xpos", src.xPos);
    	vals.set("ypos", src.yPos);
        vals.set("rowno", src.rowNo);
        ind = i_ok;
    }
};



template<>
struct type_conversion<sim_mob::aimsun::Signal>
{
    typedef values base_type;

    static void
    from_base(soci::values const & values, soci::indicator & indicator, sim_mob::aimsun::Signal& signal)
    {
        signal.id = values.get<int>("signal_id", 0);
        signal.nodeId = values.get<int>("node_id", 0);
        signal.xPos = values.get<double>("xpos", 0.0);
        signal.yPos = values.get<double>("ypos", 0.0);
        signal.typeCode = values.get<std::string>("type_cd", "");
        signal.bearing = values.get<double>("bearg", 0.0);
    }

    static void
    to_base(sim_mob::aimsun::Signal const & signal, soci::values & values, soci::indicator & indicator)
    {
        values.set("signal_id", signal.id);
        values.set("node_id", signal.nodeId);
        values.set("xpos", signal.xPos);
        values.set("ypos", signal.yPos);
        values.set("type_cd", signal.typeCode);
        values.set("bearg", signal.bearing);
        indicator = i_ok;
    }
};


template<> struct type_conversion<sim_mob::aimsun::BusStop>
{
    typedef values base_type;
    static void from_base(const soci::values& vals, soci::indicator& ind, sim_mob::aimsun::BusStop &res)
    {
    	res.bus_stop_no = vals.get<std::string>("bus_stop_no", "");
    	res.TMP_AtSectionID= vals.get<int>("section_id", 0);
    	res.status = vals.get<std::string>("status", "");
    	res.lane_type = vals.get<std::string>("lane_type", "");
    	res.road_name = vals.get<std::string>("road_name", "");
    	res.xPos = vals.get<double>("x_pos", 0.0);
    	res.yPos = vals.get<double>("y_pos", 0.0);
    }
    static void to_base(const sim_mob::aimsun::BusStop& src, soci::values& vals, soci::indicator& ind)
    {
    	//std::cout<<"I am here"<<src.xPos<<"    "<<src.yPos<<std::endl;
    	vals.set("bus_stop_id", src.bus_stop_no);
    	vals.set("section_id", src.atSection->id);
    	vals.set("status", src.status);
    	vals.set("lane_type", src.lane_type);
    	vals.set("road_name", src.road_name);
        vals.set("x_pos", src.xPos);
        vals.set("y_pos", src.yPos);
        ind = i_ok;
    }
};

template<> struct type_conversion<sim_mob::aimsun::BusStopSG>
{
    typedef values base_type;
    static void from_base(const soci::values& vals, soci::indicator& ind, sim_mob::aimsun::BusStopSG &res)
    {
    	res.bus_stop_no = vals.get<std::string>("stop_id", "");
    	res.stop_code= vals.get<std::string>("stop_code", "");
    	res.stop_name = vals.get<std::string>("stop_name", "");
    	res.stop_lat = vals.get<std::string>("stop_lat", "");
    	res.stop_lon = vals.get<std::string>("stop_lon", "");
    	res.section_id = vals.get<std::string>("section_id", "");
    	res.aimsun_section = vals.get<int>("aimsun_section", 0);
    }
    static void to_base(const sim_mob::aimsun::BusStopSG& src, soci::values& vals, soci::indicator& ind)
    {
    	//std::cout<<"I am here"<<src.xPos<<"    "<<src.yPos<<std::endl;
    	vals.set("stop_id", src.bus_stop_no);
    	vals.set("stop_code", src.stop_code);
    	vals.set("stop_name", src.stop_name);
    	vals.set("stop_lat", src.xPos);
    	vals.set("stop_lon", src.yPos);
        vals.set("section_id", src.section_id);
        vals.set("aimsun_section", src.aimsun_section);
        ind = i_ok;
    }
};

template<> struct type_conversion<sim_mob::TurningSection>
{
    typedef values base_type;
    static void from_base(const soci::values& vals, soci::indicator& ind, sim_mob::TurningSection &res)
    {
    	res.setDbId(vals.get<int>("id", -1));
    	res.setFrom_xpos(vals.get<double>("from_xpos", -1.0));
    	res.setFrom_ypos(vals.get<double>("from_ypos", -1.0));
    	res.setTo_xpos(vals.get<double>("to_xpos", -1.0));
    	res.setTo_ypos(vals.get<double>("to_ypos", -1.0));
    	res.setFrom_road_section(vals.get<std::string>("from_road_section", ""));
    	res.setTo_road_section(vals.get<std::string>("to_road_section", ""));
    	res.setFrom_lane_index(vals.get<int>("from_lane_index", -1));
    	res.setTo_lane_index(vals.get<int>("to_lane_index", -1));
    }
    static void to_base(const sim_mob::TurningSection& src, soci::values& vals, soci::indicator& ind)
    {
    	vals.set("id", src.getDbId());
    	vals.set("from_xpos", src.getFrom_xpos());
    	vals.set("from_ypos", src.getFrom_ypos());
    	vals.set("to_xpos", src.getTo_xpos());
    	vals.set("to_ypos", src.getTo_ypos());
    	vals.set("from_road_section", src.getFrom_road_section());
    	vals.set("to_road_section", src.getTo_road_section());
    	vals.set("from_lane_index", src.getFrom_lane_index());
    	vals.set("to_lane_index", src.getTo_lane_index());
        ind = i_ok;
    }
};
template<> struct type_conversion<sim_mob::TurningConflict>
{
    typedef values base_type;
    static void from_base(const soci::values& vals, soci::indicator& ind, sim_mob::TurningConflict &res)
    {
    	res.setDbId(vals.get<int>("id", -1));
    	res.setFirst_turning(vals.get<std::string>("first_turning", ""));
    	res.setSecond_turning(vals.get<std::string>("second_turning", ""));
    	res.setFirst_cd(vals.get<double>("first_cd", -1.0));
    	res.setSecond_cd(vals.get<double>("second_cd", -1.0));
    }
    static void to_base(const sim_mob::TurningConflict& src, soci::values& vals, soci::indicator& ind)
    {
    	vals.set("id", src.getDbId());
    	vals.set("first_turning", src.getFirst_turning());
    	vals.set("second_turning", src.getSecond_turning());
    	vals.set("first_cd", src.getFirst_cd());
    	vals.set("second_cd", src.getSecond_cd());
        ind = i_ok;
    }
};
}
