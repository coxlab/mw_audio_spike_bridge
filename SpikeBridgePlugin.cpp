/*
 *  SpikeBridgePlugin.cpp
 *  SpikeBridge
 *
 *  Created by David Cox on 9/1/10.
 *  Copyright 2010 Harvard University. All rights reserved.
 *
 */

#include "SpikeBridgePlugin.h"
#include "SpikeBridgeFactory.h"
#include "MWorksCore/ComponentFactory.h"
using namespace mw;

Plugin *getPlugin(){
    return new SpikeBridgePlugin();
}


void SpikeBridgePlugin::registerComponents(shared_ptr<mw::ComponentRegistry> registry) {
	
    // TODO: you need to customize the "signature" of the object your plugin will create
    //       The signature is of the form component/type Ð(e.g. stimulus/circle, or iodevice/NIDAQ)
    registry->registerFactory(std::string("stimulus/SpikeBridge"),
							  (ComponentFactory *)(new SpikeBridgeFactory()));
}

