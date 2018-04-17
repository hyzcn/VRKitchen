// Copyright (C) 2011  Carl Rogers
// Released under MIT License
// Simplied by Weichao Qiu (qiuwch@gmail.com) from https://github.com/rogersce/cnpy

#include "VRInteractPlatform.h"
#include <vector>
#include <complex>

using byte = unsigned char;

// namespace cnpy {

/** from: http://www.cplusplus.com/forum/beginner/155821/ */

static char BigEndianTest() {
	unsigned char x[] = { 1,0 };
	short y = *(short*)x;
	return y == 1 ? '<' : '>';
}

/** Modified from npy_save */
// template<typename T> void npy_dump(const T* data, const unsigned int* shape, const unsigned int ndims) {
// 	std::vector<char> header = create_npy_header(data, shape, ndims);
//
// 	unsigned int nels = 1;
// 	for (int i = 0; i < ndims; i++) nels *= shape[i];
// }

static char map_type(const double* data) { return 'f'; }
static char map_type(const float* data) { return 'f'; }
static char map_type(const long double* data) { return 'f'; }

static char map_type(const int* data) { return 'i'; }
static char map_type(const char* data) { return 'i'; }
static char map_type(const short* data) { return 'i'; }
static char map_type(const long* data) { return 'i'; }
static char map_type(const long long* data) { return 'i'; }

static char map_type(const unsigned int* data) { return 'u'; }
static char map_type(const unsigned char* data) { return 'u'; }
static char map_type(const unsigned short* data) { return 'u'; }
static char map_type(const unsigned long* data) { return 'u'; }
static char map_type(const unsigned long long* data) { return 'u'; }

static char map_type(const bool* data) { return 'b'; }

// }
