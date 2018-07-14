#ifndef BURST_ITERATOR_JOIN_ITERATOR_HPP
#define BURST_ITERATOR_JOIN_ITERATOR_HPP

#include <burst/iterator/detail/join_iterator.hpp>
#include <burst/iterator/end_tag.hpp>
#include <burst/range/pure_traversal.hpp>

#include <boost/iterator/minimum_category.hpp>
#include <boost/range/value_type.hpp>

#include <iterator>
#include <utility>

namespace burst
{
    //!     Итератор склейки.
    /*!
            Предназначен для склейки нескольких диапазонов одного типа "на лету", то есть без
        использования дополнительной памяти для хранения результирующего диапазона.
            Принимает на вход набор диапазонов и перемещается по ним последовательно, то есть после
        последнего элемента i-го диапазона сразу переходит к первому элементу (i + 1)-го. Иначе
        говоря, в склеенном диапазоне вначале будут все элементы первой последовательности, затем
        все элементы второй, и т.д:

            join(R_1, ..., R_n) = R_1[1], ..., R_1[len_1], ..., R_n[1], ..., R_n[len_n]

        где R_i — i-й склеиваемый диапазон, len_i — длина i-го слеиваемого диапазона.
            Пустые диапазоны, естественно, игнорируются.

            Полученный в результате склейки диапазон изменяем. То есть из итератора можно не только
        прочитать значения, но можно и записать в него. В результате записи в итератор будет
        изменено значение в исходном хранилище.

        \tparam Range
            Тип принимаемого на вход диапазона.

            Категория итератора склейки.

            1. Итератор произвольного доступа.

               Если внешний диапазон и внутренние диапазоны одновременно являются диапазонами
               произвольного доступа, то итератор склейки будет итератором произвольного доступа,
               правда, с одной оговоркой: продвижение итератора на n шагов будет происходить не за
               O(1), а за O(|R|), где |R| — количество склеиваемых диапазонов.

            2. Однопроходный итератор.

               Если хотя бы один из типов диапазонов (внешний или внутренний) не является
               диапазоном произвольного доступа, то итератор склейки будет однопроходным
               итератором.

            Алгоритм работы

            Инвариант:
            Итератор склейки всегда установлен либо на элемент одного из непустых склеиваемых
        диапазонов, либо сразу за последним из склеиваемых диапазонов.
            Это условие выполняется сразу же при конструировании итератора склейки, а также после
        каждого продвижения итератора в любую сторону на любое количество позиций.
            Это справедливо и для случая произвольного доступа, и для однопроходного случая.

        I. Итератор произвольного доступа.
            1. Входной диапазон диапазонов копируется внутрь итератора.
            2. Заводятся два итератора: итератор во внешнем диапазоне и итератор в текущем
               внутреннем диапазоне.
            3. При продвижении итератора в какую-либо сторону изменяются только эти два итератора,
               а сам диапазон диапазонов остаётся нетронутым.

        II. Однопроходный итератор.
            1. Входной диапазон диапазонов копируется внутрь итератора.
            2. При продвижении итератора либо продвигается текущий внутренний диапазон, либо, если
               он закончился, внешний диапазон переходит к следующему внутреннему.
            3. Текущим элементом считается первый элемент текущего внутреннего диапазона.
     */
    template <typename Iterator>
    using join_iterator =
        detail::join_iterator_impl
        <
            Iterator,
            typename boost::iterators::minimum_category
            <
                typename boost::iterators::pure_iterator_traversal<Iterator>::type,
                typename pure_range_traversal<typename std::iterator_traits<Iterator>::value_type>::type
            >
            ::type
        >;

    //!     Функция для создания итератора склейки.
    /*!
            Принимает на вход набор диапазонов, которые нужно склеить.
            Возвращает итератор на первый элемент склеенного списка, который является первым
        элементом первого из непустых входных диапазонов.
     */
    template <typename Iterator>
    auto make_join_iterator (Iterator first, Iterator last)
    {
        return join_iterator<Iterator>(std::move(first), std::move(last));
    }

    template <typename Range>
    auto make_join_iterator (Range && ranges)
    {
        using std::begin;
        using std::end;
        return
            make_join_iterator
            (
                begin(std::forward<Range>(ranges)),
                end(std::forward<Range>(ranges))
            );
    }

    //!     Функция для создания итератора на конец склейки.
    /*!
            Принимает на вход итератор на начало склеенного диапазона и специальную метку,
        обозначающую, что нужно создать итератор на конец диапазона.
            Возвращает итератор на конец склеенного диапазона.
     */
    template <typename Iterator>
    auto make_join_iterator (iterator::end_tag_t, const join_iterator<Iterator> & begin)
    {
        return join_iterator<Iterator>(iterator::end_tag, begin);
    }
} // namespace burst

#endif // BURST_ITERATOR_JOIN_ITERATOR_HPP