#pragma once

#ifndef JSON_JSON_H
#error "This should only be included by json.h"
#endif

namespace json {

class Json::Parser {
 public:
  Parser(const std::string& str) : str_(str), pos_(str_.begin()) {}

  Json operator()() { return ParseJson(); }

 private:
  void SkipBlank() {
    static const std::bitset<256> kIsBlank =
        std::move(std::remove_const<decltype(kIsBlank)>::type(0)
            .set(' ').set('\r').set('\n').set('\t'));
    while (kIsBlank.test(*pos_)) {
      ++pos_;
    }
  }

  char NextToken() {
    SkipBlank();
    if (pos_ == str_.end()) {
      failed_ = true;
      return -1;
    }
    return *pos_++;
  }

  static bool InRange(int x, int lower, int upper) {
    return x >= lower && x <= upper;
  }

  Json Expect(const std::string& expected, Json res) {
    assert(pos_ != str_.end());
    --pos_;
    const std::string found(pos_, pos_ + expected.length());
    if (expected == found) {
      pos_ += expected.length();
      return res;
    }
    failed_ = true;
    return Json();
  }

  Json ParseNumber() {
    std::string::const_iterator start_pos = pos_;
    if (*pos_ == '-') {
      ++pos_;
    }
    // integer part
    if (*pos_ == '0') {
      ++pos_;
      if (InRange(*pos_, '0', '9')) {
        failed_ = true;
        return Json();
      }
    } else if (InRange(*pos_, '1', '9')) {
      ++pos_;
      while (InRange(*pos_, '0', '9')) {
        ++pos_;
      }
    } else {
      failed_ = true;
      return Json();
    }
    if (*pos_ != '.' && *pos_ != 'e' && *pos_ != 'E') {
      return std::atol(&*start_pos);
    }
    // decimal part
    if (*pos_ == '.') {
      ++pos_;
      if (!InRange(*pos_, '0', '9')) {
        failed_ = true;
        return Json();
      }
      while (InRange(*pos_, '0', '9')) {
        ++pos_;
      }
    }
    // exponent part
    if (*pos_ == 'e' || *pos_ == 'E') {
      ++pos_;
      if (*pos_ == '+' || *pos_ == '-') {
        ++pos_;
      }
      if (!InRange(*pos_, '0', '9')) {
        failed_ = true;
        return Json();
      }
      while (InRange(*pos_, '0', '9')) {
        ++pos_;
      }
    }
    return std::atof(&*start_pos);
  }

  std::string ParseString() {
    std::string out;
    long last_escaped_codepoint = -1;
    while (true) {
      if (pos_ == str_.end()) {
        failed_ = true;
        return "";
      }
      char ch = *pos_++;
      if (ch == '"') {
        return out;
      }
      if (InRange(ch, 0, 0x1f)) {
        failed_ = true;
        return "";
      }
      // the usual case: non-escaped characters
      if (ch != '\\') {
        out += ch;
        continue;
      }
      // handle escapes
      if (pos_ == str_.end()) {
        failed_ = true;
        return "";
      }
      ch = *pos_++;
      switch (ch) {
      case 'b': out += '\b'; break;
      case 'f': out += '\f'; break;
      case 'n': out += '\n'; break;
      case 'r': out += '\r'; break;
      case 't': out += '\t'; break;
      case '"': case '\\': case '/': out += ch; break;
      // TODO: unicode
      default: failed_ = true; return "";
      }
    }
    return "";
  }

  Json ParseJson() {
    char ch = NextToken();
    if (failed_) {
      return Json();
    }
    if (ch == '-' || (ch >= '0' && ch <= '9')) {
      --pos_;
      return ParseNumber();
    }
    if (ch == 't') {
      return Expect("true", true);
    }
    if (ch == 'f') {
      return Expect("false", false);
    }
    if (ch == 'n') {
      return Expect("null", Json());
    }
    if (ch == '"') {
      return ParseString();
    }
    if (ch == '{') {
      Mapping data;
      ch = NextToken();
      if (ch == '}') {
        return data;
      }
      while (true) {
        if (ch != '"') {
          failed_ = true;
          return Json();
        }
        std::string key = ParseString();
        if (failed_) {
          return Json();
        }
        ch = NextToken();
        if (ch != ':') {
          failed_ = true;
          return Json();
        }
        data[key] = ParseJson();
        if (failed_) {
          return Json();
        }
        ch = NextToken();
        if (ch == '}') {
          break;
        }
        if (ch != ',') {
          failed_ = true;
          return Json();
        }
        ch = NextToken();
      }
      return data;
    }
    if (ch == '[') {
      Sequence data;
      ch = NextToken();
      if (ch == ']') {
        return data;
      }
      while (true) {
        --pos_;
        data.emplace_back(ParseJson());
        if (failed_) {
          return Json();
        }
        ch = NextToken();
        if (ch == ']') {
          break;
        }
        if (ch != ',') {
          failed_ = true;
          return Json();
        }
        NextToken();
      }
      return data;
    }
    failed_ = true;
    return Json();
  }

  const std::string& str_;
  std::string::const_iterator pos_;
  bool failed_ { false };
};

} // namespace json
