#ifndef ELEMENT_HPP
#define ELEMENT_HPP

#include <vector>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/serialization/vector.hpp>

class Element {
    private:
        friend class boost::serialization::access;
        template<class Archive>
        void serialize(Archive & ar, const unsigned int version) {
          ar & x1 & y1;
          ar & x2 & y2;
          ar & k & l;
          ar & dofs;
        }        
    public:
        uint64_t x1, y1;
        uint64_t x2, y2;
        uint64_t k, l;
        std::vector<uint64_t> dofs;
};

#endif // ELEMENT_HPP
