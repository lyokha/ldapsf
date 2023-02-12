/*
 * =============================================================================
 *
 *       Filename:  ldap_sf_grammar.h
 *
 *    Description:  LDAP Search Filter grammar
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

#ifndef LDAP_SF_GRAMMAR_H
#define LDAP_SF_GRAMMAR_H

#define BOOST_SPIRIT_UNICODE

#include <string>
#include <vector>
#include <memory>
#include <boost/spirit/include/qi.hpp>
#include <boost/phoenix/core.hpp>
#include <boost/phoenix/operator.hpp>
#include <boost/phoenix/function.hpp>
#include <boost/optional.hpp>
#include "ldap_sf_ast.h"


namespace  ldap
{
namespace  sf
{

using namespace boost::spirit;
using namespace boost::spirit::qi;
using namespace boost::spirit::unicode;
using namespace boost::phoenix;
using boost::spirit::qi::rule;
using boost::spirit::unicode::space_type;
using boost::spirit::unicode::alpha;
using boost::spirit::unicode::alnum;
using boost::spirit::unicode::char_;
using boost::spirit::unused_type;
using boost::spirit::_1;
using boost::spirit::_2;
using boost::spirit::_3;


struct  Compiler
{
  template  < typename  A, typename  B = unused_type,
              typename  C = unused_type, typename  D = unused_type >
  struct  result { typedef void  type; };

  void  operator()( NodeList &  nodes, Node &  node ) const;

  void  operator()( Node &  node, NodeList &  nodes, FilterComp  comp ) const;

  void  operator()( Node &  node, Node &  nodeNest ) const;

  void  operator()( ItemPtr &  item, AttrOptionsList &  attrs,
                    SimpleItemOp  op, Value &  value ) const;

  void  operator()( AttrOptionsList &  attrs, Attr &  value,
                    boost::optional< AttrOptionsList > &  attrs_options ) const;

  void  operator()( ItemPtr &  item, AttrOptionsList &  attrs,
                    ValueListMore &  values ) const;

  void  operator()( ValueListMore &  values,
                    boost::optional< Value > &  value_first,
                    ValueList &  values_middle,
                    boost::optional< Value > &  value_last ) const;
};


template  < typename  Iterator >
struct  Grammar : grammar< Iterator, Node(), space_type >
{
  Grammar();

  rule< Iterator, Node(), space_type >             filter;

  rule< Iterator, NodeList(), space_type >         filterlist;

  rule< Iterator, Node(), space_type >             filtercomp;

  rule< Iterator, Node(), space_type >             and_expr;

  rule< Iterator, Node(), space_type >             or_expr;

  rule< Iterator, Node(), space_type >             not_expr;

  rule< Iterator, ItemPtr(), space_type >          item;

  rule< Iterator, ItemPtr(), space_type >          simple;

  rule< Iterator, ItemPtr(), space_type >          substring;

  rule< Iterator, ItemPtr(), space_type >          extensible;

  rule< Iterator, SimpleItemOp(), space_type >     filtertype;

  rule< Iterator, AttrOptionsList(), space_type >  attr;

  rule< Iterator, Attr(), space_type >             attr_item;

  rule< Iterator, Value(), space_type >            value;

  rule< Iterator, ValueListMore(), space_type >    values;

  rule< Iterator, MatchingRule(), space_type >     matchingrule;

  uint_parser< unsigned char, 16, 2, 2 >           hex_2;

  uint_parser< unsigned int, 16, 8, 8 >            hex_8;

  function< Compiler >                             op;
};


template  < typename  Iterator >
Grammar< Iterator >::Grammar() : Grammar::base_type( filter )
{
  filter = lit( '(' ) >> filtercomp >> lit( ')' );

  filtercomp = and_expr | or_expr | not_expr | item;

  filterlist = +( filter[ op( _val, _1 ) ] );

  and_expr = lit( '&' ) >> filterlist[ op( _val, _1, FC_And ) ];

  or_expr = lit( '|' ) >> filterlist[ op( _val, _1, FC_Or ) ];

  not_expr = lit( '!' ) >> filter[ op( _val, _1 ) ];

  item = substring | simple | extensible;

  simple = ( attr >> filtertype >> value )[ op( _val, _1, _2, _3 ) ];

  attr = ( attr_item >> -( lit( ';' ) >> attr ) )[ op( _val, _1, _2 ) ];

  attr_item = alpha >> *( alnum | '-' );

  value = +( lit( '\\' ) >> ( hex_8 | hex_2 ) | ( char_ - '*' - ')' ) );

  filtertype = lit( '=' )[ _val = SIO_Equal ] |
      lit( "~=" )[ _val = SIO_Approx ] |
      lit( ">=" )[ _val = SIO_Greater ] |
      lit( "<=" )[ _val = SIO_Less ];

  substring = ( attr >> lit( '=' ) >> values )[ op( _val, _1, _2 ) ];

  values = ( -( value ) >> lit( '*' ) >> *( value >> lit( '*' ) ) >>
             -( value ) )[ op( _val, _1, _2, _3 ) ];

  matchingrule = alnum >> *( alnum | '-' | '.' );

  extensible = ( ( attr >> -( lit ( ":dn" ) ) >>
                   -( lit( ':' ) >> matchingrule ) >> lit( ":=" ) >> value )
                 |
                 ( -( lit( ":dn" ) ) >> lit( ':' ) >> matchingrule >>
                   lit( ":=" ) >> value ) )
      [ _pass = false /* extensible items are not implemented! */ ];
}

}   // namespace sf
}   // namespace ldap

#endif

