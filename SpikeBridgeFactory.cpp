/*
 *  SpikeBridgeFactory.cpp
 *  SpikeBridge
 *
 *  Created by David Cox on 9/1/10.
 *  Copyright 2010 Harvard University. All rights reserved.
 *
 */

#include "SpikeBridgeFactory.h"

#include <boost/regex.hpp>
#include "MWorksCore/ComponentRegistry.h"

using namespace mw;

shared_ptr<mw::Component> SpikeBridgeFactory::createObject(std::map<std::string, std::string> parameters,
                                                               mw::ComponentRegistry *reg) {
	REQUIRE_ATTRIBUTES(parameters, 
					   "tag",
                       "another_attribute");
	
	std::string tagname(parameters["tag"]);

    
	shared_ptr<Variable> another_attribute = reg->getVariable(parameters["another_attribute"]);	
	
	shared_ptr <SpikeBridge> new_component = shared_ptr<SpikeBridge>(new SpikeBridge(tagname, another_attribute));
	
	return new_component;
}
