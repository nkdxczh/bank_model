// CSE 375/475 Assignment #1
// Fall 2017
//
// Description: This file specifies a simple map implemented using two vectors.
// << use templates for creating a map of generic types >>
// One vector is used for storing keys. One vector is used for storing values.
// The expectation is that items in equal positions in the two vectors correlate.
// That is, for all values of i, keys[i] is the key for the item in values[i].
// Keys in the map are not necessarily ordered.
//
// The specification for each function appears as comments.
// Students are responsible for implementing the simple map as specified.

#include "simplemap.h"

// Insert (key, val) if and only if the key is not currently present in
// the map.  Returns true on success, false if the key was
// already present.
template <class K, class V>
bool simplemap_t<K,V>::insert(K key, V val) {

	// The following is just an example of using C++11 features,
	// like the 'auto' type and lambda expression
	/*
	   for (auto i = keys->begin(); i != keys->end(); ++i)
	   if (*i == key)
	   return false;

	// insert the key and value.  Note that indices should, by default, match
	// between these two vectors.
	keys->push_back(key);
	values->push_back(val);
	*/
	std::pair<V, bool> res = lookup(key);
	if(!res.second){
		keys->push_back(key);
		values->push_back(val);
	}
	return res.second;
}

// If key is present in the data structure, replace its value with val
// and return true; if key is not present in the data structure, return
// false.
template <class K, class V>
bool simplemap_t<K,V>::update(K key, V val) {
	auto v = values->begin();
	for (auto i = keys->begin(); i != keys->end(); ++i){
		if (*i == key){
			*v = val;
			return true;
		}
		++v;
	}
	return false;
}

// Remove the (key, val) pair if it is present in the data structure.
// Returns true on success, false if the key was not already present.
template <class K, class V>
bool simplemap_t<K,V>::remove(K key) {
	auto v = values->begin();
	for (auto i = keys->begin(); i != keys->end(); ++i){
		if (*i == key){
			keys->erase(key);
			values->erase(v);
			return true;
		}
		++v;
	}
	return false;
}

// If key is present in the map, return a pair consisting of
// the corresponding value and true. Otherwise, return a pair with the
// boolean entry set to false.
// Attention to not share the memory of the map with application threads, you might
// get unexpected race conditions
template <class K, class V>
std::pair<V, bool> simplemap_t<K,V>::lookup(K key) {
	auto v = values->begin();
	for (auto i = keys->begin(); i != keys->end(); ++i){
		if (*i == key){
			return std::make_pair(*v, true);
		}
		++v;
	}

	return std::make_pair(0, false);
}
