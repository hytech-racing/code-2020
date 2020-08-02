#pragma once

template<typename K, typename V> class HT_Map : public HT_Vector<std::pair<K,V>> {
    public: 
        HT_Map(size_t size = 15) : HT_Vector<std::pair<K,V>>() {}

        std::pair<K,V>* insert(K key, V value) {
            std::pair<K,V> *it = this->find(key);
            if (it != this->end()) {
                it->second = value;
                return it;
            }
            this->push_back({ key, value });
            it = this->end() - 1;
            while (it != this->begin() && it->first < (it-1)->first) {
                *it = *(it-1);
                it--;
            }
            *it = { key, value };
            return it;
        }

        std::pair<K,V>* find(K key) {
            size_t L = 0, R = this->mSize - 1;
            while (L <= R) {
                size_t m = (L+R) / 2;
                if (this->arr[m].first < key)
                    L = m + 1;
                else if (this->arr[m].first > key)
                    R = m - 1;
                else
                    return this->arr + m;
            }
            return this->arr + this->mSize;
        }
};
