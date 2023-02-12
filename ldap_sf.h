/*
 * =============================================================================
 *
 *       Filename:  ldap_sf.h
 *
 *    Description:  LDAP Search Filter
 *
 *        Version:  1.0
 *        Created:  26.02.2015 13:06:40
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Alexey Radkov (), 
 *        Company:  
 *
 * =============================================================================
 */

#ifndef LDAP_SF_H
#define LDAP_SF_H

#include "ldap_sf_encoding.h"
#include "ldap_sf_grammar.h"
#include "ldap_sf_eval.h"


namespace  ldap
{

class  SearchFilter
{
  public:
    sf::Node       buildQuery( const std::string &  query ) const;

    RecordListPtr  operator()( const sf::Node &  ast,
                      const RecordList &  records,
                      sf::Eval::Collator::ECollationStrength  strength =
                                          sf::Eval::Collator::PRIMARY,
                      const sf::Eval::Locale &  loc =
                                          sf::Eval::getDefaultLocale() ) const;

    RecordListPtr  operator()( const std::string &  query,
                      const RecordList &  records,
                      sf::Eval::Collator::ECollationStrength  strength =
                                          sf::Eval::Collator::PRIMARY,
                      const sf::Eval::Locale &  loc =
                                          sf::Eval::getDefaultLocale() ) const;

  private:
    sf::Grammar< GrammarIterator >  grammar_;

    sf::Eval                        eval_;

    sf::space_type                  space_;
};

}   // namespace ldap

#endif

