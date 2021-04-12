#include <piejam/audio/interleaved_vector.h>

#include <gtest/gtest.h>

#include <algorithm>
#include <array>

namespace piejam::audio::test
{

TEST(interleaved_vector, static_create_and_iterate)
{
    std::vector<float> vec{1.f, 2.f, 3.f, 4.f};
    interleaved_vector<float, 2> ivec(std::move(vec));

    auto it = ivec.begin();
    ASSERT_NE(ivec.end(), it);
    EXPECT_TRUE(std::ranges::equal(std::array{1.f, 2.f}, *it));

    ++it;
    ASSERT_NE(ivec.end(), it);
    EXPECT_TRUE(std::ranges::equal(std::array{3.f, 4.f}, *it));

    ++it;
    EXPECT_EQ(ivec.end(), it);
}

} // namespace piejam::audio::test
