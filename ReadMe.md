# C++ Wapcaplet

## Installing

To include this library in your project, you can install it using vcpkg. First, add the registry for this project to your vcpkg as a repository
supplying the `wapcaplet` library.

```json
{
  "kind": "git",
  "repository": "https://github.com/Microsoft/vcpkg",
  "baseline": "84a143e4caf6b70db57f28d04c41df4a85c480fa",
  "packages": ["wapcaplet"]
}
```

Then, you can install the library using the following command:

```bash
vcpkg add wapcaplet
```

## Usage

A simple example of using wapcaplet:
```cpp
#include <wapcaplet/wapcaplet.h>

int main() {
    lwc::string* hello_world = lwc::intern("Hello, World!", 13);
    lwc::string* hello = lwc::intern_substring(s, 0, 5);
    
    std::cout << hello_world->data() << std::endl; // Outputs: Hello, World!
    std::cout << hello->data() << std::endl; // Outputs: Hello

    lwc::string* hello2 = lwc::ref(hello); // Obtains a reference to hello
    assert(hello == hello2); // Both point to the same string*
    
    lwc::string* hello3 = lwc::intern("Hello", 5);
    assert(hello->data() == hello3->data()); // Both point to the same interned string
}
```
