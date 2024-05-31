/*
 * RNG.h
 *
 *  Created on: May 30, 2024
 *      Author: danny
 */

#ifndef SRC_RNG_H_
#define SRC_RNG_H_

#include "main.h"



void rng_init();
uint32_t get_random(uint32_t n); // ? maybe have a queue for random values, so instant value?



#endif /* SRC_RNG_H_ */
