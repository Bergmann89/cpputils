#include <vector>
#include <gtest/gtest.h>
#include <cpputils/misc/linq.h>

namespace linq_tests
{
    struct test_data
    {
        int value;

        test_data() :
            value(0)
            { }

        test_data(int v) :
            value(v)
            { }
    };

    struct TestData2
    {
        int value;
        std::string name;

        TestData2() :
            value(0)
            { }

        TestData2(int v, const std::string& s) :
            value(v),
            name (s)
            { }
    };

    struct TestDataMany
    {
        std::vector<int> values;

        TestDataMany(const std::vector<int>& v) :
            values(v)
            { }
    };

    struct MoveOnlyData
    {
        int value;

        MoveOnlyData() :
            value(0)
            { LINQ_CTOR(); }

        MoveOnlyData(int v) :
            value(v)
            { LINQ_CTOR(); }

        MoveOnlyData(MoveOnlyData&& o) :
            value(0)
            { std::swap(value, o.value); LINQ_MOVE_CTOR(); }

        MoveOnlyData(const MoveOnlyData&) = delete;

        ~MoveOnlyData()
            { LINQ_DTOR(); }
    };
}

using namespace ::utl;
using namespace ::utl::linq;
using namespace ::linq_tests;

TEST(LinqTest, from_iterator)
{
    std::vector<int> data({ 4, 5, 6, 7, 8 });
    auto range = from_iterator(data.begin(), data.end());
    ASSERT_TRUE (range.next());
    ASSERT_EQ   (&data.at(0), &range.front());
    ASSERT_EQ   (4,            range.front());
    ASSERT_TRUE (range.next());
    ASSERT_EQ   (&data.at(1), &range.front());
    ASSERT_EQ   (5,            range.front());
    ASSERT_TRUE (range.next());
    ASSERT_EQ   (&data.at(2), &range.front());
    ASSERT_EQ   (6,            range.front());
    ASSERT_TRUE (range.next());
    ASSERT_EQ   (&data.at(3), &range.front());
    ASSERT_EQ   (7,            range.front());
    ASSERT_TRUE (range.next());
    ASSERT_EQ   (&data.at(4), &range.front());
    ASSERT_EQ   (8,            range.front());
    ASSERT_FALSE(range.next());
}

TEST(LinqTest, from_container)
{
    std::vector<int> data;
    auto range = from_container(data);
    data.assign({ 4, 5, 6, 7, 8 });
    ASSERT_TRUE (range.next());
    ASSERT_EQ   (&data.at(0), &range.front());
    ASSERT_EQ   (4,            range.front());
    ASSERT_TRUE (range.next());
    ASSERT_EQ   (&data.at(1), &range.front());
    ASSERT_EQ   (5,            range.front());
    ASSERT_TRUE (range.next());
    ASSERT_EQ   (&data.at(2), &range.front());
    ASSERT_EQ   (6,            range.front());
    ASSERT_TRUE (range.next());
    ASSERT_EQ   (&data.at(3), &range.front());
    ASSERT_EQ   (7,            range.front());
    ASSERT_TRUE (range.next());
    ASSERT_EQ   (&data.at(4), &range.front());
    ASSERT_EQ   (8,            range.front());
    ASSERT_FALSE(range.next());
}

TEST(LinqTest, from_array)
{
    int data[] = { 4, 5, 6, 7, 8 };
    auto range = from_array(data);
    ASSERT_TRUE (range.next());
    ASSERT_EQ   (&data[0], &range.front());
    ASSERT_EQ   (4,         range.front());
    ASSERT_TRUE (range.next());
    ASSERT_EQ   (&data[1], &range.front());
    ASSERT_EQ   (5,         range.front());
    ASSERT_TRUE (range.next());
    ASSERT_EQ   (&data[2], &range.front());
    ASSERT_EQ   (6,         range.front());
    ASSERT_TRUE (range.next());
    ASSERT_EQ   (&data[3], &range.front());
    ASSERT_EQ   (7,         range.front());
    ASSERT_TRUE (range.next());
    ASSERT_EQ   (&data[4], &range.front());
    ASSERT_EQ   (8,         range.front());
    ASSERT_FALSE(range.next());
}

