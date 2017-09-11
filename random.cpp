/*
 * Bermuda Syndrome engine rewrite
 * Copyright (C) 2007 Gregory Montoir
 */

#include <ctime>
#include "random.h"

RandomGenerator::RandomGenerator() {
	uint16 seed = time(0);
	setSeed(seed);
}

void RandomGenerator::setSeed(uint16 seed) {
	_randomSeed = seed;
}

// Borland C random generator
uint16 RandomGenerator::getNumber() {
	uint16 rnd = 0x15A * (_randomSeed & 0xFFFF);
	if ((_randomSeed >> 16) != 0) {
		rnd += 0x4E35 * (_randomSeed >> 16);
	}
	_randomSeed = (rnd << 16) | (0x4E35 * (_randomSeed & 0xFFFF));
	++_randomSeed;
	return _randomSeed & 0x7FFF;
}
