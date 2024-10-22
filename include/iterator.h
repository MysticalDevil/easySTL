#pragma once

#ifndef EASYSTL_ITERATOR_H_
#define EASY_STL_ITERATOR_H_

#include <cstddef>

namespace easystl {
    /**
    * @class TrueType
    * @brief Helper class for type traits, used to represent `true` in type checking.
    */
    class TrueType {
    public:
        static constexpr bool value = true; ///< Constant `true` value.
    };

    /**
    * @class FalseType
    * @brief Helper class for type traits, used to represent `false` in type checking.
    */
    class FalseType {
    public:
        static constexpr bool value = false; ///< Constant `false` value.
    };

    /**
     * @typedef void_t
     * @brief Helper alias template that resolves to `void` for SFINAE-based templated specialization.
     */
    template<typename...>
    using void_t = void;

    /**
    * @class InputIteratorTag
    * @brief Tag class representing the input iterator category.
    */
    class InputIteratorTag {
    };

    /**
    * @class OutputIteratorTag
    * @brief Tag class representing the output iterator category.
    */
    class OutputIteratorTag {
    };

    /**
    * @class ForwardIteratorTag
    * @brief Tag class representing the forward iterator category, derived from InputIteratorTag.
    */
    class ForwardIteratorTag : public InputIteratorTag {
    };

    /**
    * @class BidirectionalIteratorTag
    * @brief Tag class representing the bidirectional iterator category, derived from FrowardIteratorTag.
    */
    class BidirectionalIteratorTag : public ForwardIteratorTag {
    };

    /**
    * @class RandomAccessIteratorTag
    * @brief Tag class representing the random access iterator category, derived from BidirectionalIteratorTag.
    */
    class RandomAccessIteratorTag : public BidirectionalIteratorTag {
    };

    /**
    * @class Iterator
    * @brief Template class representing a general iterator.
    * @tparam Category The category of the iterator (e.g., InputIteratorTag.).
    * @tparam T The type of the elements pointed to by the iterator.
    * @tparam distance The difference type, representing the distance between two iterators.
    * @tparam pointer The pointer type.
    * @tparam reference The reference type.
    */
    template<class Category,
        class T,
        class distance = std::ptrdiff_t,
        class pointer = T *,
        class reference = T &>
    class Iterator {
        using IteratorCategory = Category; ///< The category type of the iterator.
        using ValueType = T; ///< The value type of the iterator.
        using Pointer = pointer; ///< The pointer for the iterator.
        using Reference = reference; ///< The reference for the iterator.
        using DifferenceType = distance; ///< The difference type, used for pointer arithmetic.
    };

    /**
    * @class IteratorTraits
    * @brief Traits class template for extracting iterator properties.
    * Primary template for non-iterators (SFINAE fallback).
    * @tparam T The type to be specialized.
    * `void` Specialization parameter used for SFINAE.
    */
    template<class T, class = void>
    class IteratorTraits {
    };

    /**
    * @class IteratorTraits
    * @brief Specialization of IteratorTraits for types that define `IteratorTraits`.
    * @tparam T A class type that defines iterator properties.
    */
    template<class T>
    class IteratorTraits<T, void_t<typename T::IteratorCategory> > {
    public:
        using IteratorCategory = typename T::IteratorCategory; ///< The iterator category type.
        using ValueType = typename T::ValueType; ///< The value type.
        using Pointer = typename T::Pointer; ///< The pointer type.
        using Reference = typename T::Reference; ///< The reference type.
        using DifferenceType = typename T::DifferenceType; ///< The difference type.
    };

    /**
    * @class IteratorTraits
    * @brief Specialization of IteratorTraits for pointer types.
    * @tparam T The type of the elements pointed to.
    */
    template<class T>
    class IteratorTraits<T *, void> {
    public:
        using IteratorCategory = RandomAccessIteratorTag; ///< Iterator category for raw pointers.
        using ValueType = T; ///< The value type of the pointer.
        using Pointer = T *; ///< The pointer value.
        using Reference = T &; ///< The reference value.
        using DifferenceType = std::ptrdiff_t; ///< The difference type.
    };

    /**
    * @class IteratorTraits
    * @brief Specialization of IteratorTraits for const pointer types.
    * @tparam T The type of the elements pointed to.
    */
    template<class T>
    class IteratorTraits<const T *, void> {
    public:
        using IteratorCategory = RandomAccessIteratorTag; ///< Iterator category for const raw pointers.
        using ValueType = T; ///< The value type of the pointer.
        using Pointer = const T *; ///< The pointer type.
        using Reference = T &; ///< The reference type.
        using DifferenceType = std::ptrdiff_t; /// The difference type.
    };

    /**
    * @class IsIterator
    * @brief Helper class to determine if a type is an iterator by checking if it has `IteratorCategory`.
    * @tparam T The type to be checked.
    */
    template<class T>
    class IsIterator {
    private:
        // Fallback if T does not have an IteratorCategory.
        template<class U>
        static auto test(...) -> FalseType;

