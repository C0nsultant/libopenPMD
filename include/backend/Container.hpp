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
#pragma once


#include <map>

#include "Attributable.hpp"


/** @brief Map-like container that enforces openPMD requirements and handles IO.
 *
 * @see http://en.cppreference.com/w/cpp/container/map
 *
 * @tparam T            Type of objects stored
 * @tparam T_key        Key type to look elements up by
 * @tparam T_container  Type of container used for internal storage (must supply the same type traits and interface as std::map)
 */
template<
        typename T,
        typename T_key = std::string,
        typename T_container = std::map< T_key, T >
>
class Container : public Attributable
{
    static_assert(std::is_base_of< Writable, T >::value, "Type of container element must be derived from Writable");
    using InternalContainer = T_container;

    friend class Iteration;
    friend class ParticleSpecies;
    friend class Series;

public:
    using key_type = typename InternalContainer::key_type;
    using mapped_type = typename InternalContainer::mapped_type;
    using value_type = typename InternalContainer::value_type;
    using size_type = typename InternalContainer::size_type;
    using difference_type = typename InternalContainer::difference_type;
    using allocator_type = typename InternalContainer::allocator_type;
    using reference = typename InternalContainer::reference;
    using const_reference = typename InternalContainer::const_reference;
    using pointer = typename InternalContainer::pointer;
    using const_pointer = typename InternalContainer::const_pointer;
    using iterator = typename InternalContainer::iterator;
    using const_iterator = typename InternalContainer::const_iterator;

    virtual ~Container() { }

    iterator begin() noexcept { return m_container.begin(); }
    const_iterator begin() const noexcept { return m_container.begin(); }
    const_iterator cbegin() const noexcept { return m_container.cbegin(); }

    iterator end() noexcept { return m_container.end(); }
    const_iterator end() const noexcept { return m_container.end(); }
    const_iterator cend() const noexcept { return m_container.cend(); }

    bool empty() const noexcept { return m_container.empty(); }

    size_type size() const noexcept { return m_container.size(); }

    /** Remove all objects from the container and (if written) from disk.
     *
     * @note    Calling this operation on any container in a Series with <code>AccessType::READ_ONLY</code> will throw an exception.
     * @throws  std::runtime_error
     */
    void clear()
    {
        if( AccessType::READ_ONLY == IOHandler->accessType )
            throw std::runtime_error("Can not clear a container in a read-only Series.");

        clear_unchecked();
    }

    std::pair< iterator, bool > insert(value_type const& value) { return m_container.insert(value); }
    template< class P >
    std::pair< iterator, bool > insert(P&& value) { return m_container.insert(value); }
    iterator insert(const_iterator hint, value_type const& value) { return m_container.insert(hint, value); }
    template< class P >
    iterator insert(const_iterator hint, P&& value) { return m_container.insert(hint, value); }
    template< class InputIt >
    void insert(InputIt first, InputIt last) { m_container.insert(first, last); }
    void insert(std::initializer_list< value_type > ilist) { m_container.insert(ilist); }

    void swap(Container & other) { m_container.swap(other.m_container); }

    mapped_type& at(key_type const& key) { return m_container.at(key); }
    mapped_type const& at(key_type const& key) const { return m_container.at(key); }

    /** Access the value that is mapped to a key equivalent to key, creating it if such key does not exist already.
     *
     * @param   key Key of the element to find (lvalue).
     * @return  Reference to the mapped value of the new element if no element with key key existed. Otherwise a reference to the mapped value of the existing element whose key is equivalent to key.
     */
    virtual mapped_type& operator[](key_type const& key)
    {
        auto it = m_container.find(key);
        if( it != m_container.end() )
            return it->second;
        else
        {
            T t = T();
            t.IOHandler = IOHandler;
            t.parent = this;
            return m_container.insert({key, std::move(t)}).first->second;
        }
    }
    /** Access the value that is mapped to a key equivalent to key, creating it if such key does not exist already.
     *
     * @param   key Key of the element to find (rvalue).
     * @return  Reference to the mapped value of the new element if no element with key key existed. Otherwise a reference to the mapped value of the existing element whose key is equivalent to key.
     */
    virtual mapped_type& operator[](key_type&& key)
    {
        auto it = m_container.find(key);
        if( it != m_container.end() )
            return it->second;
        else
        {
            T t = T();
            t.IOHandler = IOHandler;
            t.parent = this;
            return m_container.insert({std::move(key), std::move(t)}).first->second;
        }
    }

    size_type count(key_type const& key) const { return m_container.count(key); }

    iterator find(key_type const& key) { return m_container.find(key); }
    const_iterator find(key_type const& key) const { return m_container.find(key); }

    /** Remove a single element from the container and (if written) from disk.
     *
     * @note    Calling this operation on any container in a Series with <code>AccessType::READ_ONLY</code> will throw an exception.
     * @throws  std::runtime_error
     * @param   key Key of the element to remove.
     * @return  Number of elements removed (either 0 or 1).
     */
    virtual size_type erase(key_type const& key)
    {
        if( AccessType::READ_ONLY == IOHandler->accessType )
            throw std::runtime_error("Can not erase from a container in a read-only Series.");

        auto res = m_container.find(key);
        if( res != m_container.end() && res->second.written )
        {
            Parameter< Operation::DELETE_PATH > pDelete;
            pDelete.path = ".";
            IOHandler->enqueue(IOTask(&res->second, pDelete));
            IOHandler->flush();
        }
        return m_container.erase(key);
    }

protected:
    InternalContainer m_container;

    void clear_unchecked()
    {
        if( written )
            throw std::runtime_error("Clearing a written container not (yet) implemented.");

        m_container.clear();
    }

    virtual void flush(std::string const& path)
    {
        if( !written )
        {
            Parameter< Operation::CREATE_PATH > pCreate;
            pCreate.path = path;
            IOHandler->enqueue(IOTask(this, pCreate));
            IOHandler->flush();
        }

        flushAttributes();
    }
};
