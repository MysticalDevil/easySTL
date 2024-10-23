#pragma once

#ifndef EASYSTL_ALGO_H
#define EASYSTL_ALGO_H

namespace easystl {
    /**
    * @brief Returns the maximum of two values.
    * @tparam T The type of the values.
    * @param a First value.
    * @param b Second value.
    * @return The maximum value.
    */
    template<typename T>
    auto Max(const T &a, const T &b) noexcept -> const T & { return a > b ? a : b; }

    /**
    * @brief Copies elements from one range to another.
    * @tparam InputIterator The type of the input iterator.
    * @tparam OutputIterator The type of the output iterator.
    * @param first The beginning of the range to copy from.
    * @param last The end of the range to copy from.
    * @param result The beginning of the range to copy to.
    * @return The iterator to the end of the destination range.
    */
    template<typename InputIterator, typename OutputIterator>
    auto Copy(InputIterator first, InputIterator last, OutputIterator result) noexcept -> OutputIterator {
        while (first != last) {
            *result = *first;
            ++result;
            ++first;
        }
        return result;
    }

    /**
    * @brief Copies elements from one range to another in reverse.
    * @tparam BidirectionalIterator1 The type of the input iterator.
    * @tparam BidirectionalIterator2 The type of the output iterator.
    * @param first The beginning of the range to copy from.
    * @param last The end of the range to copy from.
    * @param result The iterator to the end of the destination range.
    * @return The iterator to the beginning of the destination range.
    */
    template<typename BidirectionalIterator1, typename BidirectionalIterator2>
    auto CopyBackward(BidirectionalIterator1 first, BidirectionalIterator1 last,
                      BidirectionalIterator2 result) -> BidirectionalIterator2 {
        while (first != last) {
            *(--result) = *(--last);
        }
        return result;
    }

    /**
    * @brief Fills a range with a specified value.
    * @tparam ForwardIterator The type of the forward iterator.
    * @tparam T The type of the value.
    * @param first The beginning of the range to fill.
    * @param last The end of the range to fill.
    * @param value The value to fill the range with.
    */
    template<typename ForwardIterator, typename T>
    auto Fill(ForwardIterator first, ForwardIterator last, const T &value) noexcept -> void {
        while (first != last) {
            *first = value;
            ++first;
        }
    }

    /**
    * @brief Swaps two values.
    * @tparam T The type of the values.
    * @param a First value.
    * @param b Second value.
    */
    template<typename T>
    auto Swap(T &a, T &b) noexcept -> void {
        T temp = a;
        a = b;
        b = temp;
    }
} // namespace easystl

#endif // !EASYSTL_ALGO_H
