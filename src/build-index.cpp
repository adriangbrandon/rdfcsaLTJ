/*
 * build-index.cpp
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



#include <configuration.hpp>
#include <iostream>
#include <fstream>
#include <sdsl/construct.hpp>
#include <ltj_algorithm.hpp>

using namespace std;

using namespace std::chrono;
using timer = std::chrono::high_resolution_clock;

template<class index>
void build_index(const std::string &dataset, const std::string &output){


    memory_monitor::start();
    auto start = timer::now();

    index A(dataset);
    auto stop = timer::now();
    memory_monitor::stop();
    cout << "  Index built  " << A.bytes() << " bytes" << endl;

    A.save(output);
    cout << "Index saved" << endl;
    cout << duration_cast<seconds>(stop-start).count() << " seconds." << endl;
    cout << memory_monitor::peak() << " bytes." << endl;

}

int main(int argc, char **argv)
{

    if(argc != 2){
        std::cout << "Usage: " << argv[0] << " <dataset>" << std::endl;
        return 0;
    }

    std::string dataset = argv[1];
    std::string index_name = dataset + ".rdfcsa";
    build_index<rdfcsa::rdfcsa_dual>(dataset, index_name);

    return 0;
}

