CREATE TABLE IF NOT EXISTS `action` (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            action TEXT NOT NULL
);

INSERT INTO `action` (action) VALUES
('EntityMove')