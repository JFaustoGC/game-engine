#ifndef ECS_H
#define ECS_H
#include <bitset>
#include <memory>
#include <set>
#include <typeindex>
#include <unordered_map>
#include <vector>

#include "../Logger/Logger.h"


constexpr unsigned int MAX_COMPONENTS = 32;

typedef std::bitset<MAX_COMPONENTS> Signature;

struct IComponent {
protected:
    static int nextId;
};

template<typename T>
class Component : public IComponent {
public:
    static int GetId()
    {
        static auto id = nextId++;
        return id;
    }
};

class Entity {
    int id;

public:
    explicit Entity(const int id) : id(id) {};

    int GetId() const;

    bool operator==(const Entity &entity) const;

    bool operator<(const Entity &entity) const;

    template<typename TComponent, typename... TArgs>
    void AddComponent(TArgs &&... args);

    template<typename TComponent>
    void RemoveComponent() const;

    template<typename TComponent>
    bool HasComponent() const;

    template<typename TComponent>
    TComponent &GetComponent() const;

    mutable class Registry *registry;
};

class System {
    Signature componentSignature;
    std::vector<Entity> entities;

public:
    System() = default;

    ~System() = default;

    void AddEntityToSystem(Entity entity);

    void RemoveEntityFromSystem(Entity entity);

    std::vector<Entity> GetSystemEntities() const;

    const Signature &GetComponentSignature() const;

    template<typename T>
    void RequireComponent();
};



template<typename T>
void System::RequireComponent()
{
    const auto componentId = Component<T>::GetId();
    componentSignature.set(componentId);
}

class IPool {
public:
    virtual ~IPool() = default;
};

template<typename T>
class Pool final : public IPool {
    std::vector<T> data;

public:
    explicit Pool(const int size = 100) { Resize(size); }

    ~Pool() override = default;

    bool isEmpty() { return data.empty(); }

    int GetSize() { return data.size(); }

    void Resize(int n) { data.resize(n); }

    void Clear() { data.clear(); }

    void Add(T object) { data.push_back(object); }

    void Set(int index, T object) { data[index] = object; }

    T &Get(int index) { return static_cast<T &>(data[index]); }

    T &operator[](const unsigned int index) { return Get(index); }
};

class Registry {
    int numEntities = 0;

    std::vector<std::shared_ptr<IPool> > componentPools;

    std::vector<Signature> entityComponentSignatures;

    std::unordered_map<std::type_index, std::shared_ptr<System> > systems;

    std::set<Entity> entitiesToBeAdded;
    std::set<Entity> entitiesToBeKilled;

public:
    Registry() = default;

    Entity CreateEntity();

    void Update();

    template<typename TComponent, typename... TArgs>
    void AddComponent(Entity entity, TArgs &&... args);

    template<typename TComponent>
    void RemoveComponent(Entity entity);

    template<typename TComponent>
    bool HasComponent(Entity entity);

    template<typename TComponent>
    TComponent &GetComponent(Entity entity) const;

    template<typename TSystem, typename... TArgs>
    void AddSystem(TArgs &&... args);

    template<typename TSystem>
    void RemoveSystem();

    template<typename TSystem>
    bool HasSystem() const;

    template<typename TSystem>
    TSystem &GetSystem() const;

    void AddEntityToSystem(Entity entity) const;
};


template<typename TComponent, typename... TArgs>
void Registry::AddComponent(const Entity entity, TArgs &&... args)
{
    const auto componentId = Component<TComponent>::GetId();
    const auto entityId = entity.GetId();

    if (componentId >= static_cast<int>(componentPools.size()))
    {
        componentPools.resize(componentPools.size() + 1, nullptr);
    }

    if (!componentPools[componentId])
    {
        std::shared_ptr<Pool<TComponent> > newComponentPool = std::make_shared<Pool<TComponent> >();
        componentPools[componentId] = newComponentPool;
    }

    std::shared_ptr<Pool<TComponent> > componentPool = std::static_pointer_cast<Pool<TComponent> >(
        componentPools[componentId]);


    if (entityId >= componentPool->GetSize())
    {
        componentPool->Resize(numEntities);
    }

    TComponent newComponent(std::forward<TArgs>(args)...);

    componentPool->Set(entityId, newComponent);

    entityComponentSignatures[entityId].set(componentId);

    Logger::Log(
        "Component id = " + std::to_string(componentId) + " was added to entity id = " + std::to_string(entityId));
}

template<typename T>
void Registry::RemoveComponent(const Entity entity)
{
    const auto componentId = Component<T>::GetId();
    const auto entityId = entity.GetId();

    entityComponentSignatures[entityId].set(componentId, false);
}

template<typename T>
bool Registry::HasComponent(const Entity entity)
{
    const auto componentId = Component<T>::GetId();
    const auto entityId = entity.GetId();

    return entityComponentSignatures[entityId].test(componentId);
}

template<typename TComponent>
TComponent &Registry::GetComponent(const Entity entity) const
{
    const auto componentId = Component<TComponent>::GetId();
    const auto entityId = entity.GetId();
    auto componentPool = std::static_pointer_cast<Pool<TComponent> >(componentPools[componentId]);
    return componentPool->Get(entityId);
}


template<typename TSystem, typename... TArgs>
void Registry::AddSystem(TArgs &&... args)
{
    std::shared_ptr<TSystem> newSystem = std::make_shared<TSystem>(std::forward<TArgs>(args)...);
    systems.insert(std::make_pair(std::type_index(typeid(TSystem)), newSystem));
}

template<typename TSystem>
void Registry::RemoveSystem()
{
    const auto system = systems.find(std::type_index(typeid(TSystem)));
    systems.erase(system);
}

template<typename TSystem>
bool Registry::HasSystem() const
{
    return systems.find(std::type_index(typeid(TSystem))) != systems.end();
}

template<typename TSystem>
TSystem &Registry::GetSystem() const
{
    const auto system = systems.find(std::type_index(typeid(TSystem)));
    return *(std::static_pointer_cast<TSystem>(system->second));
}

template<typename TComponent, typename... TArgs>
void Entity::AddComponent(TArgs &&... args)
{
    registry->AddComponent<TComponent>(*this, std::forward<TArgs>(args)...);
}

template<typename TComponent>
void Entity::RemoveComponent() const
{
    registry->RemoveComponent<TComponent>(*this);
}

template<typename TComponent>
bool Entity::HasComponent() const
{
    return registry->HasComponent<TComponent>(*this);
}

template<typename TComponent>
TComponent &Entity::GetComponent() const
{
    return registry->GetComponent<TComponent>(*this);
}
#endif //ECS_H
