#include "network_shared/connectionString.h"
#include <fmt/format.h>

namespace network_types {

myConnString::myConnString(std::string_view user,
                           std::string_view password,
                           std::string_view host,
                           std::string_view dbname,
                           unsigned int port)
    : user(user), password(password),
      host(host),
      dbname(dbname),
      port(port) {
  UpdateFormat();
}
myConnString::operator std::string_view() {
  return formatted_string;
}
const char *myConnString::CStr() const {
  return formatted_string.c_str();
}
void myConnString::SetPassword(std::string_view new_password) {
  password = new_password;
  UpdateFormat();
}
void myConnString::SetHost(std::string_view new_host) {
  host = new_host;
  UpdateFormat();
}
void myConnString::SetPort(unsigned int new_port) {
  port = new_port;
  UpdateFormat();
}
void myConnString::SetDbname(std::string_view new_dbname) {
  dbname = new_dbname;
  UpdateFormat();
}
void myConnString::SetUser(std::string_view new_user) {
  user = std::forward<std::string_view>(new_user);
  UpdateFormat();
}
const std::string &myConnString::GetUser() const {
  return user;
}
const std::string &myConnString::GetPassword() const {
  return password;
}
const std::string &myConnString::GetHost() const {
  return host;
}
const std::string &myConnString::GetDbname() const {
  return dbname;
}
unsigned int myConnString::GetPort() const {
  return port;
}
void myConnString::UpdateFormat() {
  formatted_string = fmt::format("postgresql://{}:{}@{}:{}/{}",
                                 user.c_str(), password.c_str(), host.c_str(), port, dbname.c_str());
}
std::string myConnString::GetVerboseName() const {
  return fmt::format("{}:{} db:{}", host, port, dbname);
}
bool myConnString::operator==(const myConnString &rhs) const {
  return user == rhs.user &&
      password == rhs.password &&
      host == rhs.host &&
      dbname == rhs.dbname &&
      port == rhs.port &&
      formatted_string == rhs.formatted_string;
}


}