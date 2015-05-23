/*
 * =============================================================================
 *
 *       Filename:  ldap_records.h
 *
 *    Description:  LDAP records list
 *
 *        Version:  1.0
 *        Created:  25.02.2015 12:57:57
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Alexey Radkov (), 
 *        Company:  
 *
 * =============================================================================
 */

#ifndef LDAP_RECORDS_H
#define LDAP_RECORDS_H

#include <map>
#include <vector>
#include <string>
#include <memory>
#include <iostream>


namespace  ldap
{

typedef std::map< std::string, std::string >  Record;

typedef std::shared_ptr< Record >             RecordPtr;

typedef std::vector< RecordPtr >              RecordList;

typedef std::shared_ptr< RecordList >         RecordListPtr;

}   // namespace ldap


inline std::ostream &  operator<<( std::ostream &  out,
                                   const ldap::RecordPtr &  record )
{
  out << std::endl;

  if ( ! record )
    out << "<null>";

  for ( const std::pair< std::string, std::string > &  k : *record )
    out << k.first << ": " << k.second << std::endl;

  return out;
}


#endif

