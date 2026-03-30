#ifndef BUILD_MODULE
#include "cina.hpp"
#else
import cina;
#endif

#include <gtest/gtest.h>
#include <iterator>
#include <vector>

template <typename T> class custom_allocator : public std::allocator<T> {
public:
  using value_type = T;
  using is_always_equal = std::false_type;

  constexpr custom_allocator(int i) : tag{i} {}

  template <typename U>
  constexpr custom_allocator(const custom_allocator<U>& other) noexcept
      : tag{other.tag} {}

private:
  template <typename U> friend class custom_allocator;

  template <typename U>
  friend bool operator==(const custom_allocator<U>& lhs,
                         const custom_allocator<U>& rhs) {
    return lhs.tag == rhs.tag;
  }

  int tag{};
};

TEST(TestVectorLikeContainer, TestConstructor) {
  using vector_type = cina::new_type<struct VectorTag, std::vector<int>>;

  vector_type v1;
  EXPECT_TRUE(v1.unwrap().empty());
  EXPECT_EQ(v1.unwrap().size(), 0);

  custom_allocator<int> alloc{42};
  using vector_type2 = cina::new_type<struct VectorTag2,
                                      std::vector<int, custom_allocator<int>>>;
  vector_type2 v2(alloc);
  EXPECT_TRUE(v2.unwrap().empty());
  EXPECT_EQ(v2.unwrap().size(), 0);
  EXPECT_EQ(v2.unwrap().get_allocator(), alloc);

  vector_type v3(2, 42);
  EXPECT_EQ(v3.unwrap().size(), 2);
  EXPECT_EQ(v3.unwrap()[0], 42);
  EXPECT_EQ(v3.unwrap()[1], 42);

  std::vector vec{1, 2, 3};
  vector_type v4(vec.begin(), vec.end());
  EXPECT_EQ(v4.unwrap().size(), 3);
  EXPECT_EQ(v4.unwrap()[0], 1);
  EXPECT_EQ(v4.unwrap()[1], 2);
  EXPECT_EQ(v4.unwrap()[2], 3);

  vector_type v5{1, 2, 3};
  EXPECT_EQ(v5.unwrap().size(), 3);
  EXPECT_EQ(v5.unwrap()[0], 1);
  EXPECT_EQ(v5.unwrap()[1], 2);
  EXPECT_EQ(v5.unwrap()[2], 3);

  using vector_reference = cina::new_type<struct VectorTag2, std::vector<int>&>;
  vector_reference ref{vec};
  EXPECT_EQ(ref.unwrap().size(), 3);

  using vector_const_reference =
      cina::new_type<struct VectorTag3, const std::vector<int>&>;
  vector_const_reference ref2{vec};
  EXPECT_EQ(ref.unwrap().size(), 3);
}

