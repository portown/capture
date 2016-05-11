// string_view.hpp

#pragma once

#if __has_include(<string_view>)
#include <string_view>
#elif __has_include(<experimental/string_view>)
#include <experimental/string_view>

namespace std {
    using experimental::basic_string_view;
    using experimental::string_view;
    using experimental::wstring_view;
    using experimental::u16string_view;
    using experimental::u32string_view;
}
#else
#error No string_view
#endif
