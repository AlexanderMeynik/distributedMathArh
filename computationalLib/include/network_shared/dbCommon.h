#pragma once

#include <pqxx/pqxx>
#include "network_shared/networkTypes.h"
#include "common/Printers.h"
#include "common/Parsers.h"

/// Namespace that contains database related utils
namespace db_common {

using network_types::myConnString;

using ConnPtr = std::shared_ptr<pqxx::connection>;
using TransactionT = pqxx::transaction<pqxx::isolation_level::read_committed>;
using NonTransType = pqxx::nontransaction;
using ResType = pqxx::result;
using Subtransaction = pqxx::subtransaction;

using IndexType = int64_t;
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


static const auto steady_to_sys = []{
  using namespace std::chrono;
  auto sys = system_clock::now().time_since_epoch();
  auto st  = steady_clock::now().time_since_epoch();
  return duration_cast<system_clock::duration>(sys)
      - duration_cast<system_clock::duration>(st);
}();

std::string inline myTimeStampToTimestamp(uint64_t steady_us) {
  using namespace std::chrono;

  microseconds ms{steady_us};

  system_clock::time_point tp{
      duration_cast<system_clock::duration>(ms) /*+ steady_to_sys*/
  };

  auto epoch_us = duration_cast<microseconds>(tp.time_since_epoch()).count();
  time_t secs = epoch_us / 1'000'000;
  int    frac = epoch_us % 1'000'000;

  std::tm tm{};
  gmtime_r(&secs, &tm);

  std::ostringstream os;
  os << std::put_time(&tm, "%Y-%m-%d %H:%M:%S")
     << '.' << std::setw(6) << std::setfill('0') << frac;
  return os.str();
}

std::optional<uint64_t> inline fromTimestamp(std::string_view ts) {
  std::tm tm{};
  int micros = 0;

  auto dot = ts.find('.');

  if (std::sscanf(ts.data(),
                  "%d-%d-%d %d:%d:%d.%d",
                  &tm.tm_year, &tm.tm_mon,  &tm.tm_mday,
                  &tm.tm_hour, &tm.tm_min,  &tm.tm_sec,
                  &micros) != 7)
    return std::nullopt;

  std::string_view frac_part = ts.substr(dot+1);
  micros*=std::pow(10,6-frac_part.size());

  tm.tm_year -= 1900;
  tm.tm_mon  -= 1;

  time_t secs = timegm(&tm);
  uint64_t total_us = uint64_t(secs) * 1'000'000 + micros;

  using namespace std::chrono;
  auto sys_dur = microseconds(total_us);
  //todo test scenario when stedy clock time is used to be inserted intodb
 /* auto st_dur  = duration_cast<steady_clock::duration>(
      sys_dur - steady_to_sys
  );*/
  return uint64_t(duration_cast<microseconds>(/*st_dur*/sys_dur).count());
}


struct User {
  IndexType user_id;
  std::string login;
  std::string hashed_password;
  UserRole role;
  IndexType created_at;
  std::optional<IndexType> last_login;

  User()=default;

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