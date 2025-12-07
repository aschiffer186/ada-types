#ifndef STREAMING_HPP
#define STREAMING_HPP

#include <ostream>

#include "concepts.hpp"

namespace ada_types {
template <typename Derived> class stream_operator {
  friend auto operator<<(std::ostream& os, const Derived& d) -> std::ostream&
    requires streamable<typename Derived::underlying_type>
  {
    os << d.get();
    return os;
  }
};
} // namespace ada_types

#endif