/*
 * =============================================================================
 *
 *       Filename:  test.cpp
 *
 *    Description:  test
 *
 *        Version:  1.0
 *        Created:  18.02.2015 18:28:33
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Alexey Radkov (), 
 *        Company:  
 *        Compile:  g++ -Wall -std=c++11 -I/usr/include/jsoncpp -o test \
 *                  test.cpp -lldapsf -ljsoncpp
 *
 * =============================================================================
 */

#include <string>
#include <ldapsf/ldap_records.h>
#include <ldapsf/ldap_sf.h>
#include <json/json.h>
#include <fstream>

#include <iostream>

using namespace  ldap;
using namespace  ldap::sf;


void  printQuery( const Node &  tree )
{
  const Subtree *    subtree( boost::get< Subtree >( &tree ) );

  std::cout << std::endl << ">>> Query" << std::endl;

  if ( subtree )
  {
    subtree->print();
  }
  else
  {
    const ItemPtr *  item( boost::get< ItemPtr >( &tree ) );
    if ( item )
      ( *item )->print();
  }
}


int  main( void )
{
  Json::Value        root;
  Json::Value        dummy;
  std::ifstream      stream( "records.json", std::ifstream::binary );
  RecordList         records;

  stream >> root;

  const Json::Value &  data( root.get( "data", dummy ) );

  for ( unsigned int  i( 0 ); i < data.size(); ++i )
  {
    Record  r;

    for ( auto  k: data[ i ].getMemberNames() )
      r.insert( Record::value_type( k, data[ i ][ k ].asString() ) );

    records.emplace_back( std::make_shared< Record >( std::move( r ) ) );
  }

  std::cout << ">>> Data";

  for ( auto  k : records )
      std::cout << k;

  SearchFilter       filter;

  std::string        query( u8"(&(name=*мен)(!(sname=*ван*))(grade=*))" );
  Node               tree( filter.buildQuery( query ) );
  Node               tree0( tree );

  std::cout << std::endl;
  printQuery( tree );

  RecordListPtr      found( filter( tree, records ) );

  std::cout << std::endl << ">>> Found (primary collation)";

  for ( const RecordPtr &  k : *found )
    std::cout << k;

  query = "(grade>=4)";
  tree  = filter.buildQuery( query );

  std::cout << std::endl;
  printQuery( tree );

  found = filter( tree, *found );

  std::cout << std::endl << ">>> Found (among prev result)";

  for ( const RecordPtr &  k : *found )
    std::cout << k;

  std::cout << std::endl;
  printQuery( tree0 );

  found = filter( tree0, records, Eval::Collator::IDENTICAL );

  std::cout << std::endl << ">>> Found (identical collation)";

  for ( const RecordPtr &  k : *found )
    std::cout << k;

  query = "(|(grade>=4)(extra=*))";
  tree  = filter.buildQuery( query );

  std::cout << std::endl;
  printQuery( tree );

  found = filter( tree, records );

  std::cout << std::endl << ">>> Found (primary collation)";

  for ( const RecordPtr &  k : *found )
    std::cout << k;

  return 0;
}

