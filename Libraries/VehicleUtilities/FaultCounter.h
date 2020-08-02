typedef struct _FaultCounter {
    uint8_t consecutive_faults = 0;
    const uint8_t threshold;

    _FaultCounter(uint8_t t) : threshold(t) {};

    bool update(bool fault) {
        if (fault) 
            if (consecutive_faults >= threshold)
                return true;
            else
                consecutive_faults++;
        else
            consecutive_faults = 0;
        return false;
    }
    bool fault() { return consecutive_faults >= threshold; }
} FaultCounter;