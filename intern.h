/*
 * Bermuda Syndrome engine rewrite
 * Copyright (C) 2007 Gregory Montoir
 */

#ifndef INTERN_H__
#define INTERN_H__

#include <cstdio>
#include <cassert>
#include <cstdlib>
#include <cstring>
#include <cerrno>

typedef unsigned char uint8;
typedef signed char int8;
typedef unsigned short uint16;
typedef signed short int16;
typedef unsigned int uint32;
typedef signed int int32;

#include "util.h"

#define ARRAYSIZE(a) (sizeof(a)/sizeof(a[0]))

struct Point {
	int x, y;
};

struct Rect {
	int x, y;
	int w, h;
};

inline uint16 READ_LE_UINT16(const void *ptr) {
	const uint8 *b = (const uint8 *)ptr;
	return (b[1] << 8) | b[0];
}

inline uint32 READ_LE_UINT32(const void *ptr) {
	const uint8 *b = (const uint8 *)ptr;
	return (b[3] << 24) | (b[2] << 16) | (b[1] << 8) | b[0];
}

template<typename T>
inline T MIN(T v1, T v2) {
	return (v1 < v2) ? v1 : v2;
}

template<typename T>
inline T MAX(T v1, T v2) {
	return (v1 > v2) ? v1 : v2;
}

template<typename T>
inline T ABS(T t) {
	return (t < 0) ? -t : t;
}

template<typename T>
inline void SWAP(T &a, T &b) {
	T tmp = a; a = b; b = tmp;
}

#endif
