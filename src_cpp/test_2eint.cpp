#include <iostream>
#include <gtest/gtest.h>
#include "gtest_plus.hpp"
#include "b2eint.hpp"
#include "symmolint.hpp"

using namespace std;
using namespace Eigen;
using namespace l2func;

TEST(first, first) {
  EXPECT_EQ(2, 1+1);
}
class TestB2EInt :public ::testing::Test {
public:
  IB2EInt *eri;
public:
  TestB2EInt() {
    eri = new B2EIntMem(100);
    eri->Set(1, 2, 3, 4,
	     5, 6, 7, 8, 1.1);
    eri->Set(0, 0, 0, 1,
	     0, 0, 3, 0, 1.2);
    eri->Set(0, 2, 0, 0,
	     0, 0, 1, 0, 1.3);
    eri->Set(1, 0, 0, 0,
	     0, 1, 0, 0, 1.4);
  }
  ~TestB2EInt() {
    delete eri;
  }
};
TEST_F(TestB2EInt, Get) {
  int ib,jb,kb,lb,i,j,k,l,t;
  dcomplex v;
  EXPECT_TRUE(eri->Get(&ib,&jb,&kb,&lb,&i,&j,&k,&l, &t, &v));
  EXPECT_EQ(1, ib);
  EXPECT_EQ(2, jb);
  EXPECT_EQ(3, kb);
  EXPECT_EQ(4, lb);
  EXPECT_EQ(5, i);
  EXPECT_EQ(6, j);
  EXPECT_EQ(7, k);
  EXPECT_EQ(8, l);
  EXPECT_EQ(0, t);
  EXPECT_C_EQ(1.1, v);
  
  EXPECT_TRUE(eri->Get(&ib,&jb,&kb,&lb,&i,&j,&k,&l, &t, &v));
  EXPECT_EQ(k, 3);
  EXPECT_TRUE(eri->Get(&ib,&jb,&kb,&lb,&i,&j,&k,&l, &t, &v));
  EXPECT_EQ(jb, 2);
  EXPECT_TRUE(eri->Get(&ib,&jb,&kb,&lb,&i,&j,&k,&l, &t, &v));
  EXPECT_EQ(v, 1.4);
  EXPECT_FALSE(eri->Get(&ib,&jb,&kb,&lb,&i,&j,&k,&l, &t, &v));
  
  eri->Reset();
  while(eri->Get(&ib,&jb,&kb,&lb,&i,&j,&k,&l, &t, &v)) {
    cout << ib << jb << kb << lb << endl;
  }
}
TEST_F(TestB2EInt, At) {

  dcomplex v;
  v = eri->At(1, 2, 3, 4, 5, 6, 7, 8);
  EXPECT_C_EQ(1.1, v);

  EXPECT_ANY_THROW(eri->At(0, 0, 0, 0, 1, 1, 1, 1));
}
TEST_F(TestB2EInt, size) {
  EXPECT_EQ(4, eri->size());
  EXPECT_EQ(100, eri->capacity());
}
VectorXcd OneVec(dcomplex z) {
  VectorXcd zs(1); zs << z;
  return zs;
}
TEST(SymGTOs, CalcERI) {

  SymGTOs gtos(SymmetryGroup_C1());

  // -- A --
  gtos.AddSub(Sub_s(0, Vector3cd(0.0, 0.0,  0.4), OneVec(1.2)));
  gtos.AddSub(Sub_s(0, Vector3cd(0.0, 0.0,  0.0), OneVec(1.4)));
  gtos.AddSub(Sub_s(0, Vector3cd(0.0, -0.2, 0.0), OneVec(1.1)));
  gtos.AddSub(Sub_s(0, Vector3cd(0.2, 0.0,  0.1), OneVec(1.0)));
  

  // -- potential --
  MatrixXcd xyzq(4, 1); xyzq << 0.0, 0.0, 0.0, 1.0;
  gtos.SetAtoms(xyzq);
  try {
    gtos.SetUp();
  } catch(exception& e) {
    cout << "in setup" << endl;
    cout << e.what() << endl;
    throw e;
  }

  // -- Calculation --
  IB2EInt *eri = new B2EIntMem(pow(4, 4));
  gtos.CalcERI(eri);
  // -- size check --
  EXPECT_EQ(pow(4, 4), eri->size());

  // -- Symmetry check --
  // eri is chemist's notation.
  EXPECT_C_EQ(eri->At(0, 0, 0, 0,  0, 1, 2, 3),
	      eri->At(0, 0, 0, 0,  1, 0, 2, 3));
  EXPECT_C_EQ(eri->At(0, 0, 0, 0,  0, 1, 2, 3),
	      eri->At(0, 0, 0, 0,  0, 1, 3, 2));
  EXPECT_C_EQ(eri->At(0, 0, 0, 0,  0, 1, 2, 3),
	      eri->At(0, 0, 0, 0,  2, 3, 0, 1));

  // -- copied from neo_ccol/aoint/gto_utest.cpp --
  // (00|00): (1.23607744235395,0)
  // (01|23): (1.01774464000324,0)
  // (00|11): (1.03695966484363,0)
  // (00|01): (1.11023051353687,0)
  // (00|12): (0.992414842562312,0)

  double eps(pow(10.0, -8.0));
  dcomplex ref0000(1.23607744235395, 0);
  dcomplex ref0123(1.01774464000324, 0);
  dcomplex ref0011(1.03695966484363,0);
  dcomplex ref0001(1.11023051353687,0);
  dcomplex ref0012(0.992414842562312,0);
  
  EXPECT_C_NEAR(ref0000, eri->At(0, 0, 0, 0, 0, 0, 0, 0), eps);
  EXPECT_C_NEAR(ref0123, eri->At(0, 0, 0, 0, 0, 2, 1, 3), eps);
  EXPECT_C_NEAR(ref0011, eri->At(0, 0, 0, 0, 0, 1, 0, 1), eps);
  EXPECT_C_NEAR(ref0001, eri->At(0, 0, 0, 0, 0, 0, 0, 1), eps);
  EXPECT_C_NEAR(ref0012, eri->At(0, 0, 0, 0, 0, 1, 0, 2), eps);
  delete eri;

}
SubSymGTOs SubC1(Vector3i ns, Vector3cd xyz, dcomplex z) {

  SubSymGTOs sub;
  sub.AddNs(ns);
  sub.AddXyz(xyz);
  sub.AddRds(Reduction(0, MatrixXcd::Ones(1, 1)));
  sub.SetSym(MatrixXi::Ones(1, 1), MatrixXi::Ones(1, 1));
  VectorXcd zeta(1); zeta << z;
  sub.AddZeta(zeta);
  return sub;
}
TEST(SymGTOs, CalcERI2) {
/*
  SymGTOs gtos(SymmetryGroup_C1());
  gtos.AddSub(SubC1(Vector3i(0, 1, 0), Vector3cd(0.0, 0.0, 0.4), 1.2));
  gtos.AddSub(SubC1(Vector3i(1, 1, 0), Vector3cd(0.0, 0.0, 0.0), 1.4));
  gtos.AddSub(SubC1(Vector3i(1, 1, 1), Vector3cd(0.0,-0.2, 0.0), 1.1));
  gtos.AddSub(SubC1(Vector3i(0, 3, 0), Vector3cd(0.2, 0.0, 0.1), 1.0));

  // -- potential --
  cout << 1 << endl;
  MatrixXcd xyzq(4, 1); xyzq << 0.0, 0.0, 0.0, 1.0;
  gtos.SetAtoms(xyzq);
  gtos.SetUp();

  // -- Calculation --
  IB2EInt *eri = new B2EIntMem(pow(4, 4));
  gtos.CalcERI(eri);

  // -- size check --
  //  cout << 1 << endl;
  //  EXPECT_EQ(pow(4, 4), eri->size());

  // -- reference --
  // (00|00): (1.00946324498146,0)
  // (00|11): (0.133478467219949,0)
  // (00|01): (0,0)
  // (00|12): (0.0481552620386253,3.14588102745493e-19)
  // (01|23): (0.0240232215271162,1.00002719131888e-18)

  double eps(pow(10.0, -8.0));
  EXPECT_C_NEAR(1.00946324498146,  eri->At(0, 0, 0, 0, 0, 0, 0, 0), eps);
  EXPECT_C_NEAR(0.133478467219949, eri->At(0, 0, 0, 0, 0, 1, 0, 1), eps);
  EXPECT_C_NEAR(0.0, eri->At(0, 0, 0, 0, 0, 0, 0, 1), eps);
  EXPECT_C_NEAR(0.0481552620386253, eri->At(0, 0, 0, 0, 0, 1, 0, 2), eps);
  EXPECT_C_NEAR(0.0240232215271162, eri->At(0, 0, 0, 0, 0, 2, 1, 3), eps);

  delete eri;
*/

}
TEST(SymGTOs, CalcERI_time) {
/*
  SymGTOs gtos(SymmetryGroup_C1());

  Vector3cd xyz1(0.0, 0.0, 0.0);
  Vector3cd xyz2(0.0, 0.4, 0.0);

  SubSymGTOs sub1;
  
  int num_at(2);
  sub1.AddXyz(Vector3cd(0.0, 0.4, 0.0));
  sub1.AddXyz(Vector3cd(0.0, 0.0, 0.5));

  int  num_ns(3);
  sub1.AddNs(Vector3i(1, 0, 0));
  sub1.AddNs(Vector3i(0, 1, 0));
  sub1.AddNs(Vector3i(0, 0, 1));


  int num_z(5);
  VectorXcd zs(num_z); zs << 1.1, 1.2, 1.3, 1.4, 1.5;
  sub1.AddZeta(zs);
  MatrixXcd cs = MatrixXcd::Ones(num_at, num_ns);
  sub1.AddRds(Reduction(0, cs));
  sub1.SetSym(MatrixXi::Ones(1, 2*num_ns),
	      MatrixXi::Ones(1, 2*num_ns));
  sub1.SetUp();
  
  gtos.AddSub(sub1);
  gtos.SetUp();

  // -- potential --
  MatrixXcd xyzq(4, 1); xyzq << 0.0, 0.0, 0.0, 1.0;
  gtos.SetAtoms(xyzq);
  gtos.SetUp();

  IB2EInt *eri = new B2EIntMem(pow(num_z, 4));
  gtos.CalcERI(eri);
  delete eri;
*/
}
TEST(SymGTOs, CalcERI_sym_p) {

  SymGTOs gtos(SymmetryGroup(2, "2d_rot"));

  Vector3cd xyz0(0.0, 0.0, 0.0);

  // ---- p orbital ----
  SubSymGTOs sub_p;
  sub_p.AddXyz(Vector3cd(0, 0, 0));
  sub_p.AddNs( Vector3i( 1, 0, 0));
  sub_p.AddNs( Vector3i( 0, 1, 0));
  MatrixXi sym_p(2, 2); sym_p <<
			  0, 1,  // E
			  1, 0;  // C4(z)
  MatrixXi sign_p(2, 2); sign_p <<
			   +1, +1,
			   +1, -1;
  sub_p.SetSym(sym_p, sign_p);
  int num_z_p(1);
  VectorXcd zs_p(num_z_p); zs_p << 1.1;
  sub_p.AddZeta(zs_p);
  MatrixXcd c(1, 2);
  c(0, 0) = 1.0; c(0, 1) = 0.0;
  sub_p.AddRds(Reduction(0, c));
  c(0, 0) = 0.0; c(0, 1) = 1.0;
  sub_p.AddRds(Reduction(1, c));

  sub_p.SetUp();
  gtos.AddSub(sub_p);

  // -- potential --
  MatrixXcd xyzq(4, 1); xyzq << 0.0, 0.0, 0.0, 1.0;
  gtos.SetAtoms(xyzq);
  gtos.SetUp();
  //  cout << gtos.str() << endl;

  IB2EInt *eri1 = new B2EIntMem(pow(3*num_z_p, 4));
  IB2EInt *eri2 = new B2EIntMem(pow(3*num_z_p, 4));
  gtos.CalcERI(eri1, 1);
  gtos.CalcERI(eri2, 2);

  int ib,jb,kb,lb,i,j,k,l,t;
  dcomplex v;
  eri1->Reset();
  while(eri2->Get(&ib,&jb,&kb,&lb,&i,&j,&k,&l, &t, &v)) {
    if(abs(v) > 0.000001)
      EXPECT_C_EQ(v, eri1->At(ib, jb, kb, lb, i, j, k, l)) <<
	ib << jb << kb << lb << i << j << k << l;
  }

  delete eri1;
  delete eri2;

}
TEST(SymGTOs, CalcERI_sym) {

  SymGTOs gtos(SymmetryGroup_SO3(1));

  Vector3cd xyz0(0.0, 0.0, 0.0);

  // ---- s orbital ----
  SubSymGTOs sub_s;
  sub_s.AddXyz(Vector3cd(0, 0, 0));
  sub_s.AddNs( Vector3i( 0, 0, 0));
  int num_z(1);
  VectorXcd zs(num_z); zs << 1.1;
  sub_s.AddZeta(zs);
  sub_s.AddRds(Reduction(0, MatrixXcd::Ones(1, 1)));
  MatrixXi sym_s = MatrixXi::Zero(4, 1);
  MatrixXi sign_s = MatrixXi::Ones(4, 1);
  sub_s.SetSym(sym_s, sign_s);
  cout << "s" << endl;
  sub_s.SetUp();
  gtos.AddSub(sub_s);

  // ---- p orbital ----
  SubSymGTOs sub_p;
  sub_p.AddXyz(Vector3cd(0, 0, 0));
  sub_p.AddNs( Vector3i( 1, 0, 0));
  sub_p.AddNs( Vector3i( 0, 1, 0));
  sub_p.AddNs( Vector3i( 0, 0, 1));
  MatrixXi sym_p(4, 3); sym_p <<
			  0, 1, 2,
			  0, 2, 1,
			  2, 1, 0,
			  1, 0, 2;
  MatrixXi sign_p(4, 3); sign_p <<
			   +1, +1, +1,
			   +1, +1, -1,
			   -1, +1, +1,
			   +1, -1, +1;
  int num_z_p(1);
  VectorXcd zs_p(num_z_p); zs_p << 1.1;
  sub_p.AddZeta(zs_p);
  MatrixXcd c(1, 3);
  c(0, 0) = 1.0; c(0, 1) = 0.0; c(0, 2) = 0.0; 
  sub_p.AddRds(Reduction(1, c));
  c(0, 0) = 0.0; c(0, 1) = 1.0; c(0, 2) = 0.0; 
  sub_p.AddRds(Reduction(2, c));
  c(0, 0) = 0.0; c(0, 1) = 0.0; c(0, 2) = 1.0; 
  sub_p.AddRds(Reduction(3, c));
  sub_p.SetSym(sym_p, sign_p);
  sub_p.SetUp();
  gtos.AddSub(sub_p);

  // -- potential --
  MatrixXcd xyzq(4, 1); xyzq << 0.0, 0.0, 0.0, 1.0;
  gtos.SetAtoms(xyzq);
  gtos.SetUp();
  //  cout << gtos.str() << endl;

  IB2EInt *eri1 = new B2EIntMem(pow(num_z+3*num_z_p, 4));
  IB2EInt *eri2 = new B2EIntMem(pow(num_z+3*num_z_p, 4));
  gtos.CalcERI(eri1, 1);
  gtos.CalcERI(eri2, 2);

  int ib,jb,kb,lb,i,j,k,l,t;
  dcomplex v;
  eri1->Reset();
  while(eri2->Get(&ib,&jb,&kb,&lb,&i,&j,&k,&l, &t, &v)) {
    if(abs(v) > 0.000001)
      EXPECT_C_EQ(v, eri1->At(ib, jb, kb, lb, i, j, k, l)) <<
	ib << jb << kb << lb << i << j << k << l;
  }

  delete eri1;
  delete eri2;
}

