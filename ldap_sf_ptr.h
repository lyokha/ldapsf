/*
 * =============================================================================
 *
 *       Filename:  ldap_sf_ptr.h
 *
 *    Description:  LDAP Search Filter shared pointer declaration
 *
 *        Version:  1.0
 *        Created:  26.02.2015 21:56:32
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Alexey Radkov (), 
 *        Company:  
 *
 * =============================================================================
 */

#ifndef LDAP_SF_PTR_H
#define LDAP_SF_PTR_H

#include <memory>


namespace  ldap
{

class  SearchFilter;

typedef std::shared_ptr< SearchFilter >  SearchFilterPtr;

}   // namespace ldap

#endif

