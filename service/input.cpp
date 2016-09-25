#include <service/input.h>

Input::Input(const std::string & name, bool delay): ActiveObject(name, 1, delay) {}
Input::~Input() {}