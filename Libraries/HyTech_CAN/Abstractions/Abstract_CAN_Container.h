template<typename T>
class Abstract_CAN_Container : public Interface_CAN_Container {
    public:
        Abstract_CAN_Container();
        Abstract_CAN_Container(uint8_t buf[8]);
        virtual void load(uint8_t buf[8]);
        virtual void write(uint8_t buf[8]);
        virtual void write(writeFunc callback, uint32_t id);
        virtual uint32_t messageLen();
    protected: 
        T message;
};

template<typename T>
Abstract_CAN_Container<T>::Abstract_CAN_Container() {
    message = {};
}

template<typename T>
Abstract_CAN_Container<T>::Abstract_CAN_Container(uint8_t buf[]) {
    message = {};
    load(buf);
}

template<typename T>
void Abstract_CAN_Container<T>::load(uint8_t buf[]) {
    memcpy(&message, &buf, sizeof(T));
}

template<typename T>
void Abstract_CAN_Container<T>::write(uint8_t buf[]) {
    memcpy(&buf, &message, sizeof(T));
}

template<typename T>
void Abstract_CAN_Container<T>::write(writeFunc callback, uint32_t id) {
    callback(id, this);
}

template<typename T>
uint32_t Abstract_CAN_Container<T>::messageLen() {
    return sizeof (T);
}