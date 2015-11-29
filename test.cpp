#include <iostream>
#include <boost/fusion/sequence.hpp>
#include <boost/fusion/include/sequence.hpp>
#include <boost/timer.hpp>
#include <gtest/gtest.h>

#include "utils.hpp"

#include "erfc.hpp"
#include "fact.hpp"
#include "lgamma.hpp"

#include "exp_func.hpp"
#include "cut_exp.hpp"
#include "delta.hpp"
#include "lin_func.hpp"
#include "lin_func_impl.hpp"

#include "op.hpp"
#include "op_func.hpp"

#include "cip_impl.hpp"
#include "cip.hpp"

using namespace std;
using namespace l2func;
using namespace erfc_mori;


TEST(math, Factorial) {

  using namespace fact;

  EXPECT_ANY_THROW(Factorial(-1));
  EXPECT_EQ(1, Factorial(0));
  EXPECT_EQ(1, Factorial(1));
  EXPECT_EQ(2, Factorial(2));
  EXPECT_EQ(6, Factorial(3));
  EXPECT_EQ(24, Factorial(4));
  EXPECT_EQ(120, Factorial(5));
  EXPECT_EQ(720, Factorial(6));

  EXPECT_ANY_THROW(DoubleFactorial(-1));
  EXPECT_EQ(1,   DoubleFactorial(0));
  EXPECT_EQ(1,   DoubleFactorial(1));
  EXPECT_EQ(2,   DoubleFactorial(2));
  EXPECT_EQ(3,   DoubleFactorial(3));
  EXPECT_EQ(8,   DoubleFactorial(4));
  EXPECT_EQ(15,  DoubleFactorial(5));
  EXPECT_EQ(48,  DoubleFactorial(6));
  EXPECT_EQ(105, DoubleFactorial(7));
   
}
TEST(math, real_erfc) {

  using namespace erfc_mori;

  // this function is forbidden
  //  erfc_add_Eh_q<int>(1, 2);
  
  double y, x, expect;
  ErfcCalcData calc_data;

  x = 1.0;
  expect =0.157299207050285130658779364917390740703933002034;
  //erfc_d(x, y, calc_data);
  Erfc<double>(x, y, calc_data);
  EXPECT_DOUBLE_EQ(expect, y);
  EXPECT_TRUE(calc_data.convergence);
  
  x = 1.0;
  x = 1.0 / 100.0;
  expect = 0.98871658444415038308409047645193078905089904517;
  //  erfc_d(x, y, calc_data);
  Erfc<double>(x, y, calc_data);
  EXPECT_DOUBLE_EQ(expect, y);
  EXPECT_TRUE(calc_data.convergence);

  x = 3.0;
  expect = 0.0000220904969985854413727761295823203798477070873992;
  //  erfc_d(x, y, calc_data);
  // erfc(x, y, calc_data);
  Erfc<double>(x, y, calc_data);
  EXPECT_DOUBLE_EQ(expect, y);
  EXPECT_TRUE(calc_data.convergence);

}
TEST(math, complex_erfc) {

  using namespace erfc_mori;

  CD y;
  ErfcCalcData calc_data;
  double eps = 10.0 * machine_eps();

  CD x(1, -1);
  CD y_expect( -0.31615128169794764488027108024367,
	       +0.190453469237834686284108861969162);

  Erfc(x, y, calc_data);

  EXPECT_DOUBLE_EQ(y.real(), y_expect.real());
  EXPECT_NEAR(y.real(), y_expect.real(), eps);
  EXPECT_DOUBLE_EQ(y.imag(), y_expect.imag());
  EXPECT_NEAR(y.imag(), y_expect.imag(), eps);

  x = CD(0.0157073173118206757532953533099,
	 0.9998766324816605986389071277312);
  y_expect = CD(0.95184545524179913420177473658805,
		-1.64929108965086517748934245403332);

  Erfc(x, y, calc_data);
  EXPECT_TRUE(calc_data.convergence);
  EXPECT_NEAR(     y.real(), y_expect.real(), eps);
  EXPECT_NEAR(     y.imag(), y_expect.imag(), eps);

  x = CD(0.001564344650402308690101053194671668923139,
	 0.009876883405951377261900402476934372607584);
  y_expect = CD(0.9982346553205423153337357292658472915601,
		-0.0111452046101524188315708507537751407281);

  Erfc(x, y, calc_data);
  EXPECT_TRUE(calc_data.convergence);
  EXPECT_NEAR(y.real(), y_expect.real(), eps);
  EXPECT_NEAR(y.imag(), y_expect.imag(), eps);

}
TEST(math, lower_gamma) {
  double eps = pow(10.0, -13.0);
  CD a(1.1, -0.3);

  EXPECT_ANY_THROW(LowerGamma(-1, a));

  EXPECT_PRED_FORMAT3(AssertComplexNear, 
		      1.0-exp(-a),
		      LowerGamma(1, a), 
		      eps);
  EXPECT_PRED_FORMAT3(AssertComplexNear, 
		      -a*exp(-a) + 1.0-exp(-a),
		      LowerGamma(2, a), 
		      eps);

}
TEST(ExpFunc, Construct) {
  
  RSTO s0(1.0, 1, 1.0);
  RSTO s1(1.0, 2, 1.0);
  RGTO g1(1.0, 2, 1.0);

  EXPECT_EQ(2, s1.n());
  EXPECT_EQ(2, g1.n());
  //  EXPECT_EQ(3, s2.n());

  CSTO s3(1.0, 3, CD(1.0, -0.2));
  CGTO g3(1.0, 3, CD(1.0, -0.2));
  CSTO s4(s1);
  EXPECT_EQ(3, s3.n());
  EXPECT_EQ(3, g3.n());
  EXPECT_DOUBLE_EQ(1.0, s4.z().real());

  EXPECT_EQ(1, CSTO::exp_power);
}
TEST(ExpFunc, accessor) {

  RSTO s1(1.1, 2, 0.2);
  double x0(3.0);
  EXPECT_DOUBLE_EQ(1.1 * x0 * x0 * exp(-0.2 * x0), 
		   s1.at(x0));

  s1.set_z(1.3);
  EXPECT_DOUBLE_EQ(1.3, s1.z());


}
TEST(ExpFunc, stream) {
  RSTO n_s1(2.1, 2, 1.1);
  cout << n_s1 << endl;
}
TEST(ExpFunc, ComplexConj) {

  CSTO s(CD(1.1, 1.2), 2, CD(2.1, -2.2));

  CSTO cs(s); cs.SetComplexConjugate();
  
  EXPECT_DOUBLE_EQ(1.1, cs.c().real());
  EXPECT_DOUBLE_EQ(-1.2, cs.c().imag());
  EXPECT_EQ(2, cs.n());
  EXPECT_DOUBLE_EQ(2.1, cs.z().real());
  EXPECT_DOUBLE_EQ(2.2, cs.z().imag());

  // if below line are tried to compile, it fails.
  RSTO rsto(1.0 ,2, 3.1);
  RSTO c_rsto(rsto); c_rsto.SetComplexConjugate();
  EXPECT_DOUBLE_EQ(1.0, c_rsto.c());
  EXPECT_EQ(2, c_rsto.n());
  EXPECT_DOUBLE_EQ(3.1, c_rsto.z());
}
TEST(ExpFunc, Product) {

  CSTO s1(1.2, 3, std::complex<double>(1.5));
  s1.SetScalarProd(2.0);
  s1.SetRmProd(2);

  EXPECT_EQ(5, s1.n());
  EXPECT_DOUBLE_EQ(2.4, s1.c().real());
}
TEST(CutExp, Construct) {

  CSTO csto(1.2, 2, 2.5);
  CutCSTO cut_csto(1.2, 2, 2.5, 10.0);

  EXPECT_C_EQ(csto.at(1.2), cut_csto.at(1.2));
  EXPECT_C_EQ(0.0, cut_csto.at(10.1));


}
TEST(LinFunc, Construct) {

  RSTO s1(1.0, 2, 1.2);
  RSTO s2(1.0, 3, 1.4);
  LinFunc<RSTO> stos; 
  stos.Add(2.5, s1);
  stos.Add(1.2, s2);

  double r0(0.3);

  EXPECT_DOUBLE_EQ(2.5*s1.at(r0) + 1.2*s2.at(r0),
		   stos.at(r0));

}
TEST(CIP, ExpFunc) {
  RSTO s1(2.5, 2, 1.1);
  CSTO s2(1.2, 3, CD(0.4, 0.2));
  RGTO g1(0.3, 1, 1.2);
  CGTO g2(0.4, 4, CD(0.1, -0.1));
  double eps = pow(10.0, -9.0);

  
  EXPECT_DOUBLE_EQ(2.9105687018397886, CIP(s1, s1));

  CD sol(20.98619989895233, -21.40636768181864);
  EXPECT_C_NEAR(sol, CIP(CSTO(s1), s2), eps);

  sol = CD(20.98619989895233, -21.40636768181864);
  EXPECT_C_EQ(sol, CIP(s2, CSTO(s1)));
  
  EXPECT_NEAR(0.0764996359892135, CIP(s1, g1), eps);
  EXPECT_NEAR(0.0764996359892135, CIP(g1, s1), eps);

  CSTO c_s1(s1);
  EXPECT_NEAR(5.562595882704702, CIP(c_s1, g2).real(),      eps);
  EXPECT_NEAR(5.562595882704702, CIP(g2, CSTO(s1)).real(),  eps);
  EXPECT_NEAR(+22.587241177071004, CIP(CSTO(s1), g2).imag(),eps);
  EXPECT_NEAR(+22.587241177071004, CIP(g2, CSTO(s1)).imag(),eps);

  EXPECT_NEAR(0.05270913901892936, CIP(CGTO(g1), g2).real(), eps);
  EXPECT_NEAR(0.05270913901892936, CIP(g2, CGTO(g1)).real(), eps);
  EXPECT_NEAR(0.012359047425198447, CIP(CGTO(g1), g2).imag(), eps);
  EXPECT_NEAR(0.012359047425198447, CIP(g2, CGTO(g1)).imag(), eps);
  
}
TEST(CIP, CutExpFunc) {

  CutCSTO cut_csto(1.2, 2, 2.5, 10.0);
  CutCSTO cut_csto2(1.1, 3, 1.5, 10.0);
  double sol = 0.0386718749998404;
  EXPECT_C_EQ(sol, CIP(cut_csto, cut_csto2));
  EXPECT_C_EQ(sol, CIP(cut_csto2, cut_csto));

  CSTO csto2(1.1, 3, 1.5);
  EXPECT_C_EQ(CIP(csto2, cut_csto), 
	      CIP(cut_csto, csto2));
  EXPECT_C_EQ(CIP(cut_csto2, cut_csto), 
	      CIP(cut_csto, csto2));

}
TEST(CIP, Normalized) {
  
  RSTO s1(1.0, 2, 1.1); CNormalize(&s1);
  RGTO g1(1.0, 2, 1.1); CNormalize(&g1);
  CSTO s2(1.0, 3, CD(2.1, -0.3)); CNormalize(&s2);
  
  EXPECT_DOUBLE_EQ(1.0, CIP(s1, s1));
  EXPECT_DOUBLE_EQ(1.0, CNorm(s1));
  EXPECT_DOUBLE_EQ(1.0, CNorm(g1));
  EXPECT_C_EQ(1.0, CNorm(s2));

/*
  Op<RSTO> op = OpDDr<RSTO>();

  EXPECT_NEAR( CIP(n_s1, op(n_s2)),
	       CIP(s1,   op(s2)) /
	       sqrt(CIP(s1, s1) * CIP(s2, s2)),
	       0.000000000001);

  RGTO n_g1(2, 1.1, Normalized);
  RGTO g1(1.0, 2, 1.1);
  RGTO n_g2(3, 1.2, Normalized);
  RGTO g2(1.0, 3, 1.2);
  
  EXPECT_DOUBLE_EQ(1.0, CIP(n_g1, n_g1));

  Op<RGTO> op_g = OpDDr<RGTO>();

  EXPECT_NEAR( CIP(n_g1, op_g(n_g2)),
	       CIP(g1,   op_g(g2)) /
	       sqrt(CIP(g1, g1) * CIP(g2, g2)),
	       0.000000000001);  
*/
}
TEST(CIP, Delta) {

  double r0(2.4);
  RDelta d0(r0);
  RSTO  sto(1.1, 2, 3.2);

  EXPECT_DOUBLE_EQ(sto.at(2.4), CIP(d0, sto));
  EXPECT_DOUBLE_EQ(sto.at(2.4), CIP(sto, d0));
  
}
TEST(CIP, LinFunc) {

  LinFunc<CSTO> f;
  CD c0(1.0, 1.2);
  CSTO u0(1.2, 2, CD(0.4, -0.4));
  f.Add(c0, u0);
  CD c1(0.3, 0.2);
  CSTO u1(1.0, 1, CD(0.1, -0.2));
  f.Add(c1, u1);

  LinFunc<CSTO> g;
  CD d0(0.1, 1.2);
  CSTO v0(0.3, 1, CD(0.1, -0.5));
  g.Add(d0, v0);
  CD d1(0.1, 0.5);
  CSTO v1(0.8, 1, CD(0.8, -0.0));
  g.Add(d1, v1);  

  EXPECT_C_EQ(CIP(f, g),
	      c0*d0*CIP(u0, v0)+c1*d0*CIP(u1, v0)+c0*d1*CIP(u0, v1)+c1*d1*CIP(u1, v1));
  
}
TEST(CIP, OpD2) {

  RSTO s1(2.0, 3, 4.0);
  RSTO s2(1.1, 2, 2.2);

  EXPECT_DOUBLE_EQ(CIP(s1, OP(OpD2(), s2)), 
		   CIP(s1, OpD2(), s2));
  
}
TEST(CIP, time) {

  
  RSTO s1(1.2, 5, 0.3);
  LinFunc<RSTO> ss; 
  for(int i = 0; i < 100; i++)
    ss.Add(1.2, s1);

  int num(100);

  boost::timer t0;
  for(int i = 0; i < num; i++) 
    CIP(s1, OpD2(), s1);  
  cout << "t[CIP(A,O,B)] = " << t0.elapsed() << endl;

  boost::timer t1;
  for(int i = 0; i < num; i++) 
    CIP(ss, OP(OpD2(), ss));
  cout << "t[CIP(A,O[B])] = " << t1.elapsed() << endl;

}
TEST(OP, OpRm) {

  RSTO s1(2.3, 2, 1.1);
  LinFunc<RSTO> r2_s1 = OP(OpRm(2), s1);

  EXPECT_EQ(4, r2_s1.begin()->second.n());

}
TEST(OP, OpD1) {

  // d/dr 2r^3 exp(-4r)
  // = (6r^2 -8r^3)exp(-4r)

  RSTO s1(2.0, 3, 4.0);
  LinFunc<RSTO> s2 = OP(OpD1(), s1);
  double r = 2.2;
  EXPECT_DOUBLE_EQ((6.0*r*r-8*r*r*r)*exp(-4*r),
		   s2.at(r));

  // d/dr 2r^3 exp(-4r^2)
  // = (6r^2 -16r^4) *exp(-4rr)
  RGTO g1(2.0, 3, 4.0);
  LinFunc<RGTO> g2 = OP(OpD1(), g1);
  EXPECT_DOUBLE_EQ((6.0*r*r-16*r*r*r*r)*exp(-4*r*r),
		   g2.at(r));
}
TEST(OP, OpD2) {

  RSTO s(2.0, 3, 4.0);
  LinFunc<RSTO> s1 = OP(OpD2(), s);
  LinFunc<RSTO> ss = OP(OpD1(), s);
  LinFunc<LinFunc<RSTO> > s2 = OP(OpD1(), ss);
  
  cout << s1 << endl;
  cout << s2 << endl;

  double r0(2.1);
  EXPECT_DOUBLE_EQ(s1.at(r0), s2.at(r0));
  
}
TEST(OP, Add) {

  RSTO s(1.1, 2, 1.2);

  LinFunc<RSTO> As = OP(OpRm(2), s);
  LinFunc<RSTO> Bs = OP(OpD2(), s);

  //  LinFunc<RSTO> ABs = OP(OpAdd<OpRm, OpD2>(OpRm(2), OpD2()), s);
  LinFunc<RSTO> ABs = OP(AddOp(OpRm(2), OpD2()), s);

  double r0(2.1);
  EXPECT_DOUBLE_EQ(As.at(r0)+Bs.at(r0), 
		   ABs.at(r0));

}
TEST(OP, ScalarProd) {

  RSTO s(1.1, 2, 1.2);

  LinFunc<RSTO> s1 = OP(ProdOp(0.3, AddOp(OpRm(2), OpD2())), s);

  double r0(0.4);
  EXPECT_DOUBLE_EQ(
		   0.3 * (OP(OpRm(2), s).at(r0) + OP(OpD2(), s).at(r0)),
		   s1.at(r0));

}

int main (int argc, char **args) {
  ::testing::InitGoogleTest(&argc, args);
  return RUN_ALL_TESTS();
}

/*
int practice_fusion () {

  using boost::fusion::vector;
  using boost::fusion::at_c;

  vector<int, double> x(1, 0.2);
  std::cout << at_c<0>(x) << endl;
  return 0;
}
int main ()
{
  ExpFunc<double, 1> stoA(1.0, 2, 2.5);
  ExpFunc<double, 1> stoB(2.0, 3, 2.5);
  OpRm r2(2);

  double ab = CIP(stoA, stoB);
  double abc = CIP(stoA, r2, stoB);

  cout << ab << endl;
  cout << abc<< endl;

  std::vector<double> cs;
  cs.push_back(1.1); cs.push_back(1.2); cs.push_back(0.7);
  OpLin<double , OpRm, OpRm, OpRm> op_lin(cs, r2, r2, r2);
  op_lin.print();
  //  cout << op_lin << endl;

  cout << endl;
  cout << CIP(stoA, op_lin, stoB) << endl;

  std::stringstream ss;
  
  return 0;
}
*/