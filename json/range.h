// Copyright 2015, Xiaojie Chen (swly@live.com). All rights reserved.
// https://github.com/vorfeed/json
//
// Use of this source code is governed by a BSD-style license
// that can be found in the License file.

#pragma once

namespace json {

template <typename T>
class Range {
 public:
  class iterator {
   public:
    iterator(T value, T step) :
        value_(value), step_(step) {
    }

    bool operator !=(iterator const& other) const {
      return value_ != other.value_;
    }
    T operator*() const {
      return value_;
    }
    iterator const& operator++() {
      value_ += step_;
      return *this;
    }

   private:
    T value_, step_;
  };

  Range(T end) :
      start_(T()), end_(end), step_(1) {
  }
  Range(T start, T end, T step = 1) :
      start_(start), end_(end), step_(step) {
  }

  iterator begin() const {
    return iterator(start_, step_);
  }
  iterator end() const {
    return iterator(end_, step_);
  }

 private:
  T start_, end_, step_;
};

typedef Range<int> range;
typedef Range<size_t> sequence;

} // namespace json
