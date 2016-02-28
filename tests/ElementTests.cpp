//
// Created by Tom Fewster on 15/02/2016.
//

#include <gtest/gtest.h>
#include <Message.h>

TEST(Elements, CreateString) {

    // set the string as a char * &
    const char *b = "fgsg";
    DCF::DataField e3;
    e3.set(0, b);
    ASSERT_EQ(e3.type(), DCF::StorageType::string);
    ASSERT_STREQ(b, e3.get<const char *>());

    // set the string as a char * & retrive it as a char *
    const char *b4 = "test";
    DCF::DataField e4;
    e4.set(0, b4);
    ASSERT_EQ(e4.type(), DCF::StorageType::string);
    const byte *result4 = nullptr;
    ASSERT_EQ(5, e4.get(&result4));
    ASSERT_STREQ(b4, reinterpret_cast<const char *>(result4));
}

TEST(Elements, CreateInt32) {
    DCF::ScalarField e;
    int32_t t = 42;
    e.set(0, t);
    ASSERT_EQ(e.type(), DCF::StorageType::int32);
    ASSERT_EQ(t, e.get<int32_t>());
}

TEST(Elements, CreateInt64) {
    DCF::ScalarField e;
    int64_t t = 42;
    e.set(0, t);
    ASSERT_EQ(e.type(), DCF::StorageType::int64);
    ASSERT_EQ(t, e.get<int64_t>());
}

TEST(Elements, CreateFloat32) {
    DCF::ScalarField e;
    float32_t t = 42.999;
    e.set(0, t);
    ASSERT_EQ(e.type(), DCF::StorageType::float32);
    ASSERT_FLOAT_EQ(t, e.get<float32_t>());
}

TEST(Elements, CreateFloat64) {
    DCF::ScalarField e;
    float64_t t = 42.999;
    e.set(0, t);
    ASSERT_EQ(e.type(), DCF::StorageType::float64);
    ASSERT_FLOAT_EQ(t, e.get<float64_t>());
}


TEST(Elements, CreateData) {
    const char *temp = "Hello world";

    DCF::DataField e;
    e.set(0, reinterpret_cast<const byte *>(temp), strlen(temp));
    ASSERT_EQ(e.type(), DCF::StorageType::data);

    const byte *result = nullptr;
    ASSERT_EQ(strlen(temp), e.get(&result));
    ASSERT_NE(reinterpret_cast<const char *>(result), temp);  // Pointers can't be the same, data should have been copied
    ASSERT_EQ(0, strncmp(reinterpret_cast<const char *>(result), temp, strlen(temp)));
}

TEST(Elements, SerializeString) {
    const char *temp = "Hello world";
    DCF::DataField in;
    in.set(0, reinterpret_cast<const byte *>(temp), strlen(temp));

    DCF::MessageBuffer buffer(256);
    const size_t len_in = in.encode(buffer);
    EXPECT_EQ(len_in, buffer.length());

    DCF::DataField out;
    size_t len_out = 0;
    EXPECT_TRUE(out.decode(buffer.byteStorage(), len_out));
    EXPECT_EQ(len_in, len_out);

    EXPECT_EQ(in, out);
}

TEST(Elements, SerializeScalar) {
    const uint32_t temp = 1234567890u;
    DCF::ScalarField in;
    in.set(0, temp);

    DCF::MessageBuffer buffer(256);
    const size_t len_in = in.encode(buffer);
    EXPECT_EQ(len_in, buffer.length());

    DCF::ScalarField out;
    size_t len_out = 0;
    EXPECT_TRUE(out.decode(buffer.byteStorage(), len_out));
    EXPECT_EQ(len_in, len_out);

    EXPECT_EQ(in, out);
}