CREATE TABLE IF NOT EXISTS config (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            key TEXT NOT NULL,
            value TEXT NOT NULL
);

CREATE UNIQUE INDEX IF NOT EXISTS key_index ON config (key);