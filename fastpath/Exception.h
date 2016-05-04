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

#ifndef TFException_h
#define TFException_h

#include <sstream>
#include <stdexcept>

#define ThrowException(exception, msg) { \
		std::ostringstream mess; \
		mess << msg; \
		throw exception(mess.str()); \
}

#define ThrowExceptionWithArgs(exception, msg, ...) { \
		std::ostringstream mess; \
		mess << msg; \
		throw exception(mess.str(), __VA_ARGS__); \
}

namespace fp {
    class exception : public std::logic_error {
    public:
        exception(const std::string &description) : std::logic_error(description) {}

        friend std::ostream &operator<<(std::ostream &os, const exception &exception) {
            os << exception.what();
            return os;
        }
    };
} // namespace

#endif