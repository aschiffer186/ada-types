#include <cina.hpp>

#include <concepts>
#include <gtest/gtest.h>
#include <type_traits>

struct Foo {
public:
  explicit Foo(int i) : i(i) {}

  int operator()(int j) const { return i + j; }

private:
  int i{};
};

struct Bar {
public:
  explicit Bar(int j) : i(j) {}

  Bar(const Bar&) = delete;
  Bar& operator=(const Bar&) = delete;

  Bar(Bar&&) = default;
  Bar& operator=(Bar&&) = default;

  int operator()(int j) const { return i + j; }

private:
  int i;
};

int func(int j) { return 42 + j; }
int func2(int j) noexcept { return 42 + j; }
void func3() noexcept {}

TEST(TestCallable, TestCXXProperties) {
  using callable_type = cina::callable_type<struct CallableTag, Foo, int>;

  EXPECT_FALSE(std::is_default_constructible_v<callable_type>);
  EXPECT_TRUE(std::copyable<callable_type>);

  EXPECT_EQ(sizeof(callable_type), sizeof(Foo));
  EXPECT_EQ(alignof(callable_type), alignof(Foo));
}

TEST(TestCallable, TestConstructor) {
  using callable_type = cina::callable_type<struct CallableTag, Foo, int>;

  const callable_type c{Foo{42}};
  EXPECT_EQ(c.unwrap()(10), 52);

  using callable_type2 =
      cina::callable_type<struct CallableTag2, std::function<int(int)>, int>;
  const callable_type2 c2{
      std::function<int(int)>{[](int j) { return 42 + j; }}};
  EXPECT_EQ(c2.unwrap()(10), 52);

  using callable_type3 =
      cina::callable_type<struct CallableTag3, std::function<int(int)>, int>;
  EXPECT_FALSE((std::constructible_from<callable_type2, callable_type3>));

  using pointer_to_function_type =
      cina::callable_type<struct CallableTag4, int (*)(int), int>;
  const pointer_to_function_type c3{func};
  EXPECT_EQ(c3.unwrap()(10), 52);

  using callable_type5 =
      cina::callable_type<struct CallableTag5, decltype(func), int>;
  const callable_type5 c4{func};
  EXPECT_EQ(c4.unwrap()(10), 52);

  using callable_type6 = cina::callable_type<struct CallableTag6, Bar, int>;
  const callable_type6 c5{std::in_place, 42};
  EXPECT_EQ(c5.unwrap()(10), 52);
}

TEST(TestCallable, TestCallOperator) {
  using callable_type = cina::callable_type<struct CallableTag, Foo, int>;

  const callable_type c{Foo{42}};
  EXPECT_EQ(c(10), 52);
  EXPECT_TRUE((std::invocable<callable_type, int>));
  EXPECT_FALSE((std::is_nothrow_invocable_v<callable_type, int>));

  using callable_type2 =
      cina::callable_type<struct CallableTag2, std::function<int(int)>, int>;
  const callable_type2 c2{
      std::function<int(int)>{[](int j) { return 42 + j; }}};
  EXPECT_EQ(c2(10), 52);
  EXPECT_TRUE((std::invocable<callable_type2, int>));
  EXPECT_FALSE((std::is_nothrow_invocable_v<callable_type2, int>));

  using callable_type3 =
      cina::callable_type<struct CallableTag3, int (*)(int), int>;
  const callable_type3 c3{func};
  EXPECT_EQ(c3(10), 52);
  EXPECT_TRUE((std::invocable<callable_type3, int>));
  EXPECT_FALSE((std::is_nothrow_invocable_v<callable_type3, int>));

  using callable_type4 =
      cina::callable_type<struct CallableTag4, decltype(func), int>;
  const callable_type4 c4{func};
  EXPECT_EQ(c4(10), 52);
  EXPECT_TRUE((std::invocable<callable_type4, int>));
  EXPECT_FALSE((std::is_nothrow_invocable_v<callable_type4, int>));

  using callable_type5 =
      cina::callable_type<struct CallableTag5, decltype(func2), int>;
  const callable_type5 c5{func2};
  EXPECT_EQ(c5(10), 52);
  EXPECT_TRUE((std::is_nothrow_invocable_v<callable_type5, int>));

  using callable_type6 =
      cina::callable_type<struct CallableTag6, decltype(func3)>;
  const callable_type6 c6{func3};
  EXPECT_NO_THROW(c6());
  EXPECT_TRUE((std::is_nothrow_invocable_v<callable_type6>));

  using callable_type7 = cina::callable_type<struct CallableTag7, Bar, int>;
  const callable_type7 c7{std::in_place, 42};
  EXPECT_EQ(c7(10), 52);
}