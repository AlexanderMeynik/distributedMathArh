#pragma once

#include <string_view>
#include <string>

#include "common/errorHandling.h"

/// Namespace for network related types
namespace network_types {

/**
 * @brief Replaces all url delimiters with spaces for scanf parsing
 * @details Since scn::scan parses strings as words, there exist a need to manually
 * @details separate them with whitespaces [see](https://www.scnlib.dev/group__format-string.html#type-string)
 * @param url
 * @see  PostgreSQLCStr::FromString
 * @see  AMQPSQLCStr::FromString
 */
std::string UrlDecode(std::string_view url);

/// us it for verbose names
/**
 * @brief Default interfacer for the connection strings
 */
class AbstractConnectionString {
 public:

  AbstractConnectionString() = default;

  /**
   * @brief Parses fields from std::string_view
   * @param s
   */
  virtual void FromString(std::string_view s)=0;

  /**
   * @brief explicit std::string type cast
   *//*
  explicit operator std::string();*/
   /**
    * @brief Cast to std::string
    */
   std::string to_string() const;

  /**
   * @brief implicit std::string_view cast
   */
  operator std::string_view();

  /**
   * @brief Retrieves cString contents for verbose logging
   */
  virtual std::string GetVerboseName() const;

  /**
   * @brief Retrieves const char * representation
   */
  const char *CStr() const;

  bool operator==(const AbstractConnectionString &rhs) const;
 protected:
  virtual void UpdateFormat() = 0;

  std::string formatted_string_; ///< formatted string for c string
};

/**
* @brief  Structure to store and format PostgreSQL connection string
*/
class PostgreSQLCStr : public AbstractConnectionString {
 public:
  PostgreSQLCStr();

  void FromString(std::string_view s) override;

  PostgreSQLCStr(std::string_view user,
                 std::string_view password,
                 std::string_view host,
                 std::string_view dbname, unsigned port);

  void SetUser(std::string_view new_user);

  void SetPassword(std::string_view new_password);

  void SetHost(std::string_view new_host);

  void SetPort(unsigned new_port);

  void SetDbname(std::string_view new_dbname);

  [[nodiscard]] std::string_view GetUser() const;

  [[nodiscard]] std::string_view GetPassword() const;

  [[nodiscard]] std::string_view GetHost() const;

  [[nodiscard]] std::string_view GetDbname() const;

  [[nodiscard]] unsigned int GetPort() const;

  [[nodiscard]] std::string GetVerboseName() const override;

 protected:
  void UpdateFormat() override;

  std::string user_, password_, host_, dbname_;
  unsigned port_;
};

/**
* @brief  Structure to store and format AMQP connection string
*/
class AMQPSQLCStr : public AbstractConnectionString {
 public:
  AMQPSQLCStr();

  AMQPSQLCStr(std::string_view host_port,
              std::string_view user,
              std::string_view password,
              bool secure = false);

  void FromString(std::string_view s) override;

  [[nodiscard]] std::string GetVerboseName() const override;

  std::string_view GetHostPort() const;

  void SetHostPort(std::string_view host_port);

  std::string_view GetUser() const;

  void SetUser(std::string_view user);

  std::string_view GetPassword() const;

  void SetPassword(std::string_view password);

  bool IsSecure() const;

  void SetSecure(bool secure);

 protected:
  void UpdateFormat() override;

 private:
  std::string host_port_, user_, password_;
  bool secure_;
};

}