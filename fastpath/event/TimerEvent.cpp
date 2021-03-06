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

#include "TimerEvent.h"
#include "Queue.h"

#include <cassert>

#if defined HAVE_EPOLL
#   include <sys/timerfd.h>
#endif

namespace fp {

#if defined HAVE_KQUEUE
    TimerEvent::TimerEvent(Queue *queue, const std::chrono::microseconds &timeout, const std::function<void(TimerEvent *)> &callback)
            : Event(queue), m_timeout(timeout), m_callback(callback), m_identifier(++TimerEvent::s_identifier) {
    }
#elif defined HAVE_EPOLL
    TimerEvent::TimerEvent(Queue *queue, const std::chrono::microseconds &timeout, const std::function<void(TimerEvent *)> &callback)
            : Event(queue), m_timeout(timeout), m_callback(callback), m_identifier(timerfd_create(CLOCK_MONOTONIC, TFD_NONBLOCK)) {

            if (m_identifier == -1) {
                throw fp::exception("Failed to timerfd_create: "); // << strerror(errno));
            }

            struct timespec t;
            t.tv_sec = std::chrono::duration_cast<std::chrono::seconds>(timeout).count();
            t.tv_nsec = static_cast<decltype(t.tv_nsec)>(std::chrono::duration_cast<std::chrono::nanoseconds>(timeout).count() - (t.tv_sec * 1000000000));

            struct itimerspec interval;
            interval.it_interval = t;
            interval.it_value = t;

            if (timerfd_settime(m_identifier, 0, &interval, NULL) == -1) {
                throw fp::exception("Failed to create timer: "); // << strerror(errno));
            }
    }
#endif

    TimerEvent::TimerEvent(TimerEvent &&other) noexcept : Event(std::move(other)), m_timeout(std::move(other.m_timeout)), m_callback(std::move(other.m_callback)), m_identifier(other.m_identifier) {
    }

    void TimerEvent::dispatch(TimerEvent *event) noexcept {
        this->__popDispatch();
        if (tf::likely(!m_pendingRemoval)) {
            m_callback(event);
        }
    }

    void TimerEvent::reset() noexcept {
#if defined HAVE_KQUEUE
        m_queue->updateEvent(this);
#elif defined HAVE_EPOLL
        struct timespec t;
        t.tv_sec = std::chrono::duration_cast<std::chrono::seconds>(m_timeout).count();
        t.tv_nsec = static_cast<decltype(t.tv_nsec)>(std::chrono::duration_cast<std::chrono::nanoseconds>(m_timeout).count() - (t.tv_sec * 1000000000));

        struct itimerspec interval;
        interval.it_interval = t;
        interval.it_value = t;

        if (timerfd_settime(m_identifier, 0, &interval, NULL) == -1) {
            throw fp::exception("Failed to create timer: ");// << strerror(errno));
        }
#endif
    }

    void TimerEvent::setTimeout(const std::chrono::microseconds &timeout) noexcept {
        m_timeout = timeout;
        this->reset();
    }

    const bool TimerEvent::isEqual(const Event &other) const noexcept {
        if (typeid(other) == typeid(TimerEvent)) {
            const TimerEvent &f = static_cast<const TimerEvent &>(other);
            return m_timeout == f.m_timeout;
        }
        return false;
    }

    const bool TimerEvent::__notify(const EventType &eventType) noexcept {
        assert(m_queue != nullptr);
        this->__pushDispatch();
#if defined HAVE_EPOLL
        uint64_t v;
        ::read(m_identifier, &v, sizeof(uint64_t));
#endif
        return m_queue->__enqueue(QueueElement(this, std::bind(&TimerEvent::dispatch, this, this)));
    }

    void TimerEvent::__destroy() noexcept {
        m_queue->unregisterEvent(this);
    }

#if defined HAVE_KQUEUE
    std::atomic<int> TimerEvent::s_identifier = ATOMIC_VAR_INIT(0);
#endif
}
