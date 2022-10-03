#include <cstddef>
#include <memory>

template <typename T>
class Vector {
   public:
    Vector() = default;
    Vector(size_t n) : data(n) {
        std::uninitialized_value_construct_n(data.buf, n);
        sz = n;
    }

    void Swap(Vector&& other) {
        data.Swap(other.data);
        std::swap(sz, other.sz);
    }

    Vector(const Vector& other) : data(other.sz) {
        std::uninitialized_copy_n(other.data.buf, other.sz, data.buf);
        sz = other.sz;
    }

    Vector(Vector&& other) {
        Swap(std::move(other));
    }

    ~Vector() {
        std::destroy_n(data.buf, sz);
    }

    Vector& operator=(const Vector& other) {
        if (other.sz > data.capacity) {
            Vector tmp(other);
            Swap(std::move(tmp));
        } else {
            for (size_t i = 0; i < sz && i < other.sz; ++i) {
                data[i] = other.data[i];
            }
            if (sz < other.sz) {
                std::uninitialized_copy_n(other.data + sz, other.sz - sz, data.buf + sz);
            } else if (sz > other.sz) {
                std::destroy_n(data + other.sz, sz - other.sz);
            }
        }
        sz = other.sz;
        return *this;
    }

    Vector& operator=(Vector&& other) noexcept {
        Swap(std::move(other));
        return *this;
    }

    void Reserve(size_t n) {
        if (n <= data.capacity) return;
        RawMemory data2(n);
        std::uninitialized_move_n(data.buf, sz, data2.buf);
        std::destroy_n(data.buf, data.capacity);
        data.Swap(data2);
    }

    void Resize(size_t n) {
        Reserve(n);
        if (n < sz) {
            std::destroy_n(data + n, sz - n);
        } else if (n > sz) {
            std::uninitialized_value_construct_n(data + sz, n - sz);
        }
        sz = n;
    }

    void PushBack(const T& elem) {
        if (sz == data.capacity) {
            Reserve(sz == 0 ? 1 : 2 * sz);
        }
        new (data + sz) T(elem);
        ++sz;
    }

    void PushBack(T&& elem) {
        if (sz == data.capacity) {
            Reserve(sz == 0 ? 1 : 2 * sz);
        }
        new (data + sz) T(std::move(elem));
        ++sz;
    }

    template <typename... Args>
    T& EmplaceBack(Args&&... args) {
        if (sz == data.capacity) {
            Reserve(sz == 0 ? 1 : 2 * sz);
        }
        new (data + sz) T(std::forward<Args>(args)...);
        return data[sz++];
    }

    void PopBack() {
        std::destroy_at(data + sz - 1);
        sz--;
    }

    size_t Size() const noexcept {
        return sz;
    }

    size_t Capacity() const noexcept {
        return data.capacity;
    }

    const T& operator[](size_t i) const {
        return data[i];
    }

    T& operator[](size_t i) {
        return data[i];
    }

    // В данной части задачи реализуйте дополнительно эти функции:
    using iterator = T*;
    using const_iterator = const T*;

    iterator begin() noexcept {
        return data.buf;
    }

    iterator end() noexcept {
        return data + sz;
    }

    const_iterator begin() const noexcept {
        return data.buf;
    }

    const_iterator end() const noexcept {
        return data + sz;
    }

    const_iterator cbegin() const noexcept {
        return data.buf;
    }

    const_iterator cend() const noexcept {
        return data + sz;
    }

    void SwapIterators(const_iterator a, const_iterator b) {
    }
    // Вставляет элемент перед pos
    // Возвращает итератор на вставленный элемент
    iterator Insert(const_iterator pos, const T& elem) {
        size_t ans = pos - cbegin();
        PushBack(elem);
        size_t prev = sz - 1;
        for (size_t i = ans; i < sz; ++i) {
            T tmp = std::move(data[i]);
            data[i] = std::move(data[prev]);
            data[prev] = std::move(tmp);
        }
        return (data + ans);
    }

    iterator Insert(const_iterator pos, T&& elem) {
        size_t ans = pos - cbegin();
        PushBack(std::move(elem));
        size_t prev = sz - 1;
        for (size_t i = ans; i < sz; ++i) {
            T tmp = std::move(data[i]);
            data[i] = std::move(data[prev]);
            data[prev] = std::move(tmp);
        }
        return (data + ans);
    }

    // Конструирует элемент по заданным аргументам конструктора перед pos
    // Возвращает итератор на вставленный элемент
    template <typename... Args>
    iterator Emplace(const_iterator it, Args&&... args) {
        size_t ans = it - cbegin();
        EmplaceBack(std::forward<Args>(args)...);
        size_t prev = sz - 1;
        for (size_t i = ans; i < sz; ++i) {
            T tmp = std::move(data[i]);
            data[i] = std::move(data[prev]);
            data[prev] = std::move(tmp);
        }
        return (data + ans);
    }

    // Удаляет элемент на позиции pos
    // Возвращает итератор на элемент, следующий за удалённым
    iterator Erase(const_iterator it) {
        size_t ans = it - cbegin();
        std::destroy_at(it);
        for (size_t i = ans, next = i + 1; next < sz; ++next, ++i) {
            data[i] = std::move(data[next]);
        }
        PopBack();
        return (data + ans);
    }

   private:
    struct RawMemory {
        size_t capacity = 0;
        T* buf = nullptr;

        static T* allocate(size_t n) {
            return static_cast<T*>(operator new(n * sizeof(T)));
        }

        static void deallocate(T* buf) {
            operator delete(buf);
        }

        T* operator+(size_t i) {
            return buf + i;
        }

        const T* operator+(size_t i) const {
            return buf + i;
        }

        T& operator[](size_t i) {
            return buf[i];
        }

        const T& operator[](size_t i) const {
            return buf[i];
        }

        void Swap(RawMemory& other) {
            std::swap(buf, other.buf);
            std::swap(capacity, other.capacity);
        }

        RawMemory() = default;
        RawMemory& operator=(const RawMemory&) = delete;
        RawMemory(size_t n) : buf(allocate(n)), capacity(n) {
        }
        RawMemory& operator=(RawMemory&& other) {
            Swap(std::move(other));
        }

        ~RawMemory() {
            deallocate(buf);
        }
    };

    size_t sz = 0;
    RawMemory data;
};