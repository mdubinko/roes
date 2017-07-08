///\file

/******************************************************************************
The MIT License(MIT)

Embedded Template Library.
https://github.com/ETLCPP/etl
http://www.etlcpp.com

Copyright(c) 2015 jwellbelove

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files(the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and / or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions :

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
******************************************************************************/

#ifndef __ETL_FLAT_SET__
#define __ETL_FLAT_SET__

#include <stddef.h>
#include <iterator>
#include <functional>

#include "iflat_set.h"
#include "vector.h"
#include "pool.h"

//*****************************************************************************
///\defgroup flat_set flat_set
/// A flat_set with the capacity defined at compile time.
/// Has insertion of O(N) and flat_set of O(logN)
/// Duplicate entries and not allowed.
///\ingroup containers
//*****************************************************************************

namespace etl
{
  template <typename T, const size_t MAX_SIZE_, typename TCompare = std::less<T> >
  //***************************************************************************
  /// A flat_set implementation that uses a fixed size buffer.
  ///\tparam T        The value type.
  ///\tparam TCompare The type to compare keys. Default = std::less<T>
  ///\tparam MAX_SIZE_ The maximum number of elements that can be stored.
  ///\ingroup flat_set
  //***************************************************************************
  class flat_set : public iflat_set<T, TCompare>
  {
  public:

    static const size_t MAX_SIZE = MAX_SIZE_;

    //*************************************************************************
    /// Constructor.
    //*************************************************************************
    flat_set()
      : iflat_set<T, TCompare>(lookup, storage)
    {
    }

    //*************************************************************************
    /// Copy constructor.
    //*************************************************************************
    flat_set(const flat_set& other)
      : iflat_set<T, TCompare>(lookup, storage)
    {
      iflat_set<T, TCompare>::assign(other.cbegin(), other.cend());
    }

    //*************************************************************************
    /// Constructor, from an iterator range.
    ///\tparam TIterator The iterator type.
    ///\param first The iterator to the first element.
    ///\param last  The iterator to the last element + 1.
    //*************************************************************************
    template <typename TIterator>
    flat_set(TIterator first, TIterator last)
      : iflat_set<T, TCompare>(lookup, storage)
    {
      iflat_set<T, TCompare>::assign(first, last);
    }

    //*************************************************************************
    /// Destructor.
    //*************************************************************************
    ~flat_set()
    {
      iflat_set<T, TCompare>::clear();
    }

    //*************************************************************************
    /// Assignment operator.
    //*************************************************************************
    flat_set& operator = (const flat_set& rhs)
    {
      if (&rhs != this)
      {
        iflat_set<T, TCompare>::assign(rhs.cbegin(), rhs.cend());
      }

      return *this;
    }

  private:

    typedef typename iflat_set<T, TCompare>::value_type node_t;

    // The pool of nodes.
    etl::pool<node_t, MAX_SIZE> storage;

    // The vector that stores pointers to the nodes.
    etl::vector<node_t*, MAX_SIZE> lookup;
  };
}

#endif
