// Copyright (C) 2011  Carl Rogers
// Released under MIT License
// Simplied by Weichao Qiu (qiuwch@gmail.com) from https://github.com/rogersce/cnpy
#pragma once

#include <string>
#include <vector>
#include <complex>
#include "cnpy.cpp"

namespace cnpy {
	template<typename T>
	std::vector<char> create_npy_header(const T* data, const std::vector<int> shape);

	template<typename T>
	std::vector<char>& operator+=(std::vector<char>& lhs, const T rhs) {
		//write in little endian
		for (char byte = 0; byte < sizeof(T); byte++) {
			char val = *((char*)&rhs + byte);
			lhs.push_back(val);
		}
		return lhs;
	}
	static std::vector<char>& operator+=(std::vector<char>& lhs, const std::string rhs);
	static std::vector<char>& operator+=(std::vector<char>& lhs, const char* rhs);
}

static std::vector<char>& cnpy::operator+=(std::vector<char>& lhs, const std::string rhs) {
	lhs.insert(lhs.end(), rhs.begin(), rhs.end());
	return lhs;
}

static std::vector<char>& cnpy::operator+=(std::vector<char>& lhs, const char* rhs) {
	//write in little endian
	size_t len = strlen(rhs);
	lhs.reserve(len);
	for (size_t byte = 0; byte < len; byte++) {
		lhs.push_back(rhs[byte]);
	}
	return lhs;
}

template<typename T>
std::string tostring(T i, int pad = 0, char padval = ' ') {
	std::stringstream s;
	s << i;
	return s.str();
}

template<typename T> std::vector<char> cnpy::create_npy_header(const T* data, const std::vector<int> shape)
{
	std::vector<char> dict;
	dict += "{'descr': '";
	dict += BigEndianTest();
	// dict += map_type(typeid(T));
	dict += map_type(data);
	dict += tostring(sizeof(T));
	dict += "', 'fortran_order': False, 'shape': (";
	dict += tostring(shape[0]);

	int ndims = shape.size();
	for (int i = 1; i < ndims; i++) {
		dict += ", ";
		dict += tostring(shape[i]);
	}
	if (ndims == 1) dict += ",";
	dict += "), }";
	//pad with spaces so that preamble+dict is modulo 16 bytes. preamble is 10 bytes. dict needs to end with \n
	int remainder = 16 - (10 + dict.size()) % 16;
	dict.insert(dict.end(), remainder, ' ');
	dict.back() = '\n';

	std::vector<char> header;
	header += (char)0x93u;
	header += "NUMPY";
	header += (char)0x01u; //major version of numpy format
	header += (char)0x00u; //minor version of numpy format
	header += (unsigned short)dict.size();
	header.insert(header.end(), dict.begin(), dict.end());

	return header;
}








