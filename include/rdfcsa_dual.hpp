/***
BSD 2-Clause License

Copyright (c) 2018, Adrián
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

* Redistributions of source code must retain the above copyright notice, this
  list of conditions and the following disclaimer.

* Redistributions in binary form must reproduce the above copyright notice,
  this list of conditions and the following disclaimer in the documentation
  and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
**/


//
// Created by Adrián on 17/1/23.
//

#ifndef RING_RDFCSA_DUAL_HPP
#define RING_RDFCSA_DUAL_HPP

#include <interfaceDual.h>
#include <configuration.hpp>

namespace rdfcsa {

    class rdfcsa_dual {

    public:
        typedef uint64_t size_type;
        typedef uint64_t value_type;

    private:
        void* m_rdfcsa_index;

        void copy(const rdfcsa_dual &o){
            m_rdfcsa_index = o.m_rdfcsa_index; //realmente non copia que son punteiros
        }

    public:

        rdfcsa_dual() = default;

        rdfcsa_dual(const std::string &file_name){
            char* build_options = const_cast<char *>(BUILD_OPTIONS.c_str());
            build_index_dual(file_name.c_str(), build_options, &m_rdfcsa_index);
        }

        ~rdfcsa_dual(){
            std::cout << "Freeing RdfCsa Dual" << std::endl;
            free_index_dual(m_rdfcsa_index);
        }

        //! Copy constructor
        rdfcsa_dual(const rdfcsa_dual &o) {
            copy(o);
        }

        //! Move constructor
        rdfcsa_dual(rdfcsa_dual &&o) {
            *this = std::move(o);
        }

        //! Copy Operator=
        rdfcsa_dual &operator=(const rdfcsa_dual &o) {
            if (this != &o) {
                m_rdfcsa_index = o.m_rdfcsa_index;
            }
            return *this;
        }

        //! Move Operator=
        rdfcsa_dual &operator=(rdfcsa_dual &&o) {
            if (this != &o) {
                m_rdfcsa_index = o.m_rdfcsa_index;
            }
            return *this;
        }

        void swap(rdfcsa_dual &o) {
            std::swap(m_rdfcsa_index, o.m_rdfcsa_index);
        }

        //! Serializes the data structure into the given file
        void save(const std::string &file_name) const {
            save_index_dual(m_rdfcsa_index, file_name.c_str());
        }

        void load(const std::string &file_name){
            load_index_dual(file_name.c_str(), &m_rdfcsa_index);
        }

        size_type bytes(){
            ulong size;
            index_size_dual(m_rdfcsa_index, &size);
            return size;
        }

        inline void* get_index(){
            return m_rdfcsa_index;
        }


    };
}

#endif //RING_RDFCSA_DUAL_HPP
