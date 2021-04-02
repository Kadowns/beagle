//
// Created by Ricardo on 3/25/2021.
//

#include "entity_manager.h"

namespace beagle {

uint32_t beagle::BaseComponentHelper::s_familyCounter = 0;

void Entity::destroy() {
    m_manager->destroy(m_id);
}

bool Entity::valid() const {
    return m_manager->valid(m_id);
}

const ComponentMask& Entity::component_mask() const {
    return m_manager->component_mask(m_id);
}

}