/* Copyright 2017 Fabian Koller
 *
 * This file is part of libopenPMD.
 *
 * libopenPMD is free software: you can redistribute it and/or modify
 * it under the terms of of either the GNU General Public License or
 * the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * libopenPMD is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License and the GNU Lesser General Public License
 * for more details.
 *
 * You should have received a copy of the GNU General Public License
 * and the GNU Lesser General Public License along with libopenPMD.
 * If not, see <http://www.gnu.org/licenses/>.
 */
#include <iostream>
#include <set>

#include "backend/Attributable.hpp"
#include "auxiliary/StringManip.hpp"

Attributable::Attributable()
        : m_attributes{std::make_shared< A_MAP >()}
{ }

Attributable::Attributable(Attributable const& rhs)
// Deep-copy the entries in the Attribute map since the lifetime of the rhs does not end
        : Writable{rhs},
          m_attributes{std::make_shared< A_MAP >(*rhs.m_attributes)}
{ }

Attributable::Attributable(Attributable&& rhs)
// Take ownership of the Attribute map pointer since the lifetime of the rhs does end
        : Writable{rhs},
          m_attributes{std::move(rhs.m_attributes)}
{ }

Attributable&
Attributable::operator=(Attributable const& a)
{
    if( this != &a )
    {
        Attributable tmp(a);
        std::swap(m_attributes, tmp.m_attributes);
    }
    return *this;
}

Attributable&
Attributable::operator=(Attributable&& a)
{
    m_attributes = std::move(a.m_attributes);
    return *this;
}

Attribute
Attributable::getAttribute(std::string const& key) const
{
    auto it = m_attributes->find(key);
    if( it != m_attributes->cend() )
        return it->second;

    throw no_such_attribute_error(key);
}

bool
Attributable::deleteAttribute(std::string const& key)
{
    if( AccessType::READ_ONLY == IOHandler->accessType )
        throw std::runtime_error("Can not delete an Attribute in a read-only Series.");

    auto it = m_attributes->find(key);
    if( it != m_attributes->end() )
    {
        Parameter< Operation::DELETE_ATT > aDelete;
        aDelete.name = key;
        IOHandler->enqueue(IOTask(this, aDelete));
        IOHandler->flush();
        m_attributes->erase(it);
        return true;
    }
    return false;
}

std::vector< std::string >
Attributable::attributes() const
{
    std::vector< std::string > ret;
    ret.reserve(m_attributes->size());
    for( auto const& entry : *m_attributes )
        ret.emplace_back(entry.first);

    return ret;
}

size_t
Attributable::numAttributes() const
{
    return m_attributes->size();
}

std::string
Attributable::comment() const
{
    return getAttribute("comment").get< std::string >();
}

Attributable&
Attributable::setComment(std::string const& c)
{
    setAttribute("comment", c);
    return *this;
}

void
Attributable::flushAttributes()
{
    if( dirty )
    {
        Parameter< Operation::WRITE_ATT > aWrite;
        for( std::string const & att_name : attributes() )
        {
            aWrite.name = att_name;
            aWrite.resource = getAttribute(att_name).getResource();
            aWrite.dtype = getAttribute(att_name).dtype;
            IOHandler->enqueue(IOTask(this, aWrite));
            IOHandler->flush();
        }

        dirty = false;
    }
}

