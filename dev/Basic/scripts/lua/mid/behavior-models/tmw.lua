--[[
Model - Mode choice for work tour to usual location
Type - NL
Authors - Siyu Li, Harish Loganathan
]]

-- all require statements do not work with C++. They need to be commented. The order in which lua files are loaded must be explicitly controlled in C++. 
--require "NLogit"

--Estimated values for all betas
--Note: the betas that not estimated are fixed to zero.

--!! see the documentation on the definition of AM,PM and OP table!!

local beta_cons_bus = -2.08
local beta_cons_mrt = -2.19
local beta_cons_privatebus = -2.71
local beta_cons_drive1 = 0
local beta_cons_share2 = -4.73
local beta_cons_share3 = -5.85
local beta_cons_motor = -8.51
local beta_cons_walk = -2.23
local beta_cons_taxi = -5.57

local beta1_1_tt = -0.344
local beta1_2_tt = -0.472
local beta1_3_tt = -1.10

local beta_private_1_tt = -0.344

local beta2_tt_drive1 = -0.432
local beta2_tt_share2 = -0.893
local beta2_tt_share3 = -0.913
local beta2_tt_motor = -0.815

local beta_tt_walk = -2.14
local beta_tt_taxi = -0.458

local beta4_1_cost = -10.5
local beta4_2_cost = -0.121
local beta5_1_cost = -11.1
local beta5_2_cost = -0.125
local beta6_1_cost = -6.74
local beta6_2_cost = -0.0545
local beta7_1_cost = -8.99
local beta7_2_cost = -0.0617
local beta8_1_cost = -8.76
local beta8_2_cost = -0.0704
local beta9_1_cost = -6.53
local beta9_2_cost = -0.0301
local beta10_1_cost = -5.17
local beta10_2_cost = -0.0382

local beta_cost_erp = 0 
local beta_cost_parking = 0

local beta_central_bus = 0.512
local beta_central_mrt = 0.815
local beta_central_privatebus = 0.330
local beta_central_share2 = 0.433
local beta_central_share3 = -0.128
local beta_central_motor = 0.474
local beta_central_taxi = 1.17
local beta_central_walk = 0.880

local beta_female_bus = 1.6
local beta_female_mrt = 1.57
local beta_female_privatebus = 1.54
local beta_female_drive1 = 0
local beta_female_share2 = 1.46
local beta_female_share3 = 1.08
local beta_female_motor = -2.85
local beta_female_taxi = 1.56
local beta_female_walk = 1.70

local beta_zero_drive1 = 0
local beta_oneplus_drive1 = 0
local beta_twoplus_drive1 = 1.36 
local beta_threeplus_drive1 = 0.915

local beta_zero_share2 = 0
local beta_oneplus_share2 = 2.84
local beta_twoplus_share2 = 0.547
local beta_threeplus_share2 = 0.802

local beta_zero_share3 = 0
local beta_oneplus_share3 = 2.01
local beta_twoplus_share3 = 0.400
local beta_threeplus_share3 = 0

local beta_zero_motor = 0
local beta_oneplus_motor = 8.71
local beta_twoplus_motor = 0.757
local beta_threeplus_motor = 0.0263

local beta_transfer = -0.0329

local beta_distance = 0
local beta_residence = 0.0190
local beta_residence_2 = 0
local beta_attraction = -0.0162 
local beta_attraction_2 = 0 


--choice set
-- 1 for public bus; 2 for MRT/LRT; 3 for private bus; 4 for drive1;
-- 5 for shared2; 6 for shared3+; 7 for motor; 8 for walk; 9 for taxi
local choice = {}
choice["PT"] = {1,2,3}
choice["non-PT"] = {4,5,6,7,8,9}


