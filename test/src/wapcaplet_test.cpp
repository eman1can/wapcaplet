#include "wapcaplet/wapcaplet.h"

#include <gtest/gtest.h>

const uint16_t CHS_Jam[] = {0x4A, 0x61, 0x6D};
const uint16_t CHS_A[] = {0x41};
const uint16_t CHS_B[] = {0x42};
const uint16_t CHS_Badger[] = {0x42, 0x61, 0x64, 0x67, 0x65, 0x72};
const uint16_t CHS_one[] = {0x6F, 0x6E, 0x65};
const uint16_t CHS_ONE[] = {0x4F, 0x4E, 0x45};
const uint16_t CHS_two[] = {0x74, 0x77, 0x6F};
const uint16_t CHS_three[] = {0x74, 0x68, 0x72, 0x65, 0x65};
const uint16_t CHS_yay[] = {0x79, 0x61, 0x79};
const uint16_t CHS_YAY[] = {0x59, 0x41, 0x59};
const uint16_t CHS_hre[] = {0x68, 0x72, 0x65};

TEST(Basic, InternThree) {
    lwc::string* intern_three = lwc::intern(CHS_three, 5);
    lwc::string* intern_three2 = lwc::intern(CHS_three, 5);
    EXPECT_EQ(intern_three, intern_three2);
    lwc::cleanup();
}

TEST(Basic, InvalidInternAborts) {
    EXPECT_THROW(lwc::intern(nullptr, 0), std::out_of_range);
}

TEST(Basic, InvalidInternSubStringAborts) {
    EXPECT_THROW(lwc::intern_substring(nullptr, 0, 0), std::out_of_range);
}

TEST(Basic, ToLowerAborts) {
    EXPECT_THROW(lwc::intern_substring(nullptr, 0, 0), std::out_of_range);
}

TEST(Basic, InternStringOk) {
    lwc::string* intern_string = lwc::intern(CHS_A, 1);
    EXPECT_EQ(intern_string->len, 1);
}

TEST(Basic, InternStringTwiceOk) {
    lwc::string* intern_string1 = lwc::intern(CHS_A, 1);
    EXPECT_EQ(intern_string1->len, 1);
    lwc::string* intern_string2 = lwc::intern(CHS_B, 1);
    EXPECT_EQ(intern_string2->len, 1);
}

TEST(Basic, InternStringTwiceSameOk) {
    lwc::string* intern_string1 = lwc::intern(CHS_A, 1);
    EXPECT_EQ(intern_string1->len, 1);
    lwc::string* intern_string2 = lwc::intern(CHS_A, 1);
    EXPECT_EQ(intern_string2->len, 1);
}

class WithFilledContext : public testing::Test {
public:
    lwc::string* intern_one {nullptr};
    lwc::string* intern_two {nullptr};
    lwc::string* intern_three {nullptr};
    lwc::string* intern_YAY {nullptr};

    void SetUp() override {
        intern_one = lwc::intern(CHS_one, 3);
        intern_two = lwc::intern(CHS_two, 3);
        intern_three = lwc::intern(CHS_three, 5);
        intern_YAY = lwc::intern(CHS_YAY, 3);

        EXPECT_FALSE(strcmp(intern_one, intern_two)) << "Interned strings should be distinct";
        EXPECT_FALSE(strcmp(intern_one, intern_three)) << "Interned strings should be distinct";
        EXPECT_FALSE(strcmp(intern_two, intern_three)) << "Interned strings should be distinct";
    }

    void TearDown() override {
        if (intern_one != nullptr) {
            lwc::unref(intern_one);
        }
        lwc::unref(intern_two);
        lwc::unref(intern_three);
        lwc::unref(intern_YAY);
        lwc::cleanup();
    }
};

TEST_F(WithFilledContext, InternWorks) {
    lwc::string* new_one = lwc::intern(CHS_one, 3);
    EXPECT_NE(new_one, nullptr) << "Interned substring should not be null";
    EXPECT_TRUE(strcmp(new_one, intern_one)) << "Interned string should match original";
}

TEST_F(WithFilledContext, InternSubstring) {
    lwc::string* new_hre = lwc::intern(CHS_hre, 3);
    lwc::string* sub_hre = lwc::intern_substring(intern_three, 1, 3);
    EXPECT_NE(new_hre, nullptr) << "Interned substring should not be null";
    EXPECT_NE(sub_hre, nullptr) << "Interned substring should not be null";
    EXPECT_TRUE(strcmp(new_hre, sub_hre)) << "Interned substring should match original";
}

