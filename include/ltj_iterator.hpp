/*
 * ltj_iterator.hpp
 * Copyright (C) 2020 Author removed for double-blind evaluation
 *
 *
 * This is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This software is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef RING_LTJ_ITERATOR_HPP
#define RING_LTJ_ITERATOR_HPP

#define VERBOSE 0

#include <interfaceDual.h>
#include <vector>

namespace rdfcsa {

    template<class index_t, class var_t, class cons_t>
    class ltj_iterator {

    public:
        typedef cons_t value_type;
        typedef var_t var_type;
        typedef index_t index_type;
        typedef uint64_t size_type;
        //enum state_type {s, p, o};
        //std::vector<value_type> leap_result_type;

    private:
        const triple_pattern *m_ptr_triple_pattern;
        void* m_iterator;
        bool m_is_empty = false;


        void copy(const ltj_iterator &o) {
            m_ptr_triple_pattern = o.m_ptr_triple_pattern;
            m_is_empty = o.m_is_empty;
            m_iterator = cloneIterador_dual(m_iterator);
        }

        inline bool is_variable_subject(var_type var) {
            return m_ptr_triple_pattern->term_s.is_variable && var == m_ptr_triple_pattern->term_s.value;
        }

        inline bool is_variable_predicate(var_type var) {
            return m_ptr_triple_pattern->term_p.is_variable && var == m_ptr_triple_pattern->term_p.value;
        }

        inline bool is_variable_object(var_type var) {
            return m_ptr_triple_pattern->term_o.is_variable && var == m_ptr_triple_pattern->term_o.value;
        }

    public:
        const bool &is_empty = m_is_empty;

        ltj_iterator() = default;

        ltj_iterator(const triple_pattern *triple, index_type *index) {
            m_ptr_triple_pattern = triple;
            m_iterator = createIterator_dual(index->get_index());
            //Init current values and intervals according to the triple
            if (!m_ptr_triple_pattern->s_is_variable() && !m_ptr_triple_pattern->p_is_variable()
                && !m_ptr_triple_pattern->o_is_variable()) {

                //SPO
                auto res = init_spo_dual(m_iterator, m_ptr_triple_pattern->term_s.value,
                              m_ptr_triple_pattern->term_p.value, m_ptr_triple_pattern->term_o.value);

                if(res == 0){
                    m_is_empty = true;
                    return;
                }
            } else if (!m_ptr_triple_pattern->s_is_variable() && !m_ptr_triple_pattern->p_is_variable()) {
                //SP
                auto res = init_sp_dual(m_iterator, m_ptr_triple_pattern->term_s.value,
                                         m_ptr_triple_pattern->term_p.value);
                if(res == 0){
                    m_is_empty = true;
                    return;
                }

            } else if (!m_ptr_triple_pattern->p_is_variable() && !m_ptr_triple_pattern->o_is_variable()) {
                //PO

                auto res = init_po_dual(m_iterator,
                                         m_ptr_triple_pattern->term_p.value, m_ptr_triple_pattern->term_o.value);

                if(res == 0){
                    m_is_empty = true;
                    return;
                }

            } else if (!m_ptr_triple_pattern->s_is_variable() && !m_ptr_triple_pattern->o_is_variable()) {
                //SO to avoid forward steps
                auto res = init_so_dual(m_iterator, m_ptr_triple_pattern->term_s.value,
                                        m_ptr_triple_pattern->term_o.value);

                if(res == 0){
                    m_is_empty = true;
                    return;
                }

            } else if (!m_ptr_triple_pattern->s_is_variable()) {

                auto res = init_s_dual(m_iterator, m_ptr_triple_pattern->term_s.value);

                if(res == 0){
                    m_is_empty = true;
                    return;
                }

            } else if (!m_ptr_triple_pattern->p_is_variable()) {

                auto res = init_p_dual(m_iterator, m_ptr_triple_pattern->term_p.value);

                if(res == 0){
                    m_is_empty = true;
                    return;
                }

            } else if (!m_ptr_triple_pattern->o_is_variable()) {

                auto res = init_o_dual(m_iterator, m_ptr_triple_pattern->term_o.value);

                if(res == 0){
                    m_is_empty = true;
                    return;
                }

            }
        }

        //! Copy constructor
        ltj_iterator(const ltj_iterator &o) {
            copy(o);
        }

        //! Move constructor
        ltj_iterator(ltj_iterator &&o) {
            *this = std::move(o);
        }

        //! Copy Operator=
        ltj_iterator &operator=(const ltj_iterator &o) {
            if (this != &o) {
                copy(o);
            }
            return *this;
        }

        //! Move Operator=
        ltj_iterator &operator=(ltj_iterator &&o) {
            if (this != &o) {
                m_ptr_triple_pattern = std::move(o.m_ptr_triple_pattern);
                m_is_empty = o.m_is_empty;
                m_iterator = o.m_iterator;
            }
            return *this;
        }

        void swap(ltj_iterator &o) {
            // m_bp.swap(bp_support.m_bp); use set_vector to set the supported bit_vector
            std::swap(m_ptr_triple_pattern, o.m_ptr_triple_pattern);
            std::swap(m_is_empty, o.m_is_empty);
            std::swap(m_iterator, o.m_iterator);
        }

        void down(var_type var, size_type c) { //Go down in the trie
            if (is_variable_subject(var)) {
                down_dual(m_iterator, SUBJECT, c);
            } else if (is_variable_predicate(var)) {
                down_dual(m_iterator, PREDICATE, c);
            } else {
                down_dual(m_iterator, OBJECT, c);
            }
        };


        void up(var_type var) { //Go up in the trie
            up_dual(m_iterator);
        };

        value_type leap(var_type var, size_type c = 1) {
            if (is_variable_subject(var)) {
                return leap_dual(m_iterator, SUBJECT, c);
            } else if (is_variable_predicate(var)) {
                return leap_dual(m_iterator, PREDICATE, c);
            } else {
                return leap_dual(m_iterator, OBJECT, c);
            }
        };


        bool in_last_level(){
            return is_last_level_dual(m_iterator);
        }

        //Solo funciona en último nivel, en otro caso habría que reajustar

        std::vector<uint> seek_all(var_type var){
            if (is_variable_subject(var)){
                return get_all_dual(m_iterator, SUBJECT);
            }else if (is_variable_predicate(var)){
                return get_all_dual(m_iterator, PREDICATE);
            }else{
                return get_all_dual(m_iterator, OBJECT);
            }
        }

        size_type interval_size() const{
            return get_range_length_dual(m_iterator);
        }
    };

}

#endif //RING_LTJ_ITERATOR_HPP
