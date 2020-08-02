class Interface_CAN_Container;

typedef void (*writeFunc)(uint32_t, Interface_CAN_Container*);

class Interface_CAN_Container {
    public:
        virtual bool load(uint8_t buf[8]) = 0;
        virtual void write(uint8_t buf[8]) = 0;
        virtual void write(writeFunc callback, uint32_t id) = 0;
        virtual uint32_t messageLen() = 0;
#ifdef HYTECH_COMPRESSION_EN
        virtual bool hasChanged() = 0;
#endif
#ifdef HYTECH_LOGGING_EN
        virtual void print(Stream& serial) {};
#endif
};