TEST(LinqTest, from_generator)
{
    size_t pos = 0;
    std::vector<int> data({ 4, 5, 6, 7, 8 });
    auto range = from_generator([&]{
        int* ret = nullptr;
        if (pos < data.size())
        {
            ret = &data.at(pos);
            ++pos;
        }
        return ret;
    });
    ASSERT_TRUE (range.next());
    ASSERT_EQ   (&data[0], &range.front());
    ASSERT_EQ   (4,         range.front());
    ASSERT_TRUE (range.next());
    ASSERT_EQ   (&data[1], &range.front());
    ASSERT_EQ   (5,         range.front());
    ASSERT_TRUE (range.next());
    ASSERT_EQ   (&data[2], &range.front());
    ASSERT_EQ   (6,         range.front());
    ASSERT_TRUE (range.next());
    ASSERT_EQ   (&data[3], &range.front());
    ASSERT_EQ   (7,         range.front());
    ASSERT_TRUE (range.next());
    ASSERT_EQ   (&data[4], &range.front());
    ASSERT_EQ   (8,         range.front());
    ASSERT_FALSE(range.next());
}

TEST(LinqTest, where)
{
    std::vector<int> data({ 4, 5, 6, 7, 8 });
    auto range = from_container(data)
        >>  where([](int& i) {
                return (i & 1) == 0;
            });
    ASSERT_TRUE (range.next());
    ASSERT_EQ   (&data[0], &range.front());
    ASSERT_EQ   (4,         range.front());
    ASSERT_TRUE (range.next());
    ASSERT_EQ   (&data[2], &range.front());
    ASSERT_EQ   (6,         range.front());
    ASSERT_TRUE (range.next());
    ASSERT_EQ   (&data[4], &range.front());
    ASSERT_EQ   (8,         range.front());
    ASSERT_FALSE(range.next());
}

TEST(LinqTest, select)
{
    std::vector<test_data> data({ test_data(1), test_data(2), test_data(3) });
    auto range = from_container(data)
        >>  select([](test_data& td)->int& {
                return td.value;
            });
    ASSERT_TRUE (range.next());
    ASSERT_EQ   (&data[0].value, &range.front());
    ASSERT_EQ   (1,               range.front());
    ASSERT_TRUE (range.next());
    ASSERT_EQ   (&data[1].value, &range.front());
    ASSERT_EQ   (2,               range.front());
    ASSERT_TRUE (range.next());
    ASSERT_EQ   (&data[2].value, &range.front());
    ASSERT_EQ   (3,               range.front());
    ASSERT_FALSE(range.next());
}

TEST(LinqTest, select_many)
{
    std::vector<TestDataMany> data({ TestDataMany({ 1, 2, 3, 4 }), TestDataMany({ }), TestDataMany({ 5, 6 }) });
    auto range = from_container(data)
        >>  select_many([](TestDataMany& td)->std::vector<int>& {
                return td.values;
            });
    ASSERT_TRUE (range.next());
    ASSERT_EQ   (&data[0].values[0], &range.front());
    ASSERT_EQ   (1,                   range.front());
    ASSERT_TRUE (range.next());
    ASSERT_EQ   (&data[0].values[1], &range.front());
    ASSERT_EQ   (2,                   range.front());
    ASSERT_TRUE (range.next());
    ASSERT_EQ   (&data[0].values[2], &range.front());
    ASSERT_EQ   (3,                   range.front());
    ASSERT_TRUE (range.next());
    ASSERT_EQ   (&data[0].values[3], &range.front());
    ASSERT_EQ   (4,                   range.front());
    ASSERT_TRUE (range.next());
    ASSERT_EQ   (&data[2].values[0], &range.front());
    ASSERT_EQ   (5,                   range.front());
    ASSERT_TRUE (range.next());
    ASSERT_EQ   (&data[2].values[1], &range.front());
    ASSERT_EQ   (6,                   range.front());
    ASSERT_FALSE(range.next());
}

