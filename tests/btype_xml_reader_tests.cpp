/* @file btype_xml_reader_tests.cpp
   @brief Unit tests for the BTypeFactory::buildFromXml method.

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

#include <fstream>
#include <sstream>
#include <thread>
#include <vector>

#include "btype.h"
#include "tinyxml2.h"

class BTypeTest : public ::testing::Test {
 protected:
  void SetUp() override {}
};

TEST_F(BTypeTest, XMLBuildTest) {
  const char* xmlContent = R"(
    <RichTypesInfo>
      <RichType id="0">
        <INTEGER/>
      </RichType>
      <RichType id="1">
        <BOOL/>
      </RichType>
      <RichType id="2">
        <PowerSet arg="0"/>
      </RichType>
      <RichType id="3">
        <CartesianProduct arg1="0" arg2="1"/>
      </RichType>
      <RichType id="4">
        <AbstractSet name="MyAbstractSet"/>
      </RichType>
      <RichType id="5">
        <EnumeratedSet name="MyEnumSet">
          <EnumeratedValue name="Value1"/>
          <EnumeratedValue name="Value2"/>
        </EnumeratedSet>
      </RichType>
      <RichType id="6">
        <StructType>
          <Field name="field1" type="0"/>
          <Field name="field2" type="1"/>
        </StructType>
      </RichType>
    </RichTypesInfo>
  )";

  tinyxml2::XMLDocument doc;
  tinyxml2::XMLError result = doc.Parse(xmlContent);
  ASSERT_EQ(result, tinyxml2::XML_SUCCESS)
      << "XML Parsing failed: " << doc.ErrorStr();

  tinyxml2::XMLElement* root = doc.FirstChildElement("RichTypesInfo");
  ASSERT_NE(root, nullptr) << "Root element 'RichTypesInfo' not found";

  try {
    BTypeFactory::buildFromXML(root);
  } catch (const BTypeFactory::Exception& e) {
    FAIL() << "Exception during XML build: " << e.what();
  }

  ASSERT_EQ(BTypeFactory::size(), 7);
  ASSERT_EQ(BTypeFactory::at(0)->getKind(), BType::Kind::INTEGER);
  ASSERT_EQ(BTypeFactory::at(1)->getKind(), BType::Kind::BOOLEAN);
  ASSERT_EQ(BTypeFactory::at(2)->getKind(), BType::Kind::PowerType);
  ASSERT_EQ(BTypeFactory::at(3)->getKind(), BType::Kind::ProductType);
  ASSERT_EQ(BTypeFactory::at(4)->getKind(), BType::Kind::AbstractSet);
  ASSERT_EQ(BTypeFactory::at(5)->getKind(), BType::Kind::EnumeratedSet);
  ASSERT_EQ(BTypeFactory::at(6)->getKind(), BType::Kind::Struct);

  ASSERT_EQ(BTypeFactory::at(2)->toPowerType()->m_content->getKind(),
            BType::Kind::INTEGER);
  ASSERT_EQ(BTypeFactory::at(3)->toProductType()->lhs->getKind(),
            BType::Kind::INTEGER);
  ASSERT_EQ(BTypeFactory::at(3)->toProductType()->rhs->getKind(),
            BType::Kind::BOOLEAN);
  ASSERT_EQ(BTypeFactory::at(4)->toAbstractSetType()->getName(),
            "MyAbstractSet");
  ASSERT_EQ(BTypeFactory::at(5)->toEnumeratedSetType()->getName(), "MyEnumSet");
  ASSERT_EQ(BTypeFactory::at(5)->toEnumeratedSetType()->getValues(),
            std::vector<std::string>({"Value1", "Value2"}));
  ASSERT_EQ(BTypeFactory::at(6)->toStructType()->getFields().size(), 2);
  ASSERT_EQ(BTypeFactory::at(6)->toStructType()->getFields()[0].first,
            "field1");
  ASSERT_EQ(
      BTypeFactory::at(6)->toStructType()->getFields()[0].second->getKind(),
      BType::Kind::INTEGER);
  ASSERT_EQ(BTypeFactory::at(6)->toStructType()->getFields()[1].first,
            "field2");
  ASSERT_EQ(
      BTypeFactory::at(6)->toStructType()->getFields()[1].second->getKind(),
      BType::Kind::BOOLEAN);
}

int main(int argc, char** argv) {
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
