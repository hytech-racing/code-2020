#include "Interface_CAN_Container.h"
#include "Abstract_CAN_Container.h"
#include "Abstract_CAN_Set.h"

class Interface_Indexable_CAN { 
    public: 
        virtual uint8_t getIndex() = 0; 
        virtual void setIndex(uint8_t) = 0; 
};