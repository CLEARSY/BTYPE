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
#ifndef BTYPE_H
#define BTYPE_H

#include <algorithm>
#include <exception>
#include <memory>
#include <utility>
#include <vector>

// Forward declaration
class BTypeFactory;
class BTypeCache;

class BType : public std::enable_shared_from_this<BType> {
 public:
  enum class Kind {
    INTEGER,
    BOOLEAN,
    FLOAT,
    REAL,
    STRING,
    ProductType,
    PowerType,
    Struct,
    AbstractSet,
    EnumeratedSet
  };
  Kind getKind() const { return m_kind; };

  class ProductType;
  class PowerType;
  class StructType;
  class AbstractSet;
  class EnumeratedSet;

  std::shared_ptr<const ProductType> toProductType() const;
  std::shared_ptr<const PowerType> toPowerType() const;
  std::shared_ptr<const StructType> toStructType() const;
  std::shared_ptr<const AbstractSet> toAbstractSetType() const;
  std::shared_ptr<const EnumeratedSet> toEnumeratedSetType() const;

  class Visitor {
   public:
    virtual void visitINTEGER() = 0;
    virtual void visitBOOLEAN() = 0;
    virtual void visitFLOAT() = 0;
    virtual void visitREAL() = 0;
    virtual void visitSTRING() = 0;
    virtual void visitAbstractSet(const AbstractSet &) = 0;
    virtual void visitEnumeratedSet(const EnumeratedSet &) = 0;
    virtual void visitProductType(const ProductType &) = 0;
    virtual void visitPowerType(const PowerType &) = 0;
    virtual void visitStructType(const StructType &) = 0;
  };
  virtual void accept(Visitor &v) const;

  // Comparisons
  static int compare(const BType &v1, const BType &v2);
  static int vec_compare(const std::vector<std::shared_ptr<BType>> &v1,
                         const std::vector<std::shared_ptr<BType>> &v2);
  inline bool operator==(const BType &other) const {
    return compare(*this, other) == 0;
  }
  inline bool operator!=(const BType &other) const {
    return compare(*this, other) != 0;
  }
  inline bool operator<(const BType &other) const {
    return compare(*this, other) < 0;
  }
  inline bool operator>(const BType &other) const {
    return compare(*this, other) > 0;
  }
  inline bool operator<=(const BType &other) const {
    return compare(*this, other) <= 0;
  }
  inline bool operator>=(const BType &other) const {
    return compare(*this, other) >= 0;
  }

  virtual size_t hash_combine(size_t seed) const;

  friend class BTypeFactory;
  Kind m_kind;

  // Only the factory can create BType instances.
  BType(Kind kind) : m_kind{kind}, m_hash_valid(false), m_hash(0) {};
  virtual ~BType() = default;
  size_t hash() const {
    if (!m_hash_valid) {
      m_hash = hash_combine(0);
      m_hash_valid = true;
    }
    return m_hash;
  }

 private:
  BType(const BType &) = delete;
  BType &operator=(const BType &) = delete;
  mutable bool m_hash_valid = false;
  mutable size_t m_hash;
};

// Factory class for creating BType instances
class BTypeFactory {
 public:
  // Delete constructor, copy constructor and assignment operator
  BTypeFactory() = delete;
  BTypeFactory(const BTypeFactory &) = delete;
  BTypeFactory &operator=(const BTypeFactory &) = delete;

  // Basic types
  static std::shared_ptr<BType> Integer();
  static std::shared_ptr<BType> Boolean();
  static std::shared_ptr<BType> Float();
  static std::shared_ptr<BType> Real();
  static std::shared_ptr<BType> String();

  // Complex type constructors
  static std::shared_ptr<BType> Product(std::shared_ptr<BType> lhs,
                                        std::shared_ptr<BType> rhs);
  static std::shared_ptr<BType> PowerSet(std::shared_ptr<BType> content);
  static std::shared_ptr<BType> AbstractSet(const std::string &name);
  static std::shared_ptr<BType> EnumeratedSet(
      const std::string &, const std::vector<std::string> &values);
  static std::shared_ptr<BType> Struct(
      const std::vector<std::pair<std::string, std::shared_ptr<BType>>>
          &fields);

