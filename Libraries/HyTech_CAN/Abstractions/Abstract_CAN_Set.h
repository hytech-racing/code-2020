template<typename T>
class Abstract_CAN_Set : public Interface_CAN_Container {
    public:
        Abstract_CAN_Set();
        Abstract_CAN_Set(T* set, uint8_t size);
        Abstract_CAN_Set(uint8_t buf[8]);
        virtual bool load(uint8_t buf[8]);
        virtual void write(uint8_t buf[8]);
        virtual void write(writeFunc callback, uint32_t id);
        virtual uint32_t messageLen();
#ifdef HYTECH_COMPRESSION_EN
        virtual bool hasChanged();
#endif
    protected:
        T* elements;
        uint8_t numElements;
};

template<typename T>
Abstract_CAN_Set<T>::Abstract_CAN_Set() {}

template<typename T>
Abstract_CAN_Set<T>::Abstract_CAN_Set(T* set, uint8_t size) {
    elements = set;
    numElements = size;
    for (int i = 0; i < size; ++i)
        set[i].setIndex(i);
}

template<typename T>
Abstract_CAN_Set<T>::Abstract_CAN_Set(uint8_t buf[]) {}

template<typename T>
bool Abstract_CAN_Set<T>::load(uint8_t buf[]) {
    return elements[T(buf).getIndex()].load(buf);
}

template<typename T>
void Abstract_CAN_Set<T>::write(uint8_t buf[]) {}

template<typename T>
void Abstract_CAN_Set<T>::write(writeFunc callback, uint32_t id) {
    for (int i = 0; i < numElements; ++i)
        callback(id, elements + i);
}

template<typename T>
uint32_t Abstract_CAN_Set<T>::messageLen() {
    return sizeof (T);
}

#ifdef HYTECH_COMPRESSION_EN
    template<typename T>
    bool Abstract_CAN_Set<T>::hasChanged() {
        return true;
    }
#endif