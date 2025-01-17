/* 
 * Copyright Singapore-MIT Alliance for Research and Technology
 * 
 * File:   LuaProxy.cpp
 * Author: Pedro Gandola <pedrogandola@smart.mit.edu>
 * 
 * Created on October 9, 2013, 4:39 PM
 */

#include "LuaProvider.hpp"
#include <boost/thread/thread.hpp>
#include <boost/thread/tss.hpp>

using namespace sim_mob;
using namespace sim_mob::long_term;
using namespace sim_mob::lua;

namespace {

    struct ModelContext {
        HM_LuaModel hmModel;
        ExternalEventsModel exEventsModel;
        DeveloperLuaModel developerModel;
    };

    boost::thread_specific_ptr<ModelContext> threadContext;

    void ensureContext() {
        if (!threadContext.get()) {
            ModelContext* modelCtx = new ModelContext();
            modelCtx->hmModel.loadDirectory(HM_LUA_DIR);
            modelCtx->hmModel.initialize();
            modelCtx->exEventsModel.loadDirectory(EX_EV_LUA_DIR);
            modelCtx->exEventsModel.initialize();
            modelCtx->developerModel.loadDirectory(DEV_LUA_DIR);
            modelCtx->developerModel.initialize();
            threadContext.reset(modelCtx);
        }
    }
}

const HM_LuaModel& LuaProvider::getHM_Model() {
    ensureContext();
    return threadContext.get()->hmModel;
}

const ExternalEventsModel& LuaProvider::getExternalEventsModel(){
    ensureContext();
    return threadContext.get()->exEventsModel;
}

const DeveloperLuaModel& LuaProvider::getDeveloperModel(){
    ensureContext();
    return threadContext.get()->developerModel;
}