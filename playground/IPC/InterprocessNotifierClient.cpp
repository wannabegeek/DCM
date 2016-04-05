//
// Created by Tom Fewster on 03/04/2016.
//

#include <transport/UnixSocketClient.h>
#include <utils/logger.h>
#include "InterprocessNotifierClient.h"

namespace DCF {
    InterprocessNotifierClient::InterprocessNotifierClient() : InterprocessNotifier(std::make_unique<UnixSocketClient>("test_unix")) {
        if (m_socket->connect(DCF::SocketOptionsNone)) {

            int p[] = {outbound_notification.read_handle(), inbound_notification.signal_handle()};
            this->send_fds(p, 2);
//            ::close(outbound_notification.read_handle());
//            ::close(inbound_notification.signal_handle());
        } else {
            ERROR_LOG("Failed to connect");
        }
    }

    bool InterprocessNotifierClient::notify() noexcept {
        return outbound_notification.notify();
    }
}
