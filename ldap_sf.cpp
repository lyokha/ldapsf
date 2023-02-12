/*
 * =============================================================================
 *
 *       Filename:  ldap_sf.cpp
 *
 *    Description:  LDAP Search Filter
 *
 *        Version:  1.0
 *        Created:  26.02.2015 20:53:52
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Alexey Radkov (), 
 *        Company:  
 *
 * =============================================================================
 */

#include <stdexcept>
#include "ldap_sf.h"


namespace  ldap
{

sf::Node  SearchFilter::buildQuery( const std::string &  query ) const
{
  GrammarIterator  begin( query.begin() );
  GrammarIterator  end( query.end() );
  sf::Node         ast;

  if ( ! sf::phrase_parse( begin, end, grammar_, space_, ast ) ||
       begin != end )
    throw std::runtime_error( "Failed to build a query '" + query + "'" );

  return ast;
}


RecordListPtr  SearchFilter::operator()( const sf::Node &  ast,
                              const RecordList &  records,
                              sf::Eval::Collator::ECollationStrength  strength,
                              const sf::Eval::Locale &  loc ) const
{
  return eval_( ast, records, strength, loc );
}


RecordListPtr  SearchFilter::operator()( const std::string &  query,
                              const RecordList &  records,
                              sf::Eval::Collator::ECollationStrength  strength,
                              const sf::Eval::Locale &  loc ) const
{
  sf::Node  ast( buildQuery( query ) );
  return eval_( ast, records, strength, loc );
}

}   // namespace ldap

