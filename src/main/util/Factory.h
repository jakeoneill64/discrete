#ifndef DISCRETE_FACTORY_H
#define DISCRETE_FACTORY_H

#include <string>
#include <unordered_map>
#include <functional>
#include <memory>

template <typename Derived, typename Base>
concept IsDerivedFrom = std::is_base_of_v<Base, Derived>;

template <typename Ancestor>
class Factory{
    public:

//        template <typename Descendant>
//        requires IsDerivedFrom<Descendant, Ancestor>
        inline void registerType(
                const std::string& name,
                std::function<std::unique_ptr<Ancestor>()> factoryFunction
        ){
            m_factoriesByTypename[name] = std::move(factoryFunction);
        }

        inline std::unique_ptr<Ancestor> create(const std::string& type){
            return m_factoriesByTypename[type]();
        }

    private:
        std::unordered_map<std::string, std::function<std::unique_ptr<Ancestor>()>> m_factoriesByTypename;
};



#endif //DISCRETE_FACTORY_H
