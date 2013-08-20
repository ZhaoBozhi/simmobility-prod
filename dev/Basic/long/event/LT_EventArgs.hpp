/* 
 * Copyright Singapore-MIT Alliance for Research and Technology
 * 
 * File:   LT_EventArgs.h
 * Author: Pedro Gandola <pedrogandola@smart.mit.edu>
 *
 * Created on April 4, 2013, 5:42 PM
 */
#pragma once
#include "event/args/EventArgs.hpp"
#include "event/EventListener.hpp"
#include "database/entity/Bid.hpp"

namespace sim_mob {

    namespace long_term {

        DECLARE_CUSTOM_CALLBACK_TYPE(HM_ActionEventArgs)
        class HM_ActionEventArgs : public sim_mob::event::EventArgs {
        public:
            HM_ActionEventArgs(UnitId unitId);
            HM_ActionEventArgs(const HM_ActionEventArgs& orig);
            virtual ~HM_ActionEventArgs();

            /**
             * Gets the unit affected by the action.
             * @return 
             */
            const UnitId GetUnitId()const;
        private:
            UnitId unitId;
        };
    }
}

