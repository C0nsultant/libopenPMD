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


#include <memory>

class AbstractFilePosition;
class AbstractIOHandler;

/** @brief Layer to mirror structure of data logiclly and in file.
 *
 * Hierarchy of objects (datasets, groups, attributes, ...) in openPMD is
 * managed in this class.
 * It also indicates the current synchronization state between logical
 * and persistent data: - whether the object has been created in peristent form
 *                      - whether the logical object has been modified compared
 *                        to last persistent state
 */
class Writable
{
    template<
            typename T,
            typename T_key,
            typename T_container
    >
    friend class Container;
    friend class Iteration;
    friend class ADIOS1IOHandlerImpl;
    friend class ParallelADIOS1IOHandlerImpl;
    friend class ADIOS2IOHandlerImpl;
    friend class HDF5IOHandlerImpl;
    friend class ParallelHDF5IOHandlerImpl;
    friend std::string concrete_h5_file_position(Writable*);

public:
    Writable();
    virtual ~Writable();

protected:
    std::shared_ptr< AbstractFilePosition > abstractFilePosition;
    Writable* parent;
    std::shared_ptr< AbstractIOHandler > IOHandler;
    bool dirty;
    bool written;
};
