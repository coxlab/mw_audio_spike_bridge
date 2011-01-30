/*
 *  SpikeBridgePlugin.h
 *  SpikeBridge
 *
 *  Created by David Cox on 9/1/10.
 *  Copyright 2010 Harvard University. All rights reserved.
 *
 */
#ifndef SpikeBridge_PLUGIN_H_
#define SpikeBridge_PLUGIN_H_

#include <MWorksCore/Plugin.h>
using namespace mw;

extern "C"{
    Plugin *getPlugin();
}

class SpikeBridgePlugin : public Plugin {
    
	virtual void registerComponents(shared_ptr<mw::ComponentRegistry> registry);	
};


#endif