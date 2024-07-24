#include "db_connector.h"
#include <iostream>
#include <memory>
#include <ctime>

DBConnector::DBConnector(const std::string& host, const std::string& user, const std::string& password, const std::string& database)
    : host(host), user(user), password(password), database(database), driver(nullptr), con(nullptr) {
}

DBConnector::~DBConnector() {
    delete con;
}

void DBConnector::connect() {
    try {
        driver = get_driver_instance();
        con = driver->connect(host, user, password);
        con->setSchema(database);
    } catch (sql::SQLException &e) {
        std::cerr << "SQL Error: " << e.what() << std::endl;
        // Handle exception
    }
}

void DBConnector::insertSudoku(const std::string& puzzle, const std::string& solution) {
    try {
        // Calculate tomorrow's date
        std::time_t t = std::time(nullptr); // Get current time
        std::tm *tm = std::localtime(&t);
        tm->tm_mday += 1; // Add one day
        mktime(tm); // Normalize the tm structure

        char dateStr[11];
        strftime(dateStr, sizeof(dateStr), "%Y-%m-%d", tm);

        std::unique_ptr<sql::PreparedStatement> prep_stmt(
            con->prepareStatement("INSERT INTO daily_sudoku (puzzle, date, solution) VALUES (?, ?, ?)")
        );
        prep_stmt->setString(1, puzzle);
        prep_stmt->setString(2, dateStr);
        prep_stmt->setString(3, solution);
        prep_stmt->execute();
    } catch (sql::SQLException &e) {
        std::cerr << "SQL Error: " << e.what() << std::endl;
        // Handle exception
    }
}