TEST(LinqTest, order_by)
{
    std::vector<int> data({ 6, 3, 8, 5, 1 });
    auto range = from_container(data) >> order_by();
    ASSERT_TRUE (range.next());
    ASSERT_EQ   (&data.at(4), &range.front());
    ASSERT_EQ   (1,            range.front());
    ASSERT_TRUE (range.next());
    ASSERT_EQ   (&data.at(1), &range.front());
    ASSERT_EQ   (3,            range.front());
    ASSERT_TRUE (range.next());
    ASSERT_EQ   (&data.at(3), &range.front());
    ASSERT_EQ   (5,            range.front());
    ASSERT_TRUE (range.next());
    ASSERT_EQ   (&data.at(0), &range.front());
    ASSERT_EQ   (6,            range.front());
    ASSERT_TRUE (range.next());
    ASSERT_EQ   (&data.at(2), &range.front());
    ASSERT_EQ   (8,            range.front());
    ASSERT_FALSE(range.next());
}

TEST(LinqTest, distinct)
{
    std::vector<test_data> data({ test_data(1), test_data(2), test_data(3), test_data(1), test_data(2), test_data(4) });
    auto range = from_container(data)
        >>  distinct([](const test_data& l, const test_data& r){
                return l.value < r.value;
            });
    ASSERT_TRUE (range.next());
    ASSERT_EQ   (&data.at(0), &range.front());
    ASSERT_TRUE (range.next());
    ASSERT_EQ   (&data.at(1), &range.front());
    ASSERT_TRUE (range.next());
    ASSERT_EQ   (&data.at(2), &range.front());
    ASSERT_TRUE (range.next());
    ASSERT_EQ   (&data.at(5), &range.front());
    ASSERT_FALSE(range.next());
}

TEST(LinqTest, default_if_empty)
{
    std::vector<int> values0;
    auto e0 = from_container(values0) >> default_if_empty(5);
    ASSERT_TRUE (e0.next());
    ASSERT_EQ   (5, e0.front());
    ASSERT_FALSE(e0.next());

    std::vector<int> values1({ 1, 2, 3 });
    auto e1 = from_container(values1) >> default_if_empty(5);
    ASSERT_TRUE (e1.next());
    ASSERT_EQ   (1, e1.front());
    ASSERT_TRUE (e1.next());
    ASSERT_EQ   (2, e1.front());
    ASSERT_TRUE (e1.next());
    ASSERT_EQ   (3, e1.front());
    ASSERT_FALSE(e1.next());
}

TEST(LinqTest, count)
{
    std::vector<int> data({ 4, 5, 6, 7, 8 });
    size_t cnt = from_container(data) >> count();
    ASSERT_EQ(5, cnt);
}

TEST(LinqTest, sum)
{
    std::vector<int> data({ 4, 5, 6, 7, 8 });
    auto sum = from_container(data) >> linq::sum();
    ASSERT_EQ(30, sum);
}

TEST(LinqTest, min)
{
    std::vector<int> data({ 4, 5, 6, 7, 8 });
    ASSERT_EQ(4, from_container(data) >> min());
}

TEST(LinqTest, max)
{
    std::vector<int> data({ 4, 5, 6, 7, 8 });
    ASSERT_EQ(8, from_container(data) >> max());
}

TEST(LinqTest, any)
{
    std::vector<int> data({ 4, 5, 6, 7, 8 });
    ASSERT_TRUE (from_container(data) >> any());
    data.clear();
    ASSERT_FALSE(from_container(data) >> any());
}

TEST(LinqTest, contains)
{
    std::vector<int> data({ 4, 5, 6, 7, 8 });
    ASSERT_TRUE (from_container(data) >> contains(5));
    ASSERT_FALSE(from_container(data) >> contains(9));
}

TEST(LinqTest, single)
{
    std::vector<int> data({ 4, 5, 6, 7, 8 });
    EXPECT_ANY_THROW(from_container(data) >> single());
    data.assign({ 9 });
    EXPECT_EQ(9, from_container(data) >> single());
    data.clear();
    EXPECT_ANY_THROW(from_container(data) >> single());
}

TEST(LinqTest, single_with_struct)
{
    std::vector<TestData2> data({
        { 4, "name1" },
    });

    {
    auto d = from_container(data) >> single();
    EXPECT_EQ(4, d.value);
    EXPECT_EQ("name1", d.name);

    ASSERT_EQ(1, data.size());
    EXPECT_EQ(4, data[0].value);
    EXPECT_EQ("name1", data[0].name);
    }
}

TEST(LinqTest, single_or_default)
{
    std::vector<int> data({ 4, 5, 6, 7, 8 });
    EXPECT_EQ(0, from_container(data) >> single_or_default());
    data.assign({ 9 });
    EXPECT_EQ(9, from_container(data) >> single_or_default());
    data.clear();
    EXPECT_EQ(0, from_container(data) >> single_or_default());
}

