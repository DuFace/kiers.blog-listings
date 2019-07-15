//==============================================================================
// Implementation of a Python-style zip function in C++.
//------------------------------------------------------------------------------
// Copyright (c) 2019 Kier Dugan.
// MIT Licence, see LICENCE file for details.
//==============================================================================

#pragma once

#include <iterator>
#include <tuple>
#include <utility>
#include <vector>


namespace kjd
{

namespace detail
{

template <typename... Args> struct SizeHelper;

template <typename ArgN>
struct SizeHelper<ArgN> final
{
    static const size_t Size = 1;
};

template <typename ArgN, typename... Args>
struct SizeHelper<ArgN, Args...> final
{
    static const size_t Size = 1 + SizeHelper<Args...>::Size;
};

template <size_t N, size_t... Ns>
struct Indexes final
{};

template <typename... Args>
struct ParameterPack final
{
    static const size_t Size = SizeHelper<Args...>::Size;

};

template <typename Arg> struct IteratorHelper;

template <typename Arg>
struct IteratorHelper<std::vector<Arg>> final
{
    using CollectionType = std::vector<Arg>;
    using IteratorType   = typename CollectionType::const_iterator;

    static IteratorType begin(const CollectionType &collection_)
    {
        return std::begin(collection_);
    }

    static IteratorType end(const CollectionType &collection_)
    {
        return std::end(collection_);
    }
};

//template <typename Arg, typename... Args>
//struct IteratorHelper<Arg, Args...> final
//{
//};

template <typename... Args>
void discard(const Args &...) {}

template <typename... Args>
struct ZipperTraits final
{
    static const size_t ArgCount = SizeHelper<Args...>::Size;

    using IteratorTuple  = std::tuple<typename std::vector<Args>::const_iterator...>;
    using ReferenceTuple = std::tuple<const Args &...>;
    using Indices        = std::make_index_sequence<ArgCount>;

    static IteratorTuple begin(const std::vector<Args> &... input_)
    {
        return { IteratorHelper<std::vector<Args>>::begin(input_)... };
    }

    static IteratorTuple end(const std::vector<Args> &... input_)
    {
        return { IteratorHelper<std::vector<Args>>::end(input_)... };
    }

    static void advance(IteratorTuple &iter_)
    {
        advance_impl(iter_, Indices{});
    }

    static ReferenceTuple unpack(const IteratorTuple &iter_)
    {
        return unpack_impl(iter_, Indices{});
    }

private:
    template <size_t... Is>
    static void advance_impl(
        IteratorTuple &iter_, std::index_sequence<Is...>)
    {
        discard(++std::get<Is>(iter_)...);
    }

    template <size_t... Is>
    static ReferenceTuple unpack_impl(
        const IteratorTuple &iter_, std::index_sequence<Is...>)
    {
        return { (*std::get<Is>(iter_))... };
    }
};

template <typename... Args>
class Zipper final
{
    using Traits         = ZipperTraits<Args...>;
    using IteratorTuple  = typename Traits::IteratorTuple;
    using ReferenceTuple = typename Traits::ReferenceTuple;

public:
    struct const_iterator final
    {
        using difference_type   = std::ptrdiff_t;
        using value_type        = const ReferenceTuple;
        using pointer           = value_type *;
        using reference         = value_type &;
        using iterator_category = std::forward_iterator_tag;

        explicit const_iterator(IteratorTuple state_) :
            m_state(state_)
        {}

        bool operator== (const const_iterator &other_) const
            { return m_state == other_.m_state; }
        bool operator!= (const const_iterator &other_) const
            { return m_state != other_.m_state; }

        value_type operator++ ()
        {
            auto cur_values = values();
            Traits::advance(m_state);
            return cur_values;
        }
        value_type operator++ (int)
        {
            Traits::advance(m_state);
            return values();
        }

        value_type operator* () const
            { return values(); }

        value_type values() const
        {
            return Traits::unpack(m_state);
        }


    private:
        IteratorTuple m_state;
    };

    explicit Zipper(const std::vector<Args> &... args_) :
        m_begins(Traits::begin(args_...)),
        m_ends(Traits::end(args_...))
    {}

    const_iterator begin() const { return const_iterator(m_begins); }
    const_iterator end() const   { return const_iterator(m_ends); }

private:
    const IteratorTuple m_begins;
    const IteratorTuple m_ends;
};

}

template <typename... Args>
detail::Zipper<Args...> zip(const std::vector<Args> &... args_)
{
    return detail::Zipper<Args...> { args_... };
}

}
