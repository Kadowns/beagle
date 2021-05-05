//
// Created by Ricardo on 5/1/2021.
//

#ifndef BEAGLE_ASSET_H
#define BEAGLE_ASSET_H

#include <vector>
#include <cstdint>

namespace beagle {

template<typename T, typename Pool = std::vector<T>, typename IndexType = typename Pool::index_type>
class Asset {
public:

    inline bool operator<(const Asset<T, Pool, IndexType>& other) const {
        return m_index < other.m_index;
    }

    inline bool operator!=(const Asset<T, Pool, IndexType>& other) const {
        return m_index != other.m_index || m_pool == other.m_pool;
    }

    inline bool operator==(const Asset<T, Pool, IndexType>& other) const {
        return m_index == other.m_index && m_pool == other.m_pool;
    }

    T& operator*() { return (*m_pool)[m_index]; }
    T& operator*() const { return (*m_pool)[m_index]; }

    T* operator->() { return &operator*(); }
    T* operator->() const { return &operator*(); }

protected:
    friend Pool;
    Asset(Pool* pool, IndexType index) : m_pool(pool), m_index(index) {}

private:
    Pool* m_pool;
    IndexType m_index;
};


}


#endif //BEAGLE_ASSET_H
