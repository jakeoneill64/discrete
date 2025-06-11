CREATE TABLE IF NOT EXISTS config (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            key TEXT NOT NULL,
            value TEXT NOT NULL
);

INSERT INTO config (key, value)
VALUES
('client.logger.name', 'discrete engine'),
('client.window.name', 'discrete engine'),
('client.window.initial_width', '800'),
('client.window.initial_height', '600')

CREATE UNIQUE INDEX key_index ON config (key);