TEST(TestVectorLikeContainer, TestContainerMethods) {
  using vector_type = cina::new_type<struct VectorTag, std::vector<int>>;

  vector_type v{1, 2, 3};
  EXPECT_EQ(v.size(), 3);
  EXPECT_FALSE(v.empty());
  EXPECT_EQ(*v.begin(), 1);
  EXPECT_EQ(*v.cbegin(), 1);
  EXPECT_EQ(std::distance(v.begin(), v.end()), 3);
  EXPECT_EQ(std::distance(v.cbegin(), v.cend()), 3);

  EXPECT_TRUE((std::random_access_iterator<vector_type::iterator>));
  EXPECT_TRUE((std::random_access_iterator<vector_type::const_iterator>));

  vector_type v2{4, 5, 6};
  v.swap(v2);
  EXPECT_EQ(v.unwrap()[0], 4);
  EXPECT_EQ(v.unwrap()[1], 5);
  EXPECT_EQ(v.unwrap()[2], 6);
  EXPECT_EQ(v2.unwrap()[0], 1);
  EXPECT_EQ(v2.unwrap()[1], 2);
  EXPECT_EQ(v2.unwrap()[2], 3);

  swap(v, v2);
  EXPECT_EQ(v.unwrap()[0], 1);
  EXPECT_EQ(v.unwrap()[1], 2);
  EXPECT_EQ(v.unwrap()[2], 3);
  EXPECT_EQ(v2.unwrap()[0], 4);
  EXPECT_EQ(v2.unwrap()[1], 5);
  EXPECT_EQ(v2.unwrap()[2], 6);

  using vector_reference = cina::new_type<struct VectorTag2, std::vector<int>&>;
  std::vector vec{1, 2, 3};
  vector_reference ref{vec};
  EXPECT_EQ(ref.size(), 3);
  EXPECT_FALSE(ref.empty());
  EXPECT_EQ(*ref.begin(), 1);
  EXPECT_EQ(*ref.cbegin(), 1);
  EXPECT_EQ(std::distance(ref.begin(), ref.end()), 3);
  EXPECT_EQ(std::distance(ref.cbegin(), ref.cend()), 3);

  EXPECT_TRUE((std::random_access_iterator<vector_reference::iterator>));
  EXPECT_TRUE((std::random_access_iterator<vector_reference::const_iterator>));

  std::vector vec2{4, 5, 6};
  vector_reference ref2{vec2};

  ref2.swap(ref);
  EXPECT_EQ(ref.unwrap()[0], 4);
  EXPECT_EQ(ref.unwrap()[1], 5);
  EXPECT_EQ(ref.unwrap()[2], 6);
  EXPECT_EQ(vec[0], 4);
  EXPECT_EQ(vec[1], 5);
  EXPECT_EQ(vec[2], 6);
  EXPECT_EQ(ref2.unwrap()[0], 1);
  EXPECT_EQ(ref2.unwrap()[1], 2);
  EXPECT_EQ(ref2.unwrap()[2], 3);
  EXPECT_EQ(vec2[0], 1);
  EXPECT_EQ(vec2[1], 2);
  EXPECT_EQ(vec2[2], 3);

  swap(ref, ref2);
  EXPECT_EQ(ref.unwrap()[0], 1);
  EXPECT_EQ(ref.unwrap()[1], 2);
  EXPECT_EQ(ref.unwrap()[2], 3);
  EXPECT_EQ(vec[0], 1);
  EXPECT_EQ(vec[1], 2);
  EXPECT_EQ(vec[2], 3);
  EXPECT_EQ(ref2.unwrap()[0], 4);
  EXPECT_EQ(ref2.unwrap()[1], 5);
  EXPECT_EQ(ref2.unwrap()[2], 6);
  EXPECT_EQ(vec2[0], 4);
  EXPECT_EQ(vec2[1], 5);
  EXPECT_EQ(vec2[2], 6);
}

TEST(TestVectorLikeContainer, TestModificationMethods) {
  using vector_type = cina::new_type<struct VectorTag, std::vector<int>>;

  vector_type v;
  v.push_back(1);
  v.push_back(2);
  EXPECT_EQ(v.size(), 2);
  EXPECT_EQ(*v.begin(), 1);
  EXPECT_EQ(*std::next(v.begin()), 2);

  v.pop_back();
  EXPECT_EQ(v.size(), 1);

  v.emplace_back(3);
  EXPECT_EQ(v.size(), 2);
  EXPECT_EQ(*std::next(v.begin()), 3);

  using vector_reference = cina::new_type<struct VectorTag2, std::vector<int>&>;
  std::vector vec{1, 2, 3};
  vector_reference ref{vec};
  ref.push_back(4);
  EXPECT_EQ(ref.size(), 4);
  EXPECT_EQ(vec.size(), 4);
  EXPECT_EQ(vec.back(), 4);

  ref.pop_back();
  EXPECT_EQ(ref.size(), 3);
  EXPECT_EQ(vec.size(), 3);
  EXPECT_EQ(vec.back(), 3);

  ref.emplace_back(4);
  EXPECT_EQ(ref.size(), 4);
  EXPECT_EQ(vec.size(), 4);
  EXPECT_EQ(vec.back(), 4);
}