void
Attributable::readAttributes()
{
    Parameter< Operation::LIST_ATTS > aList;
    IOHandler->enqueue(IOTask(this, aList));
    IOHandler->flush();
    std::vector< std::string > written_attributes = attributes();

    /* std::set_difference requires sorted ranges */
    std::sort(aList.attributes->begin(), aList.attributes->end());
    std::sort(written_attributes.begin(), written_attributes.end());

    std::set< std::string > attributes;
    std::set_difference(aList.attributes->begin(), aList.attributes->end(),
                        written_attributes.begin(), written_attributes.end(),
                        std::inserter(attributes, attributes.begin()));

    using DT = Datatype;
    Parameter< Operation::READ_ATT > aRead;

    for( auto const& att_name : attributes )
    {
        aRead.name = att_name;
        std::string att = strip(att_name, {'\0'});
        IOHandler->enqueue(IOTask(this, aRead));
        try
        {
            IOHandler->flush();
        } catch( unsupported_data_error e )
        {
            std::cerr << "Skipping non-standard attribute "
                      << att << " ("
                      << e.what()
                      << ")\n";
            continue;
        }
        Attribute a(*aRead.resource);
        switch( *aRead.dtype )
        {
            case DT::CHAR:
                setAttribute(att, a.get< char >());
                break;
            case DT::UCHAR:
                setAttribute(att, a.get< unsigned char >());
                break;
            case DT::INT16:
                setAttribute(att, a.get< int16_t >());
                break;
            case DT::INT32:
                setAttribute(att, a.get< int32_t >());
                break;
            case DT::INT64:
                setAttribute(att, a.get< int64_t >());
                break;
            case DT::UINT16:
                setAttribute(att, a.get< uint16_t >());
                break;
            case DT::UINT32:
                setAttribute(att, a.get< uint32_t >());
                break;
            case DT::UINT64:
                setAttribute(att, a.get< uint64_t >());
                break;
            case DT::FLOAT:
                setAttribute(att, a.get< float >());
                break;
            case DT::DOUBLE:
                setAttribute(att, a.get< double >());
                break;
            case DT::LONG_DOUBLE:
                setAttribute(att, a.get< long double >());
                break;
            case DT::STRING:
                setAttribute(att, a.get< std::string >());
                break;
            case DT::VEC_CHAR:
                setAttribute(att, a.get< std::vector< char > >());
                break;
            case DT::VEC_INT16:
                setAttribute(att, a.get< std::vector< int16_t > >());
                break;
            case DT::VEC_INT32:
                setAttribute(att, a.get< std::vector< int32_t > >());
                break;
            case DT::VEC_INT64:
                setAttribute(att, a.get< std::vector< int64_t > >());
                break;
            case DT::VEC_UCHAR:
                setAttribute(att, a.get< std::vector< unsigned char > >());
                break;
            case DT::VEC_UINT16:
                setAttribute(att, a.get< std::vector< uint16_t > >());
                break;
            case DT::VEC_UINT32:
                setAttribute(att, a.get< std::vector< uint32_t > >());
                break;
            case DT::VEC_UINT64:
                setAttribute(att, a.get< std::vector< uint64_t > >());
                break;
            case DT::VEC_FLOAT:
                setAttribute(att, a.get< std::vector< float > >());
                break;
            case DT::VEC_DOUBLE:
                setAttribute(att, a.get< std::vector< double > >());
                break;
            case DT::VEC_LONG_DOUBLE:
                setAttribute(att, a.get< std::vector< long double > >());
                break;
            case DT::VEC_STRING:
                setAttribute(att, a.get< std::vector< std::string > >());
                break;
            case DT::ARR_DBL_7:
                setAttribute(att, a.get< std::array< double, 7 > >());
                break;
            case DT::BOOL:
                setAttribute(att, a.get< bool >());
                break;
            case DT::DATATYPE:
            case DT::UNDEFINED:
                throw std::runtime_error("Invalid Attribute datatype during read");
        }
    }

    IOHandler->flush();
    dirty = false;
}

void warnWrongDtype(std::string const& key,
                    Datatype store,
                    Datatype request)
{
    std::cerr << "Warning: Attribute '" << key
              << "' stored as " << store
              << ", requested as " << request
              << ". Casting unconditionally with possible loss of precision.\n";
}

template
float
Attributable::readFloatingpoint(std::string const& key) const;

template
double
Attributable::readFloatingpoint(std::string const& key) const;

template
long double
Attributable::readFloatingpoint(std::string const& key) const;

template
std::vector< float >
Attributable::readVectorFloatingpoint(std::string const& key) const;

template
std::vector< double >
Attributable::readVectorFloatingpoint(std::string const& key) const;

template
std::vector< long double >
Attributable::readVectorFloatingpoint(std::string const& key) const;