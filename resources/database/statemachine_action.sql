CREATE TABLE IF NOT EXISTS `statemachine_action` (
    statemachine_id INTEGER NOT NULL,
    action_id INTEGER NOT NULL,
    PRIMARY KEY (statemachine_id, action_id),
    FOREIGN KEY (statemachine_id) REFERENCES statemachine(id) ON DELETE CASCADE,
    FOREIGN KEY (action_id) REFERENCES action(id) ON DELETE CASCADE
);
