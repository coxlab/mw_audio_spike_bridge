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
#include <MWorksCore/IODevice.h>
#include <zmq.hpp>
#include "mw_parameters.h"

using namespace mw;

class SpikeBridge : public mw::IODevice {

public:

    // -----------------------------------------------------------------------
    BEGIN_MW_PARAMETERS
    
    MW_SIGNATURE("iodevice/spike_bridge");
    
    MW_PARAMETER(tag,               string,           "Spike Bridge");
    MW_PARAMETER(spike_variable,    VariablePtr,      "spike");
    MW_PARAMETER(spike_path,        string,           "/tmp/spike_channels");
    
    END_MW_PARAMETERS
    
    static shared_ptr<Component> createComponent(MW_PARAMETERS p){
        SpikeBridge *bridge = new SpikeBridge(p->tag,
                                                  p->spike_path,
                                                  p->spike_variable);
        shared_ptr<SpikeBridge> b(bridge);
        return b;
    }   
    // -----------------------------------------------------------------------


protected:

    // variable where the spikes will be "posted"
    shared_ptr<Variable> spike_variable;
    
    // 0mq stuff
    zmq::context_t message_ctx;
    vector< shared_ptr<zmq::socket_t> > sockets;
    zmq_pollitem_t *poll_items;
    
    int n_spike_channels;

public:
	SpikeBridge(std::string _tag, std::string _url_root, shared_ptr<Variable> _spike_variable);
	SpikeBridge(const SpikeBridge &tocopy);
	~SpikeBridge();
    
    virtual void pollForSpikes();
    
    virtual bool startDeviceIO();
    virtual bool stopDeviceIO();

};

#endif 
