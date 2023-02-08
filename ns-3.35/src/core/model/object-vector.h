/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2007 INRIA, Mathieu Lacage
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation;
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * Authors: Mathieu Lacage <mathieu.lacage@gmail.com>
 */
#ifndef OBJECT_VECTOR_H
#define OBJECT_VECTOR_H

#include "object.h"
#include "ptr.h"
#include "attribute.h"
#include "object-ptr-container.h"

/**
 * \file
 * \ingroup attribute_ObjectVector
 * ns3::ObjectVectorValue attribute value declarations and template implementations.
 */

namespace ns3 {

/**
 * \ingroup attribute_ObjectVector
 * ObjectVectorValue is an alias for ObjectPtrContainerValue
 */
typedef ObjectPtrContainerValue ObjectVectorValue;

/**
 * \ingroup attribute_ObjectVector
 * MakeAccessorHelper implementation for ObjectVector.
 * \copydetails ns3::DoMakeAccessorHelperOne(U T::*)
 */
template <typename T, typename U>
Ptr<const AttributeAccessor> MakeObjectVectorAccessor (U T::*memberVariable);

// Documentation generated by print-introspected-doxygen.cc
template <typename T>
Ptr<const AttributeChecker> MakeObjectVectorChecker (void);

/**
 * \ingroup attribute_ObjectVector
 * \copydoc ns3::MakeObjectPtrContainerAccessor()
 */
template <typename T, typename U, typename INDEX>
Ptr<const AttributeAccessor> MakeObjectVectorAccessor (Ptr<U> (T::*get) (INDEX) const,
                                                       INDEX (T::*getN) (void) const);

/**
 * \ingroup attribute_ObjectVector
 * \copydoc ns3::MakeObjectPtrContainerAccessor()
 */
template <typename T, typename U, typename INDEX>
Ptr<const AttributeAccessor> MakeObjectVectorAccessor (INDEX (T::*getN) (void) const,
                                                       Ptr<U> (T::*get) (INDEX) const);

/***************************************************************
 *  Implementation of the templates declared above.
 ***************************************************************/

template <typename T, typename U>
Ptr<const AttributeAccessor>
MakeObjectVectorAccessor (U T::*memberVector)
{
  struct MemberStdContainer : public ObjectPtrContainerAccessor
  {
    virtual bool
    DoGetN (const ObjectBase *object, std::size_t *n) const
    {
      const T *obj = dynamic_cast<const T *> (object);
      if (obj == 0)
        {
          return false;
        }
      *n = (obj->*m_memberVector).size ();
      return true;
    }
    virtual Ptr<Object>
    DoGet (const ObjectBase *object, std::size_t i, std::size_t *index) const
    {
      const T *obj = static_cast<const T *> (object);
      typename U::const_iterator begin = (obj->*m_memberVector).begin ();
      typename U::const_iterator end = (obj->*m_memberVector).end ();
      std::size_t k = 0;
      for (typename U::const_iterator j = begin; j != end; j++, k++)
        {
          if (k == i)
            {
              *index = k;
              return *j;
              break;
            }
        }
      NS_ASSERT (false);
      // quiet compiler.
      return 0;
    }
    U T::*m_memberVector;
  } *spec = new MemberStdContainer ();
  spec->m_memberVector = memberVector;
  return Ptr<const AttributeAccessor> (spec, false);
}

template <typename T>
Ptr<const AttributeChecker>
MakeObjectVectorChecker (void)
{
  return MakeObjectPtrContainerChecker<T> ();
}

template <typename T, typename U, typename INDEX>
Ptr<const AttributeAccessor>
MakeObjectVectorAccessor (Ptr<U> (T::*get) (INDEX) const, INDEX (T::*getN) (void) const)
{
  return MakeObjectPtrContainerAccessor<T, U, INDEX> (get, getN);
}

template <typename T, typename U, typename INDEX>
Ptr<const AttributeAccessor>
MakeObjectVectorAccessor (INDEX (T::*getN) (void) const, Ptr<U> (T::*get) (INDEX) const)
{
  return MakeObjectPtrContainerAccessor<T, U, INDEX> (get, getN);
}

} // namespace ns3

#endif /* OBJECT_VECTOR_H */
