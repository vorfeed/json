// Copyright 2015, Xiaojie Chen (swly@live.com). All rights reserved.
// https://github.com/vorfeed/json
//
// Use of this source code is governed by a BSD-style license
// that can be found in the License file.

#pragma once

#ifndef JSON_JSON_H
#error "This should only be included by json.h"
#endif

namespace json {

Json Json::Create(const std::string& json) {
  return Parser(json)();
}

Json::Json() noexcept : type_(EMPTY), i_(0) {}

/*
template <class T>
Json::Json() : type_(Json::TypeTraits<T>::json_type) {
  Json::TypeTraits<T>::init(*this);
}
*/

template <class T, Json::enable_if_not_json<T>*>
Json::Json(T&& value) :
    type_(Json::TypeTraits<typename std::decay<T>::type>::json_type) {
  Json::TypeTraits<typename std::decay<T>::type>::value(*this,
      std::forward<T>(value));
}

template <class T, Json::enable_if_not_json<T>*>
Json& Json::operator=(T&& value) {
  this->~Json();
  new (this) Json(std::forward<T>(value));
  return *this;
}

Json::Json(const Json& other) : type_(other.type_) {
  switch (type_) {
  case EMPTY:     return;
  case STRING:    s_ = new std::string(*other.s_); return;
  case SEQUENCE:  a_ = new Sequence(*other.a_); return;
  case MAPPING:   m_ = new Mapping(*other.m_); return;
  default:        i_ = other.i_; return;
  }
}

Json& Json::operator=(const Json& other) {
  this->~Json();
  new (this) Json(other);
  return *this;
}

Json::Json(Json&& other) noexcept : type_(other.type_), i_(other.i_) {
  other.type_ = EMPTY;
  other.i_ = 0;
}

Json& Json::operator=(Json&& other) noexcept {
  this->~Json();
  new (this) Json(std::move(other));
  return *this;
}

Json::~Json() {
  Clear();
}

template <class T>
bool Json::Is() const {
  return type_ == Json::TypeTraits<T>::json_type;
}

template <class T>
bool Json::CanBe() const {
  return type_ & Json::TypeTraits<T>::convertable_type;
}

template <class T>
auto Json::As() const -> typename std::result_of<
    decltype(&Json::TypeTraits<T>::convert)(const Json&)>::type {
  if (type_ & Json::TypeTraits<T>::convertable_type) {
    return Json::TypeTraits<T>::convert(*this);
  }
  throw std::logic_error("type mismatch");
}

template <class T>
auto& Json::As() {
  if (type_ == Json::TypeTraits<T>::json_type) {
    return Json::TypeTraits<T>::value(*this);
  }
  if (type_ & Json::TypeTraits<T>::convertable_type) {
    return Json::TypeTraits<T>::value(*this) =
        Json::TypeTraits<T>::convert(static_cast<const Json&>(*this));
  }
  throw std::logic_error("type mismatch");
}

template <class T>
T& Json::To() {
  this->~Json();
//  new (this) Json<T>();
  type_ = Json::TypeTraits<T>::json_type;
  Json::TypeTraits<T>::init(*this);
  return Json::TypeTraits<T>::value(*this);
}

const Json& Json::operator[](size_t index) const {
  if (type_ != SEQUENCE) {
    throw std::logic_error("not sequence");
  }
  if (index >= a_->size()) {
    throw std::logic_error("sequence out of range");
  }
  return (*a_)[index];
}

Json& Json::operator[](size_t index) {
  return const_cast<Json&>(static_cast<const Json*>(this)->operator[](index));
}

Json& Json::operator[](const std::string& key) {
  switch (type_) {
  case EMPTY:   To<Mapping>(); return (*m_)[key];
  case MAPPING: return (*m_)[key];
  default:      break;
  }
  throw std::logic_error("not mapping");
}

Json& Json::operator[](std::string&& key) {
  switch (type_) {
  case EMPTY:   To<Mapping>(); return (*m_)[std::move(key)];
  case MAPPING: return (*m_)[std::move(key)];
  default:      break;
  }
  throw std::logic_error("not mapping");
}

void Json::Clear() {
  switch (type_) {
  case EMPTY:     return;
  case STRING:    delete s_; break;
  case SEQUENCE:  delete a_; break;
  case MAPPING:   delete m_; break;
  default:        break;
  }
  type_ = EMPTY;
  i_ = 0;
}

std::string Json::str() const {
  std::ostringstream oss;
  oss << *this;
  return oss.str();
}

std::ostream& operator<<(std::ostream& out, const Json& value) {
  switch(value.type_) {
  case Json::EMPTY:             out << "null"; break;
  case Json::BOOLEAN:           out << (value.b_ ? "true" : "false"); break;
  case Json::SIGNED_INTEGER:    out << std::to_string(value.i_); break;
  case Json::UNSIGNED_INTEGER:  out << std::to_string(value.u_); break;
  case Json::FLOAT:             char buf[32]; sprintf(buf, "%g", value.d_); out << buf; break;
  case Json::STRING:            Json::Escape(out, *value.s_); break;
  case Json::SEQUENCE:          out << *value.a_; break;
  case Json::MAPPING:           out << *value.m_; break;
  default:                      break;
  }
  return out;
}

std::ostream& operator<<(std::ostream& out, const Sequence& values) {
  out << "[";
  for (const auto& value : values) {
    out << value << ",";
  }
  if (!values.empty()) {
    out.seekp(-1, std::ios_base::end);
  }
  return out << "]";
}

std::ostream& operator<<(std::ostream& out, const Mapping& values) {
  out << "{";
  for (const auto& kv : values) {
    Json::Escape(out, kv.first);
    out << ":" << kv.second << ",";
  }
  if (!values.empty()) {
    out.seekp(-1, std::ios_base::end);
  }
  return out << "}";
}

template <class T>
std::string to_string(const T& value) {
  std::ostringstream oss;
  oss << value;
  return oss.str();
}

bool Json::operator==(const Json& rhs) const {
  if (type_ != rhs.type_) { return false; }
  switch (type_) {
  case EMPTY:             return true;
  case BOOLEAN:           return b_ == rhs.b_;
  case SIGNED_INTEGER:    return i_ == rhs.i_;
  case UNSIGNED_INTEGER:  return u_ == rhs.u_;
  case FLOAT:             return d_ == rhs.d_;
  case STRING:            return *s_ == *rhs.s_;
  case SEQUENCE:          return *a_ == *rhs.a_;
  case MAPPING:           return *m_ == *rhs.m_;
  default:                break;
  }
  return false;
}

void Json::Escape(std::ostream& out, const std::string& str) {
  out << '"';
  for (const char c : str) {
    switch (c) {
    case '\"': out << "\\\""; break;
    case '\\': out << "\\\\"; break;
    case '\b': out << "\\b";  break;
    case '\f': out << "\\f";  break;
    case '\n': out << "\\n";  break;
    case '\r': out << "\\r";  break;
    case '\t': out << "\\t";  break;
    // TODO: unicode
    default:   out << c;      break;
    }
  }
  out << '"';
}

} // namespace json
