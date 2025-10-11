import sqlite3

# Connect to a local database file (creates it if it doesn't exist)
conn = sqlite3.connect('todo_list.db')  # This file will be saved locally

# Create a cursor to interact with the database
cursor = conn.cursor()


#Python file to basically create the base idea of the project
cursor.execute('''
CREATE TABLE IF NOT EXISTS tasks (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    description TEXT NOT NULL,
    completed BOOLEAN NOT NULL DEFAULT 0
)
''')


conn.commit()

cursor.execute('INSERT INTO tasks (description, completed) VALUES (?, ?)', ('Buy groceries', 0))
conn.commit()

cursor.execute('SELECT * FROM tasks')
rows = cursor.fetchall()
for row in rows:
    print(row)

conn.close()

