CREATE TABLE IF NOT EXISTS `input` (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            context TEXT NOT NULL,  -- context, actions could be separated into another table.
            glfw_macro TEXT NOT NULL,
            display_name TEXT NOT NULL,
);