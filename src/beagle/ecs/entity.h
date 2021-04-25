//
// Created by Ricardo on 3/25/2021.
//

#ifndef BEAGLE_ENTITY_H
#define BEAGLE_ENTITY_H

#include <cstdint>
#include <bitset>
#include <set>

#include <eagle/memory/pool_allocator.h>
#include <eagle/events/event.h>

#include <beagle/beagle_global_definitions.h>

#define MAX_COMPONENTS 256

using ComponentMask = std::bitset<MAX_COMPONENTS>;

namespace beagle {

class EntityManager;

template<typename T>
class Component;

class Entity {
public:

    struct Id {
        Id() : m_id(0) {}
        explicit Id(uint64_t id) : m_id(id) {}
        Id(uint32_t index, uint32_t version) : m_id(uint64_t(index) | uint64_t(version) << 32UL) {}

        uint64_t id() const { return m_id; }

        bool operator == (const Id &other) const { return m_id == other.m_id; }
        bool operator != (const Id &other) const { return m_id != other.m_id; }
        bool operator < (const Id &other) const { return m_id < other.m_id; }

        uint32_t index() const { return m_id & 0xffffffffUL; }
        uint32_t version() const { return m_id >> 32; }

    private:
        friend EntityManager;

        uint64_t m_id;
    };

    Entity() = default;
    Entity(EntityManager *manager, Entity::Id id) : m_manager(manager), m_id(id) {}
    Entity(const Entity &other) = default;
    Entity &operator = (const Entity &other) = default;

    Id id() const { return m_id; }

    template<typename T, typename ...Args>
    Component<T> assign(Args&& ...args);

    template<typename T>
    Component<T> component();

    template<typename ...Components>
    std::tuple<Component<Components>...> components();

    template<typename T>
    bool has_component();

    template<typename T>
    void remove();

    void destroy();

    bool valid() const;

    const ComponentMask& component_mask() const;


private:
    EntityManager* m_manager;
    Id m_id;
};


class ComponentPool {
public:
    ComponentPool(size_t elementSize, size_t chunkElementCount) :
    m_elementSize(elementSize), m_chunkElementCount(chunkElementCount) {}

    ~ComponentPool(){
        for (auto chunk : m_chunks) {
            delete[] chunk;
        }
    }

    void expand(size_t expandedElementCount) {
        size_t expandedSize = expandedElementCount * m_elementSize;
        while (expandedSize > m_size){
            m_chunks.emplace_back(new uint8_t[m_elementSize * m_chunkElementCount]);
            m_size += m_elementSize * m_chunkElementCount;
        }
    }

    void* get(size_t index){
        assert(index < m_size);
        return m_chunks[index / m_chunkElementCount] + (index % m_chunkElementCount) * m_elementSize;
    }

private:
    size_t m_elementSize;
    size_t m_chunkElementCount;
    size_t m_size = 0;
    std::vector<uint8_t*> m_chunks;
};


class BaseComponentHelper {
public:
    virtual ~BaseComponentHelper() = default;
    virtual void destroy_component(Entity::Id e) = 0;

protected:
    static uint32_t s_familyCounter;
};

template<typename T>
class ComponentHelper : public BaseComponentHelper {
public:

    explicit ComponentHelper(EntityManager* entityManager) : m_entityManager(entityManager) {}

    static uint32_t family() {
        const static uint32_t familyIndex = s_familyCounter++;
        return familyIndex;
    }

    void destroy_component(Entity::Id e) override;
private:
    EntityManager* m_entityManager;
};

template<typename T>
class Component {
public:
    Component() : m_owner(), m_data(nullptr) {}
    Component(Entity owner, T* data) : m_owner(owner), m_data(data) {}

    T* operator->() { return m_data; }
    const T* operator->() const { return m_data; }

    Entity owner() { return m_owner; }
    Entity owner() const { return m_owner; }

    bool valid() const { return m_owner.valid() && m_data != nullptr; }

private:
    Entity m_owner;
    T* m_data;
};


struct OnEntityCreated {
    Entity entity;
};

struct OnEntityDestroyed {
    Entity entity;
};

template<typename T>
struct OnComponentAdded {
    Component<T> component;
};

template<typename T>
struct OnComponentRemoved {
    Component<T> component;
};

class EntityManager {
public:

    class Iterator : public std::iterator<std::input_iterator_tag, Entity::Id> {
    public:
        Iterator& operator++() {
            ++m_i;
            next();
            return *static_cast<Iterator*>(this);
        }

        bool operator==(const Iterator& rhs) const { return m_i == rhs.m_i; }

        bool operator!=(const Iterator& rhs) const { return m_i != rhs.m_i; }

        Entity operator*() { return m_manager->entity_from_index(m_i); }


        explicit Iterator(EntityManager *manager, size_t i) : m_manager(manager), m_i(i), m_capacity(m_manager->capacity()), m_freeCursor(0){}
    protected:
        void next() {
            while (m_i < m_capacity && !valid_entity()) {
                ++m_i;
            }
        }

