/*
 *  SpikeBridgePlugin.cpp
 *  SpikeBridge
 *
 *  Created by David Cox on 9/1/10.
 *  Copyright 2010 Harvard University. All rights reserved.
 *
 */

#include "SpikeBridge.h"
#include "SpikeBridgePlugin.h"
#include "MWorksCore/ComponentFactory.h"
#include "SelfDescribingComponentFactory.h"


using namespace mw;

Plugin *getPlugin(){
    return new SpikeBridgePlugin();
}


void SpikeBridgePlugin::registerComponents(shared_ptr<mw::ComponentRegistry> registry) {
	
    std::string signature = SpikeBridge::parameters::getSignature();
    
    registry->registerFactory(signature,
							  (ComponentFactory *)(new SelfDescribingStimulusFactory<SpikeBridge>()));
}

