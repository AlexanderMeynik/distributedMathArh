#pragma once

#include <pqxx/pqxx>
#include "common/Printers.h"
#include "common/Parsers.h"
#include "parallelUtils/timingUtils.h"


/// Namespace that contains database related utils
namespace db_common {
using namespace enum_utils;
using namespace timing;

using ConnPtr = std::shared_ptr<pqxx::connection>;
using TransactionT = pqxx::transaction<pqxx::isolation_level::read_committed>;
using NonTransType = pqxx::nontransaction;
using ResType = pqxx::result;
using Subtransaction = pqxx::subtransaction;

using IndexType = uint64_t;
using shared::SQL_ERROR;
using shared::Already_Connected;
using shared::Broken_Connection;
static const char *const SampleTempDb = "template1";


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

/**
 * @brief Function to retrieve optional value
 * @tparam T
 * @param a
 * @return T optional
 */
template<typename T>
std::optional<T> inline GetOpt(pqxx::field&&a)
{
  return (a.is_null())?std::nullopt:std::optional<T>{a.as<T>()};
}

/**
 * @brief Function to retrieve optional value while using func to cast it to T
 * @tparam T
 * @tparam func
 * @param a
 * @return T optinal
 */
template<typename T,std::optional<T>(*func)(std::string_view)>
std::optional<T> inline GetFunctionOpt(pqxx::field&&a)
{
  return (a.is_null())?std::nullopt:std::optional<T>{*func(a.as<std::string>())};
}


/**
 * @brief Parses specified continuous struct from ResType
 * @note Requires RetType to be constructible from pqxx::field
 * @tparam VecT
 * @tparam RetType
 * @param result
 * @return VecT<RetType>
 */
template<template<typename ...> typename VecT ,typename RetType>
VecT<RetType> ParseArray(const ResType&result);

/**
 * @brief formats request to add pagination
 * @param initial_request
 * @param page_num
 * @param page_size
 * @return
 */
std::string PaginateRequest(std::string_view initial_request,
                            IndexType page_num,
                            IndexType page_size);



/**
 * @brief Checks connection
 * @param conn_ptr
 * @return connection status
 */
bool CheckConnection(const ConnPtr &conn_ptr);

/**
 * @brief Closes connection and reset conn_ptr
 * @param conn_ptr
 */
void Disconnect(ConnPtr &conn_ptr);

/**
 * @brief Attempt to connect to PostgrSQL with conn_str
 * @param conn_str
 * @param service_name
 * @return connection pointer
 * @throws Broken_Connection - if connection cannot be established
 */
ConnPtr TryConnect(const myConnString &conn_str,
                   const std::string service_name);

/**
 * @brief Checks db_name database existence
 * @param non_trans
 * @param db_name
 * @return Number of databases with name db_name
 */
size_t CheckDatabaseExistence(NonTransType &non_trans,
                              std::string_view db_name);

/**
 * @brief terminates all connections for the specified database
 * @param no_trans_exec
 * @param db_name
 * @return ResType for the specified terminate requests
 */
ResType
TerminateAllDbConnections(NonTransType &no_trans_exec,
                          std::string_view db_name);

/**
 * @brief Creates database with db_name
 * @param db_name
 * @param c_string
 * @return connection string pointer for the created database
 */
ConnPtr CreateDatabase(myConnString c_string, std::string_view db_name);

/**
 * @brief Drops specified database
 * @details Will call TerminateAllDbConnections to end all other connections
 * @param db_name
 * @param c_string
 */
void DropDatabase(myConnString c_string, std::string_view db_name);

/**
 * @brief Executes the script ot inidb
 * @param c_string
 * @param script
 */
void FillDatabase(myConnString c_string, std::string_view script);

/**
 * @brief Executes function in the form of transaction
 * @details Creates new transaction to execute provided function
 * @param ptr
 * @param func
 * @param service_name
 * @param conn_str
 * @throws SQL_ERROR if any sql syntax or semantics issue arises
 * @throws MyException if any other error happens
 */
ResType ExecuteTransaction(ConnPtr &ptr,
                           const std::function<ResType(TransactionT &)> &func,
                           std::string_view service_name,
                           const myConnString &conn_str);

/**
 * @brief Executes function inside subtransaction of txn
 * @param txn
 * @param func
 * @param sub_name - name for subtransaction
 * @throws SQL_ERROR if any sql syntax or semantics issue arises
 * @throws MyException if any other error happens
 */
ResType ExecuteSubTransaction(TransactionT &txn,
                              const std::function<ResType(Subtransaction &)> &func,
                              std::string_view sub_name = "");

}


namespace db_common
{
template<template<typename ...> typename VecT ,typename RetType>
VecT<RetType> ParseArray(const ResType&result)
{
  VecT<RetType> res;
  res.reserve(result.size());
  for (auto row : result) {
    res.emplace_back(row);
  }
  return res;
}
}