        inline bool valid_entity() {
            const std::vector<uint32_t> &freeList = m_manager->m_freedEntities;
            if (m_freeCursor < freeList.size() && freeList[m_freeCursor] == m_i) {
                ++m_freeCursor;
                return false;
            }
            return true;
        }

    protected:
        EntityManager *m_manager;
        uint32_t m_i;
        uint32_t m_capacity;
        uint32_t m_freeCursor;
    };

    class View {
    public:
        Iterator begin() { return Iterator(m_manager, 0); }
        Iterator end() { return Iterator(m_manager, m_manager->capacity()); }
        const Iterator begin() const { return Iterator(m_manager, 0); }
        const Iterator end() const { return Iterator(m_manager, m_manager->capacity()); }

    private:
        friend class EntityManager;

        explicit View(EntityManager *manager) : m_manager(manager) {}

        EntityManager *m_manager;
    };

public:

    virtual ~EntityManager() {
        reset();
    }

    void reset(){
        for (uint32_t i = 0; i < m_entityComponentMask.size(); i++) {
            Entity::Id id = Entity::Id(i, m_entityVersions[i]);
            if (valid(id)){
                destroy(id);
            }
        }
        for (auto *pool : m_componentPools) {
            delete pool;
        }
        for (auto *helper : m_componentHelpers) {
            delete helper;
        }

        m_entityComponentMask.clear();
        m_componentPools.clear();
        m_componentHelpers.clear();
        m_entityVersions.clear();
        m_freedEntities.clear();
        m_indexCounter = 0;
    }

    Entity create(){
        uint32_t index, version;
        if (m_freedEntities.empty()) {
            index = m_indexCounter++;
            accomodate_entity(index);
            version = m_entityVersions[index] = 1;
        } else {
            index = m_freedEntities.back();
            m_freedEntities.pop_back();
            version = m_entityVersions[index];
        }

        Entity entity = entity_from_id(Entity::Id(index, version));
        m_eventBus.emit(OnEntityCreated{entity});
        return entity;
    }

    void destroy(Entity::Id entity){
        m_eventBus.emit(OnEntityDestroyed{entity_from_id(entity)});
        uint32_t index = entity.index();
        m_entityVersions[index]++;
        auto mask = m_entityComponentMask[index];
        for (int i = 0; i < m_componentHelpers.size(); i++){
            auto helper = m_componentHelpers[i];
            if (helper && mask.test(i)){
                helper->destroy_component(entity);
            }
        }
        m_entityComponentMask[index].reset();
        m_freedEntities.push_back(index);
    }

    template<typename T, typename ...Args>
    Component<T> assign(Entity::Id entity, Args&& ...args){
        uint32_t index = entity.index();
        uint32_t family = component_family<T>();
        accomodate_component<T>();

        T* address = component_ptr<T>(entity);
        ::new(address) T(std::forward<Args>(args)...);

        m_entityComponentMask[index].set(family);

        Component<T> component = Component<T>(entity_from_id(entity), address);
        m_eventBus.emit(OnComponentAdded<T>{component});
        return component;
    }

    template<typename T>
    void remove(Entity::Id entity){
        Component<T> component = component_from_entity<T>(entity);
        m_eventBus.emit(OnComponentRemoved<T>{component});
        uint32_t index = entity.index();
        uint32_t family = component_family<T>();
        component->~T();
        m_entityComponentMask[index].reset(family);
    }

    template<typename T>
    inline uint32_t component_family() {
        return ComponentHelper<typename std::remove_reference<T>::type>::family();
    }

    template<typename T>
    inline T* component_ptr(Entity::Id entity){
        return static_cast<T*>(m_componentPools[component_family<T>()]->get(entity.index()));
    }

    inline bool valid(Entity::Id entity) const {
        return entity.index() < m_indexCounter && m_entityVersions[entity.index()] == entity.version();
    }

    inline Entity entity_from_id(Entity::Id entity){
        return Entity(this, entity);
    }

    inline Entity entity_from_index(uint32_t index){
        return Entity(this, Entity::Id(index, m_entityVersions[index]));
    }

    template<typename T>
    inline Component<T> component_from_entity(Entity::Id entity) {
        return Component<T>(entity_from_id(entity), component_ptr<T>(entity));
    }

    template<typename ...Components>
    std::tuple<Component<Components>...> components_from_entity(Entity::Id entity){
        return std::make_tuple(component_from_entity<Components>(entity)...);
    }

    inline const ComponentMask& component_mask(Entity::Id entity) const {
        return m_entityComponentMask[entity.index()];
    }

    template<typename T>
    inline bool entity_has_component(Entity::Id entity) {
        return m_entityComponentMask[entity.index()].test(component_family<T>());
    }

    View all(){
        return View(this);
    }

    inline uint32_t capacity() const { return m_entityComponentMask.size(); }
    inline uint32_t size() const { return capacity() - m_freedEntities.size(); }

private:

