/*
 * =============================================================================
 *
 *       Filename:  ldap_sf_eval.h
 *
 *    Description:  LDAP Search Filter evaluation
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

#ifndef LDAP_SF_EVAL_H
#define LDAP_SF_EVAL_H

#include <string>
#include <utility>
#include <unicode/coll.h>
#include "ldap_sf_ast.h"
#include "ldap_records.h"


namespace  ldap
{
namespace  sf
{

class  Eval
{
  public:
    typedef icu::Collator  Collator;
    typedef icu::Locale    Locale;

  private:
    typedef std::pair< RecordListPtr, RecordListPtr >  RecordListPartition;

  public:
    RecordListPtr  operator()( const Node &  node, const RecordList &  records,
                               Collator::ECollationStrength  strength =
                                                              Collator::PRIMARY,
                               const Locale &  loc = getDefaultLocale() ) const
    {
      return eval( node, records, strength, loc ).first;
    }

  private:
    RecordListPartition  eval( const Node &  node,
                               const RecordList &  records,
                               Collator::ECollationStrength  strength,
                               const Locale &  loc ) const;

    RecordListPartition  evalItem( const ItemPtr  item,
                                   const RecordList &  records,
                                   Collator::ECollationStrength  strength,
                                   const Locale &  loc ) const;

    bool  testSubstring( const ValueListMore &  values,
                         const std::string &  s,
                         Collator::ECollationStrength  strength,
                         const Locale &  loc ) const;

  public:
    static const Locale &  getDefaultLocale( void );
};

}   // namespace sf
}   // namespace ldap

#endif