TEST(SymGTOs, CalcERI_time2) {

  SymGTOs gtos(SymmetryGroup_SO3(1));

  Vector3cd xyz0(0.0, 0.0, 0.0);

  // ---- s orbital ----
  SubSymGTOs sub_s;
  sub_s.AddXyz(Vector3cd(0, 0, 0));
  sub_s.AddNs( Vector3i( 0, 0, 0));

  int num_z(10);
  VectorXcd zs(num_z); zs << 1.1, 1.2, 1.3, 1.4, 1.5, 1.6, 1.7, 1.8, 1.9, 2.0;
  /*
  int num_z(1);
  VectorXcd zs(num_z); zs << 1.1;
  */
  sub_s.AddZeta(zs);
  sub_s.AddRds(Reduction(SO3_LM(0, 0), MatrixXcd::Ones(1, 1)));
  MatrixXi sym_s = MatrixXi::Ones(4, 1);
  MatrixXi sign_s = MatrixXi::Ones(4, 1);
  sub_s.SetSym(sym_s, sign_s);
  cout << "s" << endl;
  sub_s.SetUp();
  gtos.AddSub(sub_s);

  // ---- p orbital ----
  SubSymGTOs sub_p;
  sub_p.AddXyz(Vector3cd(0, 0, 0));
  sub_p.AddNs( Vector3i( 1, 0, 0));
  sub_p.AddNs( Vector3i( 0, 1, 0));
  sub_p.AddNs( Vector3i( 0, 0, 1));
  int num_z_p(8);
  VectorXcd zs_p(num_z_p); zs_p << 1.1, 1.2, 1.3, 1.4, 1.5, 1.6, 1.7, 1.8;
  /*
  int num_z_p(1);
  VectorXcd zs_p(num_z_p); zs_p << 1.1;
  */
  sub_p.AddZeta(zs_p);
  MatrixXcd c(1, 3);
  c(0, 0) = 1.0; c(0, 1) = 0.0; c(0, 2) = 0.0; 
  sub_p.AddRds(Reduction(SO3_LM(1, -1), c));
  c(0, 0) = 0.0; c(0, 1) = 1.0; c(0, 2) = 0.0; 
  sub_p.AddRds(Reduction(SO3_LM(1, +1), c));
  c(0, 0) = 0.0; c(0, 1) = 0.0; c(0, 2) = 1.0; 
  sub_p.AddRds(Reduction(SO3_LM(1, 0), c));
  MatrixXi sym_p(4, 3); sym_p <<
			  0, 1, 2,
			  0, 2, 1,
			  2, 1, 0,
			  1, 0, 2;
  MatrixXi sign_p(4, 3); sign_p <<
			   +1, +1, +1,
			   +1, +1, -1,
			   -1, +1, +1,
			   +1, -1, +1;
  sub_p.SetSym(sym_p, sign_p);
  cout << "p" << endl;
  sub_p.SetUp();
  gtos.AddSub(sub_p);

  // -- potential --
  MatrixXcd xyzq(4, 1); xyzq << 0.0, 0.0, 0.0, 1.0;
  gtos.SetAtoms(xyzq);
  gtos.SetUp();

  IB2EInt *eri1 = new B2EIntMem(pow(num_z+3*num_z_p, 4));
  IB2EInt *eri2 = new B2EIntMem(pow(num_z+3*num_z_p, 4));
  gtos.CalcERI(eri1, 1);
  gtos.CalcERI(eri2, 2);
  EXPECT_C_EQ(eri1->At(0, 0, 0, 0, 2, 1, 5, 0),
	      eri2->At(0, 0, 0, 0, 2, 1, 5, 0));
  delete eri1;
  delete eri2;
}

int main (int argc, char **args) {
  ::testing::InitGoogleTest(&argc, args);
  return RUN_ALL_TESTS();
}