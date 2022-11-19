#pragma once
#include <limits>
#include <type_traits>

namespace persistence
{
    /**
     * True if the value is assignable to the numeric type without loss of precision.
     * @tparam T The numeric type to assign to.
     * @tparam V The numeric type of the value.
     * @param value The value to assign.
     */
    template<typename T, typename V>
    constexpr static bool is_assignable(V value)
    {
        if constexpr (std::is_integral_v<T>) {
            if constexpr (std::is_floating_point_v<V>) {
                // loss of precision when casting to integer
                return false;
            } else if constexpr (std::is_integral_v<V>) {
                if constexpr (std::is_signed_v<T> && std::is_signed_v<V>) {
                    if constexpr (sizeof(T) >= sizeof(V)) {
                        // non-narrowing assignment, e.g. int64_t <-- int or int64_t <-- int64_t
                        return true;
                    } else {
                        // narrowing assignment, check if value fits in signed range
                        return value >= std::numeric_limits<T>::min() && value <= std::numeric_limits<T>::max();
                    }
                } else if constexpr (!std::is_signed_v<T> && !std::is_signed_v<V>) {
                    if constexpr (sizeof(T) >= sizeof(V)) {
                        // non-narrowing assignment, e.g. uint64_t <-- unsigned or uint64_t <-- uint64_t
                        return true;
                    } else {
                        // narrowing assignment, check if value fits in unsigned range
                        return value <= std::numeric_limits<T>::max();
                    }
                } else if constexpr (!std::is_signed_v<T> && std::is_signed_v<V>) {
                    if constexpr (sizeof(T) >= sizeof(V)) {
                        // non-narrowing assignment, all non-negative signed values fit into unsigned
                        return value >= 0;
                    } else {
                        // narrowing assignment, check if non-negative value fits in unsigned range
                        return value >= 0 && static_cast<std::make_unsigned_t<V>>(value) <= std::numeric_limits<T>::max();
                    }
                } else if constexpr (std::is_signed_v<T> && !std::is_signed_v<V>) {
                    if constexpr (sizeof(T) > sizeof(V)) {
                        // widening assignment, all values fit
                        return true;
                    } else {
                        // non-widening assignment, check if unsigned value is at most signed integer upper limit
                        return value <= static_cast<std::make_unsigned_t<T>>(std::numeric_limits<T>::max());
                    }
                } else {
                    // unreachable
                    return false;
                }
            } else {
                // wrong operand type
                return false;
            }
        } else if constexpr (std::is_floating_point_v<T>) {
            if constexpr (std::is_floating_point_v<V>) {
                if constexpr (sizeof(T) >= sizeof(V)) {
                    // non-narrowing assignment
                    return true;
                } else {
                    // check if value is within range of receiving floating point type
                    return value >= std::numeric_limits<T>::lowest() && value <= std::numeric_limits<T>::max();
                }
            } else if constexpr (std::is_integral_v<V>) {
                if constexpr (std::numeric_limits<T>::digits >= std::numeric_limits<V>::digits) {
                    // integer binary digits fit in floating point mantissa
                    return true;
                } else if constexpr (std::is_signed_v<V>) {
                    // check if signed value fits in floating point type mantissa range
                    constexpr V limit = static_cast<V>(1) << std::numeric_limits<T>::digits;
                    return value > -limit && value < limit;
                } else if constexpr (!std::is_signed_v<V>) {
                    // check if unsigned value fits in floating point type mantissa range
                    constexpr V limit = static_cast<V>(1) << std::numeric_limits<T>::digits;
                    return value < limit;
                } else {
                    // unreachable
                    return false;
                }
            } else {
                // wrong operand type
                return false;
            }
        } else {
            // wrong assignee type
            return false;
        }
    }
}
