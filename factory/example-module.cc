#include "example-interface.h"

class ImplClass : public InterfaceClass
{
public:
  ImplClass(const std::string &s) : InterfaceClass(s)
  {
    class_data = "data set by ImplClass: " + s;
  }

  virtual std::string class_name(void) const
  {
    return "ImplClass";
  }
};

namespace __factory_InterfaceClass
{
using factory = InterfaceClass::factory;

auto _impl0 = factory::record<ImplClass>("impl0");

auto _impl1 = factory::record(
    "impl1",
    [](const std::string &s)
    {
      auto p = std::make_unique<ImplClass>(s);
      p->class_data = "data set by creator function: " + s;
      return p;
    });

auto _impl2 = factory::record<ImplClass>(
    "impl2",
    [](std::unique_ptr<InterfaceClass> &p)
    {
      p->class_data.replace(12, 9, "handler function");
      p->class_data.replace(5, 3, "modified");
    });

} // namespace __factory_InterfaceClass

// tests
#include <gtest/gtest.h>

TEST(factory, throw_logic_error_at_duplicated_id)
{
  ASSERT_THROW(
      InterfaceClass::factory::record(
          "impl0", [](const std::string &s)
          { return std::make_unique<InterfaceClass>("[" + s + "]"); }),
      std::logic_error);
}

TEST(factory, create_with_default_creator)
{
  auto p = InterfaceClass::factory::create("impl0", "a");
  ASSERT_NE(p, nullptr);
  EXPECT_EQ(p->class_name(), "ImplClass");
  EXPECT_EQ(p->class_data, "data set by ImplClass: a");
}

TEST(factory, create_with_custom_creator)
{
  auto p = InterfaceClass::factory::create("impl1", "b");
  ASSERT_NE(p, nullptr);
  EXPECT_EQ(p->class_name(), "ImplClass");
  EXPECT_EQ(p->class_data, "data set by creator function: b");
}

TEST(factory, modified_with_custom_handler)
{
  auto p = InterfaceClass::factory::create("impl2", "c");
  ASSERT_NE(p, nullptr);
  EXPECT_EQ(p->class_name(), "ImplClass");
  EXPECT_EQ(p->class_data, "data modified by handler function: c");
}
