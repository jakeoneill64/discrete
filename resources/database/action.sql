CREATE TABLE IF NOT EXISTS `action` (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            action TEXT NOT NULL,  -- context, actions could be separated into another table.
);