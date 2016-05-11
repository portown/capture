// optional.hpp

#if __has_include(<optional>)
#include <optional>
#elif __has_include(<experimental/optional>)
#include <experimental/optional>

namespace std {
    using experimental::optional;
    using experimental::make_optional;
    using experimental::nullopt;
    using experimental::in_place;
    using experimental::bad_optional_access;
}
#else
#error No optionals
#endif
