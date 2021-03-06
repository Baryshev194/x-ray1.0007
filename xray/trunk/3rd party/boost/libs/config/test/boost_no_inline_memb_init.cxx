//  (C) Copyright John Maddock 2001. Permission to copy, use, modify, sell and
//  distribute this software is granted provided this copyright notice appears
//  in all copies. This software is provided "as is" without express or implied
//  warranty, and with no claim as to its suitability for any purpose.

//  MACRO:         BOOST_NO_INCLASS_MEMBER_INITIALIZATION
//  TITLE:         inline member constant initialisation
//  DESCRIPTION:   Compiler violates std::9.4.2/4.


namespace boost_no_inclass_member_initialization{

struct UDT{};


template <bool b1, bool b2, bool b3, bool b4, bool b5, bool b6, bool b7>
struct ice_or_helper
{
   static const bool value = true;
};
template <>
struct ice_or_helper<false, false, false, false, false, false, false>
{
   static const bool value = false;
};

template <bool b1, bool b2, bool b3 = false, bool b4 = false, bool b5 = false, bool b6 = false, bool b7 = false>
struct ice_or
{
   static const bool value = ice_or_helper<b1, b2, b3, b4, b5, b6, b7>::value;
};

template <class T>
struct is_int
{
   static const bool value = false;
};

template <>
struct is_int<int>
{
   static const bool value = true;
};

int test()
{
   typedef int a1[ice_or< is_int<int>::value, is_int<UDT>::value>::value ? 1 : -1];
   return 0;
}

}

