/*
 * =============================================================================
 *
 *       Filename:  ldap_sf_grammar.cpp
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

#include <utility>
#include <iterator>
#include <algorithm>
#include <boost/variant/get.hpp>
#include "ldap_sf_grammar.h"


namespace  ldap
{
namespace  sf
{

void  Compiler::operator()( NodeList &  nodes, Node &  node ) const
{
  nodes.emplace_back( std::move( node ) );
}


void  Compiler::operator()( Node &  node, NodeList &  nodes,
                            FilterComp  comp ) const
{
  node = comp;
  Subtree &  tree = boost::get< Subtree >( node );
  tree.children_ = std::move( nodes );
}


void  Compiler::operator()( Node &  node, Node &  nodeNest ) const
{
  node = FC_Not;
  Subtree &  tree = boost::get< Subtree >( node );
  tree.children_.emplace_back( std::move( nodeNest ) );
}


void  Compiler::operator()( ItemPtr &  item, AttrOptionsList &  attrs,
                            SimpleItemOp  op, Value &  value ) const
{
  item = std::make_shared< Item >( IT_Simple );
  item->attr_ = std::move( attrs.front() );
  std::move( std::next( attrs.begin() ), attrs.end(),
             std::back_inserter( item->attr_ops_ ) );
  item->simple_op_ = op;
  item->value_ = std::move( value );
}


void  Compiler::operator()( AttrOptionsList &  attrs, Attr &  value,
                  boost::optional< AttrOptionsList > &  attrs_options ) const
{
  attrs.emplace_back( std::move( value ) );
  if ( attrs_options )
  {
    AttrOptionsList &  attrs_more_list = *attrs_options;
    std::move( attrs_more_list.begin(), attrs_more_list.end(),
               std::back_inserter( attrs ) );
  }
}


void  Compiler::operator()( ItemPtr &  item, AttrOptionsList &  attrs,
                            ValueListMore &  values ) const
{
  ItemType  type( values.data_.empty() ? IT_Present : IT_Substring );
  item = std::make_shared< Item >( type );
  item->attr_ = std::move( attrs.front() );
  std::move( std::next( attrs.begin() ), attrs.end(),
             std::back_inserter( item->attr_ops_ ) );
  item->values_ = std::move( values );
}


void  Compiler::operator()( ValueListMore &  values,
                            boost::optional< Value > &  value_first,
                            ValueList &  values_middle,
                            boost::optional< Value > &  value_last ) const
{
  if ( value_first )
  {
    Value &  value = *value_first;
    values.data_.emplace_back( std::move( value ) );
    values.has_front_any_ = false;
  }
  std::move( values_middle.begin(), values_middle.end(),
             std::back_inserter( values.data_ ) );
  if ( value_last )
  {
    Value &  value = *value_last;
    values.data_.emplace_back( std::move( value ) );
    values.has_back_any_ = false;
  }
}

}   // namespace sf
}   // namespace ldap

