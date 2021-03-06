#include <iostream>

#include "Datatype.hpp"

std::ostream&
operator<<(std::ostream& os, Datatype d)
{
    using DT = Datatype;
    switch( d )
    {
        case DT::CHAR:
            os << "CHAR";
            break;
        case DT::UCHAR:
            os << "UCHAR";
            break;
        case DT::INT16:
            os << "INT16";
            break;
        case DT::INT32:
            os << "INT32";
            break;
        case DT::INT64:
            os << "INT64";
            break;
        case DT::UINT16:
            os << "UINT16";
            break;
        case DT::UINT32:
            os << "UINT32";
            break;
        case DT::UINT64:
            os << "UINT64";
            break;
        case DT::FLOAT:
            os << "FLOAT";
            break;
        case DT::DOUBLE:
            os << "DOUBLE";
            break;
        case DT::LONG_DOUBLE:
            os << "LONG_DOUBLE";
            break;
        case DT::STRING:
            os << "STRING";
            break;
        case DT::VEC_CHAR:
            os << "VEC_CHAR";
            break;
        case DT::VEC_INT16:
            os << "VEC_INT16";
            break;
        case DT::VEC_INT32:
            os << "VEC_INT32";
            break;
        case DT::VEC_INT64:
            os << "VEC_INT64";
            break;
        case DT::VEC_UCHAR:
            os << "VEC_UCHAR";
            break;
        case DT::VEC_UINT16:
            os << "VEC_UINT16";
            break;
        case DT::VEC_UINT32:
            os << "VEC_UINT32";
            break;
        case DT::VEC_UINT64:
            os << "VEC_UINT64";
            break;
        case DT::VEC_FLOAT:
            os << "VEC_FLOAT";
            break;
        case DT::VEC_DOUBLE:
            os << "VEC_DOUBLE";
            break;
        case DT::VEC_LONG_DOUBLE:
            os << "VEC_LONG_DOUBLE";
            break;
        case DT::VEC_STRING:
            os << "VEC_STRING";
            break;
        case DT::ARR_DBL_7:
            os << "ARR_DBL_7";
            break;
        case DT::BOOL:
            os << "BOOL";
            break;
        case DT::DATATYPE:
            os << "DATATYPE";
            break;
        case DT::UNDEFINED:
            os << "UNDEFINED";
            break;
    }

    return os;
}
