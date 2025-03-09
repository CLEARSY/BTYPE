#include <fmt/format.h>

#include <iostream>
#include <map>
#include <memory>
#include <sstream>

#include "btype.h"
#include "btype_fmt.h"

void BTypeFactory::writeXMLRichTypesInfo(std::ostream &os) {
  std::size_t nbTypes = BTypeFactory::size();
  std::map<std::shared_ptr<BType>, int> reverseIndex;
  for (auto i = 0u; i < nbTypes; i++) {
    reverseIndex[BTypeFactory::at(i)] = i;
  }
  os << "<RichTypesInfo>\n";
  for (auto i = 0u; i < nbTypes; i++) {
    auto type = BTypeFactory::at(i);
    os << "  <RichType id=\"" << i << "\">\n";
    /*
                <xs:choice>
                <xs:element name="BOOL" />
                <xs:element name="INTEGER" />
                <xs:element name="REAL" />
                <xs:element name="FLOAT" />
                <xs:element name="STRING" />
                <xs:element name="PowerSet" type="PowerSet" />
                <xs:element name="CartesianProduct" type="CartesianProduct" />
                <xs:element name="AbstractSet" type="AbstractSet" />
                <xs:element name="EnumeratedSet" type="EnumeratedSet" />
            </xs:choice>
      */
    switch (type->getKind()) {
      case BType::Kind::BOOLEAN:
        os << "    <BOOL/>\n";
        break;
      case BType::Kind::INTEGER:
        os << "    <INTEGER/>\n";
        break;
      case BType::Kind::REAL:
        os << "    <REAL/>\n";
        break;
      case BType::Kind::FLOAT:
        os << "    <FLOAT/>\n";
        break;
      case BType::Kind::STRING:
        os << "    <STRING/>\n";
        break;
      case BType::Kind::PowerType:
        /*
        <xs:complexType name="PowerSet">
          <xs:attribute name="arg" type="xs:integer">
        </xs:complexType>
        */
        os << "    <PowerSet arg=\""
           << reverseIndex[type->toPowerType()->m_content] << "\"/>\n";
        break;
      case BType::Kind::ProductType:
        /*
        <xs:complexType name="CartesianProduct">
        <xs:attribute name="arg1" type="xs:integer"/>
        <xs:attribute name="arg2" type="xs:integer"/>
        </xs:complexType>
        */
        os << "    <CartesianProduct"
           << " arg1=\"" << reverseIndex[type->toProductType()->lhs] << "\""
           << " arg2=\"" << reverseIndex[type->toProductType()->rhs] << "\""
           << "/>\n";
        break;
      case BType::Kind::AbstractSet:
        /*
          <xs:complexType name="AbstractSet">
          <xs:attribute name="name" type="xs:string"/>
          </xs:complexType>
        */
        os << "    <AbstractSet name=\"" << type->toAbstractSetType()->getName()
           << "\"/>\n";
        break;
      case BType::Kind::EnumeratedSet:
        /*
          <xs:complexType name="EnumeratedSet">
          <xs:attribute name="name" type="xs:string"/>
          <xs:sequence>
            <xs:element name="EnumeratedValue" type="EnumeratedValue"
          minOccurs="1" maxOccurs="unbounded" />
          </xs:sequence>
          </xs:complexType>
        */
        os << "    <EnumeratedSet name=\""
           << type->toEnumeratedSetType()->getName() << "\">\n";
        for (const auto &value :
             type->toEnumeratedSetType()
                 ->getValues()) {  // std::vector<std::string>
          /*
            <xs:complexType name="EnumeratedValue">
            <xs:attribute name="name" type="xs:string"/>
            </xs:complexType>
          */
          os << "      <EnumeratedValue name=\"" << value << "\"/>\n";
        }
        os << "    </EnumeratedSet>\n";
        break;
      case BType::Kind::Struct:
        /*
          <xs:complexType name="StructType">
          <xs:sequence>
            <xs:element name="Field" type="Field" minOccurs="1"
          maxOccurs="unbounded" />
          </xs:sequence>
          </xs:complexType>
        */
        os << "    <StructType>\n";
        for (const auto &field :
             type->toStructType()
                 ->getFields()) {  // std::vector<std::pair<std::string,
          /*
            <xs:complexType name="Field">
            <xs:attribute name="name" type="xs:string"/>
            <xs:attribute name="type" type="xs:integer"/>
            </xs:complexType>
          */
          os << "      <Field name=\"" << field.first << "\" type=\""
             << reverseIndex[field.second] << "\"/>\n";
        }
        os << "    </StructType>\n";
        break;
    }
    os << "  </RichType>\n";
  }
  os << "</RichTypesInfo>\n";
}