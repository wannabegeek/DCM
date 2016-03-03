//
//  TFSpinLock.h
//  CeloxicaTest
//
//  Created by Tom Fewster on 25/04/2013.
//

#ifndef tfspinlock_h
#define tfspinlock_h

#include <iostream>
#include <atomic>

namespace tf {

	class spinlock {
		typedef enum {
			spinlock_locked, spinlock_unlocked
		} spinlock_state;
		std::atomic<spinlock_state> m_state = ATOMIC_VAR_INIT(spinlock_unlocked);

	public:
        spinlock() : m_state(spinlock_unlocked) {
		}

		inline void lock() {
			while (m_state.exchange(spinlock_locked, std::memory_order_acquire) == spinlock_locked);
		}

		inline void unlock() {
			m_state.store(spinlock_unlocked, std::memory_order_release);
		}
	};

	class spinlock_auto {
		spinlock &m_lock;
	public:
		explicit spinlock_auto(spinlock &lock) : m_lock(lock) {
			m_lock.lock();
		}

		~spinlock_auto() {
			m_lock.unlock();
		}
	};
}

#endif
