/*
 * Event.cpp
 *
 *  Created on: 04.03.2018
 *      Author: fabia
 */

#include "../../include/level1/events.hpp"

#include "../../../lib-common/include/collections.hpp"

static List<Event*>* eventList = new List<Event*>();

void registerEvent(char* name, bool kernelEvent) {
	Event* event = new Event(name, kernelEvent);

	eventList->add(event);
}

Event* getEventByName(char* name) {
	Iterator<Event*>* i = eventList->iterator();

	while(i->hasNext()) {
		Event* e = i->pop();

		if(!strcmp(e->eventName, name)) {
			return e;
		}
	}

	return nullptr;
}

void kernelEvent(EventData* data) {

}
