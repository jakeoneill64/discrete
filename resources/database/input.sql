PRAGMA foreign_keys = ON;

CREATE TABLE IF NOT EXISTS `input` (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            context TEXT NOT NULL,
            action_id INTEGER NOT NULL,
            event text NOT NULL,
            FOREIGN KEY (action_id) REFERENCES action(id) ON DELETE CASCADE
);

INSERT INTO `input` (context, action_id, event) VALUES
("default", (SELECT id FROM action WHERE name="EntityMoveForward"), "KEY_W"),
("default", (SELECT id FROM action WHERE name="EntityMoveBackward"), "KEY_S"),
("default", (SELECT id FROM action WHERE name="EntityMoveLeft"), "KEY_A"),
("default", (SELECT id FROM action WHERE name="EntityMoveRight"), "KEY_D");