TEST_F(WithFilledContext, InternSubstringBadOffsetStart) {
    EXPECT_THROW({
        lwc::intern_substring(intern_three, 100, 1);
    }, std::out_of_range) << "Interning substring with bad offset should throw";
}

TEST_F(WithFilledContext, InternSubstringBadOffsetEnd) {
    EXPECT_THROW({
        lwc::intern_substring(intern_three, 1, 100);
    }, std::out_of_range) << "Interning substring with bad offset should throw";
}

TEST_F(WithFilledContext, RefString) {
    EXPECT_TRUE(strcmp(intern_one, lwc::ref(intern_one))) << "Ref should return the same string";
}

TEST_F(WithFilledContext, RefUnRefString) {
    lwc::ref(intern_one);
    lwc::unref(intern_one);
}

TEST_F(WithFilledContext, IsEqual) {
    EXPECT_FALSE(intern_one == intern_two) << "Interned strings should not be equal";
}

TEST_F(WithFilledContext, CaselessIsEqual) {
    lwc::string* new_ONE = lwc::intern(CHS_ONE, 3);
    EXPECT_NE(new_ONE, nullptr) << "Interned string should not be null";
    EXPECT_FALSE(strcmp(intern_one, new_ONE)) << "Interned strings should not be equal";
    EXPECT_TRUE(stricmp(intern_one, new_ONE)) << "Interned strings should be equal caseless";
}

TEST_F(WithFilledContext, CaselessIsEqual2) {
    lwc::string* new_yay = lwc::intern(CHS_yay, 3);
    EXPECT_NE(new_yay, nullptr) << "Interned string should not be null";
    EXPECT_FALSE(strcmp(new_yay, intern_YAY)) << "YAY and yay should not be equal";
    EXPECT_TRUE(stricmp(new_yay, intern_YAY)) << "YAY and yay should be equal caseless";
}

TEST_F(WithFilledContext, CaselessIsEqual3) {
    EXPECT_FALSE(strcmp(intern_YAY, intern_one)) << "YAY and one should not be equal";
}

TEST_F(WithFilledContext, ExtractData) {
    EXPECT_EQ(memcmp(CHS_one, intern_one->data, intern_one->len), 0) << "Interned string data doesn't match original";
}

TEST_F(WithFilledContext, IsNullTerminated) {
    lwc::string* new_ONE = lwc::intern(CHS_ONE, 3);
    EXPECT_NE(new_ONE, nullptr) << "Interned string should not be null";
    EXPECT_EQ(new_ONE->data[new_ONE->len], '\0') << "Interned string isn't NULL terminated";
}

TEST_F(WithFilledContext, IsSubstringNullTerminated) {
    lwc::string* new_ONE = lwc::intern(CHS_ONE, 3);
    EXPECT_NE(new_ONE, nullptr) << "Interned string should not be null";

    lwc::string* new_O = lwc::intern_substring(new_ONE, 0, 1);
    EXPECT_NE(new_O, nullptr) << "Interned substring should not be null";

    EXPECT_EQ(new_O->data[new_O->len], '\0') << "Interned substring isn't NULL terminated";
}

TEST_F(WithFilledContext, ToLower) {
    lwc::string* new_ONE = lwc::intern(CHS_ONE, 3);
    EXPECT_NE(new_ONE, nullptr) << "Interned string should not be null";
    lwc::string* new_one = lwc::to_lower(new_ONE);
    EXPECT_NE(new_one, nullptr) << "Lowercase string should not be null";
    EXPECT_TRUE(strcmp(new_one, intern_one)) << "Lowercase string should match 'one'";
    EXPECT_TRUE(stricmp(new_one, intern_one)) << "Lowercase string should match 'one' caseless";
}

TEST_F(WithFilledContext, ToLower2) {
    lwc::string* new_ONE = lwc::intern(CHS_ONE, 3);
    EXPECT_NE(new_ONE, nullptr) << "Interned string should not be null";
    lwc::string* new_one = lwc::to_lower(new_ONE);
    EXPECT_NE(new_one, nullptr) << "Lowercase string should not be null";
    EXPECT_TRUE(strcmp(new_one, intern_one)) << "Lowercase string should match 'one'";
    EXPECT_TRUE(stricmp(new_one, intern_one)) << "Lowercase string should match 'one' caseless";
}
