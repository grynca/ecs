#ifndef OFFSETS_H
#define OFFSETS_H

#include "TypeTuple.h"

namespace Grynca {
    namespace Offsets {
        static size_t offsets_(const TypeTuple<>&, std::vector<std::pair<unsigned int, size_t> >&)
        {
            return 0;
        }

        template<typename T, typename ...Ts>
        static size_t offsets_(const TypeTuple<T, Ts...>&, std::vector<std::pair<unsigned int, size_t>>& v) {
            unsigned int type_id = T::componentTypeId;
            std::pair<unsigned int, size_t> p;
            size_t my_offset = offsets_(TypeTuple<Ts...>(), v);
            size_t parent_offset = my_offset + sizeof(T);
            v.push_back(std::make_pair(type_id, my_offset));
            return parent_offset;
        }

        // This can get offsets of tuple into vector
        //  TODO: rework to something nicer
        template <typename Tpl>
        static std::vector<int> getOffsets(unsigned int max_comps) {
            std::vector<std::pair<unsigned int, size_t> > v;
            offsets_(Tpl(), v);
            std::vector<int> offsets_all;
            offsets_all.resize(max_comps, -1);
            for (unsigned int i=0; i<v.size(); i++) {
                offsets_all[v[i].first] = (int)v[i].second;
            }
            return offsets_all;
        }
    }
}
#endif // OFFSETS_H
