#include <stdexcept>
#include "../src_cpp/macros.hpp"
#include "r1basis.hpp"
#include "r1_lc.hpp"

using namespace std;
using namespace Eigen;

namespace cbasis {

  // ==== create =====
  template<int m>
  typename _EXPs<m>::EXPs Create_EXPs() {

    typedef typename _EXPs<m>::EXPs EXPs;
    EXPs ptr(new _EXPs<m>);
    return ptr;

  }
  STOs Create_STOs() {

    return Create_EXPs<1>();
    
  }
  GTOs Create_GTOs() {

    return Create_EXPs<2>();
    
  }


  // ==== Calculation ====
  dcomplex GTOInt(int n, dcomplex a) {

    if(n < 0) {
      std::string msg; SUB_LOCATION(msg);
      msg += "maxn must be bigger than 0";
      throw std::runtime_error(msg);
    }

    if(n == 0)
      return sqrt(M_PI)/(2.0*sqrt(a));
    if(n == 1)
      return 0.5/a;

    return dcomplex(n-1)/(2.0*a) * GTOInt(n-2, a);

  }
  dcomplex GTOIntLC(LC_GTOs a, int m, LC_GTOs b) {

    dcomplex acc(0);
    for(int i = 0; i < a->size(); i++)
      for(int j = 0; j < b->size(); j++) {
	dcomplex c(a->c(i) * b->c(j));
	int      n(a->n(i) + b->n(j));
	dcomplex z(a->z(i) + b->z(j));
	acc +=  c * GTOInt(n+m, z);
      }
    return acc;
  }

  template<int m>
  dcomplex EXPInt(int n, dcomplex a);
  template<>
  dcomplex EXPInt<1>(int n, dcomplex a) {
    return 0.0;
  }
  template<>
  dcomplex EXPInt<2>(int n, dcomplex a) {
    return GTOInt(n, a);
  }


  // ==== member field ====
  template<int m>
  _EXPs<m>::_EXPs() {
    this->setupq_ =false;
  }
  template<int m>
  bool _EXPs<m>::OnlyPrim() const {

    bool acc(true);

    typedef typename vector<LC_EXPs>::const_iterator It;
    for(It it = this->basis_.begin();
	it != this->basis_.end();
	++it) {
      acc &= (*it)->size() == 1;
    }

    return acc;
    
  }
  template<int m>
  VectorXcd _EXPs<m>::AtR(const VectorXcd& rs, const VectorXcd& cs) const {

    if(cs.size() != this->size()) {
      string msg;
      SUB_LOCATION(msg);
      msg += " : size mismatch.";
      throw runtime_error(msg);
    }

    int num(this->size());
    VectorXcd ys = VectorXcd::Zero(rs.size());

    for(int i = 0; i < num; i++) {
      VectorXcd ys0 = this->basis(i)->AtR(rs);
      ys += ys0;
    }

    return ys;

  }
  template<int m>
  VectorXcd _EXPs<m>::DAtR(const VectorXcd& rs, const VectorXcd& cs) const {

    if(cs.size() != this->size()) {
      string msg;
      SUB_LOCATION(msg);
      msg += " : size mismatch.";
      throw runtime_error(msg);
    }

    int num(this->size());
    VectorXcd ys = VectorXcd::Zero(rs.size());

    for(int i = 0; i < num; i++) {
      VectorXcd ys0 = this->basis(i)->DAtR(rs);
      ys += ys0;
    }

    return ys;

  }
  template<int m>
  string _EXPs<m>::str() const {
    return "EXPs";
  }

  template<int m>
  _EXPs<m>* _EXPs<m>::AddPrim(int n, dcomplex z) {
    this->setupq_ = false;
    LC_EXPs g = Create_LC_EXPs<m>();
    g->Add(1.0, n, z);
    this->basis_.push_back(g);
    return this;
  }
  template<int m>
  _EXPs<m>* _EXPs<m>::AddPrims(int n, Eigen::VectorXcd zs) {
    this->setupq_ = false;
    for(int i = 0; i < zs.size(); i++)
      this->AddPrim(n, zs[i]);
    return this;
  }
  template<int m>
  _EXPs<m>* _EXPs<m>::AddLC(LC_EXPs lc) {
    this->setupq_ =false;
    this->basis_.push_back(lc);
    return this;
  }
  template<int m>
  _EXPs<m>* _EXPs<m>::SetUp() {

    if(this->setupq_)
      return this;

    int num(this->size());
    
    for(int i = 0; i < num; i++) {

      LC_EXPs bi = this->basis(i);
      /*

      dcomplex acc(0);
      for(int ii = 0; ii < bi->size(); ii++) {
	for(int jj = 0; jj < bi->size(); jj++) {
	  dcomplex c(bi->c(ii) * bi->c(jj));
	  int      n(bi->n(ii) + bi->n(jj));
	  dcomplex z(bi->z(ii) + bi->z(jj));
	  acc +=  c * EXPInt(n, z);
	}
      }
      
      dcomplex nterm(1.0/sqrt(acc));
      */
      dcomplex nterm(1.0/sqrt(EXPIntLC(bi, 0, bi)));
      for(int ii = 0; ii < bi->size(); ii++) {
	bi->c(ii) *= nterm;
      }
      
    }

    this->setupq_ = true;
    return this;
  }