  static size_t size();
  static std::shared_ptr<BType> at(size_t index);

  // Get a named type (abstract set or enumerated set)
  static std::shared_ptr<BType> Named(const std::string &name);

  class Exception : public std::exception {
   public:
    Exception(const std::string &msg) : msg{msg} {}
    const char *what() const noexcept override { return msg.c_str(); }

   private:
    std::string msg;
  };

 private:
  // Basic types (initialized in cpp file)
  static std::shared_ptr<BType> INTEGER;
  static std::shared_ptr<BType> BOOLEAN;
  static std::shared_ptr<BType> FLOAT;
  static std::shared_ptr<BType> REAL;
  static std::shared_ptr<BType> STRING;

  // Power set types
  static std::shared_ptr<BType> POW_INTEGER;
  static std::shared_ptr<BType> POW_BOOLEAN;
  static std::shared_ptr<BType> POW_FLOAT;
  static std::shared_ptr<BType> POW_STRING;
  static std::shared_ptr<BType> POW_REAL;
  static std::shared_ptr<BType> RELATION_INTEGER;
};

class BType::ProductType : public BType {
 public:
  void accept(Visitor &v) const override { v.visitProductType(*this); }

  size_t hash_combine(size_t seed) const override;
  std::shared_ptr<BType> lhs;
  std::shared_ptr<BType> rhs;
  ProductType(std::shared_ptr<BType> lhs, std::shared_ptr<BType> rhs)
      : BType(Kind::ProductType), lhs{lhs}, rhs{rhs} {}
  virtual ~ProductType() = default;
  friend class BTypeFactory;
  friend class BTypeCache;
};

class BType::PowerType : public BType {
 public:
  size_t hash_combine(size_t seed) const override;
  void accept(Visitor &v) const override { v.visitPowerType(*this); }
  const std::shared_ptr<BType> m_content;
  PowerType(std::shared_ptr<BType> content)
      : BType(BType::Kind::PowerType), m_content{content} {};
  virtual ~PowerType() = default;
  friend class BTypeFactory;
  friend class BTypeCache;
};

class BType::AbstractSet : public BType {
 public:
  size_t hash_combine(size_t seed) const override;
  void accept(Visitor &v) const override { v.visitAbstractSet(*this); }
  const std::string &getName() const { return m_name; }
  AbstractSet(const std::string &name)
      : BType(BType::Kind::AbstractSet), m_name{name} {};
  virtual ~AbstractSet() = default;
  const std::string m_name;
  friend class BTypeFactory;
  friend class BTypeCache;
};

class BType::EnumeratedSet : public BType {
 public:
  size_t hash_combine(size_t seed) const override;
  void accept(Visitor &v) const override { v.visitEnumeratedSet(*this); }

  const std::string &getName() const { return m_name; }
  const std::vector<std::string> &getContent() const { return m_values; }
  const std::string m_name;
  const std::vector<std::string> m_values;
  EnumeratedSet(const std::pair<std::string, std::vector<std::string>> &values)
      : BType(BType::Kind::EnumeratedSet),
        m_name(values.first),
        m_values{values.second} {}
  virtual ~EnumeratedSet() = default;
  friend class BTypeFactory;
  friend class BTypeCache;
};

class BType::StructType : public BType {
 public:
  size_t hash_combine(size_t seed) const override;

  void accept(Visitor &v) const override { v.visitStructType(*this); }
  //
  const std::vector<std::pair<std::string, std::shared_ptr<BType>>>
      m_fields;  // invariant: fields are sorted alphabetically
  static std::vector<std::pair<std::string, std::shared_ptr<BType>>> sort(
      const std::vector<std::pair<std::string, std::shared_ptr<BType>>>
          &fields);
  StructType(
      const std::vector<std::pair<std::string, std::shared_ptr<BType>>> &fields)
      : BType(BType::Kind::Struct), m_fields{sort(fields)} {}
  virtual ~StructType() = default;

  friend class BTypeFactory;
  friend class BTypeCache;
};

#endif
