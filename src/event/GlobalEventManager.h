//
// Created by Tom Fewster on 07/03/2016.
//

#ifndef TFDCF_GLOBALEVENTMANAGER_H
#define TFDCF_GLOBALEVENTMANAGER_H

#include <utils/concurrentqueue.h>
#include "EventManager.h"

namespace DCF {
    class GlobalEventManager final : public EventManager {
    private:
        using PendingTimerType = moodycamel::ConcurrentQueue<TimerEvent *>;
        using PendingIOType = moodycamel::ConcurrentQueue<IOEvent *>;

        ActionNotifier m_actionNotifier;
        PendingTimerType m_pendingTimerRegistrations;
        PendingIOType m_pendingIORegistrations;

        std::vector<TimerEvent *> m_timerHandlers;
        std::vector<IOEvent *> m_ioHandlers;

        void serviceEvent(const EventPollElement &event) override;

        void processPendingRegistrations() override;
        void __foreach_timer(std::function<void(TimerEvent *)> callback) const override;
        void __foreach_ioevent(std::function<void(IOEvent *)> callback) const override;

        const bool haveHandlers() const override;
    public:
        GlobalEventManager();
        GlobalEventManager(GlobalEventManager &&other);

        ~GlobalEventManager();

        void registerHandler(TimerEvent &eventRegistration) override;
        void registerHandler(IOEvent &eventRegistration) override;
        void unregisterHandler(TimerEvent &handler) override;
        void unregisterHandler(IOEvent &handler) override;

        void notify() override;
    };
}

#endif //TFDCF_GLOBALEVENTMANAGER_H
