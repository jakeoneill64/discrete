CREATE TABLE IF NOT EXISTS `action` (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            name TEXT NOT NULL
);

INSERT INTO `action` (name) VALUES
('EntityMoveForward'),
('EntityMoveBackward'),
('EntityMoveLeft'),
('EntityMoveRight'),
('EntityLook')