#ifndef HATOM_H_TEMPLATE_H
#define HATOM_H_TEMPLATE_H

#include <boost/lexical_cast.hpp>
#include "macros.hpp"
#include "exp_func.hpp"
#include "lin_func.hpp"

#include "op.hpp"

namespace l2func {

  // ==== type ====
  // ---- Hamiltonian ----
  template<int L, class F> struct HOpType {
    typedef OpAdd<OpScalarProd<F, OpD2>,
		  OpAdd<OpScalarProd<F, OpRm>,
			OpScalarProd<F, OpRm> > > type; 
  }; 
  template<class F>  struct HOpType<0, F> {
    typedef OpAdd<OpScalarProd<F, OpD2>,
		  OpScalarProd<F, OpRm> > type; 
  };

  // ---- EigenFunc ----
  template<int N, int L, class F> struct HPsiType;
  template<class F> struct HPsiType<1,0,F> {
    typedef ExpFunc<F, 1> type;
  };
  template<class F> struct HPsiType<2,0,F> {
    typedef FuncAdd<ExpFunc<F, 1>, ExpFunc<F,1> > type;
  };
  template<class F> struct HPsiType<2,1,F> {
    typedef ExpFunc<F, 1> type;
  };
  
  // ---- Lenght ----
  template<int N, int L0, int L1, class F> struct HLengthType;
  template<class F> struct HLengthType<1,0,1,F> {
    typedef ExpFunc<F, 1> type;
  };

  // ---- Velocity ----
  template<int N, int L0, int L1, class F> struct HVelocityType;
  template<class F> struct HVelocityType<1,0,1,F> {
    typedef ExpFunc<F, 1> type;
  };


  // ==== H atom ====
  template<class F=double>
  class HLikeAtom {
  private:
    // ---- Field ----
    F z_;
    //    enum ENNum { n_=N };
    //    enum ELNum { l_=L };

  public:
    // ---- type ----
    typedef F Field;
    /*
    typedef ExpFunc<F, 1> STO;
    typedef NTermFunc<N-L, ExpFunc<F, 1> > EigenFunc;
    typedef OpAdd<OpScalarProd<F, OpD2>,
		  OpAdd<OpScalarProd<F, OpRm>,
			OpScalarProd<F, OpRm> > > HamiltonianOp;
    typedef OpAdd<HamiltonianOp, 
		  OpScalarProd<F, OpRm> > HMinusEnergyOp;
		  */

  public:
    // ---- constructor -----
    HLikeAtom() : z_(1) {}
    HLikeAtom(F _z): z_(_z) {}

    // ---- Getter ----
    F z() const   { return z_; }
    //    int n() const { return n_;}
    //    int l() const { return l_;}

    // ---- Method ----
    template<int L> typename HOpType<L, F>::type HOp() const;
    
  };

  // ==== H operator ====
  template<int L, class F> struct HOp {
    HOp(HLikeAtom<F> const& h) {
      value = AddOp(ProdOp(-F(1)/F(2), OpD2()),
		   AddOp(ProdOp(  -h.z(),      OpRm(-1)),
			 ProdOp( F(L*L-L)/F(2), OpRm(-2))));
    }
    typename HOpType<L, F>::type value;
  };
  template<class F> struct HOp<0, F> {
    HOp(HLikeAtom<F> const& h) : value(AddOp(ProdOp(-F(1)/F(2), OpD2()),
				  ProdOp(-h.z(),     OpRm(-1)))) {}
    typename HOpType<0, F>::type value;
  };

  // ==== eigen function ====
  template<int N, int L, class F> struct HPsi;
  template<class F> struct HPsi<1, 0, F> {
    HPsi(HLikeAtom<F> const&) : val( ExpFunc<F, 1>(F(2), 1, F(1))),
				value(val) {}

    typename HPsiType<1,0,F>::type const val;
    typename HPsiType<1,0,F>::type const& value;
  };
  template<class F> struct HPsi<2, 0, F> {
    HPsi(HLikeAtom<F> const&) : val(AddFunc
				    (ExpFunc<F,1>(F(1)/sqrt(F(2)), 
						  1, 
						  F(1)/F(2)),
				     ExpFunc<F,1>(-F(1)/(F(2)*sqrt(F(2))), 
						  2, 
						  F(1)/F(2)))),
				value(val) {}
    typename HPsiType<2,0,F>::type const val;
    typename HPsiType<2,0,F>::type const& value;
  };
  template<class F> struct HPsi<2, 1, F> {
    HPsi(HLikeAtom<F> const&) : val(ExpFunc<F,1>(F(1)/(F(2)*sqrt(F(6))), 
						 2, 
						 F(1)/F(2))),
				value(val) {}
    typename HPsiType<2,0,F>::type const val;
    typename HPsiType<2,0,F>::type const& value;    
  };
  

  // ==== length ====
  template<int N, int L0, int L1, class F>
  typename HLengthType<N,L0,L1,F>::type HLength(const HLikeAtom<F>&);
  template<class F>
  typename HLengthType<1,0,1,F>::type HLength(const HLikeAtom<F>&) {
    return STO(F(2), 2, F(1));
  }


  // ==== Velocity ====
  template<int N, int L0, int L1, class F>
  typename HVelocityType<N,L0,L1,F>::type HVelocity(const HLikeAtom<F>&);
  template<class F>
  typename HVelocityType<1,0,1,F>::type HVelocity(const HLikeAtom<F>&) {
    return STO(F(2), 1, F(1));
  }


  // ==== Eigen Energy ====
  template<int N, class F>
  F EigenEnergy(const HLikeAtom<F>&) {
    return -F(1) / (F(2*N*N));
  }


}

#endif
