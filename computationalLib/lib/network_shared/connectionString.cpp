#include "network_shared/connectionString.h"
#include <fmt/format.h>

namespace network_types {

PostgreSQLCStr::PostgreSQLCStr(std::string_view user,
                               std::string_view password,
                               std::string_view host,
                               std::string_view dbname,
                               unsigned int port)
    : user_(user), password_(password),
      host_(host),
      dbname_(dbname),
      port_(port) {
  UpdateFormat();
}

void PostgreSQLCStr::SetPassword(std::string_view new_password) {
  password_ = new_password;
  UpdateFormat();
}
void PostgreSQLCStr::SetHost(std::string_view new_host) {
  host_ = new_host;
  UpdateFormat();
}
void PostgreSQLCStr::SetPort(unsigned int new_port) {
  port_ = new_port;
  UpdateFormat();
}
void PostgreSQLCStr::SetDbname(std::string_view new_dbname) {
  dbname_ = new_dbname;
  UpdateFormat();
}
void PostgreSQLCStr::SetUser(std::string_view new_user) {
  user_ = std::forward<std::string_view>(new_user);
  UpdateFormat();
}
const std::string &PostgreSQLCStr::GetUser() const {
  return user_;
}
const std::string &PostgreSQLCStr::GetPassword() const {
  return password_;
}
const std::string &PostgreSQLCStr::GetHost() const {
  return host_;
}
const std::string &PostgreSQLCStr::GetDbname() const {
  return dbname_;
}
unsigned int PostgreSQLCStr::GetPort() const {
  return port_;
}
void PostgreSQLCStr::UpdateFormat() {
  formatted_string_ = fmt::format("postgresql://{}:{}@{}:{}/{}",
                                  user_.c_str(), password_.c_str(), host_.c_str(), port_, dbname_.c_str());
}
std::string PostgreSQLCStr::GetVerboseName() const {
  return fmt::format("{}:{} db:{}", host_, port_, dbname_);
}


AbstractConnectionString::operator std::string() {
  return formatted_string_;
}
AbstractConnectionString::operator std::string_view() {
  return formatted_string_;
}
std::string AbstractConnectionString::GetVerboseName() const {
  return "";
}
const char *AbstractConnectionString::CStr() const {
  return formatted_string_.c_str();
}
bool AbstractConnectionString::operator==(const AbstractConnectionString &rhs) const {
  return formatted_string_==rhs.formatted_string_;
}
}