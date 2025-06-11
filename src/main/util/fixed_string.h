#ifndef FIXED_STRING_H
#define FIXED_STRING_H

#include <string_view>

// TODO time to start thinking about a commons library
template <size_t N>
struct fixed_string {
    //NOLINTNEXTLINE
    constexpr fixed_string(const char (&str)[N]) {
        for (size_t i = 0; i < N; ++i)
            data[i] = str[i];
    }

    [[nodiscard]]
    constexpr std::string_view view() const {
        return std::string_view{data, N - 1};
    }

    char data[N]{};

};

template <size_t N, size_t M>
constexpr auto operator+(const fixed_string<N>& a, const fixed_string<M>& b) {
    fixed_string<N + M> result{};
    for (size_t i = 0; i < N; ++i) result.value[i] = a.value[i];
    for (size_t i = 0; i <= M; ++i) result.value[N + i] = b.value[i];
    return result;
}

#endif //FIXED_STRING_H
