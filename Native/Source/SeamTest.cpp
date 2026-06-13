#include <string>

#include <nanobind/nanobind.h>
#include <nanobind/stl/string.h>

// C++ -> Python return marshaling.
int SeamTest() {
    return 42;
}

// Python -> C++ argument marshaling (and back).
std::string SeamEcho(const std::string& str) {
    return "Native ottoon got: " + str;
}

NB_MODULE(ottoon, module) {
    module.def("seamTest", &SeamTest);
    module.def("seamEcho", &SeamEcho);
}
