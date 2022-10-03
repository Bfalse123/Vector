# Vector
Vector working with raw memory \
Vector's methods:
```cpp
void Reserve(size_t n);
void Resize(size_t n);
void PushBack(const T& elem);
void PushBack(T&& elem);
T& EmplaceBack(Args&&... args);
void PopBack();
size_t Size() const;
size_t Capacity() const;
iterator begin();
iterator end();
const_iterator begin() const;
const_iterator end() const;
const_iterator cbegin() const;
const_iterator cend() const;
iterator Insert(const_iterator pos, const T& elem);
iterator Insert(const_iterator pos, T&& elem);
iterator Emplace(const_iterator it, Args&&... args);
iterator Erase(const_iterator it);
```