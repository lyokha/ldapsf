ldapsf
======

A small C++ library that implements almost complete LDAP Search Filter grammar
as defined in [RFC 2254](http://tools.ietf.org/search/rfc2254) and a search
facility inside a vector of key-value mapped data.

Main highlights are:

* Formal Search Filter BNF grammar using header-only
  [boost::spirit](http://www.boost.org/doc/libs/release/libs/spirit/) library.
  *Extensible* filter grammar is not implemented.
* The parser supports Unicode and hex-encoded Unicode rules.
* The search facility makes use of the [ICU](http://site.icu-project.org/)
  internationalization library with adjustable collation levels which allows
  searching data in a non-strict way when case and diacritics may be set to
  ignore.
* Heavy use of newer *C++11* standard features like *move semantics* and
  *lambda functions*.

There is a test example showing how to use the library inside sub-directory
*test/*. In this example data read from a *JSON* file *records.json* which
requires linkage against *jsoncpp*. To compile the example run

```sh
g++ -std=c++11 -I/usr/include/jsoncpp -o test test.cpp -lldapsf -ljsoncpp
```

in a terminal. The data records in the test file contain UTF-8 encoded samples
of key-value maps with Russian *names*, *surnames* and optional *grades* and
*extra* marks. Inside the source code of *test.cpp* a search filter is created
to show how the *ldapsf* library can be used. Run the test with command

```sh
./test
```

