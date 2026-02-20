CREATE TABLE IF NOT EXISTS User (
    id INTEGER PRIMARY KEY,
    name TEXT NOT NULL UNIQUE,
    password TEXT NOT NULL, 
    cache_time INTEGER DEFAULT (strftime('%s', 'now'))
);

CREATE TABLE IF NOT EXISTS Chat (
    id INTEGER PRIMARY KEY,
    name TEXT,
    type TEXT NOT NULL CHECK(type IN ('personal', 'group')),
    CHECK(
        (type = 'personal' AND name IS NULL) OR 
        (type = 'group' AND name IS NOT NULL)),
    cache_time INTEGER DEFAULT (strftime('%s', 'now'))
);

CREATE TABLE IF NOT EXISTS MessagesHistory (
    id INTEGER PRIMARY KEY,
    sender_id INTEGER NOT NULL,
    chat_id INTEGER NOT NULL,
    date_time INTEGER NOT NULL,
    text TEXT NOT NULL,
    FOREIGN KEY (sender_id) REFERENCES User(id),
    FOREIGN KEY (chat_id) REFERENCES Chat(id)
);

CREATE TABLE IF NOT EXISTS ChatMembers (
    chat_id INTEGER NOT NULL,
    user_id INTEGER NOT NULL,
    FOREIGN KEY (chat_id) REFERENCES Chat(id),
    FOREIGN KEY (user_id) REFERENCES User(id)
);