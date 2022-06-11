#include "example-interface.h"

factory_init_registry(InterfaceClass::factory);

namespace __factory_InterfaceClass
{
auto _ = InterfaceClass::factory::record<InterfaceClass>("");
auto B = InterfaceClass::factory::record<InterfaceClass>("base-class");
}; // namespace __factory_InterfaceClass

// tests
#include <gtest/gtest.h>

TEST(factory, id_can_be_empty_string)
{
  auto p = InterfaceClass::factory::create("", "base");
  ASSERT_NE(p, nullptr);
  EXPECT_EQ(p->class_name(), "InterfaceClass");
  EXPECT_EQ(p->class_data, "data set by InterfaceClass: base");
}

TEST(factory, single_class_can_have_multiple_ids)
{
  auto p = InterfaceClass::factory::create("base-class", "also base");
  ASSERT_NE(p, nullptr);
  EXPECT_EQ(p->class_name(), "InterfaceClass");
  EXPECT_EQ(p->class_data, "data set by InterfaceClass: also base");
}

TEST(factory, return_nullptr_for_unregistered_id)
{
  auto p = InterfaceClass::factory::create("unregistered id", "");
  ASSERT_EQ(p, nullptr);
}
