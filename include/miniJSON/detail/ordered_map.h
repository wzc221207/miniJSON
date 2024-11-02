// Copyright (c) 2024 Zhichen (Joshua) Wen
#pragma once
#include <algorithm>
#include <cassert>
#include <unordered_map>
#include <vector>

/*
  A simple hashmap that maintains insertion order.
*/
template <typename Key, typename Value>
class ordered_map {
 public:
  ordered_map() { test_invariant(); }
  Value &operator[](const Key &key) {
    test_invariant();
    if (m_map.count(key) == 0) {
      m_insertion_order.push_back(key);
    }
    auto &res = m_map[key];
    test_invariant();
    return res;
  }
  size_t count(const Key &key) const { return m_map.count(key); }
  size_t erase(const Key &key) {
    test_invariant();
    m_insertion_order.erase(
        std::remove(m_insertion_order.begin(), m_insertion_order.end(), key),
        m_insertion_order.end());
    size_t res = m_map.erase(key);
    test_invariant();
    return res;
  }
  size_t size() { return m_map.size(); }

 public:
  struct keys_iterator {
    using iterator_category = std::forward_iterator_tag;
    using difference_type = std::ptrdiff_t;
    using value_type = const Key &;
    using pointer = Key *;
    using reference = Key &;
    keys_iterator() {}
    keys_iterator(typename std::vector<Key>::iterator it) : m_it(it) {}
    value_type operator*() const { return *m_it; }
    pointer operator->() { return &(*m_it); }

    keys_iterator &operator++() {
      m_it++;
      return *this;
    }

    keys_iterator operator++(int) {
      auto tmp = *this;
      ++*this;
      return tmp;
    }

    bool operator==(const keys_iterator &other) const {
      return this->m_it == other.m_it;
    }
    bool operator!=(const keys_iterator &other) const {
      return this->m_it != other.m_it;
    }

   private:
    typename std::vector<Key>::iterator m_it;
  };

  struct values_iterator {
    using iterator_category = std::forward_iterator_tag;
    using difference_type = std::ptrdiff_t;
    using value_type = const Value &;
    using pointer = Value *;
    using reference = Value &;
    values_iterator() {}
    values_iterator(typename std::unordered_map<Key, Value> *map,
                    typename std::vector<Key>::iterator it)
        : m_map(map), m_it(it) {}
    value_type operator*() const { return (*m_map)[*m_it]; }
    pointer operator->() { return &((*m_map)[*m_it]); }

    values_iterator &operator++() {
      m_it++;
      return *this;
    }

    values_iterator operator++(int) {
      auto tmp = *this;
      ++*this;
      return tmp;
    }

    bool operator==(const values_iterator &other) const {
      return this->m_map == other.m_map && this->m_it == other.m_it;
    }
    bool operator!=(const values_iterator &other) const {
      return this->m_map != other.m_map || this->m_it != other.m_it;
    }

   public:
    Key key() { return *m_it; }
    Value value() { return (*m_map)[*m_it]; }

   private:
    typename std::unordered_map<Key, Value> *m_map = nullptr;
    typename std::vector<Key>::iterator m_it;
  };

 public:
  values_iterator values_begin() {
    return values_iterator(&m_map, m_insertion_order.begin());
  }
  values_iterator values_end() {
    return values_iterator(&m_map, m_insertion_order.end());
  }

  keys_iterator begin() { return keys_iterator(m_insertion_order.begin()); }
  keys_iterator end() { return keys_iterator(m_insertion_order.end()); }

 private:
  void test_invariant() const {
    assert(m_map.size() == m_insertion_order.size());
  }

 private:
  std::unordered_map<Key, Value> m_map;
  std::vector<Key> m_insertion_order;
};
