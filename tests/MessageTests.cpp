//
// Created by Tom Fewster on 15/02/2016.
//

#include <gtest/gtest.h>
#include <Message.h>

TEST(Message, SetSubject) {
    DCF::Message msg;
    const char *subject = "TEST.SUBJECT";
    ASSERT_TRUE(msg.setSubject(subject));

    ASSERT_STREQ(subject, msg.subject());

    ASSERT_TRUE(msg.setSubject("UPDATED.SUBJECT"));
    ASSERT_STRNE(subject, msg.subject());
}

TEST(Message, AddStringField) {

    DCF::Message msg;
    msg.addDataField(1234, "TEST");

    std::string v;
    ASSERT_TRUE(msg.getDataField(1234, v));
    ASSERT_STREQ("TEST", v.c_str());

    msg.addDataField("TEST", "AGAIN");
    ASSERT_TRUE(msg.getDataField("TEST", v));
    ASSERT_STREQ("AGAIN", v.c_str());
}

TEST(Message, AddFloatField) {
    DCF::Message msg;
    msg.addScalarField("TEST", static_cast<float32_t>(1.4));
    float32_t t = 0.0;
    ASSERT_TRUE(msg.getScalarField("TEST", t));
    ASSERT_FLOAT_EQ(1.4, t);
}

TEST(Message, AddMixedDuplicateField) {
    DCF::Message msg;
    msg.addDataField("TEST", "AGAIN");

    msg.addScalarField("TEST", static_cast<float32_t>(1.4));
    float32_t t = 0.0;
    std::string v;
    ASSERT_TRUE(msg.getDataField("TEST", v, 0));
    ASSERT_STREQ("AGAIN", v.c_str());
    ASSERT_TRUE(msg.getScalarField("TEST", t, 1));
    ASSERT_FLOAT_EQ(1.4, t);

}

TEST(Message, AddMessageField) {
    DCF::Message msg;
    msg.addScalarField("TEST", static_cast<float32_t>(1.4));

    DCF::MessageType m = std::make_shared<DCF::Message>();
    m->addDataField("TEST2", "TOMTOMTOM");

    msg.addMessageField("MSG_TEST", m);

    std::cout << "Embedded msg: " << msg << std::endl;
}

TEST(Message, RemoveFieldByString) {
    DCF::Message msg;
    float32_t t = 22.0;
    msg.addScalarField("TEST", t);
    ASSERT_TRUE(msg.getScalarField("TEST", t));
    ASSERT_FLOAT_EQ(22, t);

    ASSERT_EQ(1u, msg.size());

    ASSERT_TRUE(msg.removeField("TEST"));
    ASSERT_EQ(0u, msg.size());
}

TEST(Message, Encode) {
    DCF::Message msg;
    msg.setSubject("SOME.TEST.SUBJECT");
    float32_t t = 22.0;
    msg.addScalarField("TEST", t);
    msg.addDataField("Name", "Tom");
    msg.addDataField("Name", "Zac");

    DCF::MessageBuffer buffer(1024);
    const size_t encoded_len = msg.encode(buffer);
    EXPECT_EQ(buffer.length(), encoded_len);

    std::cout << buffer << std::endl;
}

TEST(Message, Decode) {
    DCF::Message in;
    in.setSubject("SOME.TEST.SUBJECT");
    float32_t t = 22.0;
    in.addScalarField("TEST", t);
    in.addScalarField("TEST", true);
    in.addDataField("Name", "Tom");
    in.addDataField("Name", "Zac");

    DCF::MessageBuffer buffer(1024);
    const size_t encoded_len = in.encode(buffer);
    EXPECT_EQ(buffer.length(), encoded_len);

    std::cout << buffer << std::endl;
    std::cout << buffer.byteStorage() << std::endl;
    DCF::Message out;
    size_t decoded_len = 0;
    EXPECT_TRUE(out.decode(buffer.byteStorage(), decoded_len));
    EXPECT_EQ(encoded_len, decoded_len);

    std::cout << "IN:  " << in << std::endl;
    std::cout << "OUT: " << out << std::endl;
    EXPECT_EQ(in, out);
}

TEST(Message, MultiDecode) {
    DCF::Message in1;
    in1.setSubject("SAMPLE.MSG.1");
    float32_t t = 22.0;
    in1.addScalarField("TEST", t);
    in1.addScalarField("TEST", true);
    in1.addDataField("Name", "Tom");
    in1.addDataField("Name", "Zac");

    DCF::Message in2;
    in2.setSubject("SAMPLE.MSG.2");
    t = 26.0;
    in2.addScalarField("TEST", t);
    in2.addScalarField("TEST", false);
    in2.addDataField("Name", "Caroline");
    in2.addDataField("Name", "Heidi");

    DCF::MessageBuffer buffer(1024);
    in1.encode(buffer);
    in2.encode(buffer);

    std::cout << buffer << std::endl;

    std::cout << "Msg 1: " << in1 << std::endl;
    std::cout << "Msg 2: " << in2 << std::endl;

    DCF::Message out;
    size_t offset = 0;
//    while (buffer.length() != 0 && (offset = out.decode(buffer.byteStorage())) != 0) {
//        std::cout << "Decoded: " << out << std::endl;
//        out.clear();
//        buffer.erase_front(offset);
//    }
}


TEST(Message, MultiPartialDecode) {
    DCF::Message in1;
    float32_t t = 22.0;
    in1.addScalarField("TEST", t);
    in1.addScalarField("TEST", true);
    in1.addDataField("Name", "Tom");
    in1.addDataField("Name", "Zac");

    DCF::MessageBuffer buffer(1024);
    char subject[256];
    for (int i = 0; i < 10; i++) {
        sprintf(subject, "SAMPLE.MSG.%i", i);
        in1.setSubject(subject);
        in1.addScalarField("id", i);
        in1.encode(buffer);
    }

    std::cout << buffer << std::endl;

    const byte *bytes = nullptr;
    size_t len = 0;
    DCF::Message out;
    for (int i = 0; i < buffer.length(); i++) {
        len += 10;
        buffer.bytes(&bytes);
        DCF::ByteStorage storage(bytes, std::min(len, buffer.length()));

        size_t offset;
        if (out.decode(storage, offset)) {
            buffer.erase_front(offset);
            std::cout << "Msg decoded: " << out << std::endl;
            out.clear();
        }
    }
}