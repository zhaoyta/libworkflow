#include <service/tcp_input.h>

TCPInput::TCPInput(const std::string & name, bool delay): Input(name, delay) {}
TCPInput::~TCPInput() {}