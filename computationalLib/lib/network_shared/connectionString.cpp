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
                                  user_, password_, host_, port_, dbname_);
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
  return formatted_string_ == rhs.formatted_string_;
}

void AMQPSQLCStr::UpdateFormat() {
  formatted_string_ = fmt::format("amqp{}://{}:{}@{}/", (secure_ ? "s" : ""), user_, password_, host_port_);
}
std::string AMQPSQLCStr::GetVerboseName() const {
  return fmt::format("{} secure:{}", host_port_, std::to_string(secure_));
}
AMQPSQLCStr::AMQPSQLCStr(const std::string &host_port,
                         const std::string &user,
                         const std::string &password,
                         bool secure)
    :
    host_port_(host_port),
    user_(user),
    password_(password),
    secure_(secure) {
  UpdateFormat();
}
const std::string &AMQPSQLCStr::GetHostPort() const {
  return host_port_;
}
void AMQPSQLCStr::SetHostPort(const std::string &host_port) {
  host_port_ = host_port;
  UpdateFormat();
}
const std::string &AMQPSQLCStr::GetUser() const {
  return user_;
}
void AMQPSQLCStr::SetUser(const std::string &user) {
  user_ = user;
  UpdateFormat();
}
const std::string &AMQPSQLCStr::GetPassword() const {
  return password_;
}
void AMQPSQLCStr::SetPassword(const std::string &password) {
  password_ = password;
  UpdateFormat();
}
bool AMQPSQLCStr::IsSecure() const {
  return secure_;
}
void AMQPSQLCStr::SetSecure(bool secure) {
  secure_ = secure;
  UpdateFormat();
}
}