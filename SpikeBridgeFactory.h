/*
 *  SpikeBridgeFactory.h
 *  SpikeBridge
 *
 *  Created by David Cox on 9/1/10.
 *  Copyright 2010 Harvard University. All rights reserved.
 *
 */


#ifndef SpikeBridge_FACTORY_H
#define SpikeBridge_FACTORY_H

#include "SpikeBridge.h"

#include "MWorksCore/ComponentFactory.h"
using namespace mw;

class SpikeBridgeFactory : public ComponentFactory {
	virtual shared_ptr<mw::Component> createObject(std::map<std::string, std::string> parameters,
                                                   mw::ComponentRegistry *reg);
};

#endif