    void accomodate_entity(uint32_t index){
        if (index >= m_entityComponentMask.size()) {
            m_entityComponentMask.resize(index + 1);
            m_entityVersions.resize(index + 1);
            for (auto pool : m_componentPools){
                if (pool){
                    pool->expand(index);
                }
            }
        }
    }

    template<typename T>
    void accomodate_component(){
        uint32_t index = component_family<T>();
        if (index >= m_componentPools.size()){
            m_componentPools.resize(index + 1, nullptr);
        }
        if (m_componentPools[index] == nullptr){
            auto pool = new ComponentPool(sizeof(T), 8192);
            pool->expand(m_indexCounter);
            m_componentPools[index] = pool;
        }
        if (index >= m_componentHelpers.size()){
            m_componentHelpers.resize(index + 1, nullptr);
        }
        if (m_componentHelpers[index] == nullptr){
            auto helper = new ComponentHelper<T>(this);
            m_componentHelpers[index] = helper;
        }
    }

private:

    template<typename ...Components>
    friend class EntityGroup;

    eagle::EventBus m_eventBus;
    std::vector<ComponentMask> m_entityComponentMask;
    std::vector<ComponentPool*> m_componentPools;
    std::vector<BaseComponentHelper*> m_componentHelpers;
    std::vector<uint32_t> m_entityVersions;
    std::vector<uint32_t> m_freedEntities;
    uint32_t m_indexCounter = 0;
};

template<typename ...Components>
class EntityGroup {
public:

    class Iterator : public std::iterator<std::input_iterator_tag, Entity::Id> {
    public:
        Iterator& operator++() {
            ++m_i;
            return *static_cast<Iterator*>(this);
        }

        bool operator==(const Iterator& rhs) const { return m_i == rhs.m_i; }

        bool operator!=(const Iterator& rhs) const { return m_i != rhs.m_i; }


        std::tuple<Component<Components>...> operator*() {
            return m_group->operator[](m_i).template components<Components...>();
        }

        explicit Iterator(EntityGroup *group, size_t i) : m_group(group), m_i(i) {}

    protected:
        EntityGroup *m_group;
        uint32_t m_i;
    };

public:

    EntityGroup() {
        for_types<Components...>([this](auto t){
            using Component = typename decltype(t)::type;
            m_mask.set(m_manager->component_family<Component>());
        });
    }

    void attach(EntityManager* manager){
        detach();
        m_manager = manager;
        m_listener.attach(&manager->m_eventBus);
        for_types<Components...>([this](auto t){
            using Component = typename decltype(t)::type;
            m_listener.template subscribe<OnComponentAdded<Component>>([this](const OnComponentAdded<Component>& ev){
                if ((ev.component.owner().component_mask() & m_mask) == m_mask){
                    auto it = std::find(m_entities.begin(), m_entities.end(), ev.component.owner().id());
                    if (it == m_entities.end()){
                        m_entities.emplace_back(ev.component.owner().id());
                    }
                }
                return false;
            });

            m_listener.template subscribe<OnComponentRemoved<Component>>([this](const OnComponentRemoved<Component>& ev){
                auto it = std::find(m_entities.begin(), m_entities.end(), ev.component.owner().id());
                if (it != m_entities.end()){
                    m_entities.erase(it);
                }
                return false;
            });
        });

        for (auto entity : m_manager->all()){
            if ((entity.component_mask() & m_mask) == m_mask){
                m_entities.emplace_back(entity.id());
            }
        }
    }

    void detach(){
        m_entities.clear();
        m_listener.detach();
        m_manager = nullptr;
    }

    Iterator begin() { return Iterator(this, 0); }
    Iterator end() { return Iterator(this, size()); }
    Iterator begin() const { return Iterator(this, 0); }
    Iterator end() const { return Iterator(this, size()); }

    inline size_t size() const { return m_entities.size(); }

    inline Entity operator[](size_t index) {
        return m_manager->entity_from_id(m_entities[index]);
    }

private:
    EntityManager* m_manager = nullptr;
    ComponentMask m_mask;
    eagle::EventListener m_listener;
    std::vector<Entity::Id> m_entities;
};

template<typename T>
void ComponentHelper<T>::destroy_component(Entity::Id e) {
    m_entityManager->remove<T>(e);
}

template<typename T, typename... Args>
Component<T> Entity::assign(Args&& ... args) {
    return m_manager->template assign<T>(m_id, std::forward<Args>(args)...);
}

template<typename T>
void Entity::remove() {
    m_manager->template remove<T>(m_id);
}

template<typename T>
Component<T> Entity::component() {
    return m_manager->template component_from_entity<T>(m_id);
}

template<typename... Components>
std::tuple<Component<Components>...> Entity::components() {
    return m_manager->components_from_entity<Components...>(m_id);
}

template<typename T>
bool Entity::has_component() {
    return m_manager->entity_has_component<T>(m_id);
}

}

#endif //BEAGLE_ENTITY_H
