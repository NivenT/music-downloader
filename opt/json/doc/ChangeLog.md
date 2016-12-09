# JSON for Modern C++

## Version 1.0.0-rc1

- Release date: 2015-07-26
- MD5: fac5948417ed49bfd0852a0e9dd36935

### Summary

The 1.0.0 release should be the first "official" release after the initial announcement of the class in January 2015 via reddit ("0.1.0") and a heavily overworked second version ("0.2.0") in February.

### Changes

- *Changed:* In the generic class `basic_json`, all JSON value types (array, object, string, bool, integer number, and floating-point) are now **templated**. That is, you can choose whether you like a `std::list` for your arrays or an `std::unordered_map` for your objects. The specialization `json` sets some reasonable defaults.
- *Changed:* The library now consists of a **single header**, called `json.hpp`. Consequently, build systems such as Automake or CMake are not any longer required.
- *Changed:* The **deserialization** is now supported by a lexer generated with [re2c](http://re2c.org) from file [`src/json.hpp.re2c`](https://github.com/nlohmann/json/blob/master/src/json.hpp.re2c). As a result, we strictly follow the JSON specification. Note neither the tool re2c nor its input are required to use the class.
- *Added:* The library now satisfies the [**ReversibleContainer**](http://en.cppreference.com/w/cpp/concept/ReversibleContainer) requirement. It hence provides four different iterators (`iterator`, `const_iterator`, `reverse_iterator`, and `const_reverse_iterator`), comparison functions, `swap()`, `size()`, `max_size()`, and `empty()` member functions.
- *Added*: The class uses **user-defined allocators** which default to `std::allocator`, but can be templated via parameter `Allocator`.
- *Added:* To simplify pretty-printing, the `std::setw` **stream manipulator** has been overloaded to set the desired indentation. Pretty-printing a JSON object `j` is as simple as `std::cout << std::setw(4) << j << '\n'`.
- *Changed*: The type `json::value_t::number` is now called `json::value_t::number_integer` to be more symmetric compared to `json::value_t::number_float`.
-  *Added*: The documentation is generated with Doxygen and hosted at [nlohmann.github.io/json](http://nlohmann.github.io/json/). Every public member function is thoroughly described including an example which also can be [tried online](http://melpon.org/wandbox/permlink/GnGKwji06WeVonlI).
- *Added*: The class is heavily unit-tested (3341774 assertions) and has a [line coverage of 100%](https://coveralls.io/github/nlohmann/json). With every commit, the code is compiled with g++ 4.9, g++ 5.0, Clang 3.6 (thanks to [Travis CI](https://travis-ci.org/nlohmann/json)), and Microsoft Visual Studio 14 2015 (thanks to [AppVeyor](https://ci.appveyor.com/project/nlohmann/json)).

