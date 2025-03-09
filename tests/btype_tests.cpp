/* @file btype_tests.cpp
   @brief Unit tests for the BType class and BTypeFactory class.

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

// Tests for BTypeFactory size and at methods
TEST_F(BTypeTest, FactorySizeAndAt) {
  // Ensure the factory is initially empty
  EXPECT_EQ(BTypeFactory::size(), 0);

  // Create some types
  auto intType = BTypeFactory::Integer();
  auto boolType = BTypeFactory::Boolean();
  auto floatType = BTypeFactory::Float();

  // Check the size after creating types
  EXPECT_EQ(BTypeFactory::size(), 3);

  // Check the types at specific indices
  EXPECT_EQ(BTypeFactory::at(0)->getKind(), BType::Kind::INTEGER);
  EXPECT_EQ(BTypeFactory::at(1)->getKind(), BType::Kind::BOOLEAN);
  EXPECT_EQ(BTypeFactory::at(2)->getKind(), BType::Kind::FLOAT);

  // Create more types
  auto realType = BTypeFactory::Real();
  auto stringType = BTypeFactory::String();

  // Check the size after creating more types
  EXPECT_EQ(BTypeFactory::size(), 5);

  // Check the types at specific indices
  EXPECT_EQ(BTypeFactory::at(3)->getKind(), BType::Kind::REAL);
  EXPECT_EQ(BTypeFactory::at(4)->getKind(), BType::Kind::STRING);
}

// Basic Types Tests
TEST_F(BTypeTest, BasicTypesCreation) {
  auto intType = BTypeFactory::Integer();
  auto boolType = BTypeFactory::Boolean();
  auto floatType = BTypeFactory::Float();
  auto realType = BTypeFactory::Real();
  auto stringType = BTypeFactory::String();

  EXPECT_EQ(intType->getKind(), BType::Kind::INTEGER);
  EXPECT_EQ(boolType->getKind(), BType::Kind::BOOLEAN);
  EXPECT_EQ(floatType->getKind(), BType::Kind::FLOAT);
  EXPECT_EQ(realType->getKind(), BType::Kind::REAL);
  EXPECT_EQ(stringType->getKind(), BType::Kind::STRING);
}

// Product Type Tests
TEST_F(BTypeTest, ProductTypeCreation) {
  auto product =
      BTypeFactory::Product(BTypeFactory::Integer(), BTypeFactory::Boolean());
  EXPECT_EQ(product->getKind(), BType::Kind::ProductType);

  auto productType = product->toProductType();
  ASSERT_NE(productType, nullptr);
  EXPECT_EQ(productType->lhs->getKind(), BType::Kind::INTEGER);
  EXPECT_EQ(productType->rhs->getKind(), BType::Kind::BOOLEAN);
}

// Power Type Tests
TEST_F(BTypeTest, PowerTypeCreation) {
  auto powerSet = BTypeFactory::PowerSet(BTypeFactory::Integer());
  EXPECT_EQ(powerSet->getKind(), BType::Kind::PowerType);

  auto powerType = powerSet->toPowerType();
  ASSERT_NE(powerType, nullptr);
  EXPECT_EQ(powerType->m_content->getKind(), BType::Kind::INTEGER);
}

// Abstract Set Tests
TEST_F(BTypeTest, AbstractSetCreation) {
  auto abstractSet = BTypeFactory::AbstractSet("MySet");
  EXPECT_EQ(abstractSet->getKind(), BType::Kind::AbstractSet);

  auto setType = abstractSet->toAbstractSetType();
  ASSERT_NE(setType, nullptr);
  EXPECT_EQ(setType->getName(), "MySet");
}

// Enumerated Set Tests
TEST_F(BTypeTest, EnumeratedSetCreation) {
  std::vector<std::string> values = {"One", "Two", "Three"};
  auto enumSet = BTypeFactory::EnumeratedSet("Colors", values);
  EXPECT_EQ(enumSet->getKind(), BType::Kind::EnumeratedSet);

  auto enumType = enumSet->toEnumeratedSetType();
  ASSERT_NE(enumType, nullptr);
  EXPECT_EQ(enumType->getName(), "Colors");
  EXPECT_EQ(enumType->getValues(), values);
}

// Record Type Tests
TEST_F(BTypeTest, StructTypeCreation) {
  std::vector<std::pair<std::string, std::shared_ptr<BType>>> fields = {
      {"field1", BTypeFactory::Integer()}, {"field2", BTypeFactory::Boolean()}};
  auto record = BTypeFactory::Struct(fields);
  EXPECT_EQ(record->getKind(), BType::Kind::Struct);

  auto structType = record->toStructType();
  ASSERT_NE(structType, nullptr);
  EXPECT_EQ(structType->m_fields.size(), 2);
  EXPECT_EQ(structType->m_fields[0].first, "field1");
  EXPECT_EQ(structType->m_fields[0].second->getKind(), BType::Kind::INTEGER);
}

TEST_F(BTypeTest, StructTypeCreationOrder) {
  std::vector<std::pair<std::string, std::shared_ptr<BType>>> fields1 = {
      {"field1", BTypeFactory::Integer()}, {"field2", BTypeFactory::Boolean()}};
  std::vector<std::pair<std::string, std::shared_ptr<BType>>> fields2 = {
      {"field2", BTypeFactory::Boolean()}, {"field1", BTypeFactory::Integer()}};
  auto struct1 = BTypeFactory::Struct(fields1);
  auto struct2 = BTypeFactory::Struct(fields2);
  EXPECT_EQ(struct1->getKind(), BType::Kind::Struct);
  EXPECT_EQ(struct2->getKind(), BType::Kind::Struct);
  EXPECT_EQ(struct1, struct2);

  auto structType1 = struct1->toStructType();
  auto structType2 = struct2->toStructType();
  EXPECT_EQ(structType1, structType2);
}

// Comparison Tests
TEST_F(BTypeTest, TypeComparisons) {
  auto int1 = BTypeFactory::Integer();
  auto int2 = BTypeFactory::Integer();
  auto bool1 = BTypeFactory::Boolean();

  EXPECT_EQ(*int1, *int2);
  EXPECT_NE(*int1, *bool1);
  EXPECT_TRUE(*int1 <= *int2);
  EXPECT_TRUE(*int1 >= *int2);
}

// Thread Safety Tests
TEST_F(BTypeTest, ThreadSafety) {
  const int numThreads = 10;
  std::vector<std::thread> threads;

  for (int i = 0; i < numThreads; ++i) {
    threads.emplace_back([i]() {
      auto product = BTypeFactory::Product(BTypeFactory::Integer(),
                                           BTypeFactory::Boolean());
      auto powerSet = BTypeFactory::PowerSet(BTypeFactory::Integer());
      auto abstractSet = BTypeFactory::AbstractSet("Set" + std::to_string(i));

      EXPECT_EQ(product->getKind(), BType::Kind::ProductType);
      EXPECT_EQ(powerSet->getKind(), BType::Kind::PowerType);
      EXPECT_EQ(abstractSet->getKind(), BType::Kind::AbstractSet);
    });
  }

  for (auto &thread : threads) {
    thread.join();
  }
}

// Maximal Sharing Tests
TEST_F(BTypeTest, MaximalSharing) {
  auto product1 =
      BTypeFactory::Product(BTypeFactory::Integer(), BTypeFactory::Boolean());
  auto product2 =
      BTypeFactory::Product(BTypeFactory::Integer(), BTypeFactory::Boolean());

  EXPECT_EQ(product1->toProductType(), product2->toProductType());
}

// Hash Consistency Tests
TEST_F(BTypeTest, HashConsistency) {
  auto product1 =
      BTypeFactory::Product(BTypeFactory::Integer(), BTypeFactory::Boolean());
  auto product2 =
      BTypeFactory::Product(BTypeFactory::Integer(), BTypeFactory::Boolean());

  EXPECT_EQ(product1->hash_combine(0), product2->hash_combine(0));
}

// Visitor Pattern Tests
class TestVisitor : public BType::Visitor {
 public:
  void visitINTEGER() override { lastVisited = "INTEGER"; }
  void visitBOOLEAN() override { lastVisited = "BOOLEAN"; }
  void visitFLOAT() override { lastVisited = "FLOAT"; }
  void visitREAL() override { lastVisited = "REAL"; }
  void visitSTRING() override { lastVisited = "STRING"; }
  void visitAbstractSet(const BType::AbstractSet &) override {
    lastVisited = "AbstractSet";
  }
  void visitEnumeratedSet(const BType::EnumeratedSet &) override {
    lastVisited = "EnumeratedSet";
  }
  void visitProductType(const BType::ProductType &) override {
    lastVisited = "ProductType";
  }
  void visitPowerType(const BType::PowerType &) override {
    lastVisited = "PowerType";
  }
  void visitStructType(const BType::StructType &) override {
    lastVisited = "StructType";
  }

  std::string lastVisited;
};

TEST_F(BTypeTest, VisitorPattern) {
  TestVisitor visitor;

  BTypeFactory::Integer()->accept(visitor);
  EXPECT_EQ(visitor.lastVisited, "INTEGER");

  auto product =
      BTypeFactory::Product(BTypeFactory::Integer(), BTypeFactory::Boolean());
  product->accept(visitor);
  EXPECT_EQ(visitor.lastVisited, "ProductType");
}

int main(int argc, char **argv) {
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
