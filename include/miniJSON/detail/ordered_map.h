// Copyright (c) 2024 Zhichen (Joshua) Wen
#pragma once
#include <unordered_map>
#include <vector>

/*
  A simple hashmap that maintains insertion order.
*/
template <typename Key, typename Value>
class ordered_map {
 public:
  ordered_map() {}
  Value &operator[](const Key &key) {
    if (m_map.count(key) == 0) {
      insertion_order.push_back(key);
    }
    return m_map[key];
  }
  size_t size() { return m_map.size(); }
  typename std::vector<Key>::iterator begin() {
    return insertion_order.begin();
  }
  typename std::vector<Key>::iterator end() { return insertion_order.end(); }

 public:
 private:
  std::unordered_map<Key, Value> m_map;
  std::vector<Key> insertion_order;
};
