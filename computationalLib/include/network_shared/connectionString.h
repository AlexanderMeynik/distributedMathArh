#pragma once

#include <string_view>
#include <string>

/// Namespace for network related types
namespace network_types {
///@todo create common interface for c string
/// use it in queue
/// us it for verbose names
/**
* @brief  Structure to store and format connection string
*/
struct myConnString {
  myConnString() : port(5432) {}

  myConnString(std::string_view user,
               std::string_view password,
               std::string_view host,
               std::string_view dbname, unsigned port);

  explicit operator std::string() {
    return formatted_string;
  }

  operator std::string_view();

  [[nodiscard]] const char *CStr() const;

  void SetUser(std::string_view new_user);

  void SetPassword(std::string_view new_password);

  void SetHost(std::string_view new_host);

  void SetPort(unsigned new_port);

  void SetDbname(std::string_view new_dbname);

  [[nodiscard]] const std::string &GetUser() const;

  [[nodiscard]] const std::string &GetPassword() const;

  [[nodiscard]] const std::string &GetHost() const;

  [[nodiscard]] const std::string &GetDbname() const;

  [[nodiscard]] unsigned int GetPort() const;

  [[nodiscard]] std::string GetVerboseName() const;
  bool operator==(const myConnString &rhs) const;

 private:
  void UpdateFormat();

  std::string user, password, host, dbname;
  unsigned port;
  std::string formatted_string;
};
}