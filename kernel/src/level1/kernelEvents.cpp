/*
 * Event.cpp
 *
 *  Created on: 04.03.2018
 *      Author: fabia
 */

#include "../../include/level1/events.hpp"

#include "../../../lib-common/include/collections.hpp"

Event* evt_KERNEL_DEBUG;

void registerKernelEvents() {
	evt_KERNEL_DEBUG = registerEvent("kernelDebug", true);
}

void kernelEvent(Event* e, void* data, size_t size) {

}
