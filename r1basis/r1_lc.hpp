#ifndef R1_LC_HPP
#define R1_LC_HPP

#include <vector>
#include <boost/shared_ptr.hpp>
#include <Eigen/Core>
#include "../utils/typedef.hpp"

/**
   Linear combination of functions
 */

namespace cbasis {

  template<int M>
  class _LC_EXPs {
  private:
    // ---- Member field ----
    std::vector<dcomplex> cs;
    std::vector<int>      ns;
    std::vector<dcomplex> zs;
    
  public:
    // ---- typedef ----
    typedef boost::shared_ptr<_LC_EXPs<M> > LC_EXPs;
    
  public:
    // ---- Constructors ----

    // ---- Utils ----
    int max_n() const;
    int power() const;

    // ---- Getter ----
    int size() const;
    dcomplex c(int i) const { return cs[i]; }
    int      n(int i) const { return ns[i]; }
    dcomplex z(int i) const { return zs[i]; }
    dcomplex& c(int i) { return cs[i]; }
    int&      n(int i) { return ns[i]; }
    dcomplex& z(int i) { return zs[i]; }
    void set_c(int i, dcomplex c) { cs[i]=c; }
    void set_n(int i, int n) {      ns[i]=n; }
    void set_z(int i, dcomplex z) { zs[i]=z; }    

    Eigen::VectorXcd AtR(const Eigen::VectorXcd&) const;
    Eigen::VectorXcd DAtR(const Eigen::VectorXcd&) const;
    std::string str() const;

    // ---- Setter ----
    _LC_EXPs<M>* Add(dcomplex c, int n, dcomplex z);

    // ---- Generate other ----
    LC_EXPs Clone() const;
    LC_EXPs Conj() const;            
    
  };

  typedef boost::shared_ptr<_LC_EXPs<1> > LC_STOs;
  typedef boost::shared_ptr<_LC_EXPs<2> > LC_GTOs;
  template<int m>
  boost::shared_ptr<_LC_EXPs<m> > Create_LC_EXPs();
  LC_STOs Create_LC_STOs();
  LC_GTOs Create_LC_GTOs();
}


#endif
