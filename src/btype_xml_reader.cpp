/* @file btype_xml_reader.cpp
   @brief Implementation file for the BTypeFactory::buildFromXml method.

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
#include <functional>
#include <vector>

#include "btype.h"
#include "tinyxml2.h"

void BTypeFactory::buildFromXML(const tinyxml2::XMLElement* root) {
  std::vector<const tinyxml2::XMLElement*> richTypeElements;
  std::vector<std::shared_ptr<BType>> types;
  // First pass: create placeholder for all types to handle forward references
  for (auto typeElem = root->FirstChildElement("RichType"); typeElem;
       typeElem = typeElem->NextSiblingElement("RichType")) {
    int id = -1;
    if (typeElem->QueryIntAttribute("id", &id) != tinyxml2::XML_SUCCESS ||
        id < 0) {
      throw Exception("Invalid or missing id attribute");
    }
    if (id < 0 || static_cast<size_t>(id) != richTypeElements.size()) {
      throw Exception("RichType indexing is not contiguous");
    }
    richTypeElements.push_back(typeElem);
    types.push_back(nullptr);
  }

  std::function<void(const size_t)> typeOfXmlElement;

  typeOfXmlElement = [&](const size_t pos) -> void {
    if (types.at(pos)) {
      return;
    }
    const tinyxml2::XMLElement* typeElem = richTypeElements.at(pos);
    const tinyxml2::XMLElement* typeDefElem = typeElem->FirstChildElement();
    if (!typeDefElem) {
      throw Exception("Empty RichType element");
    }
    std::string elemName = typeDefElem->Name();
    std::shared_ptr<BType> type;

    if (elemName == "BOOL") {
      type = Boolean();
    } else if (elemName == "INTEGER") {
      type = Integer();
    } else if (elemName == "REAL") {
      type = Real();
    } else if (elemName == "FLOAT") {
      type = Float();
    } else if (elemName == "STRING") {
      type = String();
    } else if (elemName == "PowerSet") {
      int argId = -1;
      if (typeDefElem->QueryIntAttribute("arg", &argId) !=
              tinyxml2::XML_SUCCESS ||
          argId < 0 || static_cast<size_t>(argId) >= types.size()) {
        throw Exception("Invalid PowerSet arg reference");
      }
      typeOfXmlElement(argId);
      type = PowerSet(types.at(argId));
    } else if (elemName == "CartesianProduct") {
      int arg1Id = -1, arg2Id = -1;
      if (typeDefElem->QueryIntAttribute("arg1", &arg1Id) !=
              tinyxml2::XML_SUCCESS ||
          typeDefElem->QueryIntAttribute("arg2", &arg2Id) !=
              tinyxml2::XML_SUCCESS ||
          arg1Id < 0 || static_cast<size_t>(arg1Id) >= types.size() ||
          arg2Id < 0 || static_cast<size_t>(arg2Id) >= types.size()) {
        throw Exception("Invalid CartesianProduct arg references");
      }
      typeOfXmlElement(arg1Id);
      typeOfXmlElement(arg2Id);
      type = Product(types.at(arg1Id), types.at(arg2Id));
    } else if (elemName == "AbstractSet") {
      const char* name = typeDefElem->Attribute("name");
      if (!name) {
        throw Exception("Missing AbstractSet name attribute");
      }
      type = AbstractSet(name);
    } else if (elemName == "EnumeratedSet") {
      const char* name = typeDefElem->Attribute("name");
      if (!name) {
        throw Exception("Missing EnumeratedSet name attribute");
      }
      std::vector<std::string> values;
      for (auto valueElem = typeDefElem->FirstChildElement("EnumeratedValue");
           valueElem;
           valueElem = valueElem->NextSiblingElement("EnumeratedValue")) {
        const char* valueName = valueElem->Attribute("name");
        if (!valueName) {
          throw Exception("Missing EnumeratedValue name attribute");
        }
        values.push_back(valueName);
      }
      type = EnumeratedSet(name, values);
    } else if (elemName == "StructType") {
      std::vector<std::pair<std::string, std::shared_ptr<BType>>> fields;
      for (auto fieldElem = typeDefElem->FirstChildElement("Field"); fieldElem;
           fieldElem = fieldElem->NextSiblingElement("Field")) {
        const char* fieldName = fieldElem->Attribute("name");
        int fieldTypeId = -1;
        if (!fieldName ||
            fieldElem->QueryIntAttribute("type", &fieldTypeId) !=
                tinyxml2::XML_SUCCESS ||
            fieldTypeId < 0 ||
            static_cast<size_t>(fieldTypeId) >= types.size()) {
          throw Exception("Invalid Struct field definition");
        }
        typeOfXmlElement(fieldTypeId);
        fields.emplace_back(fieldName, types.at(fieldTypeId));
      }
      type = Struct(fields);
    } else {
      throw Exception("Unknown type element: " + elemName);
    }
    types[pos] = type;
  };

  for (auto i = 0u; i < richTypeElements.size(); i++) {
    typeOfXmlElement(i);
  }

  // Verify no gaps in the type vector
  for (size_t i = 0; i < types.size(); ++i) {
    if (!types[i]) {
      throw Exception("Missing type definition for id " + std::to_string(i));
    }
  }
}
