/* Copyright 2018 Axel Huebl
 *
 * This file is part of openPMD-api.
 *
 * openPMD-api is free software: you can redistribute it and/or modify
 * it under the terms of of either the GNU General Public License or
 * the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * openPMD-api is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License and the GNU Lesser General Public License
 * for more details.
 *
 * You should have received a copy of the GNU General Public License
 * and the GNU Lesser General Public License along with openPMD-api.
 * If not, see <http://www.gnu.org/licenses/>.
 */
#pragma once

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/numpy.h>

#include "openPMD/Datatype.hpp"

#include <exception>


namespace openPMD
{
    inline Datatype
    dtype_from_numpy( pybind11::dtype const dt )
    {
        // ref: https://docs.scipy.org/doc/numpy/user/basics.types.html
        // ref: https://github.com/numpy/numpy/issues/10678#issuecomment-369363551
        if( dt.is(pybind11::dtype("b")) )
            return Datatype::CHAR;
        else if( dt.is(pybind11::dtype("B")) )
            return Datatype::UCHAR;
        else if( dt.is(pybind11::dtype("short")) )
            return Datatype::SHORT;
        else if( dt.is(pybind11::dtype("intc")) )
            return Datatype::INT;
        else if( dt.is(pybind11::dtype("int_")) )
            return Datatype::LONG;
        else if( dt.is(pybind11::dtype("longlong")) )
            return Datatype::LONGLONG;
        else if( dt.is(pybind11::dtype("ushort")) )
            return Datatype::USHORT;
        else if( dt.is(pybind11::dtype("uintc")) )
            return Datatype::UINT;
        else if( dt.is(pybind11::dtype("uint")) )
            return Datatype::ULONG;
        else if( dt.is(pybind11::dtype("ulonglong")) )
            return Datatype::ULONGLONG;
        else if( dt.is(pybind11::dtype("longdouble")) )
            return Datatype::LONG_DOUBLE;
        else if( dt.is(pybind11::dtype("double")) )
            return Datatype::DOUBLE;
        else if( dt.is(pybind11::dtype("single")) )
            return Datatype::FLOAT;
        else if( dt.is(pybind11::dtype("bool")) )
            return Datatype::BOOL;
        else
            throw std::runtime_error("Datatype '...' not known in 'dtype_from_numpy'!"); // _s.format(dt)
    }

    inline pybind11::dtype
    dtype_to_numpy( Datatype const dt )
    {
        using DT = Datatype;
        switch( dt )
        {
            case DT::CHAR:
            case DT::VEC_CHAR:
            case DT::STRING:
            case DT::VEC_STRING:
                return pybind11::dtype("b");
                break;
            case DT::UCHAR:
            case DT::VEC_UCHAR:
                return pybind11::dtype("B");
                break;
            // case DT::SCHAR:
            // case DT::VEC_SCHAR:
            //     pybind11::dtype("b");
            //     break;
            case DT::SHORT:
            case DT::VEC_SHORT:
                return pybind11::dtype("short");
                break;
            case DT::INT:
            case DT::VEC_INT:
                return pybind11::dtype("intc");
                break;
            case DT::LONG:
            case DT::VEC_LONG:
                return pybind11::dtype("int_");
                break;
            case DT::LONGLONG:
            case DT::VEC_LONGLONG:
                return pybind11::dtype("longlong");
                break;
            case DT::USHORT:
            case DT::VEC_USHORT:
                return pybind11::dtype("ushort");
                break;
            case DT::UINT:
            case DT::VEC_UINT:
                return pybind11::dtype("uintc");
                break;
            case DT::ULONG:
            case DT::VEC_ULONG:
                return pybind11::dtype("uint");
                break;
            case DT::ULONGLONG:
            case DT::VEC_ULONGLONG:
                return pybind11::dtype("ulonglong");
                break;
            case DT::FLOAT:
            case DT::VEC_FLOAT:
                return pybind11::dtype("single");
                break;
            case DT::DOUBLE:
            case DT::VEC_DOUBLE:
            case DT::ARR_DBL_7:
                return pybind11::dtype("double");
                break;
            case DT::LONG_DOUBLE:
            case DT::VEC_LONG_DOUBLE:
                return pybind11::dtype("longdouble");
                break;
            case DT::BOOL:
                return pybind11::dtype("bool"); // also "?"
                break;
            case DT::DATATYPE:
            case DT::UNDEFINED:
            default:
                throw std::runtime_error("dtype_to_numpy: Invalid Datatype '{...}'!"); // _s.format(dt)
                break;
        }
    }
} // namespace openPMD
