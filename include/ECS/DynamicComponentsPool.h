#ifndef GRYNCADYNAMICCOMPONENTSPOOL_H
#define GRYNCADYNAMICCOMPONENTSPOOL_H

#include <vector>

namespace Grynca {

    // fw
    class EntityTypeInfo;

    struct Chunk {
        std::vector<uint8_t> chunk_data;
        unsigned int occupied;    // how many entities is in chunk
    };

    class DynamicComponentsPool {
    public:
        DynamicComponentsPool(EntityTypeInfo& eti);

        void* get(unsigned int chunk_id, unsigned int chunk_pos, unsigned int component_type_id);

        template<typename CompType>
        CompType* get(unsigned int chunk_id, unsigned int chunk_pos);

        bool contains(unsigned int component_type_id);

        // returns chunk_id and chunk_pos of newly created entity
        inline std::pair<unsigned int, unsigned int> createNewEntity();

        // returns -1 if no entity was moved (we removed last entity)
        //  or guid of entity which was moved to chunk_pos to fill removed entity's place
        inline int32_t destroyEntity(unsigned int chunk_id, unsigned int chunk_pos);

        unsigned int chunksCount() { return (unsigned int)chunks_.size(); }
        unsigned int entitiesInChunk(unsigned int chunk_id) { return (unsigned int)chunks_[chunk_id].occupied; }
    private:
        void createNewChunk_();

        // chunks for dynamic components
        std::vector<Chunk> chunks_;
        unsigned int chunk_size_;       // in bytes
        std::vector<int> array_starts_;   // where in chunk do dyn comps array start
    };

}

#include "EntitiesRegister.h"
#include "ComponentsRegister.h"
#include "EntityHeaderComponent.h"

namespace Grynca {

    inline DynamicComponentsPool::DynamicComponentsPool(EntityTypeInfo &eti) {
        // gather dynamic components info
        chunk_size_ = 0;
        for (unsigned int i=0; i<eti.dynamicComponentsMask().size(); ++i) {
            if (eti.dynamicComponentsMask()[i]) {
                array_starts_.push_back(chunk_size_);
                chunk_size_ += (unsigned int)ComponentsRegister::get(i).component_size*POOL_CHUNK_CAPACITY;
            }
            else
                array_starts_.push_back(-1);
        }

        // add first chunk
        createNewChunk_();
    }

    inline void* DynamicComponentsPool::get(unsigned int chunk_id, unsigned int chunk_pos, unsigned int component_type_id) {
        assert(chunk_id < chunks_.size());
        assert(chunk_pos < chunks_[chunk_id].occupied);
        assert(component_type_id<MAX_COMPONENTS);
        assert(contains(component_type_id)
               && "This entity type does not contain required dynamic component.");

        unsigned int data_id = array_starts_[component_type_id] +
            (unsigned int)ComponentsRegister::get(component_type_id).component_size*chunk_pos;
        return &(chunks_[chunk_id].chunk_data[data_id]);
    }

    template<typename CompType>
    inline CompType* DynamicComponentsPool::get(unsigned int chunk_id, unsigned int chunk_pos) {
        return (CompType*)get(chunk_id, chunk_pos, CompType::typeId);
    }

    inline bool DynamicComponentsPool::contains(unsigned int component_type_id) {
        return array_starts_[component_type_id]>=0;
    }

    inline std::pair<unsigned int, unsigned int> DynamicComponentsPool::createNewEntity() {
        // find chunk with free space for entity
        unsigned int chunk_id;
        for (chunk_id=0; chunk_id<chunks_.size(); ++chunk_id) {
            if (chunks_[chunk_id].occupied < POOL_CHUNK_CAPACITY)
                break;
        }
        if (chunk_id == chunks_.size()) {
            createNewChunk_();
        }
        unsigned int chunk_pos = chunks_[chunk_id].occupied;
        ++chunks_[chunk_id].occupied;
        return std::make_pair(chunk_id, chunk_pos);
    }

    inline int32_t DynamicComponentsPool::destroyEntity(unsigned int chunk_id, unsigned int chunk_pos) {
        unsigned int last_chunk_pos = (unsigned int)chunks_[chunk_id].occupied-1;
        // destruct dynamic components
        bool move_last = (chunk_pos < last_chunk_pos);
        for (unsigned int i=0; i<array_starts_.size(); ++i) {
            if (contains(i)) {
                void* component = get(chunk_id, chunk_pos, i);
                ComponentsRegister::get(i).destroy_func(component);
                if (move_last) {
                    // move components of last chunk entity to freed components' slots
                    memcpy(component,
                           get(chunk_id, last_chunk_pos, i),
                           ComponentsRegister::get(i).component_size);
                }
            }
        }
        --chunks_[chunk_id].occupied;

        if (move_last) {
            uint32_t moved_guid = get<EntityHeaderComponent>(chunk_id, chunk_pos)->guid;
            return moved_guid;
        }
        else
            return -1;
    }

    inline void DynamicComponentsPool::createNewChunk_() {
        chunks_.push_back(Chunk());
        chunks_.back().chunk_data.resize(chunk_size_);
        chunks_.back().occupied = 0;
    }
}

#endif // GRYNCADYNAMICCOMPONENTSPOOL_H
