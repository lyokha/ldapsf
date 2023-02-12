ldapsf
======

A small C++ library that implements almost complete LDAP Search Filter grammar
as defined in [RFC 2254](http://tools.ietf.org/search/rfc2254) and a search
facility inside a vector of key-value mapped data.

Features:

* Formal Search Filter BNF grammar using header-only
  [boost::spirit](https://www.boost.org/doc/libs/release/libs/spirit/) library.
  *Extensible* filter grammar is not implemented.
* The parser supports Unicode and hex-encoded Unicode rules.
* The search facility makes use of the [ICU](https://icu.unicode.org/)
  internationalization library with adjustable collation levels which allows
  searching data in a non-strict way when case and diacritics are ignored.

There is a test example inside sub-directory *test/* showing how to use the
library. In this example, data is read from a *JSON* file *records.json*. This
requires linkage against *jsoncpp* library. To compile the example, run

```sh
g++ -std=c++11 -o test test.cpp -lldapsf -ljsoncpp
```

in a terminal. The data records in the test file contain UTF-8 encoded samples
of key-value maps with Russian *names*, *surnames* and optional *grades* and
*extra* marks. Inside the source code of *test.cpp* a search filter is created
to show how the *ldapsf* library can be used. Run the test with command

```sh
./test
```

