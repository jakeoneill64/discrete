#include "InputRepository.h"
#include "engine/action/registration.h"
#include "persistence/database.h"
#include <ranges>

InputRepository::InputRepository(
    std::shared_ptr<EventManager> eventManager,
    std::shared_ptr<sqlite3> database
) :
    m_database{std::move(database)},
    m_eventManager{std::move(eventManager)}
{
    registerActions(m_actionFactory);
    const std::string preparedStatement
    {R"(
        SELECT input.id, input.context, action.name AS action, input.key
        FROM `input`
        INNER JOIN action ON input.action_id = action.id;
    )"};

    const auto records{
        executeQuery<
                sqlite3_int64,
                std::string,
                std::string,
                std::string
        >
        (m_database.get(), preparedStatement)
    };

    std::ranges::for_each(
        records.value(),
        [&]
        (const auto& record){
            const auto& [id, context, action, key] = record;
                m_inputMappings[{key, context}] = key;
                m_inputKeysById[id] = {key, context};
        }
    );

    m_eventManager->subscribeEvent<InputUpdate>(
        [&]
        (InputUpdate updateEvent){
            std::unique_lock<std::shared_mutex> writeLock{m_mutex};
            const auto& [operation, rowId] = updateEvent;
            switch (operation) {
                case SQLITE_DELETE:

                    m_inputMappings.erase(m_inputKeysById[rowId]);
                    m_inputKeysById.erase(rowId);
                    break;

                case SQLITE_INSERT:
                case SQLITE_UPDATE: {

                        const std::string preparedStatement
                            {R"(
                                 SELECT input.id, input.context, action.name AS action, input.key
                                 FROM `input`
                                 INNER JOIN action ON input.action_id = action.id
                                 LIMIT 1
                            )"};

                        const auto record{
                            executeQuery<
                                 sqlite3_int64,
                                 std::string,
                                 std::string,
                                 std::string
                            >
                            (m_database.get(), preparedStatement)
                        };

                        const auto &[id, context, action, inputKey]
                            = record.value()[0];

                            m_inputMappings[{inputKey, context}] = action;
                            m_inputKeysById[rowId] = {inputKey, context};
                        }
                    break;
                default:
                    break;
            }
        }
    );
}

std::unique_ptr<Action<>> InputRepository::operator[](const std::string &inputKey, const std::string &context) {
    return m_actionFactory.create(m_inputMappings[{inputKey, context}]);
}

InputRepository::~InputRepository(){
 //TODO
}
