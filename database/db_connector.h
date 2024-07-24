#ifndef DB_CONNECTOR_H
#define DB_CONNECTOR_H

#include <string>
#include <cppconn/driver.h>
#include <cppconn/exception.h>
#include <cppconn/resultset.h>
#include <cppconn/statement.h>
#include <cppconn/prepared_statement.h>

class DBConnector {
public:
    DBConnector(const std::string& host, const std::string& user, const std::string& password, const std::string& database);
    ~DBConnector();

    void connect();
    void insertSudoku(const std::string& puzzle, const std::string& solution);
    // ... other DB related methods

private:
    std::string host;
    std::string user;
    std::string password;
    std::string database;

    sql::Driver* driver;
    sql::Connection* con;
};

#endif
