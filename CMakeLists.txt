cmake_minimum_required(VERSION 3.7 FATAL_ERROR)

set(name ldapsf)
project(${name} LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 11)

find_package(Boost 1.47 REQUIRED)
find_package(ICU REQUIRED COMPONENTS uc i18n)

include_directories(${Boost_INCLUDE_DIRS}
                    ${ICU_INCLUDE_DIRS}
                    ${CMAKE_CURRENT_SOURCE_DIR})

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_library(${name} SHARED ldap_sf_ast.cpp
                           ldap_sf_grammar.cpp
                           ldap_sf_eval.cpp
                           ldap_sf.cpp)

target_link_libraries(${name} ${ICU_LIBRARIES})

install(TARGETS ${name} LIBRARY DESTINATION lib${LIB_SUFFIX})
install(FILES ldap_sf_ast.h
              ldap_sf_grammar.h
              ldap_sf_eval.h
              ldap_records.h
              ldap_sf.h
              ldap_sf_ptr.h
              ldap_sf_encoding.h
              DESTINATION include/ldapsf)

set(CPACK_PACKAGE_NAME "libldapsf")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "LDAP Search Filter library")
set(CPACK_PACKAGE_VERSION "1.0.0")
set(CPACK_PACKAGE_VERSION_MAJOR "1")
set(CPACK_PACKAGE_VERSION_MINOR "0")
set(CPACK_PACKAGE_VERSION_PATCH "0")
include(CPack)

