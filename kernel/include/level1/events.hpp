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
	List<EventListener>* listeners;
	char eventName[256];

	Event(char* name) {
		listeners = new List<EventListener>();
		strcpy(eventName, name);
	}
};

#endif /* KERNEL_INCLUDE_LEVEL1_EVENTS_HPP_ */
