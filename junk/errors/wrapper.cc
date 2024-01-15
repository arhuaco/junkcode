// Just an example.

#include <stdint.h>

#include <cstring>
#include <mutex>
#include <string>
#include <unordered_map>

static std::unordered_map<int32_t, std::string> err_to_msg_;
static std::mutex err_to_msg_mutex_;

static void init_messages() {
  err_to_msg_[0] = "OK";
  err_to_msg_[1] = "ERROR";
}

int32_t get_error_message(int32_t error_code, char *buffer,
                          int32_t max_buffer_len) {
  {
    std::lock_guard<std::mutex> lock(err_to_msg_mutex_);
    if (err_to_msg_.empty()) {
      init_messages();
    }
  }

  if (err_to_msg_.find(error_code) == err_to_msg_.end()) {
    return -1;
  }

  const auto message = err_to_msg_[error_code];

  if (message.length() + 1 > max_buffer_len) {
    return -1;
  }

  strcpy(buffer, message.c_str());

  return 0;
}

int main() {
  char buffer[256];
  int32_t code, ret;
  for (code = 0; code <= 2; ++code) {
    strcpy(buffer, "A");
    ret = get_error_message(code, buffer, 256);
    printf("code:%d ret:%d s:%s\n", code, ret, buffer);
  }
  return 0;
}
