#pragma once

#include <cstdint>

namespace GV
{
  class UUID
  {
    public:
      UUID();
      UUID(uint64_t uuid);

      operator uint64_t() const {return m_UUID;}

    private:
      uint64_t m_UUID;

  };

}

namespace std {
  template <typename T> struct hash;

  // Specialization fo the hash function in std
  template<>
  struct hash<GV::UUID>
  {
    std::size_t operator() (const GV::UUID& uuid) const
    {
      return (uint64_t)uuid;
    }
  };

}
