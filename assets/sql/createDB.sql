CREATE TABLE IF NOT EXISTS User (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    name TEXT NOT NULL UNIQUE,
    password TEXT NOT NULL
);

CREATE TABLE IF NOT EXISTS Chat (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    name TEXT,
    type TEXT NOT NULL CHECK(type IN ('personal', 'group')),
    CHECK(
        (type = 'personal' AND name IS NULL) OR 
        (type = 'group' AND name IS NOT NULL))
);

CREATE UNIQUE INDEX IF NOT EXISTS idx_chat_group_name_unique
    ON Chat(name) WHERE type = 'group';

CREATE TABLE IF NOT EXISTS MessagesHistory (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    sender_id INTEGER NOT NULL,
    chat_id INTEGER NOT NULL,
    date_time TEXT NOT NULL DEFAULT (datetime('now')),
    text TEXT NOT NULL,
    is_read INTEGER NOT NULL DEFAULT 0,
    FOREIGN KEY (sender_id) REFERENCES User(id),
    FOREIGN KEY (chat_id) REFERENCES Chat(id) ON DELETE CASCADE
);

CREATE TABLE IF NOT EXISTS ChatMembers (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    chat_id INTEGER NOT NULL,
    user_id INTEGER NOT NULL,
    FOREIGN KEY (chat_id) REFERENCES Chat(id) ON DELETE CASCADE,
    FOREIGN KEY (user_id) REFERENCES User(id)
);