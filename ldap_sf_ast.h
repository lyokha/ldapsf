/*
 * =============================================================================
 *
 *       Filename:  ldap_sf_ast.h
 *
 *    Description:  LDAP Search Filter AST
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

#ifndef LDAP_SF_AST_H
#define LDAP_SF_AST_H

#include <vector>
#include <list>
#include <memory>
#include <boost/variant/recursive_variant.hpp>
#include "ldap_sf_encoding.h"


namespace  ldap
{
namespace  sf
{

using boost::variant;
using boost::recursive_wrapper;


enum  FilterComp
{
  FC_Uninitialized,
  FC_And,
  FC_Or,
  FC_Not
};


enum  ItemType
{
  IT_Unknown,
  IT_Simple,
  IT_Present,
  IT_Substring,
  IT_Extensible
};


enum  SimpleItemOp
{
  SIO_Uninitialized,
  SIO_Equal,
  SIO_Approx,
  SIO_Greater,
  SIO_Less
};


struct  Subtree;

struct  Item;

typedef recursive_wrapper< Subtree >  Tree;

typedef std::shared_ptr< Item >       ItemPtr;

typedef variant< Tree, ItemPtr >      Node;

typedef std::vector< Node >           NodeList;

typedef ValueType                     Value;

typedef std::string                   Attr;

typedef std::string                   MatchingRule;

typedef std::list< std::string >      AttrOptionsList;

typedef std::vector< ValueType >      ValueList;


struct  ValueListMore
{
  ValueListMore() : has_front_any_( true ), has_back_any_( true )
  {}

  bool                 has_front_any_;

  bool                 has_back_any_;

  ValueList            data_;
};


struct  Item
{
  Item( ItemType  type = IT_Unknown ) : type_( type ),
    simple_op_( SIO_Uninitialized )
  {}

  void  print( int  level = 0 ) const;

  ItemType             type_;

  SimpleItemOp         simple_op_;

  Attr                 attr_;

  AttrOptionsList      attr_ops_;

  Value                value_;

  ValueListMore        values_;
};


struct  Subtree
{
  Subtree( FilterComp  comp = FC_Uninitialized ) : comp_( comp )
  {}

  void  print( int  level = 0 ) const;

  NodeList             children_;

  FilterComp           comp_;

  static const int     print_indent_ = 4;
};

}   // namespace sf
}   // namespace ldap

#endif