  template<int m>
  typename _EXPs<m>::EXPs _EXPs<m>::Clone() const {

    EXPs ptr = Create_EXPs<m>();
    typedef typename vector<LC_EXPs>::const_iterator It;
    //typedef vector<_EXPs<m>::LC_EXPs>::const_iterator It;
    for(It it = this->basis_.begin();
	it != this->basis_.end();
	++it) {
      LC_EXPs lc = (*it)->Clone();
      ptr->AddLC(*it);
    }
    ptr->SetUp();
    return ptr;

  }
  template<int m>
  typename _EXPs<m>::EXPs _EXPs<m>::Conj() const {

    EXPs ptr = Create_EXPs<m>();
    typedef typename vector<LC_EXPs>::const_iterator It;
    for(It it = this->basis_.begin();
	it != this->basis_.end();
	++it) {
      LC_EXPs lc = (*it)->Conj();
      ptr->AddLC(lc);
    }
    ptr->SetUp();
    return ptr;

  }

  template<int m>
  MatrixXcd _EXPs<m>::CalcRmMat(int M) const {
    
    if(!this->setupq_) {
      string msg; SUB_LOCATION(msg);
      msg = "\n" + msg + "EXP is not setup.";
      throw runtime_error(msg);
    }
    
    int num(this->size());
    MatrixXcd mat(num, num);
    
    for(int i = 0; i < num; i++)
      for(int j = 0; j < num; j++) {

	LC_EXPs bi = this->basis(i);
	LC_EXPs bj = this->basis(j);

	dcomplex acc(0);
	for(int ii = 0; ii < bi->size(); ii++)
	  for(int jj = 0; jj < bj->size(); jj++) {
	    dcomplex c(bi->c(ii) * bj->c(jj));
	    int      n(bi->n(ii) + bj->n(jj));
	    dcomplex z(bi->z(ii) + bj->z(jj));
	    acc +=  c * EXPInt<m>(n+M, z);
	  }
	mat(i, j) = acc;
	//mat(i, j) = EXPIntLC<m>(bi, M, bj);
      }

    return mat;

  }
  template<int m>
  MatrixXcd _EXPs<m>::CalcD2Mat() const {

    if(!this->setupq_) {
      string msg; SUB_LOCATION(msg);
      msg = "\n" + msg + "EXP is not setup.";
      throw runtime_error(msg);
    }

    int num(this->size());
    MatrixXcd mat(num, num);
    
    for(int i = 0; i < num; i++)
      for(int j = 0; j < num; j++) {

	LC_EXPs bi = this->basis(i);
	LC_EXPs bj = this->basis(j);
	dcomplex acc(0);
	for(int ii = 0; ii < bi->size(); ii++)
	  for(int jj = 0; jj < bj->size(); jj++) {
	    dcomplex c(bi->c(ii) * bj->c(jj));
	    int      ni(bi->n(ii));
	    int      nj(bj->n(jj));
	    dcomplex zi(bi->z(ii));
	    dcomplex zj(bj->z(jj));
	    acc += c * ( +4.0*zj*zj       * EXPInt<2>(ni+nj+2, zi+zj)
			 -2.0*(2*nj+1)*zj* EXPInt<2>(ni+nj,   zi+zj));
	    if(nj > 1)
	      acc += 1.0*(nj*nj-nj) * c * EXPInt<2>(ni+nj-2, zi+zj);
	  }
	mat(i, j) = acc;
      }

    return mat;
  }
  template<int m>
  VectorXcd _EXPs<m>::CalcVecSTO(LC_STOs stos) const {
    
    if(!this->setupq_) {
      string msg; SUB_LOCATION(msg);
      msg += " : EXP is not setup.";
      throw runtime_error(msg);
    }

    return VectorXcd::Zero(1);
  }
  template<int m>
  VectorXcd _EXPs<m>::CalcVecGTO(LC_GTOs gtos) const {

    if(!this->setupq_) {
      string msg; SUB_LOCATION(msg);
      msg += " : GTO is not setup.";
      throw runtime_error(msg);
    }
    
    int num(this->size());
    int numo(gtos->size());

    VectorXcd vec(num);
        
    for(int i = 0; i < num; i++) {
      LC_GTOs bi = this->basis(i);
      dcomplex acc;
      for(int oo = 0; oo < numo; oo++) {
	for(int ii = 0; ii < num; ii++) {
	  dcomplex c(bi->c(ii) * gtos->c(oo));
	  int      n(bi->n(ii) + gtos->n(oo));
	  dcomplex z(bi->z(ii) + gtos->z(oo));
	  acc += c * GTOInt(n, z);
	}
      }
      
      vec(i) = acc;
      
    }
    
    return vec;

  }

  // ==== realize ====
  //  template<> class _EXPs<1>;
  //  template<> class _EXPs<2>;

}
