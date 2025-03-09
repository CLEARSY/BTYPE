/* @file btype_xml_writer_tests.cpp
   @brief Unit tests for the BTypeFactory::writeXMLRichTypesInfo method.

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

// Test case for BFactory::writeXMLRichTypesInfo
TEST_F(BTypeTest, WriteXMLRichTypesInfo) {
  // Create various types
  auto intType = BTypeFactory::Integer();
  auto boolType = BTypeFactory::Boolean();
  auto floatType = BTypeFactory::Float();
  auto realType = BTypeFactory::Real();
  auto stringType = BTypeFactory::String();
  auto productType = BTypeFactory::Product(intType, boolType);
  auto powerSetType = BTypeFactory::PowerSet(intType);
  auto abstractSetType = BTypeFactory::AbstractSet("MySet");
  std::vector<std::string> enumValues = {"One", "Two", "Three"};
  auto enumSetType = BTypeFactory::EnumeratedSet("Colors", enumValues);
  std::vector<std::pair<std::string, std::shared_ptr<BType>>> fields = {
      {"field1", intType}, {"field2", boolType}};
  auto structType = BTypeFactory::Struct(fields);

  // Generate XML
  std::ostringstream os;
  BTypeFactory::writeXMLRichTypesInfo(os);
  std::string xmlOutput = os.str();

  // Expected XML structure (simplified for brevity)
  std::string expectedXMLStart = "<RichTypesInfo>\n";
  std::string expectedXMLEnd = "</RichTypesInfo>\n";

  // Check if the generated XML contains the expected structure
  EXPECT_TRUE(xmlOutput.find(expectedXMLStart) != std::string::npos);
  EXPECT_TRUE(xmlOutput.find(expectedXMLEnd) != std::string::npos);

  // Additional checks for specific types can be added here
  EXPECT_TRUE(
      xmlOutput.find("<RichType id=\"0\">\n    <INTEGER/>\n  </RichType>\n") !=
      std::string::npos);
  EXPECT_TRUE(
      xmlOutput.find("<RichType id=\"1\">\n    <BOOL/>\n  </RichType>\n") !=
      std::string::npos);
  EXPECT_TRUE(
      xmlOutput.find("<RichType id=\"2\">\n    <FLOAT/>\n  </RichType>\n") !=
      std::string::npos);
  EXPECT_TRUE(
      xmlOutput.find("<RichType id=\"3\">\n    <REAL/>\n  </RichType>\n") !=
      std::string::npos);
  EXPECT_TRUE(
      xmlOutput.find("<RichType id=\"4\">\n    <STRING/>\n  </RichType>\n") !=
      std::string::npos);
  EXPECT_TRUE(xmlOutput.find("<RichType id=\"5\">\n    <CartesianProduct "
                             "arg1=\"0\" arg2=\"1\"/>\n  </RichType>\n") !=
              std::string::npos);
  EXPECT_TRUE(
      xmlOutput.find(
          "<RichType id=\"6\">\n    <PowerSet arg=\"0\"/>\n  </RichType>\n") !=
      std::string::npos);
  EXPECT_TRUE(xmlOutput.find("<RichType id=\"7\">\n    <AbstractSet "
                             "name=\"MySet\"/>\n  </RichType>\n") !=
              std::string::npos);
  EXPECT_TRUE(xmlOutput.find(
                  "<RichType id=\"8\">\n    <EnumeratedSet name=\"Colors\">\n  "
                  "    <EnumeratedValue name=\"One\"/>\n      <EnumeratedValue "
                  "name=\"Two\"/>\n      <EnumeratedValue name=\"Three\"/>\n   "
                  " </EnumeratedSet>\n  </RichType>\n") != std::string::npos);
  EXPECT_TRUE(xmlOutput.find(
                  "<RichType id=\"9\">\n    <StructType>\n      <Field "
                  "name=\"field1\" type=\"0\"/>\n      <Field name=\"field2\" "
                  "type=\"1\"/>\n    </StructType>\n  </RichType>\n") !=
              std::string::npos);
}

int main(int argc, char **argv) {
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
