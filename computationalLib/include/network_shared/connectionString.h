#pragma once

#include <string_view>
#include <string>
#include "common/errorHandling.h"


/// Namespace for network related types
namespace network_types {


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
   */
  explicit operator std::string();

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

  [[nodiscard]] const std::string &GetUser() const;

  [[nodiscard]] const std::string &GetPassword() const;

  [[nodiscard]] const std::string &GetHost() const;

  [[nodiscard]] const std::string &GetDbname() const;

  [[nodiscard]] unsigned int GetPort() const;

  [[nodiscard]] std::string GetVerboseName() const;

 protected:
  void UpdateFormat();

  std::string user_, password_, host_, dbname_;
  unsigned port_;
};

/**
* @brief  Structure to store and format AMQP connection string
*/
class AMQPSQLCStr : public AbstractConnectionString {
 public:
  AMQPSQLCStr()=default;

  AMQPSQLCStr(const std::string &host_port,
              const std::string &user,
              const std::string &password,
              bool secure);

  void FromString(std::string_view s) override;

  [[nodiscard]] std::string GetVerboseName() const;

  const std::string &GetHostPort() const;

  void SetHostPort(const std::string &host_port);

  const std::string &GetUser() const;

  void SetUser(const std::string &user);

  const std::string &GetPassword() const;

  void SetPassword(const std::string &password);

  bool IsSecure() const;

  void SetSecure(bool secure);

 protected:
  void UpdateFormat();

 private:
  std::string host_port_, user_, password_;
  bool secure_;
};

}