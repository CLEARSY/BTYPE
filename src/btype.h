/* @file btype.h
   @brief Header file for the BType class and BTypeFactory class.

   @note This file is part of BTYPE.
   @copyright Copyright © CLEARSY 2025
   @license GNU General Public License (GPL) version 3

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
#include <cstdint>
#include <exception>
#include <memory>
#include <utility>
#include <vector>

namespace tinyxml2 {
class XMLElement;
};

// Forward declaration
class BTypeFactory;
class BTypeCache;

/**
 * @brief Abstract base class representing types of the B-method language.
 *
 * This class is the base for representations of B types.
 * It provides a common interface for all types and ensures maximal sharing
 * through the use of the BTypeFactory.
 *
 * BType instances should never be created directly, but only through the
 * BTypeFactory class.
 *
 * Types are internally represented in a table. The index of the type in
 * the table may be queried using the BType::index() method, and it is possible
 * get the type at a given index using the BTypeFactory::at() method.
 */
class BType : public std::enable_shared_from_this<BType> {
 public:
  /* @brief Enumeration of the different kinds of types.
   *
   * This enumeration lists the different kinds of types that can be represented
   * by the BType class.
   */
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

  /** @brief Nested class representing a Cartesian product type. */
  class ProductType;
  /** @brief Nested class representing a power set type. */
  class PowerType;
  /** @brief Nested class representing a struct type. */
  class StructType;
  /** @brief Nested class representing an abstract set type. */
  class AbstractSet;
  /** @brief Nested class representing an enumerated set type. */
  class EnumeratedSet;

  /** @brief converts to a ProductType, if possible
   * @return a shared pointer to the ProductType, or nullptr if the conversion
   * is not possible
   */
  std::shared_ptr<const ProductType> toProductType() const;
  /** @brief converts to a PowerType, if possible
   * @return a shared pointer to the PowerType, or nullptr if the conversion is
   * not possible
   */
  std::shared_ptr<const PowerType> toPowerType() const;
  /** @brief converts to a Struct, if possible
   * @return a shared pointer to the Struct, or nullptr if the conversion is not
   * possible
   */
  std::shared_ptr<const StructType> toStructType() const;
  /** @brief converts to an AbstractSet, if possible
   * @return a shared pointer to the AbstractSet, or nullptr if the conversion
   * is not possible
   */
  std::shared_ptr<const AbstractSet> toAbstractSetType() const;
  /** @brief converts to an EnumeratedSet, if possible
   * @return a shared pointer to the EnumeratedSet, or nullptr if the conversion
   * is not possible
   */
  std::shared_ptr<const EnumeratedSet> toEnumeratedSetType() const;

  /**
   * @brief Abstract visitor class for the BType hierarchy.
   *
   * Implementors can derive from this class to perform specific actions.
   */
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

  /**
   * @brief Combines the hash of this BType with a seed value.
   * @param seed The initial seed value.
   * @return The combined hash value.
   */
  virtual size_t hash_combine(size_t seed) const;

  friend class BTypeFactory;
  friend class BTypeCache;

  /**
   * @brief Constructor for BType. Only BTypeFactory can create
   * instances.
   * @param kind The kind of BType to create.
   */
  BType(Kind kind)
      : m_kind{kind}, m_index{SIZE_MAX}, m_hash_valid(false), m_hash(0) {};
  virtual ~BType() = default;
  /**
   * @brief Gets the hash value of the BType.
   * @return The hash value.
   * @note The hash is computed lazily and cached.
   */
  size_t hash() const {
    if (!m_hash_valid) {
      m_hash = hash_combine(0);
      m_hash_valid = true;
    }
    return m_hash;
  }

  /** @bref Gets the position in the BTypeFactory table
   * @return The index of the BType in the BTypeFactory table
   *
   * The index matches the creation order.
   */
  size_t index() const { return m_index; }

 private:
  /** @brief Deleted copy constructor to prevent copying. */
  BType(const BType &) = delete;
  /** @brief Deleted assignment operator to prevent assignment. */
  BType &operator=(const BType &) = delete;
  /** @brief The kind of BType. */
  Kind m_kind;

 protected:
  /** @brief The position of the BType in the BTypeFactory table */
  size_t m_index;

 private:
  /** @brief Flag to indicate if the hash value is valid. */
  mutable bool m_hash_valid = false;
  /** @brief Cached hash value. */
  mutable size_t m_hash;
};

/**
 * @brief Factory class for creating and managing BType instances.
 *
 * This class ensures maximal sharing of BType instances and provides
 * thread-safe access to create and retrieve BTypes. It implements
 * the Factory pattern and Singleton-like behavior.
 */
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

  /**
   * @brief Gets the number of BTypes created by the factory.
   * @return The number of BTypes.
   */
  static size_t size();
  /**
   * @brief Gets the BType at a specific index in the factory's internal index.
   * @param index The index of the BType to retrieve.
   * @return A shared pointer to the BType at the given index.
   */
  static std::shared_ptr<BType> at(size_t index);

  /**
   * @brief Gets a named BType (AbstractSet or EnumeratedSet) by name.
   * @param name The name of the BType.
   * @return A shared pointer to the named BType, or nullptr if not found.
   * @note Not yet implemented.
   */
  static std::shared_ptr<BType> Named(const std::string &name);

  class Exception : public std::exception {
   public:
    Exception(const std::string &msg) : msg{msg} {}
    const char *what() const noexcept override { return msg.c_str(); }

   private:
    std::string msg;
  };

  /*@desc writes the type table in XML format to the output stream
   * @param os the output stream
   * @return void
   *
   * @note the produced XML is compatible with the schema in RichTypesInfo.xsd
   */
  static void writeXMLRichTypesInfo(std::ostream &os);

 public:
  /**
   * @brief Builds B types from an XML document following RichTypesInfo schema
   * @param root The tinyxml2 XML element RichTypeInfos
   * @throw BTypeFactory::Exception if the XML is invalid or parsing fails
   */
  static void buildFromXML(const tinyxml2::XMLElement *root);

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
  const std::vector<std::string> &getValues() const { return m_values; }
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
  const std::vector<std::pair<std::string, std::shared_ptr<BType>>> &getFields()
      const {
    return m_fields;
  }
  StructType(
      const std::vector<std::pair<std::string, std::shared_ptr<BType>>> &fields)
      : BType(BType::Kind::Struct), m_fields{sort(fields)} {}
  virtual ~StructType() = default;

  friend class BTypeFactory;
  friend class BTypeCache;
};

#endif
