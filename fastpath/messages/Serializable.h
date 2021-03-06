/***************************************************************************
                          __FILE__
                          -------------------
    copyright            : Copyright (c) 2004-2016 Tom Fewster
    email                : tom@wannabegeek.com
    date                 : 04/03/2016

 ***************************************************************************/

/***************************************************************************
 * This library is free software; you can redistribute it and/or           *
 * modify it under the terms of the GNU Lesser General Public              *
 * License as published by the Free Software Foundation; either            *
 * version 2.1 of the License, or (at your option) any later version.      *
 *                                                                         *
 * This library is distributed in the hope that it will be useful,         *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of          *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU       *
 * Lesser General Public License for more details.                         *
 *                                                                         *
 * You should have received a copy of the GNU Lesser General Public        *
 * License along with this library; if not, write to the Free Software     *
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA *
 ***************************************************************************/

#ifndef FASTPATH_SERIALIZABLE_H
#define FASTPATH_SERIALIZABLE_H

#include "fastpath/MessageBuffer.h"
#include "fastpath/types.h"

namespace fp {

    template<typename T> T readScalar(const byte *p) {
        return endianScalar(*reinterpret_cast<const T *>(p));
    }

    template<typename T> byte *writeScalar(byte *p, T t) {
        *reinterpret_cast<T *>(p) = endianScalar(t);
        return p + sizeof(T);
    }

    class Serializable {
    public:
        virtual ~Serializable() noexcept = default;

        virtual const size_t encode(MessageBuffer::MutableByteStorageType &buffer) const noexcept = 0;
        virtual const bool decode(const MessageBuffer::ByteStorageType &buffer) = 0;
    };

}

#endif //FASTPATH_SERIALIZABLE_H
