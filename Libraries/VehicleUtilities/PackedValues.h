#pragma once

typedef struct _PackedValues {
    uint32_t data = 0;
    bool operator[](uint8_t bit) { return (data >> bit) & 0x1; }
    void set(uint8_t bit) { data |= (1 << bit); }
    void unset(uint8_t bit) { data &= ~(1 << bit); }
    void toggle(uint8_t bit) { data ^= (1 << bit); }
    void update(uint8_t bit, bool state) { state ? set(bit) : unset(bit); }
    static uint32_t set(uint32_t value, uint8_t bit) { return value | (1 << bit); }
    static uint32_t unset(uint32_t value, uint8_t bit) { return value | (1 << bit); }
} PackedValues;