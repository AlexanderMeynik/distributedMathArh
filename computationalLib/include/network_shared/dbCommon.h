#pragma once

#include <pqxx/pqxx>
#include "network_shared/networkTypes.h"

/// Namespace that contains database related utils
namespace db_common {

using network_types::myConnString;

using ConnPtr = std::shared_ptr<pqxx::connection>;
using TransactionT = pqxx::transaction<pqxx::isolation_level::read_committed>;
using NonTransType = pqxx::nontransaction;
using ResType = pqxx::result;
using Subtransction = pqxx::subtransaction;

using IndexType=int64_t;
using shared::SQL_ERROR;
using shared::Already_Connected;
using shared::Broken_Connection;
static const char *const SampleTempDb = "template1";


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
ConnPtr TryConnect(const myConnString& conn_str,
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
void ExecuteTransaction(ConnPtr& ptr,
                        const std::function<void(TransactionT &)> &func,
                        std::string_view service_name,
                        const myConnString & conn_str);

/**
 * @brief Executes function inside subtransaction of txn
 * @param txn
 * @param func
 * @param sub_name - name for subtransaction
 * @throws SQL_ERROR if any sql syntax or semantics issue arises
 * @throws MyException if any other error happens
 */
void ExecuteSubTransaction(TransactionT &txn,
                           const std::function<void(Subtransction &)> &func,
                           std::string_view sub_name="");

}