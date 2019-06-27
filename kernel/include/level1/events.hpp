/*
 * Event.h
 *
 *  Created on: 04.03.2018
 *      Author: fabia
 */

#ifndef KERNEL_INCLUDE_LEVEL1_EVENTS_HPP_
#define KERNEL_INCLUDE_LEVEL1_EVENTS_HPP_

#include "collections.hpp"
#include "cstring.h"
#include "scheduler.h"

struct EventListener {
	Module* assignedModule;
	uint32_t rpcID;
};

class Event;

Event*	registerEvent(char* name, bool kernelEvent);
void    registerKernelEvents();
Event*	getEventByName(char* name);
void 	  kernelEvent(Event* e, void* data, size_t size);

class Event {
private:
	List<EventListener>* listeners;

public:
	char eventName[256];

	Event(char* name, bool kernelEvent);
	void callListeners(void* object, size_t size);
};

#endif /* KERNEL_INCLUDE_LEVEL1_EVENTS_HPP_ */
