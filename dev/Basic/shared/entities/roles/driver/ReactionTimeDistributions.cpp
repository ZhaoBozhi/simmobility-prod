/*
 * ReactionTimeDistributions.cpp
 *
 *  Created on: Jun 1, 2012
 *      Author: lzm
 */

#include "ReactionTimeDistributions.hpp"

using namespace sim_mob;

GenType ReactionTimeDistributions::gt1;
GenType ReactionTimeDistributions::gt2;
NormalDis* ReactionTimeDistributions::normal_dis1;
NormalDis* ReactionTimeDistributions::normal_dis2;
LognormalDis* ReactionTimeDistributions::lognormal_dis1;
LognormalDis* ReactionTimeDistributions::lognormal_dis2;
RNG_Normal* ReactionTimeDistributions::rng_normal1;
RNG_Normal* ReactionTimeDistributions::rng_normal2;
RNG_Lognormal* ReactionTimeDistributions::rng_lognormal1;
RNG_Lognormal* ReactionTimeDistributions::rng_lognormal2;
size_t ReactionTimeDistributions::distributionType1;
size_t ReactionTimeDistributions::distributionType2;
size_t ReactionTimeDistributions::mean1;
size_t ReactionTimeDistributions::mean2;
size_t ReactionTimeDistributions::standardDev1;
size_t ReactionTimeDistributions::standardDev2;
ReactionTimeDistributions ReactionTimeDistributions::instance_;

sim_mob::ReactionTimeDistributions::~ReactionTimeDistributions()
{
	// TODO Auto-generated destructor stub
}

void sim_mob::ReactionTimeDistributions::setupDistribution1()
{
	switch(distributionType1)
	{
	case 0:
		setupNormalDisforRact1();
		break;
	case 1:
		setupLognormalDisforRact1();
		break;
	default:
		break;
	}
}

void sim_mob::ReactionTimeDistributions::setupDistribution2()
{
	switch(distributionType2)
	{
	case 0:
		setupNormalDisforRact2();
		break;
	case 1:
		setupLognormalDisforRact2();
		break;
	default:
		break;
	}
}

void sim_mob::ReactionTimeDistributions::setupNormalDisforRact1()
{
	normal_dis1 = new NormalDis(mean1, standardDev1);
	rng_normal1 = new RNG_Normal(gt1, *normal_dis1);
}

void sim_mob::ReactionTimeDistributions::setupNormalDisforRact2()
{
	normal_dis2 = new NormalDis(mean2, standardDev2);
	rng_normal2 = new RNG_Normal(gt2, *normal_dis2);
}

void sim_mob::ReactionTimeDistributions::setupLognormalDisforRact1()
{
	lognormal_dis1 = new LognormalDis(mean1, standardDev1);
	rng_lognormal1 = new RNG_Lognormal(gt1, *lognormal_dis1);
}

void sim_mob::ReactionTimeDistributions::setupLognormalDisforRact2()
{
	lognormal_dis2 = new LognormalDis(mean2, standardDev2);
	rng_lognormal2 = new RNG_Lognormal(gt2, *lognormal_dis2);
}

size_t sim_mob::ReactionTimeDistributions::reactionTime1()
{
	switch(distributionType1)
	{
	case 0:
		return rng_normal1->operator ()();
	case 1:
		return rng_lognormal1->operator ()();
	default:
		return 0;
	}
}

size_t sim_mob::ReactionTimeDistributions::reactionTime2()
{
	switch(distributionType2)
	{
	case 0:
		return rng_normal2->operator ()();
	case 1:
		return rng_lognormal2->operator ()();
	default:
		return 0;
	}
}
