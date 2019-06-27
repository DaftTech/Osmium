/*
 * Event.cpp
 *
 *  Created on: 04.03.2018
 *      Author: fabia
 */

#include "level1/events.hpp"
#include "level1/tar.h"

#include "../../../lib-common/include/collections.hpp"

Event *evt_KERNEL_DEBUG,
      *evt_KERNEL_UNPACK_INITRFS,
      *evt_AVAILABLE_TAR;

void registerKernelEvents() {
  evt_KERNEL_DEBUG = registerEvent("kernelDebug", true);
  evt_KERNEL_UNPACK_INITRFS = registerEvent("unpackInitrfs", true);
  evt_AVAILABLE_TAR = registerEvent("tarAvailable", false);
}

void kernelEvent(Event* e, void* data, size_t size) {
  if(e == evt_KERNEL_UNPACK_INITRFS) {
    tarThrowEvents();
  }
}
