#include "InputRepository.h"
#include "engine/action/registration.h"

InputRepository::InputRepository(
    std::shared_ptr<EventManager> eventManager,
    std::shared_ptr<sqlite3> database
) : m_database{std::move(database)}, m_eventManager{std::move(eventManager)}{
    registerActions(m_actionFactory);
}

std::unique_ptr<Action<>> InputRepository::operator[](const std::string &inputKey) {
    return m_actionFactory.create(m_inputMappings[inputKey]);
}
