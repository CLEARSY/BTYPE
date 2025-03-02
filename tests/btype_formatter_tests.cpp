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
#include <gtest/gtest.h>

#include <future>
#include <thread>
#include <vector>

#include "btype_fmt.h"

class BTypeFmtTest : public ::testing::Test {
 protected:
  void SetUp() override {}
};

// Formatter Tests
TEST_F(BTypeFmtTest, BasicTypeFormatting) {
  EXPECT_EQ(fmt::format("{}", BTypeFactory::Integer()), "INTEGER");
  EXPECT_EQ(fmt::format("{}", BTypeFactory::Boolean()), "BOOLEAN");
  EXPECT_EQ(fmt::format("{}", BTypeFactory::Float()), "FLOAT");
  EXPECT_EQ(fmt::format("{}", BTypeFactory::Real()), "REAL");
  EXPECT_EQ(fmt::format("{}", BTypeFactory::String()), "STRING");
}

TEST_F(BTypeFmtTest, ProductTypeFormatting) {
  auto product =
      BTypeFactory::Product(BTypeFactory::Integer(), BTypeFactory::Boolean());
  EXPECT_EQ(fmt::format("{}", product), "(INTEGER × BOOLEAN)");
}

TEST_F(BTypeFmtTest, PowerTypeFormatting) {
  auto powerSet = BTypeFactory::PowerSet(BTypeFactory::Integer());
  EXPECT_EQ(fmt::format("{}", powerSet), "ℙ(INTEGER)");
}

TEST_F(BTypeFmtTest, AbstractSetFormatting) {
  auto abstractSet = BTypeFactory::AbstractSet("MySet");
  EXPECT_EQ(fmt::format("{}", abstractSet), "MySet");
}

TEST_F(BTypeFmtTest, EnumeratedSetFormatting) {
  std::vector<std::string> values = {"One", "Two", "Three"};
  auto enumSet = BTypeFactory::EnumeratedSet("Colors", values);
  EXPECT_EQ(fmt::format("{}", enumSet), "Colors");
}

TEST_F(BTypeFmtTest, StructTypeFormatting) {
  std::vector<std::pair<std::string, std::shared_ptr<BType>>> fields = {
      {"field1f", BTypeFactory::Integer()},
      {"field2f", BTypeFactory::Boolean()}};
  auto record = BTypeFactory::Struct(fields);
  EXPECT_EQ(fmt::format("{}", record),
            "struct({field1f: INTEGER, field2f: BOOLEAN})");
}

TEST_F(BTypeFmtTest, NullptrFormatting) {
  std::shared_ptr<BType> nullType;
  EXPECT_EQ(fmt::format("{}", nullType), "nullptr");
}

TEST_F(BTypeFmtTest, NestedTypeFormatting) {
  // Test nested product type
  auto nestedProduct = BTypeFactory::Product(
      BTypeFactory::Product(BTypeFactory::Integer(), BTypeFactory::Boolean()),
      BTypeFactory::String());
  EXPECT_EQ(fmt::format("{}", nestedProduct), "((INTEGER × BOOLEAN) × STRING)");

  // Test nested power type
  auto nestedPower =
      BTypeFactory::PowerSet(BTypeFactory::PowerSet(BTypeFactory::Integer()));
  EXPECT_EQ(fmt::format("{}", nestedPower), "ℙ(ℙ(INTEGER))");

  // Test complex nested structure
  std::vector<std::pair<std::string, std::shared_ptr<BType>>> fields = {
      {"field1g",
       BTypeFactory::Product(BTypeFactory::Integer(), BTypeFactory::Boolean())},
      {"field2g", BTypeFactory::PowerSet(BTypeFactory::String())}};
  auto complexStruct = BTypeFactory::Struct(fields);
  EXPECT_EQ(fmt::format("{}", complexStruct),
            "struct({field1g: (INTEGER × BOOLEAN), field2g: ℙ(STRING)})");
}

TEST_F(BTypeFmtTest, FormatterThreadSafety) {
  const int numThreads = 10;
  std::vector<std::thread> threads;
  std::vector<std::string> results(numThreads);

  auto complexType = BTypeFactory::Product(
      BTypeFactory::PowerSet(BTypeFactory::Integer()),
      BTypeFactory::Struct({{"field1t", BTypeFactory::Boolean()},
                            {"field2t", BTypeFactory::String()}}));

  for (int i = 0; i < numThreads; ++i) {
    threads.emplace_back([&results, complexType, i]() {
      results[i] = fmt::format("{}", complexType);
    });
  }

  for (auto& thread : threads) {
    thread.join();
  }

  // All threads should produce the same formatted string
  std::string expected = fmt::format("{}", complexType);
  for (const auto& result : results) {
    EXPECT_EQ(result, expected);
  }
}

TEST_F(BTypeFmtTest, FormatterEdgeCases) {
  // Empty struct
  auto emptyStruct = BTypeFactory::Struct({});
  EXPECT_EQ(fmt::format("{}", emptyStruct), "struct({})");

  // Empty enumerated set
  auto emptyEnum = BTypeFactory::EnumeratedSet("Empty", {});
  EXPECT_EQ(fmt::format("{}", emptyEnum), "Empty");

  // Abstract set with special characters
  auto specialSet = BTypeFactory::AbstractSet("Set@#$%");
  EXPECT_EQ(fmt::format("{}", specialSet), "Set@#$%");
}

// main function
int main(int argc, char** argv) {
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}