#pragma once

#include <pqxx/pqxx>
#include "network_shared/networkTypes.h"
#include "common/Printers.h"
#include "common/Parsers.h"
#include "parallelUtils/timingUtils.h"

/// Namespace that contains database related utils
namespace db_common {

using network_types::myConnString;
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

enum class UserRole {
  USER,//< default user role
  ADMINISTRATOR //< administrator user role
};
static const std::unordered_map<std::string, UserRole> kStrToUserRole
    {
        {"user", UserRole::USER},
        {"admin", UserRole::ADMINISTRATOR}
    };
const std::array<std::string, 2> kUserRoleToStr
    {
        "user",
        "admin"
    };

struct User {
  IndexType user_id;
  std::string login;
  std::string hashed_password;
  UserRole role;
  IndexType created_at;
  std::optional<IndexType> last_login;

  User() = default;

  User(pqxx::row &row);

  bool operator==(const User &rhs) const;
};

/**
 * @brief Checks connection
 * @param conn_ptr
 * @return conenction status
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
ConnPtr CreateDatabase(network_types::myConnString c_string, std::string_view db_name);

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