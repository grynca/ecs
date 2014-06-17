/*
 * ComponentsPool.h
 *
 *  Created on: 7. 1. 2014
 *      Author: lutze
 */

#ifndef ENTITIES_POOL_H
#define ENTITIES_POOL_H

#include <stdint.h>
#include <string.h>
#include <vector>
#include <functional>

namespace Grynca {
    // fw
    class EntityTypeInfo;
    class EntityBase;

    class EntitiesPool {
    public:
        EntitiesPool(EntityTypeInfo* eti);

        ~EntitiesPool();
        EntityTypeInfo& getEntityTypeInfo()const  { return *eti_; }

        template <typename EntityType, typename ... Args>
        EntityType* createNewEntity(unsigned int guid, Args ... args);

        void destroyEntity(EntityBase* entity) ;

        void setEntityMovedCallback(std::function<void(uint32_t, EntityBase*)> moved_func ) {
            entity_moved_func_ = moved_func;
        }

        size_t entitiesCount();

        template <typename Callback>
        void loopEntities(double dt, Callback c);

    private:
        EntityBase* entityPlace_(unsigned int chunk_id, unsigned int chunk_pos);

        void createNewChunk_();

        std::vector< std::vector<uint8_t> > pool_chunks_;
        std::vector<size_t> chunk_sizes_;       // real occupied chunk sizes
        EntityTypeInfo* eti_;
        // callback when entity is moved to new place (on removal of some other entity)
        std::function<void(uint32_t/*guid*/, EntityBase*)> entity_moved_func_;
    };
}

#include "EntitiesRegister.h"
#include "Entity.h"
#include "EntityHeaderComponent.h"

namespace Grynca {
    inline EntitiesPool::EntitiesPool(EntityTypeInfo *eti)
     : eti_(eti),
       entity_moved_func_(nullptr)
    {
        createNewChunk_();
    }

    inline EntitiesPool::~EntitiesPool() {
        for (unsigned int chid=0; chid<pool_chunks_.size(); ++chid) {
            for (int pos=(int)chunk_sizes_[chid]-1; pos>=0; --pos) {
                EntityBase* ent = entityPlace_(chid, pos);
                destroyEntity(ent);
            }
        }
    }

    template <typename EntityType, typename ... Args>
    inline EntityType* EntitiesPool::createNewEntity(unsigned int guid, Args ... args) {
        unsigned int chid;
        for (chid=0; chid<chunk_sizes_.size(); ++chid) {
            if (chunk_sizes_[chid] < POOL_CHUNK_CAPACITY)
                break;
        }
        if (chid == chunk_sizes_.size()) {
            createNewChunk_();
        }
        // create with placement new
        EntityType* newent = new (entityPlace_(chid, (unsigned int)chunk_sizes_[chid])) EntityType(args...);
        // set entity header
        EntityHeaderComponent* ehc = ((EntityBase*)newent)->get<EntityHeaderComponent>();
        ehc->guid = guid;
        ehc->pool_chunk = chid;
        ehc->chunk_position = (unsigned int)chunk_sizes_[chid];
        ++chunk_sizes_[chid];
        return newent;
    }

    inline void EntitiesPool::destroyEntity(EntityBase* entity) {
        EntityHeaderComponent* ehc = entity->get<EntityHeaderComponent>();
        // call entity's destructor
        eti_->destroyFunc(entity);

        unsigned int last_chunk_ent_pos = (unsigned int)chunk_sizes_[ehc->pool_chunk]-1;
        if (ehc->chunk_position < last_chunk_ent_pos) {
            // move last entity in chunk to freed place
            void *last_ent = entityPlace_(ehc->pool_chunk, last_chunk_ent_pos);
            memcpy(entity, last_ent, eti_->getEntitySize());
            if (entity_moved_func_ != nullptr) {
                entity_moved_func_(entity->get<EntityHeaderComponent>()->guid, entity);
            }
        }
        --chunk_sizes_[ehc->pool_chunk];
    }

    inline size_t EntitiesPool::entitiesCount() {
        unsigned int accumulated_count = 0;
        for (unsigned int chid=0; chid<chunk_sizes_.size(); ++chid) {
            accumulated_count += (unsigned int)chunk_sizes_[chid];
        }
        return accumulated_count;
    }

    template <typename Callback>
    inline void EntitiesPool::loopEntities(double dt, Callback c) {
        for (unsigned int chid=0; chid<pool_chunks_.size(); ++chid) {
            for (unsigned int pos=0; pos<chunk_sizes_[chid]; ++pos) {
                c(dt, entityPlace_(chid, pos));
            }
        }
    }

    inline EntityBase* EntitiesPool::entityPlace_(unsigned int chunk_id, unsigned int chunk_pos) {
        return (EntityBase*)&(pool_chunks_[chunk_id][chunk_pos*eti_->getEntitySize()]);
    }

    inline void EntitiesPool::createNewChunk_() {
        pool_chunks_.push_back(std::vector<uint8_t>());
        pool_chunks_.back().resize(POOL_CHUNK_CAPACITY * eti_->getEntitySize());
        chunk_sizes_.push_back(0);
    }
}

#endif /* ENTITIES_POOL_H */
