#ifndef EXCEPTIONS_HPP
#define EXCEPTIONS_HPP

#include <stdexcept>
#include <string>
namespace ada_types {
class ada_exception : public std::runtime_error {
public:
  explicit ada_exception(const std::string& a_message)
      : std::runtime_error(a_message) {}
};

class input_out_of_range : public ada_exception {
public:
  explicit input_out_of_range(const std::string& a_message)
      : ada_exception(a_message) {}
};

class domain_error : public ada_exception {
public:
  explicit domain_error(const std::string& a_message)
      : ada_exception(a_message) {}
};
} // namespace ada_types

#endif