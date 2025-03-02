/*
   This file is part of BTYPE.
   Copyright © CLEARSY 2025
   BTYPE is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.
   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.
   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

#include <mutex>
#include <shared_mutex>
#include <unordered_map>

#include "btype.h"

// Hash functions for complex types
struct ProductTypeHash {
  size_t operator()(const std::pair<std::shared_ptr<BType>,
                                    std::shared_ptr<BType>>& p) const {
    return p.first->hash_combine(p.second->hash_combine(0));
  }
};

struct PowerTypeHash {
  size_t operator()(const std::shared_ptr<BType>& type) const {
    return type->hash_combine(0);
  }
};

// Thread-safe type caches
class BTypeCache {
 private:
  mutable std::shared_mutex m_mutexProduct;
  mutable std::shared_mutex m_mutexPower;
  mutable std::shared_mutex m_mutexAbstract;
  mutable std::shared_mutex m_mutexEnumerated;
  mutable std::shared_mutex m_mutexStruct;
  std::unordered_map<std::pair<std::shared_ptr<BType>, std::shared_ptr<BType>>,
                     std::shared_ptr<BType::ProductType>, ProductTypeHash>
      m_productTypes;
  std::unordered_map<std::shared_ptr<BType>, std::shared_ptr<BType::PowerType>,
                     PowerTypeHash>
      m_powerTypes;
  std::unordered_map<std::string, std::shared_ptr<BType::AbstractSet>>
      m_abstractSets;
  std::unordered_map<std::string,
                     std::shared_ptr<BType::EnumeratedSet>>
      m_enumeratedSets;  // indexed by name
  std::unordered_map<std::string,
                     std::shared_ptr<BType::StructType>>
      m_structTypes;  // indexed by space-ended field names
  std::shared_ptr<BType> m_INTEGER;
  std::shared_ptr<BType> m_BOOLEAN;
  std::shared_ptr<BType> m_FLOAT;
  std::shared_ptr<BType> m_REAL;
  std::shared_ptr<BType> m_STRING;

 public:
  BTypeCache()
      : m_INTEGER(std::make_shared<BType>(BType::Kind::INTEGER)),
        m_BOOLEAN(std::make_shared<BType>(BType::Kind::BOOLEAN)),
        m_FLOAT(std::make_shared<BType>(BType::Kind::FLOAT)),
        m_REAL(std::make_shared<BType>(BType::Kind::REAL)),
        m_STRING(std::make_shared<BType>(BType::Kind::STRING)) {}
  std::shared_ptr<BType> getInteger() { return m_INTEGER; }
  std::shared_ptr<BType> getBoolean() { return m_BOOLEAN; }
  std::shared_ptr<BType> getFloat() { return m_FLOAT; }
  std::shared_ptr<BType> getReal() { return m_REAL; }
  std::shared_ptr<BType> getString() { return m_STRING; }
  std::shared_ptr<BType> getOrCreateProductType(std::shared_ptr<BType> lhs,
                                                std::shared_ptr<BType> rhs) {
    auto key = std::make_pair(lhs, rhs);
    {
      std::shared_lock<std::shared_mutex> readLock(m_mutexProduct);
      auto it = m_productTypes.find(key);
      if (it != m_productTypes.end()) {
        return it->second;
      }
    }
    std::unique_lock<std::shared_mutex> writeLock(m_mutexProduct);
    auto it = m_productTypes.find(key);
    if (it != m_productTypes.end()) {
      return it->second;
    }
    auto newType = std::make_shared<BType::ProductType>(lhs, rhs);
    m_productTypes[key] = newType;
    return newType;
  }

  std::shared_ptr<BType> getOrCreatePowerType(std::shared_ptr<BType> content) {
    {
      std::shared_lock<std::shared_mutex> readLock(m_mutexPower);
      auto it = m_powerTypes.find(content);
      if (it != m_powerTypes.end()) {
        return it->second;
      }
    }
    std::unique_lock<std::shared_mutex> writeLock(m_mutexPower);
    auto it = m_powerTypes.find(content);
    if (it != m_powerTypes.end()) {
      return it->second;
    }
    std::shared_ptr<BType::PowerType> newType =
        std::shared_ptr<BType::PowerType>(new BType::PowerType(content));
    m_powerTypes[content] = newType;
    return newType;
  }

  std::shared_ptr<BType> getOrCreateAbstractSet(const std::string& name) {
    {
      std::shared_lock<std::shared_mutex> readLock(m_mutexAbstract);
      auto it = m_abstractSets.find(name);
      if (it != m_abstractSets.end()) {
        return it->second;
      }
    }
    std::unique_lock<std::shared_mutex> writeLock(m_mutexAbstract);
    auto it = m_abstractSets.find(name);
    if (it != m_abstractSets.end()) {
      return it->second;
    }
    auto newType = std::make_shared<BType::AbstractSet>(name);
    m_abstractSets[name] = newType;
    return newType;
  }

  std::shared_ptr<BType> getOrCreateEnumeratedSet(
      const std::string& name, const std::vector<std::string>& values) {
    {
      std::shared_lock<std::shared_mutex> readLock(m_mutexEnumerated);
      auto it = m_enumeratedSets.find(name);
      if (it != m_enumeratedSets.end()) {
        return it->second;
      }
    }
    std::unique_lock<std::shared_mutex> writeLock(m_mutexEnumerated);
    auto it = m_enumeratedSets.find(name);
    if (it != m_enumeratedSets.end()) {
      return it->second;
    }
    auto newType =
        std::make_shared<BType::EnumeratedSet>(std::pair(name, values));
    m_enumeratedSets[name] = newType;
    return newType;
  }

  std::shared_ptr<BType> getOrCreateStruct(
      const std::vector<std::pair<std::string, std::shared_ptr<BType>>>&
          fields) {
    auto sortedFields = BType::StructType::sort(fields);
    std::string keyString;
    for (const auto& field : sortedFields) {
      keyString.append(field.first);
      keyString.push_back(';');
    }
    {
      std::shared_lock<std::shared_mutex> readLock(m_mutexStruct);
      auto it = m_structTypes.find(keyString);
      if (it != m_structTypes.end()) {
        return it->second;
      }
    }
    std::unique_lock<std::shared_mutex> writeLock(m_mutexStruct);
    auto it = m_structTypes.find(keyString);
    if (it != m_structTypes.end()) {
      return it->second;
    }
    auto newType = std::make_shared<BType::StructType>(sortedFields);
    m_structTypes[keyString] = newType;
    return newType;
  }
};

std::unique_ptr<BTypeCache> cache = std::make_unique<BTypeCache>();

// Factory methods implementation
std::shared_ptr<BType> BTypeFactory::Product(std::shared_ptr<BType> lhs,
                                             std::shared_ptr<BType> rhs) {
  return cache->getOrCreateProductType(lhs, rhs);
}

std::shared_ptr<BType> BTypeFactory::PowerSet(std::shared_ptr<BType> content) {
  return cache->getOrCreatePowerType(content);
}

std::shared_ptr<BType> BTypeFactory::AbstractSet(const std::string& name) {
  return cache->getOrCreateAbstractSet(name);
}

std::shared_ptr<BType> BTypeFactory::EnumeratedSet(
    const std::string& name, const std::vector<std::string>& values) {
  return cache->getOrCreateEnumeratedSet(name, values);
}

std::shared_ptr<BType> BTypeFactory::Struct(
    const std::vector<std::pair<std::string, std::shared_ptr<BType>>>& fields) {
  return cache->getOrCreateStruct(fields);
}

std::shared_ptr<BType> BTypeFactory::Integer() { return cache->getInteger(); }
std::shared_ptr<BType> BTypeFactory::Boolean() { return cache->getBoolean(); }
std::shared_ptr<BType> BTypeFactory::Float() { return cache->getFloat(); }
std::shared_ptr<BType> BTypeFactory::Real() { return cache->getReal(); }
std::shared_ptr<BType> BTypeFactory::String() { return cache->getString(); }