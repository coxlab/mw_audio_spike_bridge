/*
 *  SpikeBridge.cpp
 *  SpikeBridge
 *
 *  Created by David Cox on 9/1/10.
 *  Copyright 2010 Harvard University. All rights reserved.
 *
 */

#include "SpikeBridge.h"
#include <boost/filesystem.hpp>
#include <MWorksCore/Scheduler.h>

namespace fs = boost::filesystem;

SpikeBridge::SpikeBridge(std::string _tag, std::string _url_root, shared_ptr<Variable> _spike_var):
    spike_variable(_spike_var), 
    message_ctx(1),
    stopping(false){ 

    
    // lookup which channels are defined in the _url_root
    fs::path base_path(_url_root);
    
    // shortcut for now:
    n_spike_channels = 32;
    vector<fs::path> channel_paths;
    for(int i = 0; i < n_spike_channels; i++){
        std::string ch_path = (boost::format("%d") % i).str();
    
        fs::path channel_path = base_path / fs::path(ch_path);
        
        channel_paths.push_back(channel_path);
    }
    
    
    // create sockets for each channel, and add to a poll set
    poll_items = new zmq_pollitem_t[n_spike_channels];
    
    for(int i = 0; i < n_spike_channels; i++){
        
        shared_ptr<zmq::socket_t> socket(new zmq::socket_t(message_ctx, ZMQ_SUB));
        sockets.push_back(socket);
        
        poll_items[i].socket = (void *)(socket.get());
        poll_items[i].events = ZMQ_POLLIN;

    }

}

SpikeBridge::SpikeBridge(const SpikeBridge &tocopy) :
    spike_variable(tocopy.spike_variable),
    message_ctx(1),
    n_spike_channels(tocopy.n_spike_channels),
    stopping(false){ 
    
    sockets = tocopy.sockets;
    
    poll_items = new zmq_pollitem_t[n_spike_channels];
    for( int p = 0; p < n_spike_channels; p++){
        poll_items[p] = tocopy.poll_items[p];
    }
}

SpikeBridge::~SpikeBridge(){ }

void *SpikeBridge::pollForSpikes(){

    shared_ptr<Clock> clock = Clock::instance();
    int rc;
    
    zmq::message_t msg;
    

    while(1){
    
        boost::mutex::scoped_lock lock(stopping_mutex);
        if(stopping){
            break;
        }
    
    
        /* Poll for events indefinitely */
        int rc = zmq::poll (poll_items, 2, -1);
        assert (rc >= 0);
        
        MWTime now = clock->getCurrentTimeUS();
        
        /* Returned events will be stored in items[].revents */
        for(int i = 0; i < n_spike_channels; i++){
            short revents = poll_items[i].revents;
            if(revents & ZMQ_POLLIN){
                rc = sockets[i]->recv(&msg, ZMQ_NOBLOCK); 
                spike_variable->setValue((long)i, now);
            }
        }
    }
    
    return NULL;
    
}    


bool SpikeBridge::startDeviceIO(){

    stopping = false;
    shared_ptr<Scheduler> scheduler = Scheduler::instance();
    
    scheduler->scheduleUS(FILELINE,
                          0, // delay
                          0, // repeat interval
                          1, // times to run
                          boost::bind(&SpikeBridge::pollForSpikes, 
                                      enable_shared_from_this<SpikeBridge>::shared_from_this()),
                          M_DEFAULT_PRIORITY,
                          (MWTime)0, // no timing warnings
                          M_DEFAULT_NETWORK_FAIL_SLOP_MS,
                          M_MISSED_EXECUTION_DROP);
                        
}

bool SpikeBridge::stopDeviceIO(){

    
    boost::mutex::scoped_lock lock(stopping_mutex);
    stopping = true;
    
}