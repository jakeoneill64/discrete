CREATE TABLE IF NOT EXISTS config (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            key TEXT NOT NULL,
            value TEXT NOT NULL
);

CREATE UNIQUE INDEX key_index ON config (key);

INSERT INTO config (key, value)
VALUES
('client.logger.name', 'discrete engine'),
('client.window.name', 'discrete engine'),
('client.render.gl_version_major', '3'),
('client.render.gl_version_minor', '3')