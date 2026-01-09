#include <mysql/mysql.h>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <sstream>
#include <string>

struct DBConfig {
    const char* host = std::getenv("TODO_DB_HOST") ? std::getenv("TODO_DB_HOST") : "127.0.0.1";
    const char* user = std::getenv("TODO_DB_USER") ? std::getenv("TODO_DB_USER") : "root";
    const char* pass = std::getenv("TODO_DB_PASS") ? std::getenv("TODO_DB_PASS") : "";
    const char* dbname = std::getenv("TODO_DB_NAME") ? std::getenv("TODO_DB_NAME") : "todo_cli_db";
    unsigned int port = std::getenv("TODO_DB_PORT") ? static_cast<unsigned int>(std::atoi(std::getenv("TODO_DB_PORT"))) : 3306;
};

MYSQL* connect_db(const DBConfig& cfg) {
    MYSQL* conn = mysql_init(nullptr);
    if (!conn) return nullptr;
    if (!mysql_real_connect(conn, cfg.host, cfg.user, cfg.pass, nullptr, cfg.port, nullptr, 0)) {
        mysql_close(conn);
        return nullptr;
    }
    // Ensure the database exists
    std::string create_db = std::string("CREATE DATABASE IF NOT EXISTS ") + cfg.dbname + ";";
    if (mysql_query(conn, create_db.c_str())) {
        mysql_close(conn);
        return nullptr;
    }
    if (mysql_select_db(conn, cfg.dbname)) {
        mysql_close(conn);
        return nullptr;
    }
    return conn;
}

std::string escape_str(MYSQL* conn, const std::string& s) {
    std::string out;
    out.resize(s.size()*2 + 1);
    unsigned long len = mysql_real_escape_string(conn, &out[0], s.c_str(), (unsigned long)s.size());
    out.resize(len);
    return out;
}

bool ensure_schema(MYSQL* conn) {
    const char* q =
        "CREATE TABLE IF NOT EXISTS todos ("
        "id INT AUTO_INCREMENT PRIMARY KEY,"
        "description TEXT NOT NULL,"
        "done TINYINT(1) NOT NULL DEFAULT 0,"
        "created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP"
        ");";
    return mysql_query(conn, q) == 0;
}

int cmd_add(MYSQL* conn, const std::string& desc) {
    std::string esc = escape_str(conn, desc);
    std::string q = "INSERT INTO todos (description) VALUES ('" + esc + "')";
    if (mysql_query(conn, q.c_str())) {
        std::cerr << "Insert failed: " << mysql_error(conn) << '\n';
        return 1;
    }
    std::cout << "Added task id=" << mysql_insert_id(conn) << '\n';
    return 0;
}

int cmd_list(MYSQL* conn) {
    const char* q = "SELECT id, description, done, created_at FROM todos ORDER BY id ASC";
    if (mysql_query(conn, q)) {
        std::cerr << "Select failed: " << mysql_error(conn) << '\n';
        return 1;
    }
    MYSQL_RES* res = mysql_store_result(conn);
    if (!res) {
        std::cerr << "No results: " << mysql_error(conn) << '\n';
        return 1;
    }
    MYSQL_ROW row;
    while ((row = mysql_fetch_row(res))) {
        unsigned long* lens = mysql_fetch_lengths(res);
        std::string id = row[0] ? row[0] : "";
        std::string desc = row[1] ? row[1] : "";
        std::string done = row[2] ? row[2] : "0";
        std::string created = row[3] ? row[3] : "";
        std::cout << id << " [" << (done == "1" ? "x" : " ") << "] " << desc << " (" << created << ")\n";
    }
    mysql_free_result(res);
    return 0;
}

int cmd_done(MYSQL* conn, const std::string& id) {
    std::string q = "UPDATE todos SET done=1 WHERE id=" + id;
    if (mysql_query(conn, q.c_str())) {
        std::cerr << "Update failed: " << mysql_error(conn) << '\n';
        return 1;
    }
    if (mysql_affected_rows(conn) == 0) {
        std::cerr << "No task with id=" << id << '\n';
        return 2;
    }
    std::cout << "Marked done: " << id << '\n';
    return 0;
}

int cmd_remove(MYSQL* conn, const std::string& id) {
    std::string q = "DELETE FROM todos WHERE id=" + id;
    if (mysql_query(conn, q.c_str())) {
        std::cerr << "Delete failed: " << mysql_error(conn) << '\n';
        return 1;
    }
    if (mysql_affected_rows(conn) == 0) {
        std::cerr << "No task with id=" << id << '\n';
        return 2;
    }
    std::cout << "Removed: " << id << '\n';
    return 0;
}

void usage(const char* prog) {
    std::cout << "Usage:\n";
    std::cout << "  " << prog << " add \"task description\"\n";
    std::cout << "  " << prog << " list\n";
    std::cout << "  " << prog << " done <id>\n";
    std::cout << "  " << prog << " remove <id>\n";
    std::cout << "Environment variables for DB connection:\n";
    std::cout << "  TODO_DB_HOST, TODO_DB_USER, TODO_DB_PASS, TODO_DB_NAME, TODO_DB_PORT\n";
}

int main(int argc, char** argv) {
    if (argc < 2) { usage(argv[0]); return 1; }
    DBConfig cfg;
    MYSQL* conn = connect_db(cfg);
    if (!conn) {
        std::cerr << "Failed to connect to MySQL. Ensure service is running and credentials are correct.\n";
        return 1;
    }
    if (!ensure_schema(conn)) {
        std::cerr << "Failed to ensure schema: " << mysql_error(conn) << '\n';
        mysql_close(conn);
        return 1;
    }

    std::string cmd = argv[1];
    int rc = 0;
    if (cmd == "add") {
        if (argc < 3) { usage(argv[0]); rc = 1; }
        else rc = cmd_add(conn, argv[2]);
    } else if (cmd == "list") {
        rc = cmd_list(conn);
    } else if (cmd == "done") {
        if (argc < 3) { usage(argv[0]); rc = 1; }
        else rc = cmd_done(conn, argv[2]);
    } else if (cmd == "remove") {
        if (argc < 3) { usage(argv[0]); rc = 1; }
        else rc = cmd_remove(conn, argv[2]);
    } else {
        usage(argv[0]); rc = 1;
    }

    mysql_close(conn);
    return rc;
}