TEST(LinqTest, single_or_default_with_struct)
{
    std::vector<TestData2> data({
        { 4, "name1" },
    });

    {
    auto d = from_container(data) >> single_or_default();
    EXPECT_EQ(4, d.value);
    EXPECT_EQ("name1", d.name);

    ASSERT_EQ(1, data.size());
    EXPECT_EQ(4, data[0].value);
    EXPECT_EQ("name1", data[0].name);
    }

    data.emplace_back(5, "name2");
    {
        auto d = from_container(data) >> single_or_default();

        EXPECT_EQ(0, d.value);
        EXPECT_EQ("", d.name);

        ASSERT_EQ(2, data.size());
        EXPECT_EQ(4, data[0].value);
        EXPECT_EQ("name1", data[0].name);
    }

    data.clear();
    {
        auto d = from_container(data) >> single_or_default();
        EXPECT_EQ(0, d.value);
        EXPECT_EQ("", d.name);
    }
}

TEST(LinqTest, first)
{
    std::vector<int> data({ 4, 5, 6, 7, 8 });
    EXPECT_EQ(4, from_container(data) >> first());
    data.assign({ 9 });
    EXPECT_EQ(9, from_container(data) >> first());
    data.clear();
    EXPECT_ANY_THROW(from_container(data) >> first());
}

TEST(LinqTest, first_with_struct)
{
    std::vector<TestData2> data({
        { 4, "name1" },
        { 5, "name2" },
    });

    {
    auto d = from_container(data) >> first();
    EXPECT_EQ(4, d.value);
    EXPECT_EQ("name1", d.name);

    ASSERT_EQ(2, data.size());
    EXPECT_EQ(4, data[0].value);
    EXPECT_EQ("name1", data[0].name);
    }
}

TEST(LinqTest, first_or_default)
{
    std::vector<int> data({ 4, 5, 6, 7, 8 });
    EXPECT_EQ(4, from_container(data) >> first_or_default());
    data.assign({ 9 });
    EXPECT_EQ(9, from_container(data) >> first_or_default());
    data.clear();
    EXPECT_EQ(0, from_container(data) >> first_or_default());
}

TEST(LinqTest, first_or_default_with_struct)
{
    std::vector<TestData2> data({
        { 4, "name1" },
        { 5, "name2" },
    });

    {
    auto d = from_container(data) >> first_or_default();
    EXPECT_EQ(4, d.value);
    EXPECT_EQ("name1", d.name);

    ASSERT_EQ(2, data.size());
    EXPECT_EQ(4, data[0].value);
    EXPECT_EQ("name1", data[0].name);
    }

    data.clear();
    {
    auto d = from_container(data) >> first_or_default();
    EXPECT_EQ(0, d.value);
    EXPECT_EQ("", d.name);
    }
}

TEST(LinqTest, last)
{
    std::vector<int> data({ 4, 5, 6, 7, 8 });
    EXPECT_EQ(8, from_container(data) >> last());
    data.assign({ 9 });
    EXPECT_EQ(9, from_container(data) >> last());
    data.clear();
    EXPECT_ANY_THROW(from_container(data) >> last());
}

TEST(LinqTest, last_with_struct)
{
    std::vector<TestData2> data({
        { 4, "name1" },
        { 5, "name2" },
    });

    {
    auto d = from_container(data) >> last();
    EXPECT_EQ(5, d.value);
    EXPECT_EQ("name2", d.name);

    ASSERT_EQ(2, data.size());
    EXPECT_EQ(5, data[1].value);
    EXPECT_EQ("name2", data[1].name);
    }
}

TEST(LinqTest, last_or_default)
{
    std::vector<int> data({ 4, 5, 6, 7, 8 });
    EXPECT_EQ(8, from_container(data) >> last_or_default());
    data.assign({ 9 });
    EXPECT_EQ(9, from_container(data) >> last_or_default());
    data.clear();
    EXPECT_EQ(0, from_container(data) >> last_or_default());
}

