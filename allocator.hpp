#include <cstdlib>
#include <limits>

namespace g {

template<typename T>
class ObjPool {
public:
    constexpr ObjPool() noexcept;
    constexpr ObjPool(const ObjPool &other) noexcept;

    T* address(T& x) const noexcept;
    T* address(const T& x) const noexcept;
    [[nodiscard]] constexpr T* allocate(size_t n);
    constexpr void deallocate(T *ptr, size_t n);
    size_t max_size() const noexcept;

private:

};

template<typename T>
size_t ObjPool<T>::max_size() const noexcept
{
    return std::numeric_limits<size_t>::max() / sizeof(T);
}


} /* namespace g */
