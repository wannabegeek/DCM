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

#include <gtest/gtest.h>
#include "fastpath/messages/MutableMessage.h"
#include "fastpath/messages/DateTimeField.h"
#include "fastpath/messages/ScalarField.h"
#include "fastpath/messages/SmallDataField.h"
#include "fastpath/messages/LargeDataField.h"
#include "fastpath/messages/MessageField.h"

TEST(Field, CreateString_Large) {

    // set the string as a char * &
    const char *b = "fgsg";
    std::allocator<byte> alloc;
    fp::LargeDataField<decltype(alloc)> e3("0", b, alloc);
    ASSERT_EQ(e3.type(), fp::storage_type::string);
    ASSERT_STREQ(b, e3.get<const char *>());

    // set the string as a char * & retrive it as a char *
    const char *b4 = "test";
    fp::LargeDataField<decltype(alloc)> e4("0", b4, alloc);
    ASSERT_EQ(e4.type(), fp::storage_type::string);
    const char *result4 = nullptr;
    ASSERT_EQ(5u, e4.get(&result4));
    ASSERT_STREQ(b4, result4);
}

TEST(Field, CreateString_Small) {

    // set the string as a char * &
    const char *b = "fgsg";
    fp::SmallDataField e3("0", b);
    ASSERT_EQ(e3.type(), fp::storage_type::string);
    ASSERT_STREQ(b, e3.get<const char *>());

    // set the string as a char * & retrive it as a char *
    const char *b4 = "test";
    fp::SmallDataField e4("0", b4);
    ASSERT_EQ(e4.type(), fp::storage_type::string);
    const char *result4 = nullptr;
    ASSERT_EQ(5u, e4.get(&result4));
    ASSERT_STREQ(b4, result4);
}

TEST(Field, CreateInt32) {
    int32_t t = 42;
    fp::ScalarField e("0", t);
    ASSERT_EQ(e.type(), fp::storage_type::int32);
    ASSERT_EQ(t, e.get<int32_t>());
}

TEST(Field, CreateInt64) {
    int64_t t = 42;
    fp::ScalarField e("0", t);
    ASSERT_EQ(e.type(), fp::storage_type::int64);
    ASSERT_EQ(t, e.get<int64_t>());
}

TEST(Field, CreateFloat32) {
    float32_t t = 42.999;
    fp::ScalarField e("0", t);
    ASSERT_EQ(e.type(), fp::storage_type::float32);
    ASSERT_FLOAT_EQ(t, e.get<float32_t>());
}

TEST(Field, CreateFloat64) {
    float64_t t = 42.999;
    fp::ScalarField e("0", t);
    ASSERT_EQ(e.type(), fp::storage_type::float64);
    ASSERT_FLOAT_EQ(t, e.get<float64_t>());
}

TEST(Field, CreateDateTime) {
    std::chrono::time_point<std::chrono::system_clock> time = std::chrono::system_clock::now();
    fp::DateTimeField e("time", time);
    ASSERT_EQ(e.type(), fp::storage_type::date_time);

    std::chrono::time_point<std::chrono::system_clock> time_out;
    e.get(time_out);
    ASSERT_EQ(std::chrono::duration_cast<std::chrono::microseconds>(time.time_since_epoch()).count(),
                      std::chrono::duration_cast<std::chrono::microseconds>(time_out.time_since_epoch()).count());
}


TEST(Field, CreateData_Large) {
    const char *temp = "Hello world";

    std::allocator<byte> alloc;
    fp::LargeDataField<decltype(alloc)> e("0", reinterpret_cast<const byte *>(temp), strlen(temp), alloc);
    ASSERT_EQ(e.type(), fp::storage_type::data);

    const byte *result = nullptr;
    ASSERT_EQ(strlen(temp), e.get(&result));
    ASSERT_NE(reinterpret_cast<const char *>(result), temp);  // Pointers can't be the same, data should have been copied
    ASSERT_EQ(0, strncmp(reinterpret_cast<const char *>(result), temp, strlen(temp)));
}

TEST(Field, CreateData_Small) {
    const char *temp = "Hello world";

    fp::SmallDataField e("0", reinterpret_cast<const byte *>(temp), strlen(temp));
    ASSERT_EQ(e.type(), fp::storage_type::data);

    const byte *result = nullptr;
    ASSERT_EQ(strlen(temp), e.get(&result));
    ASSERT_NE(reinterpret_cast<const char *>(result), temp);  // Pointers can't be the same, data should have been copied
    ASSERT_EQ(0, strncmp(reinterpret_cast<const char *>(result), temp, strlen(temp)));
}