TEST(LinqTest, last_or_default_with_struct)
{
    std::vector<TestData2> data({
        { 4, "name1" },
        { 5, "name2" },
    });

    {
    auto d = from_container(data) >> last_or_default();
    EXPECT_EQ(5, d.value);
    EXPECT_EQ("name2", d.name);

    ASSERT_EQ(2, data.size());
    EXPECT_EQ(5, data[1].value);
    EXPECT_EQ("name2", data[1].name);
    }

    data.clear();
    {
    auto d = from_container(data) >> last_or_default();
    EXPECT_EQ(0, d.value);
    EXPECT_EQ("", d.name);
    }
}

TEST(LinqTest, to_vector)
{
    int data[] = { 4, 5, 6, 7, 8 };
    auto vec = from_array(data) >> to_vector();
    ASSERT_EQ(5, vec.size());
    EXPECT_EQ(4, vec[0]);
    EXPECT_EQ(5, vec[1]);
    EXPECT_EQ(6, vec[2]);
    EXPECT_EQ(7, vec[3]);
    EXPECT_EQ(8, vec[4]);
}

TEST(LinqTest, to_list)
{
    int data[] = { 4, 5, 6, 7, 8 };
    auto list = from_array(data) >> to_list();
    auto it = list.begin();
    ASSERT_NE(it, list.end());
    EXPECT_EQ(4, *it);
    ++it;
    ASSERT_NE(it, list.end());
    EXPECT_EQ(5, *it);
    ++it;
    ASSERT_NE(it, list.end());
    EXPECT_EQ(6, *it);
    ++it;
    ASSERT_NE(it, list.end());
    EXPECT_EQ(7, *it);
    ++it;
    ASSERT_NE(it, list.end());
    EXPECT_EQ(8, *it);
    ++it;
    ASSERT_EQ(it, list.end());
}

TEST(LinqTest, to_map)
{
    using pair_type   = std::pair<int, std::string>;
    using vector_type = std::vector<pair_type>;
    vector_type data({
        std::make_pair(1, "1"),
        std::make_pair(2, "2"),
        std::make_pair(3, "3")
    });

    auto map0 = from_container(data) >> to_map();
    auto it = map0.begin();
    ASSERT_NE(it, map0.end());
    EXPECT_EQ(1, it->first);
    EXPECT_EQ(std::string("1"), it->second);
    ++it;
    ASSERT_NE(it, map0.end());
    EXPECT_EQ(2, it->first);
    EXPECT_EQ(std::string("2"), it->second);
    ++it;
    ASSERT_NE(it, map0.end());
    EXPECT_EQ(3, it->first);
    EXPECT_EQ(std::string("3"), it->second);
    ++it;
    ASSERT_EQ(it, map0.end());

    data.emplace_back(3, "4");
    EXPECT_ANY_THROW(from_container(data) >> to_map());
}

TEST(LinqTest, for_each)
{
    int index = 0;
    int data[] = { 4, 5, 6, 7, 8 };
    from_array(data) >> for_each([&](int& i){
        EXPECT_EQ(&data[index], &i);
        ++index;
    });
}

