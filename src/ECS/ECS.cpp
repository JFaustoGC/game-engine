#include "ECS.h"

#include <algorithm>

#include "../Logger/Logger.h"

int IComponent::nextId = 0;

int Entity::GetId() const
{
    return id;
}

bool Entity::operator==(const Entity &entity) const
{
    return id == entity.id;
}

bool Entity::operator<(const Entity &entity) const
{
    return id < entity.id;
}

void System::AddEntityToSystem(const Entity entity)
{
    entities.push_back(entity);
}

void System::RemoveEntityFromSystem(Entity entity)
{
    entities.erase(std::remove_if(entities.begin(), entities.end(), [&entity](const Entity other)
    {
        return other == entity;
    }), entities.end());
}

std::vector<Entity> System::GetSystemEntities() const
{
    return entities;
}


const Signature &System::GetComponentSignature() const
{
    return componentSignature;
}

Entity Registry::CreateEntity()
{
    const int entityId = numEntities++;
    const Entity entity(entityId);
    entity.registry = this;
    entitiesToBeAdded.insert(entity);

    if (entityId >= static_cast<int>(entityComponentSignatures.size()))
    {
        entityComponentSignatures.resize(entityId + 1);
    }

    Logger::Log("Entity created with id: " + std::to_string(entityId));
    return entity;
}

void Registry::Update()
{
    for (const auto entity: entitiesToBeAdded)
    {
        AddEntityToSystem(entity);
    }

    entitiesToBeAdded.clear();
}

void Registry::AddEntityToSystem(const Entity entity) const
{
    const auto entityId = entity.GetId();
    const auto &entityComponentSignature = entityComponentSignatures[entityId];

    for (auto &system: systems)
    {
        const auto &systemComponentSignature = system.second->GetComponentSignature();

        const bool isInterested = (entityComponentSignature & systemComponentSignature) == systemComponentSignature;
        if (isInterested)
        {
            system.second->AddEntityToSystem(entity);
        }
    }
}
