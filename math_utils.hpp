#ifndef MATH_UTILS_TEMPLATE_H
#define MATH_UTILS_TEMPLATE_H



namespace l2func {

/*
  template<class F, int n>
  class array {
    // ---- member field ----
  private:
    F xs_[n];

  public:
    // ---- constructors ----
    array(F* xs) {
      for(int i = 0; i < n; i++) 
	this->xs_[i] = xs[i];
    }
    template<class F2>
    array(const array<F2, n>& o) {
      for(int i = 0; i < n; i++) 
	this->xs_[i] = o[i];
    }

    // ---- accessors ----
    F operator [](int i) const { return xs_[i]; }  
  };
*/

  template<class F>
  class array3 {
    // ---- member field ----
  private:
    F xs_[3];

  public:
    // ---- constructors ----
    array3(F x, F y, F z) {
      this->xs_[0] = x;
      this->xs_[1] = y;
      this->xs_[2] = z;
    }
    template<class F2>
    array3(const array3<F2>& o) {
      for(int i = 0; i < 3; i++) 
	this->xs_[i] = o[i];
    }

    // ---- accessors ----
    F operator [](int i) const { return xs_[i]; }      
  };

  template<class F> F ConjugateIfPossible(F x);

}

#endif