TEST(LinqTest, to_lookup)
{
    using pair_type   = std::pair<int, std::string>;
    using vector_type = std::vector<pair_type>;
    vector_type data({
        { 0, "Str0-0" },
        { 1, "Str1-0" },
        { 2, "Str2-0" },
        { 0, "Str0-1" },
        { 1, "Str1-1" },
        { 2, "Str2-1" },
        { 1, "Str1-2" },
        { 0, "Str0-2" },
    });
    auto lookup = from_container(data)
        >>  to_lookup([](pair_type& p)->int&{
                return p.first;
            }, [](pair_type& p)->std::string&{
                return p.second;
            });

    auto range0 = lookup[0];
    ASSERT_TRUE (range0.next());
    ASSERT_EQ   (&data.at(0).second,    &range0.front());
    ASSERT_EQ   (std::string("Str0-0"),  range0.front());
    ASSERT_TRUE (range0.next());
    ASSERT_EQ   (&data.at(3).second,    &range0.front());
    ASSERT_EQ   (std::string("Str0-1"),  range0.front());
    ASSERT_TRUE (range0.next());
    ASSERT_EQ   (&data.at(7).second,    &range0.front());
    ASSERT_EQ   (std::string("Str0-2"),  range0.front());
    ASSERT_FALSE(range0.next());

    auto range1 = lookup[1];
    ASSERT_TRUE (range1.next());
    ASSERT_EQ   (&data.at(1).second,    &range1.front());
    ASSERT_EQ   (std::string("Str1-0"),  range1.front());
    ASSERT_TRUE (range1.next());
    ASSERT_EQ   (&data.at(4).second,    &range1.front());
    ASSERT_EQ   (std::string("Str1-1"),  range1.front());
    ASSERT_TRUE (range1.next());
    ASSERT_EQ   (&data.at(6).second,    &range1.front());
    ASSERT_EQ   (std::string("Str1-2"),  range1.front());
    ASSERT_FALSE(range1.next());

    auto range2 = lookup[2];
    ASSERT_TRUE (range2.next());
    ASSERT_EQ   (&data.at(2).second,    &range2.front());
    ASSERT_EQ   (std::string("Str2-0"),  range2.front());
    ASSERT_TRUE (range2.next());
    ASSERT_EQ   (&data.at(5).second,    &range2.front());
    ASSERT_EQ   (std::string("Str2-1"),  range2.front());
    ASSERT_FALSE(range2.next());

    using key_value_range_type = linq::lookup_key_value_range_type<int&, std::string&>;
    auto map = lookup
        >>  to_map([](key_value_range_type::value_type v){
                return v.first;
            }, [](key_value_range_type::value_type v){
                return v.second >> to_vector();
            });

    using map_type = decltype(map);
    map_type expected({
        { 0, { "Str0-0", "Str0-1", "Str0-2" } },
        { 1, { "Str1-0", "Str1-1", "Str1-2" } },
        { 2, { "Str2-0", "Str2-1" } }
    });
    EXPECT_EQ(expected, map);
}

struct StationMock
{
    uint32_t id;
    std::string name1;
    std::string name2;
    std::string name3;
    std::string name4;
};

struct op_select_names
{
    using pair_type   = std::pair<std::string, uint32_t>;
    using vector_type = std::vector<pair_type>;

    inline void addToVec(vector_type& vec, const std::string& name, uint32_t sID)
    {
        if (name.empty())
            return;
        bool exists = from_container(vec)
                        >> select(op_select_key_default())
                        >> contains(name);
        if (!exists)
            vec.emplace_back(name, sID);
    }

    inline vector_type operator()(StationMock& s)
    {
        vector_type ret;
        addToVec(ret, s.name1, s.id);
        addToVec(ret, s.name2, s.id);
        addToVec(ret, s.name3, s.id);
        addToVec(ret, s.name4, s.id);
        return ret;
    }
};

void checkMapEntry(std::map<std::string, std::vector<uint32_t>>& map, std::string name, std::vector<uint32_t> ids)
{
    auto it = map.find(name);
    ASSERT_NE(it, map.end());
    EXPECT_EQ(name, it->first);
    EXPECT_EQ(ids, it->second);
}

TEST(LinqTest, to_lookup_2)
{
    std::vector<StationMock> stations ({
        { 1, "sname11" , "sname12", "sname13", "sname14" },
        { 2, "sname21" , "sname21", "sname23", "sname24" },
        { 3, "sname11" , "sname31", "sname33", "sname34" },
    });

    using key_value_range_type = linq::lookup_key_value_range_type<std::string, uint32_t>;
    auto myMap = linq::from_container(stations)
        >>  linq::select_many(op_select_names())
        >>  linq::to_lookup()
        >>  linq::to_map([](key_value_range_type::value_type p) {
                return p.first;
            }, [](key_value_range_type::value_type p) {
                return p.second >> linq::to_vector();
            });
    checkMapEntry(myMap, "sname11", { 1, 3 });
    checkMapEntry(myMap, "sname12", { 1 });
    checkMapEntry(myMap, "sname13", { 1 });
    checkMapEntry(myMap, "sname14", { 1 });
    checkMapEntry(myMap, "sname21", { 2 });
    checkMapEntry(myMap, "sname23", { 2 });
    checkMapEntry(myMap, "sname24", { 2 });
    checkMapEntry(myMap, "sname31", { 3 });
    checkMapEntry(myMap, "sname33", { 3 });
    checkMapEntry(myMap, "sname34", { 3 });
}

