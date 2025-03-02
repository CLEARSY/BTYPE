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

#ifndef BTYPE_FMT_H
#define BTYPE_FMT_H

#include <fmt/format.h>
#include <fmt/ranges.h>

#include "btype.h"

// Forward declare formatter specializations
template <>
struct fmt::formatter<BType>;
template <>
struct fmt::formatter<std::shared_ptr<BType>>;
template <>
struct fmt::formatter<BType::ProductType>;
template <>
struct fmt::formatter<BType::PowerType>;
template <>
struct fmt::formatter<BType::AbstractSet>;
template <>
struct fmt::formatter<BType::EnumeratedSet>;
template <>
struct fmt::formatter<BType::StructType>;

// Formatter for shared_ptr<BType>
template <>
struct fmt::formatter<std::shared_ptr<BType>> {
  constexpr auto parse(format_parse_context& ctx) -> decltype(ctx.begin()) {
    return ctx.begin();
  }

  template <typename FormatContext>
  auto format(const std::shared_ptr<BType>& type, FormatContext& ctx) const
      -> decltype(ctx.out()) {
    if (!type) return fmt::format_to(ctx.out(), "nullptr");
    return fmt::format_to(ctx.out(), "{}", *type);
  }
};

// Formatter for BType
template <>
struct fmt::formatter<BType> {
  constexpr auto parse(format_parse_context& ctx) -> decltype(ctx.begin()) {
    return ctx.begin();
  }

  template <typename FormatContext>
  auto format(const BType& type, FormatContext& ctx) const
      -> decltype(ctx.out()) {
    class FormatterVisitor : public BType::Visitor {
     public:
      std::string result;

      void visitINTEGER() override { result = "INTEGER"; }
      void visitBOOLEAN() override { result = "BOOLEAN"; }
      void visitFLOAT() override { result = "FLOAT"; }
      void visitREAL() override { result = "REAL"; }
      void visitSTRING() override { result = "STRING"; }

      void visitProductType(const BType::ProductType& t) override {
        result = fmt::format("{}", t);
      }

      void visitPowerType(const BType::PowerType& t) override {
        result = fmt::format("{}", t);
      }

      void visitAbstractSet(const BType::AbstractSet& t) override {
        result = fmt::format("{}", t);
      }

      void visitEnumeratedSet(const BType::EnumeratedSet& t) override {
        result = fmt::format("{}", t);
      }

      void visitStructType(const BType::StructType& t) override {
        result = fmt::format("{}", t);
      }
    };

    FormatterVisitor visitor;
    type.accept(visitor);
    return fmt::format_to(ctx.out(), "{}", visitor.result);
  }
};

// Formatter for ProductType
template <>
struct fmt::formatter<BType::ProductType> {
  constexpr auto parse(format_parse_context& ctx) -> decltype(ctx.begin()) {
    return ctx.begin();
  }

  template <typename FormatContext>
  auto format(const BType::ProductType& type, FormatContext& ctx) const
      -> decltype(ctx.out()) {
    return fmt::format_to(ctx.out(), "({} × {})", type.lhs, type.rhs);
  }
};

// Formatter for PowerType
template <>
struct fmt::formatter<BType::PowerType> {
  constexpr auto parse(format_parse_context& ctx) -> decltype(ctx.begin()) {
    return ctx.begin();
  }

  template <typename FormatContext>
  auto format(const BType::PowerType& type, FormatContext& ctx) const
      -> decltype(ctx.out()) {
    return fmt::format_to(ctx.out(), "ℙ({})", type.m_content);
  }
};

// Formatter for AbstractSet
template <>
struct fmt::formatter<BType::AbstractSet> {
  constexpr auto parse(format_parse_context& ctx) -> decltype(ctx.begin()) {
    return ctx.begin();
  }

  template <typename FormatContext>
  auto format(const BType::AbstractSet& type, FormatContext& ctx) const
      -> decltype(ctx.out()) {
    return fmt::format_to(ctx.out(), "{}", type.getName());
  }
};

// Formatter for EnumeratedSet
template <>
struct fmt::formatter<BType::EnumeratedSet> {
  constexpr auto parse(format_parse_context& ctx) -> decltype(ctx.begin()) {
    return ctx.begin();
  }

  template <typename FormatContext>
  auto format(const BType::EnumeratedSet& type, FormatContext& ctx) const
      -> decltype(ctx.out()) {
    return fmt::format_to(ctx.out(), "{}", type.getName());
  }
};

// Formatter for RecordType
template <>
struct fmt::formatter<BType::StructType> {
  constexpr auto parse(format_parse_context& ctx) -> decltype(ctx.begin()) {
    return ctx.begin();
  }

  template <typename FormatContext>
  auto format(const BType::StructType& type, FormatContext& ctx) const
      -> decltype(ctx.out()) {
    std::string fields;
    bool first = true;
    for (const auto& [name, fieldType] : type.m_fields) {
      if (!first) fields += ", ";
      fields += fmt::format("{}: {}", name, fieldType);
      first = false;
    }
    return fmt::format_to(ctx.out(), "struct({{{}}})", fields);
  }
};

#endif  // BTYPE_FMT_H
