#pragma once

template<typename T>
class HT_Vector { 
    protected:
        T* arr; 
        size_t mCapacity; 
        size_t mSize; 
    
    public: 
        HT_Vector(size_t initialCapacity = 1) { 
            arr = new T[initialCapacity]; 
            mCapacity = initialCapacity; 
            mSize = 0; 
        }

        HT_Vector(T singleton) : HT_Vector() {
            this->push_back(singleton);
        }

        ~HT_Vector() { delete [] arr; }
    
        void push_back(T data) { 
            if (mSize == mCapacity) { 
                T* temp = new T[2 * mCapacity]; 

                for (size_t i = 0; i < mCapacity; i++)
                    temp[i] = arr[i]; 
    
                delete[] arr; 
                mCapacity *= 2; 
                arr = temp; 
            }
    
            arr[mSize++] = data; 
        } 

    
        T& operator[](size_t idx) { return arr[idx]; }
        size_t size() { return size; }
        size_t capacity() { return capacity; }
        T* begin() { return arr; }
        T* end() { return arr + mSize; }
};