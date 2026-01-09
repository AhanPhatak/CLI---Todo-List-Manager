# CLI Todo List Manager (C++ + MySQL)

A lightweight command-line todo list manager written in **C++** with **MySQL** for persistent data storage.

## Features

✨ Simple and fast CLI interface  
💾 MySQL database backend for data persistence  
🛠️ Cross-platform (macOS, Linux, with adjustments)  
🔧 Configurable via environment variables  
📝 Full CRUD operations (Create, Read, Update, Delete)

## Project Structure

```
.
├── CMakeLists.txt           # CMake build configuration
├── src/
│   └── main.cpp             # Main C++ source with MySQL integration
├── sql/
│   └── create_db.sql        # Database and table schema
├── build/                   # Compiled binary (after building)
│   └── todo                 # Executable
├── .gitignore               # Git ignore rules
└── README.md                # This file
```

## Quick Start

### Prerequisites

- **macOS** (tested) or Linux
- **C++17 compiler** (clang, gcc)
- **CMake** 3.10+
- **MySQL** 5.7+ (client and server)

### Installation

#### 1. Install Dependencies (macOS)

```bash
# Install MySQL client and server
brew install mysql cmake

# Start MySQL server
brew services start mysql
```

#### 2. Clone & Build

```bash
cd /path/to/CLI---Todo-List-Manager
mkdir -p build && cd build
cmake ..
cmake --build .
```

The executable `todo` will be in the `build/` directory.

#### 3. Test Connection

```bash
cd build
./todo list
```

If successful, it will create the database and table automatically on first run.

## Usage

### Commands

```bash
./todo add "task description"    # Add a new task
./todo list                      # List all tasks
./todo done <id>                 # Mark task as complete
./todo remove <id>               # Delete a task
```

### Examples

```bash
# Add tasks
./todo add "Buy groceries"
./todo add "Write documentation"
./todo add "Review pull requests"

# List all tasks
./todo list
# Output:
# 1 [ ] Buy groceries (2026-01-09 10:23:45)
# 2 [ ] Write documentation (2026-01-09 10:24:12)
# 3 [ ] Review pull requests (2026-01-09 10:24:18)

# Mark task 2 as done
./todo done 2

# List again
./todo list
# Output:
# 1 [ ] Buy groceries (2026-01-09 10:23:45)
# 2 [x] Write documentation (2026-01-09 10:24:12)
# 3 [ ] Review pull requests (2026-01-09 10:24:18)

# Remove task 1
./todo remove 1

# Final list
./todo list
# Output:
# 2 [x] Write documentation (2026-01-09 10:24:12)
# 3 [ ] Review pull requests (2026-01-09 10:24:18)
```

## Configuration

Configure the MySQL connection using environment variables:

| Variable | Default | Description |
|----------|---------|-------------|
| `TODO_DB_HOST` | `127.0.0.1` | MySQL server hostname/IP |
| `TODO_DB_USER` | `root` | Database username |
| `TODO_DB_PASS` | (empty) | Database password |
| `TODO_DB_NAME` | `todo_cli_db` | Database name |
| `TODO_DB_PORT` | `3306` | MySQL port |

### Example with Custom Configuration

```bash
export TODO_DB_HOST=192.168.1.100
export TODO_DB_USER=todouser
export TODO_DB_PASS=secure_password
export TODO_DB_NAME=my_todos
export TODO_DB_PORT=3306

./todo list
```

## Database Schema

The `todos` table has the following structure:

```sql
CREATE TABLE todos (
  id INT AUTO_INCREMENT PRIMARY KEY,
  description TEXT NOT NULL,
  done TINYINT(1) NOT NULL DEFAULT 0,
  created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
);
```

## Building from Source

### Step-by-Step Build

```bash
# Navigate to project
cd CLI---Todo-List-Manager

# Create build directory
mkdir -p build
cd build

# Configure (CMake will find MySQL on macOS automatically)
cmake ..

# Build
cmake --build .

# (Optional) Install to system
cmake --install .
```

### CMake Options

The `CMakeLists.txt` automatically detects MySQL on macOS via Homebrew. For other systems, you may need to specify paths:

```bash
cmake .. -DMYSQL_PREFIX=/opt/mysql
```

## Troubleshooting

### MySQL Connection Error
```
Failed to connect to MySQL. Ensure service is running and credentials are correct.
```

**Solution:**
```bash
# Check if MySQL is running
brew services list | grep mysql

# Start MySQL if not running
brew services start mysql

# Verify connection manually
mysql -u root -e "SELECT 1"
```

### CMake Cannot Find MySQL Headers
```
Could not find mysql headers. Install MySQL client development files.
```

**Solution (macOS):**
```bash
brew install mysql-client
# Set paths manually if needed
export LDFLAGS="-L$(brew --prefix mysql-client)/lib"
export CPPFLAGS="-I$(brew --prefix mysql-client)/include"
cmake ..
```

### Permission Denied / "no such file or directory"

Make sure you're in the `build/` directory:
```bash
cd /path/to/CLI---Todo-List-Manager/build
./todo list
```

## Implementation Details

- **Language:** C++17
- **Database API:** MySQL C API (`mysqlclient`)
- **Build System:** CMake 3.10+
- **Thread Safety:** Not thread-safe (single-threaded CLI)

### MySQL Escaping

User input is escaped using `mysql_real_escape_string()` to prevent SQL injection.

## Future Enhancements

- [ ] Prepared statements for better security
- [ ] Configuration file support (`.toml` / `.ini`)
- [ ] Unit tests and integration tests
- [ ] Docker support for MySQL
- [ ] SQLite as lightweight alternative backend
- [ ] Additional fields: priority, due date, categories
- [ ] Bulk operations and filtering
- [ ] Unicode support improvements

## License

This project is open source and available under the MIT License.

## Contributing

Contributions are welcome! Feel free to:
1. Fork the repository
2. Create a feature branch
3. Submit a pull request

## Support

For issues, questions, or suggestions, please open a GitHub issue.
