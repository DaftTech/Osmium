/*
 * Event.cpp
 *
 *  Created on: 04.03.2018
 *      Author: fabia
 */

#include "level1/events.hpp"
#include "level1/scheduler.h"

#include "../../../lib-common/include/collections.hpp"

Event::Event(char* name, bool kernelEvent) {
  listeners = nullptr;
  if(!kernelEvent) {
    listeners = new List<Module*>();
  }
  strcpy(eventName, name);
}

void Event::callListeners(void* object, size_t size) {
  if(listeners == nullptr) {
    kernelEvent(this, object, size);
  }
  else {
    Iterator<Module*>* i = listeners->iterator();

    while(i->hasNext()) {
      Module* e = i->pop();
      remoteCall(e, 0, (uint32_t)(this));
    }
  }

  //TODO: Implement listeners
}

static List<Event*>* eventList;

Event* registerEvent(char* name, bool kernelEvent) {
  if(eventList == nullptr) eventList = new List<Event*>();

  Event* event = new Event(name, kernelEvent);

  eventList->add(event);

  return event;
}

Event* getEventByName(char* name) {
  Iterator<Event*>* i = eventList->iterator();

  while(i->hasNext()) {
    Event* e = i->pop();

    if(!strcmp(e->eventName, name)) {
      return e;
    }
  }

  kprintf("returned 0 event for %s...\n", name);

  return nullptr;
}
