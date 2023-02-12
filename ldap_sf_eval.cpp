/*
 * =============================================================================
 *
 *       Filename:  ldap_sf_eval.cpp
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

#include <algorithm>
#include <iterator>
#include <utility>
#include <stdexcept>
#include <boost/variant/get.hpp>
#include <boost/assert.hpp>
#include <unicode/locid.h>
#include <unicode/stsearch.h>
#include "ldap_sf_eval.h"

using namespace  icu;


namespace
{
  const std::string  col_errmsg( "Failed to create an instance of a collator" );
  const std::string  stsearch_errmsg( "Failed to create an instance of "
                                      "a string search object" );
}


namespace  ldap
{
namespace  sf
{

Eval::RecordListPartition  Eval::eval( const Node &  node,
                                       const RecordList &  records,
                                       Collator::ECollationStrength  strength,
                                       const Locale &  loc ) const
{
  RecordListPartition  res;
  const Subtree *      subtree( boost::get< Subtree >( &node ) );

  if ( subtree )
  {
    BOOST_ASSERT( ! subtree->children_.empty() );
    RecordListPartition  curp;

    switch ( subtree->comp_ )
    {
    case FC_And:
      curp = eval( subtree->children_[ 0 ], records, strength, loc );
      res.second = std::move( curp.second );
      for ( NodeList::const_iterator
            k( std::next( subtree->children_.begin() ) );
            k != subtree->children_.end(); ++k )
      {
        curp = eval( *k, *curp.first, strength, loc );
        res.second->insert( res.second->end(), curp.second->begin(),
                            curp.second->end() );
      }
      res.first = std::move( curp.first );
      break;
    case FC_Or:
      curp = eval( subtree->children_[ 0 ], records, strength, loc );
      res.first = std::move( curp.first );
      for ( NodeList::const_iterator
            k( std::next( subtree->children_.begin() ) );
            k != subtree->children_.end(); ++k )
      {
        curp = eval( *k, *curp.second, strength, loc );
        res.first->insert( res.first->end(), curp.first->begin(),
                           curp.first->end() );
      }
      res.second = std::move( curp.second );
      break;
    case FC_Not:
      res = eval( subtree->children_[ 0 ], records, strength, loc );
      std::swap( res.first, res.second );
      break;
    default:
      break;
    }
  }
  else
  {
    const ItemPtr *  item( boost::get< ItemPtr >( &node ) );
    if ( item )
      res = evalItem( *item, records, strength, loc );
  }

  BOOST_ASSERT( res.first && res.second );

  return res;
}


Eval::RecordListPartition Eval::evalItem( const ItemPtr  item,
                                      const RecordList &  records,
                                      Collator::ECollationStrength  strength,
                                      const Locale &  loc ) const
{
  RecordListPartition   res( std::make_shared< RecordList >(),
                             std::make_shared< RecordList >() );

  switch ( item->type_ )
  {
  case IT_Simple:
    std::partition_copy( records.begin(), records.end(),
                         std::back_inserter( *res.first ),
                         std::back_inserter( *res.second ),
                         [ &item, strength, &loc ]( const RecordPtr &  record )
                         {
                           Record::const_iterator  it(
                                                  record->find( item->attr_ ) );
                           return ( it == record->end() ? false :
                                    [ &item, &it, strength, &loc ]( void )
                                    {
                                      UErrorCode  status( U_ZERO_ERROR );
                                      Collator *  col(
                                          Collator::createInstance( loc,
                                                                    status ) );
                                      if ( U_FAILURE( status ) )
                                        throw std::runtime_error( col_errmsg );
                                      col->setStrength( strength );
                                      status = U_ZERO_ERROR;
                                      ValueConverter  convert;
                                      UCollationResult
                                            res( col->compareUTF8(
                                                  StringPiece( it->second ),
                                                  StringPiece(
                                                      convert.to_bytes(
                                                            item->value_ ) ),
                                                  status ) );
                                      delete col;
                                      if ( U_FAILURE( status ) )
                                        return false;
                                      switch ( item->simple_op_ )
                                      {
                                      case SIO_Equal:
                                      /* approx is implemented as equal! */
                                      case SIO_Approx:
                                        return res == UCOL_EQUAL;
                                      case SIO_Greater:
                                        return res == UCOL_GREATER ||
                                               res == UCOL_EQUAL;
                                      case SIO_Less:
                                        return res == UCOL_LESS ||
                                               res == UCOL_EQUAL;
                                      default:
                                        return false;
                                      }
                                    }()
                                  );
                         }
                       );
    break;
  case IT_Present:
    std::partition_copy( records.begin(), records.end(),
                         std::back_inserter( *res.first ),
                         std::back_inserter( *res.second ),
                         [ &item ]( const RecordPtr &  record )
                         {
                           return record->find( item->attr_ ) != record->end();
                         }
                       );
    break;
  case IT_Substring:
    std::partition_copy( records.begin(), records.end(),
                         std::back_inserter( *res.first ),
                         std::back_inserter( *res.second ),
                         [ &item, strength, &loc, this ](
                                                    const RecordPtr &  record )
                         {
                           Record::const_iterator  it(
                                                  record->find( item->attr_ ) );
                           return ( it == record->end() ? false :
                                    testSubstring( item->values_, it->second,
                                                   strength, loc )
                                  );
                         }
                       );
    break;
  default:
    break;
  }

  return res;
}


