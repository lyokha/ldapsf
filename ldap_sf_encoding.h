/*
 * =============================================================================
 *
 *           File:  ldap_sf_encoding.h
 *
 *    Description:  LDAP Search Filter encoding types
 *
 *        Version:  1.0
 *        Created:  12.02.2023 02:03:00
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Alexey Radkov (),
 *        Company:  
 *
 * =============================================================================
 */

#ifndef LDAP_SF_ENCODING_H
#define LDAP_SF_ENCODING_H

#include <string>
#include <codecvt>
#include <locale>
#include <boost/regex/pending/unicode_iterator.hpp>


namespace  ldap
{

typedef boost::u8_to_u32_iterator< std::string::const_iterator >
        GrammarIterator;

typedef std::u32string  ValueType;

typedef std::wstring_convert< std::codecvt_utf8< char32_t >, char32_t >
        ValueConverter;

}   // namespace ldap

#endif

