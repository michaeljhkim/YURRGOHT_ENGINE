#pragma once
#include "meta_multi_insert.hpp"


namespace meta_hpp
{
    using metadata_multimap = std::multimap<std::string, uvalue, std::less<>>;
}


namespace meta_hpp
{
    template <bool is_multi = false>
    class metadata_bind final {
    public:
        using values_t = std::conditional_t<is_multi, metadata_multimap, metadata_map>;

        operator values_t() &&;

        template < typename... Opts >
        static values_t from_opts(Opts&&... opts);

    public:
        metadata_bind() = default;
        ~metadata_bind() = default;

        metadata_bind(metadata_bind&&) = default;
        metadata_bind& operator=(metadata_bind&&) = default;

        metadata_bind(const metadata_bind&) = delete;
        metadata_bind& operator=(const metadata_bind&) = delete;

        metadata_bind& operator()(std::string name, uvalue value) &;
        metadata_bind operator()(std::string name, uvalue value) &&;

        metadata_bind& operator()(metadata_bind bind) &;
        metadata_bind operator()(metadata_bind bind) &&;

    private:
        values_t values_;
    };
    
    template < bool is_multi >
    inline metadata_bind<is_multi> metadata_() {
        return metadata_bind<false>{};
    }

    template < bool is_multi >
    inline metadata_bind<is_multi> metadata_multi_() {
        return metadata_bind<true>{};
    }

    template < bool is_multi >
    inline metadata_bind<is_multi>::operator values_t() && {
        return std::move(values_);
    }

    template < bool is_multi >
    template < typename... Opts >
    metadata_bind<is_multi>::values_t metadata_bind<is_multi>::from_opts(Opts&&... opts) {
        metadata_bind<is_multi> bind;

        const auto process_opt = detail::overloaded{
            [&bind](metadata_bind b) { bind(std::move(b)); }, //
            [](auto&&) {},                                    // ignore other opts
        };

        (process_opt(META_HPP_FWD(opts)), ...);
        return std::move(bind.values_);
    }

    template < bool is_multi >
    inline metadata_bind<is_multi>& metadata_bind<is_multi>::operator()(std::string name, uvalue value) & {
        //values_.insert_or_assign(std::move(name), std::move(value));
        (constexpr is_multi) ?
            values_.insert( std::pair<std::string, uvalue>(std::move(name), std::move(value)) ) :
            values_.insert_or_assign(std::move(name), std::move(value));

        return *this;
    }

    template < bool is_multi >
    inline metadata_bind<is_multi> metadata_bind<is_multi>::operator()(std::string name, uvalue value) && {
        (*this)(std::move(name), std::move(value));
        return std::move(*this);
    }

    template < bool is_multi >
    inline metadata_bind<is_multi>& metadata_bind<is_multi>::operator()(metadata_bind<is_multi> bind) & {
        //detail::insert_or_assign(values_, std::move(bind.values_));
        (constexpr is_multi) ?
            detail::insert(values_, std::move(bind.values_)) :
            detail::insert_or_assign(values_, std::move(bind.values_));
            
        return *this;
    }
    
    template < bool is_multi >
    inline metadata_bind<is_multi> metadata_bind<is_multi>::operator()(metadata_bind<is_multi> bind) && {
        (*this)(std::move(bind));
        return std::move(*this);
    }
}