bool  Eval::testSubstring( const ValueListMore &  values,
                           const std::string &  s,
                           Collator::ECollationStrength  strength,
                           const Locale &  loc ) const
{
  BOOST_ASSERT( ! values.data_.empty() );

  auto            begin( values.data_.begin() );
  auto            end( values.data_.end() );
  UnicodeString   su( UnicodeString::fromUTF8( StringPiece( s ) ) );
  std::size_t     len( su.length() );
  std::size_t     pos( 0 );
  UErrorCode      status( U_ZERO_ERROR );
  ValueConverter  convert;

  if ( ! values.has_front_any_ )
  {
    UnicodeString  vu( UnicodeString::fromUTF8(
                                StringPiece( convert.to_bytes( *begin ) ) ) );
    StringSearch   it( vu, su, loc, NULL, status );

    if ( U_FAILURE( status ) )
      throw std::runtime_error( stsearch_errmsg );

    it.getCollator()->setStrength( strength );

    int32_t        su_curpos( it.first( status ) );

    if ( su_curpos != 0 )
      return false;

    pos = it.getMatchedLength();
    len -= pos;

    begin = std::next( begin );
  }

  if ( ! values.has_back_any_ )
  {
    end = std::prev( end );

    UnicodeString  vu( UnicodeString::fromUTF8(
                                StringPiece( convert.to_bytes( *end ) ) ) );
    StringSearch   it( vu, UnicodeString( su, pos ), loc, NULL, status );

    if ( U_FAILURE( status ) )
      throw std::runtime_error( stsearch_errmsg );

    it.getCollator()->setStrength( strength );

    int32_t        su_curpos( it.last( status ) );

    if ( su_curpos == USEARCH_DONE )
      return false;

    int32_t        su_curlen( it.getMatchedLength() );

    if ( su_curpos != len - su_curlen )
      return false;

    len -= su_curlen;
  }

  for ( auto  k( begin ); k != end; ++k )
  {
    UnicodeString  vu( UnicodeString::fromUTF8(
                                    StringPiece( convert.to_bytes( *k ) ) ) );
    StringSearch   it( vu, UnicodeString( su, pos, len ), loc, NULL, status );

    if ( U_FAILURE( status ) )
      throw std::runtime_error( stsearch_errmsg );

    it.getCollator()->setStrength( strength );

    int32_t        su_curpos( it.first( status ) );

    if ( su_curpos == USEARCH_DONE )
      return false;

    std::size_t    shift( su_curpos + it.getMatchedLength() );

    pos += shift;
    len -= shift;
  }

  return true;
}


const Eval::Locale &  Eval::getDefaultLocale( void )
{
  return Locale::getDefault();
}

}   // namespace sf
}   // namespace ldap

