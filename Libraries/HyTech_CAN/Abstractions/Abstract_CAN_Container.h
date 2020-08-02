template<typename T>
class Abstract_CAN_Container : public Interface_CAN_Container {
    public:
        Abstract_CAN_Container();
        Abstract_CAN_Container(uint8_t buf[8]);
        virtual bool load(uint8_t buf[8]);
        virtual void write(uint8_t buf[8]);
        virtual void write(writeFunc callback, uint32_t id);
        virtual uint32_t messageLen();
    protected: 
        T message;
#ifdef HYTECH_COMPRESSION_EN
    public: virtual bool hasChanged();
    protected: bool changed;
#endif
};

template<typename T>
Abstract_CAN_Container<T>::Abstract_CAN_Container() {
    message = {};
    #ifdef HYTECH_COMPRESSION_EN
        changed = true;
    #endif
}

template<typename T>
Abstract_CAN_Container<T>::Abstract_CAN_Container(uint8_t buf[]) {
    message = {};
    load(buf);
}

#ifdef HYTECH_COMPRESSION_EN
    template<typename T>
    bool Abstract_CAN_Container<T>::load(uint8_t buf[]) {
        changed = memcmp(&message, &buf, sizeof(T));
        if (changed)
            memcpy(&message, &buf, sizeof(T));
        return changed;
    }
#else
    template<typename T>
    bool Abstract_CAN_Container<T>::load(uint8_t buf[]) {
        memcpy(&message, &buf, sizeof(T));
        return true;
    }
#endif

template<typename T>
void Abstract_CAN_Container<T>::write(uint8_t buf[]) {
    memcpy(&buf, &message, sizeof(T));
    #ifdef HYTECH_COMPRESSION_EN
        changed = false;
    #endif
}

template<typename T>
void Abstract_CAN_Container<T>::write(writeFunc callback, uint32_t id) {
    callback(id, this);
}

template<typename T>
uint32_t Abstract_CAN_Container<T>::messageLen() {
    return sizeof (T);
}

#ifdef HYTECH_COMPRESSION_EN
    template<typename T>
    bool Abstract_CAN_Container<T>::hasChanged() {
        return changed;
    }
#endif