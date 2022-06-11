#include "factory"

/* Here is an example for a base class with virtual interfaces and also provide
 * a factory.
 */

class InterfaceClass
{
public:
  std::string class_data;

public:
  InterfaceClass(const std::string &s)
  {
    class_data = "data set by InterfaceClass: " + s;
  }
  virtual std::string class_name(void) const
  {
    return "InterfaceClass";
  }

  virtual ~InterfaceClass() = default;

  using factory = yuc::factory<InterfaceClass, std::string>;
};

factory_decl_registry(InterfaceClass::factory);
