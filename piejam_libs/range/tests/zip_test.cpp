#include <piejam/range/zip.h>

#include <gtest/gtest.h>

#include <array>
#include <vector>

namespace piejam::range::test
{

TEST(zip, basic)
{
    std::array cs{'a', 'b', 'c'};
    std::array is{2, 3, 5};

    std::vector<std::pair<char, int>> result;

    for (auto const& [c, i] : zip(cs, is))
    {
        result.emplace_back(c, i);
    }

    ASSERT_EQ(3u, result.size());

    EXPECT_EQ(result[0].first, 'a');
    EXPECT_EQ(result[1].first, 'b');
    EXPECT_EQ(result[2].first, 'c');

    EXPECT_EQ(result[0].second, 2);
    EXPECT_EQ(result[1].second, 3);
    EXPECT_EQ(result[2].second, 5);
}

} // namespace piejam::range::test
