/*
 *  SpikeBridge.cpp
 *  SpikeBridge
 *
 *  Created by David Cox on 9/1/10.
 *  Copyright 2010 Harvard University. All rights reserved.
 *
 */

#include "SpikeBridge.h"

#include <MWorksCore/Scheduler.h>

namespace fs = boost::filesystem;

SpikeBridge::SpikeBridge(std::string _tag, std::string _url_root, shared_ptr<Variable> _spike_var):
    spike_variable(_spike_var), 
    message_ctx(1),  // 1 thread
    stopping(false){ 

    
    // lookup which channels are defined in the _url_root
    fs::path base_path(_url_root);
    
    // shortcut for now:
    n_spike_channels = 2;
    
    for(int i = 0; i < n_spike_channels; i++){
        std::string ch_path = (boost::format("%d") % i).str();
    
        fs::path channel_path = base_path / fs::path(ch_path);
        
        channel_paths.push_back(channel_path);
        
        std::cerr << "Queuing up channel: " << channel_path.string() << std::endl;
    }
    
    initSockets(message_ctx);
    
    for(int i = 0; i < n_spike_channels; i++){
        
        shared_ptr<zmq::socket_t> socket = sockets[i];
        
        fs::path channel_path = channel_paths[i];
        string channel_url_path("ipc://");
        channel_url_path += channel_path.string();
        const char *url_path = channel_url_path.c_str();
        socket->connect(url_path);
    }
    
}


void SpikeBridge::initSockets(zmq::context_t& message_ctx){
    // create sockets for each channel, and add to a poll set
    poll_items = new zmq_pollitem_t[n_spike_channels];
    
    for(int i = 0; i < n_spike_channels; i++){
        
        shared_ptr<zmq::socket_t> socket(new zmq::socket_t(message_ctx, ZMQ_SUB));
        socket->setsockopt(ZMQ_SUBSCRIBE, "", 0);
        sockets.push_back(socket);
        
        //fs::path channel_path = channel_paths[i];
        //        string channel_url_path("ipc://");
        //        channel_url_path += channel_path.string();
        //        const char *url_path = channel_url_path.c_str();
        //        socket->connect(url_path);
        
        
        // oh you zeromq developers, you cheeky bastards with your
        // tricksy operator overloads...
        poll_items[i].socket = (void *)(*(socket.get()));
        poll_items[i].fd = 0;
        poll_items[i].events = ZMQ_POLLIN;
        poll_items[i].revents = 0;
    }
}    

SpikeBridge::SpikeBridge(const SpikeBridge &tocopy) :
    spike_variable(tocopy.spike_variable),
    message_ctx(1),
    n_spike_channels(tocopy.n_spike_channels),
    stopping(false){ 
    
    channel_paths = tocopy.channel_paths;
    
    //initSockets();
    
//    poll_items = new zmq_pollitem_t[n_spike_channels];
//    for( int p = 0; p < n_spike_channels; p++){
//        poll_items[p] = tocopy.poll_items[p];
//    }
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
        
        int rc;
        
        try {
            rc = zmq::poll (poll_items, n_spike_channels, -1);
        
        } catch (zmq::error_t& e){
            std::cerr << e.what() << std::endl;
            break;
        }

        MWTime now = clock->getCurrentTimeUS();
        
        /* Returned events will be stored in items[].revents */
        for(int i = 0; i < n_spike_channels; i++){
            short revents = poll_items[i].revents;
            if(revents & ZMQ_POLLIN){
                rc = sockets[i]->recv(&msg, ZMQ_NOBLOCK);
                std::cerr << "Spike on channel " << i << std::endl;
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
                                      component_shared_from_this<SpikeBridge>()),
                          M_DEFAULT_PRIORITY,
                          (MWTime)0, // no timing warnings
                          M_DEFAULT_NETWORK_FAIL_SLOP_MS,
                          M_MISSED_EXECUTION_DROP);
                        
}

bool SpikeBridge::stopDeviceIO(){

    
    boost::mutex::scoped_lock lock(stopping_mutex);
    stopping = true;
    
}