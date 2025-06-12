#include "network_shared/connectionString.h"

namespace network_types {

std::string UrlDecode(std::string_view url) {
  constexpr const char *kDelimiters = "/@:?&=#";
  std::string result(url);
  std::replace_if(result.begin(), result.end(),
                  [](char c) {
                    return std::find(kDelimiters, kDelimiters + strlen(kDelimiters), c)
                        != kDelimiters + strlen(kDelimiters);
                  },
                  ' ');
  return result;
}

PostgreSQLCStr::PostgreSQLCStr() : port_(5432) {}

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
std::string_view PostgreSQLCStr::GetUser() const {
  return user_;
}
std::string_view PostgreSQLCStr::GetPassword() const {
  return password_;
}
std::string_view PostgreSQLCStr::GetHost() const {
  return host_;
}
std::string_view PostgreSQLCStr::GetDbname() const {
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


void PostgreSQLCStr::FromString(std::string_view s) {
  auto tt= UrlDecode(s);
  auto res=scn::scan
      <std::string, std::string,std::string,uint,std::string>
      (tt, "postgresql   {} {} {} {} {}");

  if(!res.has_value()) {
    throw shared::ScanningError(res.error().code(),res.error().msg());
  }
  std::tie(user_, password_, host_, port_, dbname_) =
      res->values();

  UpdateFormat();
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
std::string AbstractConnectionString::to_string() const {
  return formatted_string_;
}

AMQPSQLCStr::AMQPSQLCStr() :secure_(false){}

AMQPSQLCStr::AMQPSQLCStr(std::string_view host_port,
                         std::string_view user,
                         std::string_view password,
                         bool secure)
    :
    host_port_(host_port),
    user_(user),
    password_(password),
    secure_(secure) {
  UpdateFormat();
}
std::string_view AMQPSQLCStr::GetHostPort() const {
  return host_port_;
}
void AMQPSQLCStr::SetHostPort(std::string_view host_port) {
  host_port_ = host_port;
  UpdateFormat();
}
std::string_view AMQPSQLCStr::GetUser() const {
  return user_;
}
void AMQPSQLCStr::SetUser(std::string_view user) {
  user_ = user;
  UpdateFormat();
}
std::string_view AMQPSQLCStr::GetPassword() const {
  return password_;
}
void AMQPSQLCStr::SetPassword(std::string_view password) {
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
void AMQPSQLCStr::UpdateFormat() {
  formatted_string_ = fmt::format("amqp{}://{}:{}@{}/", (secure_ ? "s" : ""), user_, password_, host_port_);
}
std::string AMQPSQLCStr::GetVerboseName() const {
  return fmt::format("{} secure:{}", host_port_, std::to_string(secure_));
}

void AMQPSQLCStr::FromString(std::string_view s) {

  std::string c,port;
  auto tt= UrlDecode(s);
  auto res=scn::scan<std::string, std::string,std::string,std::string,std::string>(tt, "{}   {} {} {} {} ");
  if(!res.has_value()) {
    throw shared::ScanningError(res.error().code(),res.error().msg());
  }
  std::tie(c,user_, password_,host_port_,port) =res->values();
  host_port_.append(":").append(port);
  secure_=(c.back()=='s');

  UpdateFormat();

}

}