#pragma once

#include <string_view>
#include <string>

/// Namespace for network related types
namespace network_types {
///@todo create common interface for c string
/// use it in queue
/// us it for verbose names

/**
 * @brief Default interfacer for the connection strings
 */
class AbstractConnectionString
{
 public:

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
  std::string formatted_string_; ///< formatted string for c string
};

/**
* @brief  Structure to store and format PostgreSQL connection string
*/
class PostgreSQLCStr: public AbstractConnectionString{
 public:
  PostgreSQLCStr() : port_(5432) {}

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


 private:
  void UpdateFormat();

  std::string user_, password_, host_, dbname_;
  unsigned port_;

};
}