--utility
-- 1 for public bus; 2 for MRT/LRT; 3 for private bus; 4 for drive1;
-- 5 for shared2; 6 for shared3+; 7 for motor; 8 for walk; 9 for taxi
local utility = {}
local function computeUtilities(params,dbparams)
	
	--dbparams.cost_public_first = AM[(origin,destination)]['pub_cost']
	--origin is home, destination is tour destination
	--0 if origin == destination
	local cost_public_first = dbparams.cost_public_first

	--dbparams.cost_public_second = PM[(destination,origin)]['pub_cost']
	--origin is home, destination is tour destination
	--0 if origin == destination
	local cost_public_second = dbparams.cost_public_second

	local cost_bus=cost_public_first+cost_public_second
	local cost_mrt=cost_public_first+cost_public_second
	local cost_privatebus=cost_public_first+cost_public_second

	--dbparams.cost_car_ERP_first = AM[(origin,destination)]['car_cost_erp']
	--dbparams.cost_car_ERP_second = PM[(destination,origin)]['car_cost_erp']
	--dbparams.cost_car_OP_first = AM[(origin,destination)]['distance']*0.147
	--dbparams.cost_car_OP_second = PM[(destination,origin)]['distance']*0.147
	--dbparams.cost_car_parking = 8 * ZONE[destination]['parking_rate']
	--for the above 5 variables, origin is home, destination is tour destination
	--0 if origin == destination
	local cost_car_ERP_first = dbparams.cost_car_ERP_first
	local cost_car_ERP_second = dbparams.cost_car_ERP_second
	local cost_car_OP_first = dbparams.cost_car_OP_first
	local cost_car_OP_second = dbparams.cost_car_OP_second
	local cost_car_parking = dbparams.cost_car_parking

	local cost_cardriver=cost_car_ERP_first+cost_car_ERP_second+cost_car_OP_first+cost_car_OP_second+cost_car_parking
	local cost_carpassenger=cost_car_ERP_first+cost_car_ERP_second+cost_car_OP_first+cost_car_OP_second+cost_car_parking
	local cost_motor=0.5*(cost_car_ERP_first+cost_car_ERP_second+cost_car_OP_first+cost_car_OP_second)+0.65*cost_car_parking

	--dbparams.walk_distance1= AM[(origin,destination)]['AM2dis']
	--origin is home mtz, destination is usual work location mtz
	--0 if origin == destination
	--dbparams.walk_distance2= PM[(destination,origin)]['PM2dis']
	--origin is home mtz, destination is usual work location mtz
	--0 if origin == destination
	local d1 = dbparams.walk_distance1
	local d2 = dbparams.walk_distance2

	--dbparams.central_dummy=ZONE[destination]['central_dummy']
	--destination is tour destination
	local central_dummy = dbparams.central_dummy
	
	local female_dummy = params.female_dummy
	local income_id = params.income_id
	local income_cat = {500,1250,1750,2250,2750,3500,4500,5500,6500,7500,8500,0,99999,99999}
	local income_mid = income_cat[income_id]
	local missing_income = (params.income_id >= 13) and 1 or 0

	local cost_taxi_1=3.4+((d1*(d1>10 and 1 or 0)-10*(d1>10 and 1 or 0))/0.35+(d1*(d1<=10 and 1 or 0)+10*(d1>10 and 1 or 0))/0.4)*0.22+ cost_car_ERP_first + central_dummy*3
	local cost_taxi_2=3.4+((d2*(d2>10 and 1 or 0)-10*(d2>10 and 1 or 0))/0.35+(d2*(d2<=10 and 1 or 0)+10*(d2>10 and 1 or 0))/0.4)*0.22+ cost_car_ERP_second + central_dummy*3
	local cost_taxi=cost_taxi_1+cost_taxi_2

	local cost_over_income_bus=30*cost_bus/(0.5+income_mid)
	local cost_over_income_mrt=30*cost_mrt/(0.5+income_mid)
	local cost_over_income_privatebus=30*cost_privatebus/(0.5+income_mid)
	local cost_over_income_cardriver=30*cost_cardriver/(0.5+income_mid)
	local cost_over_income_carpassenger=30*cost_carpassenger/(0.5+income_mid)
	local cost_over_income_motor=30*cost_motor/(0.5+income_mid)
	local cost_over_income_taxi=30*cost_taxi/(0.5+income_mid)

	--dbparams.tt_public_ivt_first = AM[(origin,destination)]['pub_ivt']
	--dbparams.tt_public_ivt_second = PM[(destination,origin)]['pub_ivt']
	--dbparams.tt_public_waiting_first = AM[(origin,destination)]['pub_wtt']
	--dbparams.tt_public_waiting_second = PM[(destination,origin)]['pub_wtt']
	--dbparams.tt_public_walk_first = AM[(origin,destination)]['pub_walkt']
	--dbparams.tt_public_walk_second = PM[(destination,origin)]['pub_walkt']
	--for the above 6 variables, origin is home, destination is tour destination
	--0 if origin == destination
	local tt_public_ivt_first = dbparams.tt_public_ivt_first
	local tt_public_ivt_second = dbparams.tt_public_ivt_second
	local tt_public_waiting_first = dbparams.tt_public_waiting_first
	local tt_public_waiting_second = dbparams.tt_public_waiting_second
	local tt_public_walk_first =  dbparams.tt_public_walk_first
	local tt_public_walk_second = dbparams.tt_public_walk_second

	--dbparams.tt_ivt_car_first = AM[(origin,destination)]['car_ivt']
	--dbparams.tt_ivt_car_second = PM[(destination,origin)]['car_ivt']
	local tt_ivt_car_first = dbparams.tt_ivt_car_first
	local tt_ivt_car_second = dbparams.tt_ivt_car_second

	local tt_bus_ivt=tt_public_ivt_first+tt_public_ivt_second
	local tt_bus_wait=tt_public_waiting_first+tt_public_waiting_second
	local tt_bus_walk=tt_public_walk_first+tt_public_walk_second
	local tt_bus_all=tt_bus_ivt+tt_bus_wait+tt_bus_walk

	local tt_mrt_ivt=tt_public_ivt_first+tt_public_ivt_second
	local tt_mrt_wait=tt_public_waiting_first+tt_public_waiting_second
	local tt_mrt_walk=tt_public_walk_first+tt_public_walk_second
	local tt_mrt_all=tt_mrt_ivt+tt_mrt_wait+tt_mrt_walk

	local tt_privatebus_ivt=tt_ivt_car_first+tt_ivt_car_second
	local tt_privatebus_wait=tt_public_waiting_first+tt_public_waiting_second
	local tt_privatebus_walk=tt_public_walk_first+tt_public_walk_second
	local tt_privatebus_all=tt_privatebus_ivt+tt_privatebus_wait+tt_privatebus_walk

	local tt_cardriver_ivt=tt_ivt_car_first+tt_ivt_car_second
	local tt_cardriver_out=1.0/6
	local tt_cardriver_all=tt_cardriver_ivt+tt_cardriver_out

	local tt_carpassenger_ivt=tt_ivt_car_first+tt_ivt_car_second
	local tt_carpassenger_out=1.0/6
	local tt_carpassenger_all=tt_carpassenger_ivt+tt_carpassenger_out

	local tt_motor_ivt=tt_ivt_car_first+tt_ivt_car_second
	local tt_motor_out=1.0/6
	local tt_motor_all=tt_motor_ivt+tt_motor_out

	local tt_walk=(d1+d2)/5

	local tt_taxi_ivt=tt_ivt_car_first+tt_ivt_car_second
	local tt_taxi_out=1.0/6
	local tt_taxi_all=tt_cardriver_ivt+tt_cardriver_out

	--dbparams.average_transfer_number = (AM[(origin,destination)]['avg_transfer'] + PM[(destination,origin)]['avg_transfer'])/2
	--origin is home, destination is tour destination
	-- 0 if origin == destination
	local average_transfer_number = dbparams.average_transfer_number

	--params.car_own_normal is from household table
	local zero_car = params.car_own_normal == 0 and 1 or 0
	local one_plus_car = params.car_own_normal >= 1 and 1 or 0
	local two_plus_car = params.car_own_normal >= 2 and 1 or 0
	local three_plus_car = params.car_own_normal >= 3 and 1 or 0

	--params.motor_own is from household table
	local zero_motor = params.motor_own == 0 and 1 or 0
	local one_plus_motor = params.motor_own >=1 and 1 or 0
	local two_plus_motor = params.motor_own >=2 and 1 or 0
	local three_plus_motor = params.motor_own >= 3 and 1 or 0

	--dbparams.resident_size = ZONE[origin]['resident workers']
	--dbparams.work_op = ZONE[destination]['employment'] --total employment 
	--dbparams.origin_area= ZONE[origin]['area'] -- in square km 
	--dbparams.destination_area = ZONE[destination]['area'] -- in square km
	--origin is home, destination is tour destination
	local resident_size = dbparams.resident_size
	local work_op = dbparams.work_op
	local origin_area = dbparams.origin_area
	local destination_area = dbparams.destination_area

	local residential_size=resident_size/origin_area/10000.0
	local work_attraction=work_op/destination_area/10000.0

	utility[1] = beta_cons_bus + beta1_1_tt * tt_bus_ivt + beta1_2_tt * tt_bus_walk + beta1_3_tt * tt_bus_wait + beta4_1_cost * cost_over_income_bus * (1-missing_income) + beta4_2_cost * cost_bus * missing_income + beta_central_bus * central_dummy + beta_transfer * average_transfer_number + beta_female_bus * female_dummy
	utility[2] = beta_cons_mrt + beta1_1_tt * tt_mrt_ivt + beta1_2_tt * tt_mrt_walk + beta1_3_tt * tt_mrt_wait + beta4_1_cost * cost_over_income_mrt * (1-missing_income) + beta4_2_cost * cost_mrt * missing_income + beta_central_mrt * central_dummy + beta_transfer * average_transfer_number + beta_female_mrt * female_dummy
	utility[3] = beta_cons_privatebus + beta_private_1_tt * tt_privatebus_ivt + beta5_1_cost * cost_over_income_privatebus * (1-missing_income) + beta5_2_cost * cost_privatebus * missing_income + beta_central_privatebus * central_dummy + beta_distance*(d1+d2) + beta_residence * residential_size + beta_attraction * work_attraction + beta_residence_2*math.pow(residential_size,2)+beta_attraction_2*math.pow(work_attraction,2)+beta_female_privatebus* female_dummy
	utility[4] = beta_cons_drive1 + beta2_tt_drive1 * tt_cardriver_all + beta6_1_cost * cost_over_income_cardriver * (1-missing_income) + beta6_2_cost * cost_cardriver * missing_income + beta_female_drive1 * female_dummy + beta_zero_drive1 * zero_car + beta_oneplus_drive1 * one_plus_car + beta_twoplus_drive1 * two_plus_car + beta_threeplus_drive1 * three_plus_car
	utility[5] = beta_cons_share2 + beta2_tt_share2 * tt_carpassenger_all + beta7_1_cost * cost_over_income_carpassenger/2 * (1-missing_income) + beta7_2_cost * cost_carpassenger/2 * missing_income  + beta_central_share2 * central_dummy + beta_female_share2 * female_dummy + beta_zero_share2 * zero_car + beta_oneplus_share2 * one_plus_car + beta_twoplus_share2 * two_plus_car + beta_threeplus_share2 * three_plus_car
	utility[6] = beta_cons_share3 + beta2_tt_share3 * tt_carpassenger_all + beta8_1_cost * cost_over_income_carpassenger/3 * (1-missing_income) + beta8_2_cost * cost_carpassenger/3 * missing_income  + beta_central_share3 * central_dummy + beta_female_share3 * female_dummy + beta_zero_share3 * zero_car + beta_oneplus_share3 * one_plus_car + beta_twoplus_share3 * two_plus_car + beta_threeplus_share3 * three_plus_car
	utility[7] = beta_cons_motor + beta2_tt_motor * tt_motor_all + beta9_1_cost * cost_over_income_motor * (1-missing_income) + beta9_2_cost * cost_motor * missing_income  + beta_central_motor * central_dummy + beta_zero_motor * zero_motor + beta_oneplus_motor * one_plus_motor + beta_twoplus_motor * two_plus_motor + beta_threeplus_motor * three_plus_motor + beta_female_motor * female_dummy
	utility[8] = beta_cons_walk  + beta_tt_walk * tt_walk + beta_central_walk * central_dummy+ beta_female_walk * female_dummy
	utility[9] = beta_cons_taxi + beta_tt_taxi * tt_taxi_all + beta10_1_cost * cost_over_income_taxi * (1-missing_income) + beta10_2_cost * cost_taxi * missing_income + beta_central_taxi * central_dummy + beta_female_taxi * female_dummy

end



--availability
--the logic to determine availability is the same with current implementation
local availability = {}
local function computeAvailabilities(params,dbparams)
	availability = {
		dbparams.publicbus_AV,
		dbparams.mrt_AV,
		dbparams.privatebus_AV,
		dbparams.drive1_AV,
		dbparams.share2_AV,
		dbparams.share3_AV,
		dbparams.motor_AV,
		dbparams.walk_AV,
		dbparams.taxi_AV
	}
end

--scale
local scale = {}
scale["PT"] = 3.42
scale["non-PT"] = 1

-- function to call from C++ preday simulator
-- params and dbparams tables contain data passed from C++
-- to check variable bindings in params or dbparams, refer PredayLuaModel::mapClasses() function in dev/Basic/medium/behavioral/lua/PredayLuaModel.cpp
function choose_tmw(params,dbparams)
	computeUtilities(params,dbparams) 
	computeAvailabilities(params,dbparams)
	local probability = calculate_probability("nl", choice, utility, availability, scale)
	return make_final_choice(probability)
end
