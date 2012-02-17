/* Copyright Singapore-MIT Alliance for Research and Technology */

/*
 * \file Vehicle.hpp
 *
 * \author Li Zhemin
 * \author Seth N. Hetu
 * \author Zhang Shuai
 * \author Xu Yan
 */

#pragma once

#include <stdexcept>
#include <sstream>

#include "util/MovementVector.hpp"
#include "util/DynamicVector.hpp"
#include "entities/roles/driver/GeneralPathMover.hpp"


namespace sim_mob {

#ifndef SIMMOB_DISABLE_MPI
class PackageUtils;
class UnPackageUtils;
#endif

class Vehicle {
public:
	Vehicle(std::vector<sim_mob::WayPoint> wp_path, int startLaneID);
	Vehicle(std::vector<sim_mob::WayPoint> wp_path, int startLaneID, double length, double width); //TODO: now that the constructor is non-default, we might be able to remove throw_if_error()
	Vehicle();  //There is no wpPoint to initialize one Vehicle when crossing
	Vehicle(const Vehicle& copy); ///<Copy constructor

	//Enable polymorphism
	virtual ~Vehicle(){}

public:
	const double length;  ///<length of the vehicle
	const double width;   ///<width of the vehicle

	//Call once
	void initPath(std::vector<sim_mob::WayPoint> wp_path, int startLaneID);

	//Accessors
	double getX() const;   ///<Retrieve the vehicle's absolute position, x
	double getY() const;   ///<Retrieve the vehicle's absolute position, y
	double getDistanceMovedInSegment() const;   ///<Retrieve the total distance moved in this segment so far.
	//double getCurrLaneLength() const; ///<Get the total length of this RoadSegment.

	double getLateralMovement() const;         ///<Retrieve a value representing how far to the LEFT of the current lane the vehicle has moved.
	double getVelocity() const;      ///<Retrieve forward velocity.
	double getLatVelocity() const;   ///<Retrieve lateral velocity.
	double getAcceleration() const;  ///<Retrieve forward acceleration.
	//bool reachedSegmentEnd() const;  ///<Return true if we've reached the end of the current segment.
	bool isInIntersection() const;   ///<Are we now in an intersection?
	bool isDone() const; ///<Are we fully done with our path?
	bool hasPath() const; ///<Do we have a path to move on?

	//Special
	double getAngle() const;  ///<For display purposes only.

	//Helper method; used in BusDriver
	const std::vector<const sim_mob::RoadSegment*>& getCompletePath() const;

	//More stuff; some might be optional.
	const sim_mob::RoadSegment* getCurrSegment() const;
	const sim_mob::RoadSegment* getNextSegment(bool inSameLink=true) const;
	const sim_mob::RoadSegment* getPrevSegment() const;
	const sim_mob::RoadSegment* hasNextSegment(bool inSameLink) const;
	sim_mob::DynamicVector getCurrPolylineVector() const;
	const sim_mob::Link* getCurrLink() const;
	const sim_mob::Lane* getCurrLane() const;
	const sim_mob::Node* getNodeMovingTowards() const;
	const sim_mob::Node* getNodeMovingFrom() const;
	double getCurrLinkLaneZeroLength() const;
	double getAllRestRoadSegmentsLength() const;
	double getCurrLinkReportedLength() const;
	void shiftToNewLanePolyline(bool moveLeft);
	void moveToNewLanePolyline(int laneID);
	void setPositionInIntersection(double x, double y);

	//Modifiers
	void setVelocity(double value);      ///<Set the forward velocity.
	void setLatVelocity(double value);   ///<Set the lateral velocity.
	void setAcceleration(double value);  ///<Set the forward acceleration.
	double moveFwd(double amt);            ///<Move this car forward. Automatically moved it to new Segments unless it's in an intersection.
	void moveLat(double amt);            ///<Move this car laterally. NOTE: This will _add_ the amt to the current value.
	void resetLateralMovement();         ///<Put this car back in the center of the current lane.
	const Lane* moveToNextSegmentAfterIntersection();   ///<If we're in an intersection, move out of it.

	//Complex
	//void newPolyline(sim_mob::Point2D firstPoint, sim_mob::Point2D secondPoint)

	//Temporary; workaround
	//const DynamicVector& TEMP_retrieveFwdVelocityVector();


//private:
	//temp
public:
	//Trying a slightly more dynamic moving model.
	GeneralPathMover fwdMovement;
	double latMovement;
	double fwdVelocity;
	double latVelocity;
	double fwdAccel;

	//Override for when we're in an intersection.
	DPoint posInIntersection;


	DPoint getPosition() const;


	//NOTE: The error state is a temporary sanity check to help me debug this class. There are certainly
	//      better ways to handle this (e.g., non-default constructor).
	bool error_state;
	void throw_if_error() const {
		if (error_state) {
			throw std::runtime_error("Error: can't perform certain actions on an uninitialized vehicle.");
		}
	}

	//Helper function: Reposition and take care of any remaining velocity.
	/*void repositionAndScale(DynamicVector& item, const DynamicVector& newHeading) {
		double oldMag = item.getMagnitude();
		item = newHeading;
		item.scaleVectTo(oldMag);
	}*/

#ifndef SIMMOB_DISABLE_MPI
public:
	friend class sim_mob::PackageUtils;
	friend class sim_mob::UnPackageUtils;
#endif
};

} // namespace sim_mob
