#include <meta.hpp/headers/meta.hpp/meta_all.hpp>

using metadata_multimap = std::multimap<std::string, std::string, std::less<>>;
template < meta_hpp::class_kind Class >
meta_hpp::class_bind<Class>::class_bind(metadata_multimap metadata)
: type_bind_base{resolve_type<Class>(), std::move(metadata)} {
    if constexpr ( std::is_destructible_v<Class> ) {
        if ( get_data().destructors.empty() ) {
            destructor_();
        }
    }
}