TEST(LinqTest, moveable_objects)
{
    std::vector<MoveOnlyData> data;
    data.emplace_back(1);

    {
    data.at(0).value = 1;
    auto v = from_container(data)
        >>  select([](MoveOnlyData& d) {
                return std::move(d);
            })
        >>  first();
    EXPECT_EQ(1, v.value);
    EXPECT_EQ(0, data.at(0).value);
    }

    {
    data.at(0).value = 2;
    auto v = from_container(data)
        >>  select([](MoveOnlyData& d) {
                return std::move(d);
            })
        >>  first_or_default();
    EXPECT_EQ(2, v.value);
    EXPECT_EQ(0, data.at(0).value);
    }

    {
    data.at(0).value = 3;
    auto v = from_container(data)
        >>  select([](MoveOnlyData& d) {
                return std::move(d);
            })
        >>  last();
    EXPECT_EQ(3, v.value);
    EXPECT_EQ(0, data.at(0).value);
    }

    {
    data.at(0).value = 4;
    auto v = from_container(data)
        >>  select([](MoveOnlyData& d) {
                return std::move(d);
            })
        >>  last_or_default();
    EXPECT_EQ(4, v.value);
    EXPECT_EQ(0, data.at(0).value);
    }

    {
    data.at(0).value = 5;
    auto v = from_container(data)
        >>  select([](MoveOnlyData& d) {
                return std::move(d);
            })
        >>  single();
    EXPECT_EQ(5, v.value);
    EXPECT_EQ(0, data.at(0).value);
    }

    {
    data.at(0).value = 6;
    auto v = from_container(data)
        >>  select([](MoveOnlyData& d) {
                return std::move(d);
            })
        >>  single_or_default();
    EXPECT_EQ(6, v.value);
    EXPECT_EQ(0, data.at(0).value);
    }

    {
    data.at(0).value = 7;
    auto v = from_container(data)
        >>  select([](MoveOnlyData& d) {
                return std::move(d);
            })
        >>  to_vector();
    ASSERT_EQ(1, v.size());
    EXPECT_EQ(7, v.at(0).value);
    EXPECT_EQ(0, data.at(0).value);
    }

    {
    data.at(0).value = 8;
    auto v = from_container(data)
        >>  select([](MoveOnlyData& d) {
                return std::move(d);
            })
        >>  to_map([](MoveOnlyData& d){
                return 5;
            }, [](MoveOnlyData& d){
                return std::move(d);
            });
    auto it = v.find(5);
    ASSERT_NE(it, v.end());
    EXPECT_EQ(8, it->second.value);
    EXPECT_EQ(0, data.at(0).value);
    }

    {
    data.at(0).value = 9;
    auto v = from_container(data)
        >>  select([](MoveOnlyData& d) {
                return std::move(d);
            })
        >>  to_list();
    auto it = v.begin();
    ASSERT_NE(it, v.end());
    EXPECT_EQ(9, it->value);
    EXPECT_EQ(0, data.at(0).value);
    }
}

TEST(LinqTest, const_objects)
{
    const test_data data[] = { test_data(1) };

    {
    auto v = from_array(data)
        >>  select([](const test_data& d) {
                return d;
            })
        >>  first();
    EXPECT_EQ(1, v.value);
    }

    {
    auto v = from_array(data)
        >>  select([](const test_data& d) {
                return d;
            })
        >>  first_or_default();
    EXPECT_EQ(1, v.value);
    }

    {
    auto v = from_array(data)
        >>  select([](const test_data& d) {
                return std::move(d);
            })
        >>  last();
    EXPECT_EQ(1, v.value);
    }

    {
    auto v = from_array(data)
        >>  select([](const test_data& d) {
                return d;
            })
        >>  last_or_default();
    EXPECT_EQ(1, v.value);
    }

    {
    auto v = from_array(data)
        >>  select([](const test_data& d) {
                return d;
            })
        >>  single();
    EXPECT_EQ(1, v.value);
    }

    {
    auto v = from_array(data)
        >>  select([](const test_data& d) {
                return d;
            })
        >>  single_or_default();
    EXPECT_EQ(1, v.value);
    }

    {
    auto v = from_array(data)
        >>  select([](const test_data& d) {
                return d;
            })
        >>  to_list();
    auto it = v.begin();
    ASSERT_NE(it, v.end());
    EXPECT_EQ(1, it->value);
    }
}