TEST(Field, SerializeString_Large) {
    const char *temp = "Hello world";
    fp::LargeDataField<std::allocator<byte>> in("0", reinterpret_cast<const byte *>(temp), strlen(temp));

    fp::MessageBuffer::MutableByteStorageType buffer(256);
    const size_t len_in = in.encode(buffer);
    EXPECT_EQ(len_in, buffer.length());

    fp::LargeDataField<std::allocator<byte>> out(buffer);
//    EXPECT_TRUE(out.decode(b));
    ASSERT_EQ(len_in, buffer.bytesRead());

    EXPECT_EQ(in, out);
}

TEST(Field, SerializeString_Small) {
    const char *temp = "Hello world";
    fp::SmallDataField in("0", reinterpret_cast<const byte *>(temp), strlen(temp));

    fp::MessageBuffer::MutableByteStorageType buffer(256);
    const size_t len_in = in.encode(buffer);
    EXPECT_EQ(len_in, buffer.length());

    fp::SmallDataField out(buffer);
//    EXPECT_TRUE(out.decode(b));
    ASSERT_EQ(len_in, buffer.bytesRead());

    EXPECT_EQ(in, out);
}
TEST(Field, SerializeScalar) {
    const uint32_t temp = 1234567890u;
    fp::ScalarField in("0", temp);

    fp::MessageBuffer::MutableByteStorageType buffer(256);
    const size_t len_in = in.encode(buffer);
    EXPECT_EQ(len_in, buffer.length());

    fp::ScalarField out(buffer);
//    EXPECT_TRUE(out.decode(b));
    EXPECT_EQ(len_in, buffer.bytesRead());

    EXPECT_EQ(in, out);
}

TEST(Field, SerializeDateTime) {
    std::chrono::time_point<std::chrono::system_clock> time = std::chrono::system_clock::now();
    fp::DateTimeField in("0", time);

    fp::MessageBuffer::MutableByteStorageType buffer(256);
    const size_t len_in = in.encode(buffer);
    EXPECT_EQ(len_in, buffer.length());

    fp::DateTimeField out(buffer);
//    EXPECT_TRUE(out.decode(b));
    EXPECT_EQ(len_in, buffer.bytesRead());

    EXPECT_EQ(in, out);
}

TEST(Field, SerializeSubMessage) {

    LOG_LEVEL(tf::logger::info);

    fp::MutableMessage msg;
    float32_t t = 22.0;
    EXPECT_TRUE(msg.addScalarField("TEST1", t));
    EXPECT_TRUE(msg.addScalarField("TEST2", true));
    EXPECT_TRUE(msg.addDataField("Name1", "Tom"));
    EXPECT_TRUE(msg.addDataField("Name2", "Zac"));

    DEBUG_LOG("Passing in " << msg);

    fp::MessageField in("msg", &msg);

    fp::MessageBuffer::MutableByteStorageType buffer(256);
    const size_t len_in = in.encode(buffer);
    EXPECT_EQ(len_in, buffer.length());

    DEBUG_LOG("Buffer is: " << buffer);
    fp::MessageField out(buffer);

//    EXPECT_EQ(len_in, b.bytesRead());
//
    DEBUG_LOG("IN:  " << in);
    DEBUG_LOG("OUT: " << out);
    EXPECT_EQ(in, out);
}

TEST(Field, PrintFloat64) {
    float64_t t = 123.45678901234;
    fp::ScalarField e("field", t);

    std::ostringstream os;
    os << e;
    const std::string expected = "123.4567890123";
    EXPECT_EQ(expected, os.str().substr(14, expected.length()));
}

TEST(Field, PrintFloat32) {
    float32_t t = 123.45678;
    fp::ScalarField e("field", t);

    std::ostringstream os;
    os << e;
    const std::string expected = "123.4567";
    EXPECT_EQ(std::string(expected), os.str().substr(14, expected.length()));
}

TEST(Field, PrintInt64) {
    float64_t t = 100000000000;
    fp::ScalarField e("field", t);

    std::ostringstream os;
    os << e;
    const std::string expected = "100000000000.000000000";
    EXPECT_EQ(expected, os.str().substr(14, expected.length()));
}
