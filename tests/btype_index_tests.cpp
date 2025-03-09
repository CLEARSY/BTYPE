/* @file btype_tests.cpp
   @brief Unit tests for the BType indexing,
   i.e., BType::index and BTypeFactory::at methods.

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
#include <gtest/gtest.h>

#include <thread>
#include <vector>

#include "btype.h"

class BTypeTest : public ::testing::Test {
 protected:
  void SetUp() override {}
};

// Index Tests
TEST_F(BTypeTest, TypeIndexing) {
  // Test basic types indexing
  auto intType = BTypeFactory::Integer();
  auto boolType = BTypeFactory::Boolean();
  auto floatType = BTypeFactory::Float();

  // Check that indices are assigned in creation order
  EXPECT_EQ(intType->index(), 0);
  EXPECT_EQ(boolType->index(), 1);
  EXPECT_EQ(floatType->index(), 2);

  // Test complex types indexing
  auto powerSet = BTypeFactory::PowerSet(intType);
  auto product = BTypeFactory::Product(intType, boolType);

  // Check indices continue incrementing
  EXPECT_EQ(powerSet->index(), 3);
  EXPECT_EQ(product->index(), 4);

  // Test struct type indexing
  std::vector<std::pair<std::string, std::shared_ptr<BType>>> fields = {
      {"field1", intType}, {"field2", boolType}};
  auto structType = BTypeFactory::Struct(fields);
  EXPECT_EQ(structType->index(), 5);

  // Test abstract set indexing
  auto abstractSet = BTypeFactory::AbstractSet("MySet");
  EXPECT_EQ(abstractSet->index(), 6);

  // Test enumerated set indexing
  std::vector<std::string> values = {"One", "Two", "Three"};
  auto enumSet = BTypeFactory::EnumeratedSet("Colors", values);
  EXPECT_EQ(enumSet->index(), 7);

  // Verify that getting the same type returns the same index
  auto intType2 = BTypeFactory::Integer();
  EXPECT_EQ(intType2->index(), intType->index());

  // Verify that indices match the factory's at() method
  for (size_t i = 0; i < BTypeFactory::size(); ++i) {
    EXPECT_EQ(BTypeFactory::at(i)->index(), i);
  }

  // Test that indices are consistent across equivalent types
  auto product2 = BTypeFactory::Product(intType, boolType);
  EXPECT_EQ(product2->index(), product->index());

  // Test that indices are consistent for struct types with same fields in
  // different order
  std::vector<std::pair<std::string, std::shared_ptr<BType>>> fields2 = {
      {"field2", boolType}, {"field1", intType}};
  auto structType2 = BTypeFactory::Struct(fields2);
  EXPECT_EQ(structType2->index(), structType->index());
}

int main(int argc, char **argv) {
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
