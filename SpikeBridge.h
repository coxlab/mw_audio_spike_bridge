/*
 *  SpikeBridge.h
 *  SpikeBridge
 *
 *  Created by David Cox on 9/1/10.
 *  Copyright 2010 Harvard University. All rights reserved.
 *
 */

#ifndef SpikeBridge_H_
#define SpikeBridge_H_

#include <MWorksCore/Plugin.h>
#include <zmq.hpp>

using namespace mw;

class SpikeBridge : public mw::Component {

protected:

    // variable where the spikes will be "posted"
    shared_ptr<Variable> spike_variable;
    
    // a collection of socket
    vector< zmq::socket_t > sockets;

public:
	SpikeBridge(std::string _tag, std::string _url_root, shared_ptr<Variable> _spike_variable);
	SpikeBridge(const SpikeBridge &tocopy);
	~SpikeBridge();

};

#endif 
