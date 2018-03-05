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

struct EventListener {

};

class Event {
private:
	List<EventListener>* listeners;

public:
	char eventName[256];

	Event(char* name, bool kernelEvent) {
		listeners = nullptr;
		if(!kernelEvent) {
			listeners = new List<EventListener>();
		}
		strcpy(eventName, name);
	}
};

void	registerEvent(char* name, bool kernelEvent);
Event*	getEventByName(char* name);

#endif /* KERNEL_INCLUDE_LEVEL1_EVENTS_HPP_ */
