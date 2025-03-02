# BTYPE Library

## Overview

BTYPE is a C++ library designed to represent the B language type system. It is thread-safe and implements maximal sharing of subtypes. It also includes a visitor pattern and a formatter suitable for the `fmt` library.

## Features

- Basic Types: Integer, Boolean, Float, Real, String
- Complex Types: Product, Power Set, Abstract Set, Enumerated Set, Struct
- Type Comparisons: Equality, Inequality, Less than, Greater than, etc.
- Visitor Pattern: For type-specific operations
- Thread Safety: Ensures safe operations in multi-threaded environments
- Maximal Sharing: Efficient memory usage by sharing common subtypes

## Installation

To build and install the BTYPE library, follow these steps:

Clone the repository:

```sh
git clone https://github.com/CLEARSY/BTYPE.git
cd btype
```

Create a build directory and navigate to it:
```sh
mkdir build
cd build
```

Run CMake to configure the project:

```sh
cmake ..
```

Build the project:

```sh
make
```

(Optional) Run tests to verify the build:

```sh
ctest
```

## Usage
Include the BTYPE library in your project by adding the following include directive:

```cpp
#include "btype.h"
```

## Example
Here is a simple example demonstrating the creation and usage of various types:

```
#include "btype.h"
#include <iostream>

int main() {
    auto integerType = BTypeFactory::Integer();
    auto booleanType = BTypeFactory::Boolean();
    auto productType = BTypeFactory::Product(integerType, booleanType);
    auto powerType = BTypeFactory::PowerSet(integerType);

    std::cout << "Integer Type: " << integerType->getKind() << std::endl;
    std::cout << "Boolean Type: " << booleanType->getKind() << std::endl;
    std::cout << "Product Type: " << productType->getKind() << std::endl;
    std::cout << "Power Type: " << powerType->getKind() << std::endl;

    return 0;
}
```

## Testing

The BTYPE library includes a comprehensive test suite to ensure the correctness and reliability of the types and their operations. The tests are located in the tests directory and can be run using ctest.

### Running Tests

To run the tests, navigate to the build directory and execute:

```sh
ctest
```

The output will show the results of the tests, including any failures or errors.

### Example Test Output

```text
Start testing: Mar 01 14:35 CET
----------------------------------------------------------
1/2 Testing: btype_tests
1/2 Test: btype_tests
Command: "/Users/daviddeharbe/dev/exploration/build/tests/btype_tests"
Directory: /Users/daviddeharbe/dev/exploration/build/tests
"btype_tests" start time: Mar 01 14:35 CET
Output:
----------------------------------------------------------
[==========] Running 12 tests from 1 test suite.
[----------] Global test environment set-up.
[----------] 12 tests from BTypeTest
[ RUN      ] BTypeTest.BasicTypesCreation
[       OK ] BTypeTest.BasicTypesCreation (0 ms)
[ RUN      ] BTypeTest.ProductTypeCreation
[       OK ] BTypeTest.ProductTypeCreation (0 ms)
[ RUN      ] BTypeTest.PowerTypeCreation
[       OK ] BTypeTest.PowerTypeCreation (0 ms)
[ RUN      ] BTypeTest.AbstractSetCreation
[       OK ] BTypeTest.AbstractSetCreation (0 ms)
[ RUN      ] BTypeTest.EnumeratedSetCreation
[       OK ] BTypeTest.EnumeratedSetCreation (0 ms)
[ RUN      ] BTypeTest.StructTypeCreation
[       OK ] BTypeTest.StructTypeCreation (0 ms)
[ RUN      ] BTypeTest.StructTypeCreationOrder
[       OK ] BTypeTest.StructTypeCreationOrder (0 ms)
[ RUN      ] BTypeTest.TypeComparisons
[       OK ] BTypeTest.TypeComparisons (0 ms)
[ RUN      ] BTypeTest.ThreadSafety
[       OK ] BTypeTest.ThreadSafety (0 ms)
[ RUN      ] BTypeTest.MaximalSharing
[       OK ] BTypeTest.MaximalSharing (0 ms)
[ RUN      ] BTypeTest.HashConsistency
[       OK ] BTypeTest.HashConsistency (0 ms)
[ RUN      ] BTypeTest.VisitorPattern
[       OK ] BTypeTest.VisitorPattern (0 ms)
[----------] 12 tests from BTypeTest (0 ms total)

[----------] Global test environment tear-down
[==========] 12 tests from 1 test suite ran. (0 ms total)
[  PASSED  ] 12 tests.
<end of output>
Test time =   0.23 sec
----------------------------------------------------------
Test Passed.
"btype_tests" end time: Mar 01 14:35 CET
"btype_tests" time elapsed: 00:00:00
----------------------------------------------------------

2/2 Testing: btype_formatter_tests
2/2 Test: btype_formatter_tests
Command: "/Users/daviddeharbe/dev/exploration/build/tests/btype_formatter_tests"
Directory: /Users/daviddeharbe/dev/exploration/build/tests
"btype_formatter_tests" start time: Mar 01 14:35 CET
Output:
----------------------------------------------------------
[==========] Running 10 tests from 1 test suite.
[----------] Global test environment set-up.
[----------] 10 tests from BTypeFmtTest
[ RUN      ] BTypeFmtTest.BasicTypeFormatting
[       OK ] BTypeFmtTest.BasicTypeFormatting (0 ms)
[ RUN      ] BTypeFmtTest.ProductTypeFormatting
[       OK ] BTypeFmtTest.ProductTypeFormatting (0 ms)
[ RUN      ] BTypeFmtTest.PowerTypeFormatting
[       OK ] BTypeFmtTest.PowerTypeFormatting (0 ms)
[ RUN      ] BTypeFmtTest.AbstractSetFormatting
[       OK ] BTypeFmtTest.AbstractSetFormatting (0 ms)
[ RUN      ] BTypeFmtTest.EnumeratedSetFormatting
[       OK ] BTypeFmtTest.EnumeratedSetFormatting (0 ms)
[ RUN      ] BTypeFmtTest.StructTypeFormatting
[       OK ] BTypeFmtTest.StructTypeFormatting (0 ms)
[ RUN      ] BTypeFmtTest.NullptrFormatting
[       OK ] BTypeFmtTest.NullptrFormatting (0 ms)
[ RUN      ] BTypeFmtTest.NestedTypeFormatting
[       OK ] BTypeFmtTest.NestedTypeFormatting (0 ms)
[ RUN      ] BTypeFmtTest.FormatterThreadSafety
[       OK ] BTypeFmtTest.FormatterThreadSafety (0 ms)
[ RUN      ] BTypeFmtTest.FormatterEdgeCases
[       OK ] BTypeFmtTest.FormatterEdgeCases (0 ms)
[----------] 10 tests from BTypeFmtTest (0 ms total)

[----------] Global test environment tear-down
[==========] 10 tests from 1 test suite ran. (0 ms total)
[  PASSED  ] 10 tests.
<end of output>
Test time =   0.14 sec
----------------------------------------------------------
Test Passed.
"btype_formatter_tests" end time: Mar 01 14:35 CET
"btype_formatter_tests" time elapsed: 00:00:00
----------------------------------------------------------

End testing: Mar 01 14:35 CET
```

## License

BTYPE is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with this program. If not, see https://www.gnu.org/licenses/.

## Contributing

Contributions are welcome! Please carefully read the CONTRIBUTING.md file in this repository in case you consider contributing.

## Contact

For any questions or issues, please open an issue on the GitHub repository.

