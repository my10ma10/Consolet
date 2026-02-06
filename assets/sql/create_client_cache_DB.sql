CREATE TABLE IF NOT EXISTS Messages (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    sender_id INTEGER NOT NULL,
    sender_name TEXT,
    chat_id INTEGER NOT NULL,
    date_time TEXT NOT NULL DEFAULT (datetime('now')),
    text TEXT NOT NULL,
);

CREATE INDEX IF NOT EXISTS idx_chat_time 
    ON messages(chat_id, timestamp DESC);