#include <iostream>
#include <boost/python.hpp>
#include <boost/python/suite/indexing/vector_indexing_suite.hpp>
#include <boost/python/suite/indexing/map_indexing_suite.hpp>
#include <Eigen/Core>

#include "r1_lc.hpp"


using namespace Eigen;
using namespace std;
using namespace cbasis;
using namespace boost::python;

int add(int a, int b) {
  return a+b;
}
  

void BindR1LC() {

  register_ptr_to_python<LC_STOs>();
  def("LC_STOs", &create_LC_STOs);
  class_<_LC_STOs>("_LC_STOs", init<>())
    .def("size", &_LC_STOs::size)
    .def("add", &_LC_STOs::Add, return_self<>())
    .def("str",     &_LC_STOs::str)
    .def("at_r", &_LC_STOs::AtR)
    .def("conj",     &_LC_STOs::Conj);

  /* 
     
     
    .def("d_at_r", &_LC_STOs::DAtR)
    .def("d2_at_r", &_LC_STOs::D2AtR)
  */

  /*
    
  class_<_LC_STOs>("_LC_STOs", init<>())
    .def("size", &_LC_STOs::size)
    .def("at_r", &_LC_STOs::AtR)
    .def("d_at_r", &_LC_STOs::DAtR)
    .def("d2_at_r", &_LC_STOs::D2AtR)
    .def("add",     &_LC_STOs::Add)
    .def("conj",     &_LC_STOs::Conj);

  register_ptr_to_python<LC_GTOs>();
  class_<_LC_GTOs>("_LC_GTOs", init<>())
    .def("size", &_LC_GTOs::size)
    .def("at_r", &_LC_GTOs::AtR)
    .def("d_at_r", &_LC_GTOs::DAtR)
    .def("d2_at_r", &_LC_GTOs::D2AtR)
    .def("add",     &_LC_GTOs::Add)
    .def("conj",     &_LC_GTOs::Conj);
  */
  
}



BOOST_PYTHON_MODULE(r1basis_bind) {

  Py_Initialize();
  BindR1LC();
  def("add", &add);
  
}
