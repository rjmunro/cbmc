/*******************************************************************\

Module:

Author: Daniel Kroening, kroening@kroening.com

\*******************************************************************/

#include <iostream>

#include <util/std_expr.h>
#include <util/base_type.h>

#include <langapi/language_util.h>

#include "flatten_byte_operators.h"
#include "boolbv.h"

/*******************************************************************\

Function: boolbvt::convert_equality

  Inputs:

 Outputs:

 Purpose:

\*******************************************************************/

literalt boolbvt::convert_equality(const equal_exprt &expr)
{
  if(!base_type_eq(expr.lhs().type(), expr.rhs().type(), ns))
  {
    std::cout << "######### lhs: " << expr.lhs().pretty() << '\n';
    std::cout << "######### rhs: " << expr.rhs().pretty() << '\n';
    throw "equality without matching types";
  }

  // see if it is an unbounded array
  if(is_unbounded_array(expr.lhs().type()))
  {
    // flatten byte_update/byte_extract operators if needed

    if(has_byte_operator(expr))
    {
      exprt tmp=flatten_byte_operators(expr, ns);
      return record_array_equality(to_equal_expr(tmp));
    }

    return record_array_equality(expr);
  }

  const bvt &bv0=convert_bv(expr.lhs());
  const bvt &bv1=convert_bv(expr.rhs());

  if(bv0.size()!=bv1.size())
  {
    std::cerr << "lhs: " << expr.lhs().pretty() << '\n';
    std::cerr << "lhs size: " << bv0.size() << '\n';
    std::cerr << "rhs: " << expr.rhs().pretty() << '\n';
    std::cerr << "rhs size: " << bv1.size() << '\n';
    throw "unexpected size mismatch on equality";
  }

  if(bv0.empty())
  {
    // An empty bit-vector comparison. It's not clear
    // what this is meant to say.
    return prop.new_variable();
  }

  return bv_utils.equal(bv0, bv1);
}

/*******************************************************************\

Function: boolbvt::convert_verilog_case_equality

  Inputs:

 Outputs:

 Purpose:

\*******************************************************************/

literalt boolbvt::convert_verilog_case_equality(
  const binary_relation_exprt &expr)
{
  // This is 4-valued comparison, i.e., z===z, x===x etc.
  // The result is always Boolean.

  if(!base_type_eq(expr.lhs().type(), expr.rhs().type(), ns))
  {
    std::cout << "######### lhs: " << expr.lhs().pretty() << '\n';
    std::cout << "######### rhs: " << expr.rhs().pretty() << '\n';
    throw "verilog_case_equality without matching types";
  }

  const bvt &bv0=convert_bv(expr.lhs());
  const bvt &bv1=convert_bv(expr.rhs());

  if(bv0.size()!=bv1.size())
  {
    std::cerr << "lhs: " << expr.lhs().pretty() << '\n';
    std::cerr << "lhs size: " << bv0.size() << '\n';
    std::cerr << "rhs: " << expr.rhs().pretty() << '\n';
    std::cerr << "rhs size: " << bv1.size() << '\n';
    throw "unexpected size mismatch on verilog_case_equality";
  }

  if(expr.id()==ID_verilog_case_inequality)
    return !bv_utils.equal(bv0, bv1);
  else
    return bv_utils.equal(bv0, bv1);
}
