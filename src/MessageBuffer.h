//
// Created by Tom Fewster on 23/02/2016.
//

#ifndef TFDCF_MESSAGEBUFFER2_H
#define TFDCF_MESSAGEBUFFER2_H

#include <assert.h>
#include <iomanip>
#include <cstring>
#include "MutableByteStorage.h"

namespace DCF {
    struct MsgAddressing {
        using addressing_start = uint8_t;
        using flags = uint8_t;
        using reserved = uint32_t;
        using subject_length = uint16_t;

        static constexpr const size_t size() { return sizeof(addressing_start)
                                            + sizeof(flags)
                                            + sizeof(reserved)
                                            + sizeof(subject_length);
        }
    };
//  char *subject;

    struct MsgHeader {
        using header_start = uint8_t;
        using body_length = uint64_t;     // len:8 off:0   Message Length
        using field_count = uint32_t;    // len:4 off:11  Number of fields in main body
        using mapping_count = uint32_t;  // len:4 off:15  Number of fields in main body

        static constexpr const size_t size() { return sizeof(header_start)
                                            + sizeof(body_length)
                                            + sizeof(mapping_count);
        }
    };

    struct MsgField {
        using identifier = uint16_t ;
        using type = int8_t;
        using data_length = uint32_t;

        static constexpr const size_t size() { return sizeof(identifier)
                                            + sizeof(type)
                                            + sizeof(data_length);
        }
    };
//  void *data;

    struct alignas(8) MsgMappings {
        uint16_t identifier;
        uint16_t name_length;
    };
//  char *name;

    class MessageBuffer {
    private:
        size_t m_startIndex = 0;

        MutableByteStorage m_storage;
        const size_t visible_length() const { return m_storage.length() > m_startIndex ? m_storage.length() - m_startIndex : 0; }

    public:

        explicit MessageBuffer(const size_t initialAllocation) noexcept : m_startIndex(0), m_storage(initialAllocation) {
        }

        explicit MessageBuffer(byte *buffer, const size_t initialAllocation) noexcept : m_startIndex(0), m_storage(buffer, initialAllocation) {}

        MessageBuffer(MessageBuffer &&orig) : m_startIndex(orig.m_startIndex), m_storage(std::move(orig.m_storage)) {
            orig.m_startIndex = 0;
        }

        MessageBuffer(const MessageBuffer &) = delete;
        const MessageBuffer &operator=(const MessageBuffer &) = delete;

        virtual ~MessageBuffer() noexcept {}

        byte *mutableBytes() const noexcept {
            return &m_storage.mutableBytes()[m_startIndex];
        }

        void increaseLengthBy(const size_t length) {
            allocate(length);
        }

        const ByteStorage byteStorage() const noexcept {
            const byte *bytes = nullptr;
            m_storage.bytes(&bytes);
            return ByteStorage(&bytes[m_startIndex], visible_length(), true);
        }

        inline byte *allocate(const size_t length) {
            // expand the length of our buffer if required
            const size_t previous_length = m_storage.length();
            if (m_storage.length() + length > m_storage.capacity()) {
                byte *bytes = m_storage.mutableBytes();
                memmove(bytes, &bytes[m_startIndex], m_storage.length() - m_startIndex);
                m_startIndex = 0;
                m_storage.truncate(m_storage.length() - m_startIndex);
            }

            m_storage.increaseLengthBy(length);
            return &m_storage.mutableBytes()[previous_length];
        }

        void append(const byte *data, const size_t length) {
            m_storage.append(data, length);
        }

        inline void erase_back(const size_t length) {
            assert(length <= visible_length());
            m_storage.truncate(m_storage.length() - length);
        }

        inline void erase_front(const size_t length) {
            assert(length <= visible_length());
            if (length + m_startIndex >= m_storage.length()) {
                m_storage.clear();
            } else {
                if (m_storage.length() == 0) {
                    m_startIndex = 0;
                } else {
                    m_startIndex += length;
                }
            }
        }

        inline void append(const MessageBuffer &src, const size_t length = std::numeric_limits<size_t>::max()) noexcept {
            append(src.mutableBytes(), std::min(length, src.length()));
        }

        inline const size_t bytes(const byte **data) const noexcept {
            const byte *bytes = nullptr;
            const size_t len = m_storage.bytes(&bytes);
            *data = &bytes[m_startIndex];
            return len - m_startIndex;
        }

        inline void clear() noexcept {
            m_storage.clear();
            m_startIndex = 0;
        }

        inline const size_t length() const noexcept {
            return visible_length();
        }

        const byte operator[](const size_t index) const {
            const byte *bytes = nullptr;
            m_storage.bytes(&bytes);
            return (&bytes[m_startIndex])[index];
        }

        friend std::ostream &operator<<(std::ostream &out, const MessageBuffer &msg) {
            const byte *bytes = nullptr;
            const size_t length = msg.bytes(&bytes);
            out << "[start_index: " << msg.m_startIndex << " length: " << length << "]: " << std::endl;
            const byte *output = nullptr;

            const size_t default_block = 8;
            size_t inc = 0;

            for (size_t i = 0; i < length; i += default_block) {
                inc = std::min(default_block, length - i);

                output = &bytes[i];

                out << std::setfill('0') << std::setw(5) << i << "   ";

                for (size_t j = 0; j < default_block; j++) {
                    if (j < inc) {
                        out << std::setfill('0') << std::setw(2) << std::hex << static_cast<const int>(output[j]) << " ";
                    } else {
                        out << "   ";
                    }
                }
                out << std::dec << "        ";

                for (size_t j = 0; j < default_block; j++) {
                    if (j < inc) {
                        if (output[j] < 32 || output[j] > 127) {
                            out << '.' << " ";
                        } else {
                            out << static_cast<const char>(output[j]) << " ";
                        }
                    } else {
                        out << "  ";
                    }
                }

                out << std::endl;
            }
            return out << std::endl;
        }
    };
}

#endif //TFDCF_MESSAGEBUFFER2_H
