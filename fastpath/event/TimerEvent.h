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

#ifndef TIMER_H
#define TIMER_H

#include <chrono>

#include "fastpath/event/Event.h"
#include "fastpath/status.h"

namespace fp {

    class Queue;

	class TimerEvent final : public Event {
        friend class EventManager;

    private:
#if defined HAVE_KQUEUE
        static std::atomic<int> s_identifier;
#elif defined HAVE_EPOLL
#endif
		std::chrono::microseconds m_timeout;

        std::function<void(TimerEvent *)> m_callback;
        const int m_identifier;

        void dispatch(TimerEvent *event) noexcept;

    public:
		TimerEvent(Queue *queue, const std::chrono::microseconds &timeout, const std::function<void(TimerEvent *)> &callback);
		TimerEvent(TimerEvent &&other) noexcept;

        void reset() noexcept;
		void setTimeout(const std::chrono::microseconds &timeout) noexcept;

        const int identifier() const noexcept { return m_identifier; }
        const std::chrono::microseconds &timeout() const noexcept { return m_timeout; }

		const bool isEqual(const Event &other) const noexcept override;
        const bool __notify(const EventType &eventType) noexcept override;
        void __destroy() noexcept override;
    };
}

#endif