        // Checks if T has an IteratorCategory
        template<class U>
        static auto test(int) -> decltype(typename IteratorTraits<U>::IteratorCategory(), TrueType());

    public:
        static const bool value = decltype(test<T>(0))::value; ///< True if T is an iterator, false otherwise.
    };

    /**
    * @brief Returns the iterator category of an iterator.
    * @tparam Iterator The type of the iterator.
    * @param{unnamed} The iterator instance.
    * @return The iterator category of the given iterator.
    */
    template<class Iterator>
    auto IteratorCategory(const Iterator &) -> typename IteratorTraits<Iterator>::IteratorCategory {
        typedef typename IteratorTraits<Iterator>::IteratorCategory Category;
        return Category();
    }

    /**
    * @brief Returns the difference typeof an iterator.
    * @tparam Iterator The type of the iterator.
    * @param{unnamed} The iterator instance.
    * @return The difference type of the given iterator.
    */
    template<class Iterator>
    auto DistanceType(const Iterator &) -> typename IteratorTraits<Iterator>::DifferenceType * {
        return nullptr;
    }

    /**
    * @brief Returns the value typeof an iterator.
    * @tparam Iterator The type of the iterator.
    * @param{unnamed} The iterator instance.
    * @return The value type of the given iterator.
    */
    template<class Iterator>
    auto ValueType(const Iterator &) -> typename IteratorTraits<Iterator>::ValueType * {
        return nullptr;
    }

    /**
    * @brief Calculates the distance between two input iterators.
    * @tparam InputIterator The type of the input iterator.
    * @param first The first iterator.
    * @param last The last iterator.
    * @param{unnamed} The category tag for input iterators.
    * @return The distance between the two input iterators.
    */
    template<class InputIterator>
    auto _distance(InputIterator first, InputIterator last,
                   InputIteratorTag) -> typename IteratorTraits<InputIterator>::DifferenceType {
        typename IteratorTraits<InputIterator>::DifferenceType n = 0;
        while (first != last) {
            ++first;
            ++n;
        }
        return n;
    }

    /**
    * @brief Calculates the distance between two input iterators.
    * @tparam RandomAccessIterator The type of the random access iterator.
    * @param first The first iterator.
    * @param last The last iterator.
    * @return The distance between the two random access iterators.
    */
    template<class RandomAccessIterator>
    auto _distance(RandomAccessIterator first, RandomAccessIterator last, RandomAccessIteratorTag) {
        return last - first;
    }

    /**
    * @brief Computes the distance between two iterators.
    * @tparam InputIterator The type of the input iterator.
    * @param first The first iterator.
    * @param last The last iterator.
    * @return The distance between the two iterators.
    */
    template<class InputIterator>
    auto Distance(InputIterator first, InputIterator last) -> typename IteratorTraits<InputIterator>::DifferenceType {
        return _distance(first, last, IteratorCategory(first));
    }

    /**
    * @brief Advances an input iterator by a given distance.
    * @tparam InputIterator The type of the input iterator.
    * @tparam Distance The distance of the distance.
    * @param i The iterator to be advanced.
    * @param n The number of steps to advance.
    * @param{unnamed} The category tag for input iterators.
    */
    template<class InputIterator, class Distance>
    auto _advance(InputIterator &i, Distance n, InputIteratorTag) -> void {
        while (n--) { ++i; }
    }

    /**
    * @brief Advance a bidirectional iterator by a given distance.
    * @tparam BidirectionalIterator The type of the bidirectional iterator.
    * @tparam Distance The distance of the distance.
    * @param i The iterator to be advanced.
    * @param n The number of steps to advanced.
    * @param{unnamed} The category tag for bidirectional iterators.
    */
    template<class BidirectionalIterator, class Distance>
    auto _advance(BidirectionalIterator &i, Distance n, BidirectionalIteratorTag) -> void {
        if (n > 0) {
            while (n--) { ++i; }
        } else {
            while (n++) { --i; }
        }
    }

    /**
    * @brief Advance a random access iterator by a given distance.
    * @tparam RandomAccessIterator The type of the random access iterator.
    * @tparam Distance Ths distance of the distance.
    * @param i The iterator to be advanced.
    * @param n The number of steps to advanced.
    * @param{unnamed} The category tag for random access iterators.
    */
    template<class RandomAccessIterator, class Distance>
    auto _advance(RandomAccessIterator &i, Distance n, RandomAccessIteratorTag) -> void {
        i += n;
    }

    /**
    * @brief Advances an iterator by a given distance.
    * @tparam InputIterator The type of the input iterator.
    * @tparam Distance The type representing the distance to advance.
    * @param i The iterator to be advanced.
    * @param n The number of steps to advance.
    */
    template<class InputIterator, class Distance>
    auto Advance(InputIterator &i, Distance n) -> void {
        _advance(i, n, IteratorCategory(i));
    }
} // namespace easystl

#endif // !EASYSTL_ITERATOR_H_
