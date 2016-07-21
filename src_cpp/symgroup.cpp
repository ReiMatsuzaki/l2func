#include <ostream>
#include "symgroup.hpp"
#include "macros.hpp"

using namespace std;
using namespace Eigen;

namespace l2func {

  // ==== Utilities ====
  bool IsNear(const dcomplex& a, const dcomplex& b) {
    double eps(0.00000001);
    return abs(a-b) < eps;
  }
  
  // ==== primitive GTO ====
  PrimGTO::PrimGTO():
    nx(0), ny(0), nz(0), x(0), y(0), z(0) {}
  PrimGTO::PrimGTO(int _nx, int _ny, int _nz,
		   dcomplex _ax, dcomplex _ay, dcomplex _az):
    nx(_nx), ny(_ny), nz(_nz), x(_ax), y(_ay), z(_az) {}
  string PrimGTO::str() const {
    ostringstream oss;
    oss << *this;
    return oss.str();
  }
  ostream& operator<< (std::ostream& oss, const PrimGTO& o) {
    oss << "GTO(" << o.nx << o.ny << o.nz << ": " <<
      o.x << ", " << o.y << ", " << o.z << ")";
    return oss;
  }
  bool IsNear(const PrimGTO& a, const PrimGTO& b) {
    return (a.nx == b.nx &&
	    a.ny == b.ny &&
	    a.nz == b.nz &&
	    IsNear(a.x, b.x) &&
	    IsNear(a.y, b.y) &&
	    IsNear(a.z, b.z));
  }
  
  // ==== Symmetry operation ====
  // ---- Interface ----
  int ISymOp::Op(const PrimGTO& a, const PrimGTO& b) const {
    PrimGTO op_a(a); // copy a
    bool is_prim;
    int sig;
    this->getOp(a, &op_a, &sig, &is_prim);

    if(!is_prim)
      return 0;

    if(op_a.nx == b.nx &&
       op_a.ny == b.ny &&
       op_a.nz == b.nz &&
       IsNear(op_a.x, b.x) &&
       IsNear(op_a.y, b.y) &&
       IsNear(op_a.z, b.z))
      return sig;
    else
      return 0;
  }

  // ---- Mult ----
  Mult::Mult(SymOp _sym_op, int _mult) {
    sym_op = _sym_op;
    mult = _mult;
    if(mult < 2) {
      string msg; SUB_LOCATION(msg);
      msg += ": mult must be bigger than 1";
    }
  }
  Mult::~Mult() {}
  void Mult::getOp(const PrimGTO& a, PrimGTO* b, int *sig, bool *prim) const {
    PrimGTO tmp0(a);
    PrimGTO tmp1(a);
    *sig = 1;
    int tmp_sig = 1;
    bool tmp_prim;
    for(int i = 0; i < this->mult; i++) {
      sym_op->getOp(tmp0, &tmp1, &tmp_sig, &tmp_prim);
      if(!tmp_prim) {
	*prim = false;
	return;
      }
      *sig *= tmp_sig;
      tmp0 = tmp1;
    }
    *b = tmp1;
    *prim = true;
  }
  string Mult::str() const {
    ostringstream oss;
    oss << this->mult << sym_op->str();
    return oss.str();
  }
  SymOp mult(SymOp a, int _mult) {
    Mult *ptr = new Mult(a, _mult);
    return SymOp(ptr);
  }

  // ---- Product ----
  Prod::Prod(SymOp _a, SymOp _b) {
    a = _a;
    b = _b;
  }
  Prod::~Prod() {}
  void Prod::getOp(const PrimGTO& x, PrimGTO *y, int *sig, bool *prim) const {
    PrimGTO bx(x);
    int sig_bx;
    bool prim_bx;
    b->getOp(x, &bx, &sig_bx, &prim_bx);
    if(!prim_bx) {
      *prim = false;
      return;
    }

    int sig_a;
    bool prim_a;
    a->getOp(bx, y, &sig_a, &prim_a);

    if(!prim_a) {
      *prim = false;
      return;
    }

    *sig = sig_a * sig_bx;
    *prim = true;
  }
  std::string Prod::str() const {
    ostringstream oss;
    oss << "prod(" << a->str() << ", " << b->str() << ")";
    return oss.str();
  }
  SymOp prod(SymOp a, SymOp b) {
    Prod *ptr = new Prod(a, b);
    return SymOp(ptr);
  }

  // ---- Id ----  
  Id::~Id() {}
  void Id::getOp(const PrimGTO& a, PrimGTO* b, int *sig, bool *prim) const {
    *b = a;
    *sig = 1;    
    *prim = true;
  }
  string Id::str() const {
    return "E";
  }
  SymOp id() {
    Id *ptr = new Id();
    return SymOp(ptr);
  }

  // ---- Cyclic ---- 
  Cyclic::Cyclic(Coord _coord, int _n): coord(_coord), n(_n) {
    if(n < 2) {
      string msg; SUB_LOCATION(msg);
      msg += ": n must be positive integer greater than 1";
      throw runtime_error(msg);
    }
  }
  void Cyclic::getOp(const PrimGTO& a, PrimGTO* b, int *sig, bool *prim) const {

    if(coord == CoordX && n == 2) {
      *b = a;
      b->y = -a.y;
      b->z = -a.z;
      *sig = pow(-1, a.ny + a.nz);
      *prim = true;
    } else if(coord == CoordY && n == 2) {
      *b = a;
      b->x = -a.x;
      b->z = -a.z;
      *sig = pow(-1, a.nx + a.nz);
      *prim = true;
    } else if(coord == CoordZ && n == 2) {
      *b = a;
      b->x = -a.x;
      b->y = -a.y;
      *sig = pow(-1, a.nx + a.ny);
      *prim = true;
    } else if(coord == CoordX && n == 4) {
      *b = a;
      b->y = -a.z;
      b->z = +a.y;
      b->ny = a.nz;
      b->nz = a.ny;
      *sig = pow(-1, a.nz);
      *prim = true;      
    } else if(coord == CoordY && n == 4) {
      *b = a;
      b->z = -a.x;
      b->x = +a.z;
      b->nz = a.nx;
      b->nx = a.nz;
      *sig = pow(-1, a.nx);
      *prim = true;
    } else if(coord == CoordZ && n == 4) {
      *b = a;
      b->x = -a.y;
      b->y = +a.x;
      b->nx = a.ny;
      b->ny = a.nx;
      *sig = pow(-1, a.ny);
      *prim = true;
    } else {
      string msg; SUB_LOCATION(msg);
      msg += ": not supported yet";
      throw runtime_error(msg);
    }
  }
  string Cyclic::str() const {
    ostringstream oss;
    oss << "C";
    if(this->coord == CoordX)
      oss << "x";
    if(this->coord == CoordY)
      oss << "y";
    if(this->coord == CoordZ)
      oss << "z";
    oss << "(" << this->n << ")";
    return oss.str();    
  }
  SymOp cyclic(Coord coord, int n) {
    Cyclic *ptr = new Cyclic(coord, n);
    return SymOp(ptr);
  }

  // ---- Reflection ----
  Reflect::Reflect(Coord _coord): coord(_coord) {}
  void Reflect::getOp(const PrimGTO& a, PrimGTO* b, int *sig, bool *prim) const {

    *b = a;
    *prim = true;

    if(coord == CoordX) {
      b->x = -a.x;
      *sig = pow(-1, a.nx);
    } 
    else if(coord == CoordY) {
      b->y = -a.y;
      *sig = pow(-1, a.ny);
    } else if(coord == CoordZ) {
      b->z = -a.z;
      *sig = pow(-1, a.nz);
    }
    
  }
  string Reflect::str() const {
    ostringstream oss;
    oss << "SIG" ;
    if(this->coord == CoordX)
      oss << "x";
    if(this->coord == CoordY)
      oss << "y";
    if(this->coord == CoordZ)
      oss << "z";
    return oss.str();
  }
  SymOp reflect(Coord coord) {
    Reflect *ptr = new Reflect(coord);
    return SymOp(ptr);
  }

  // ---- Inversion Center ----
  void InvCent::getOp(const PrimGTO& a, PrimGTO* b, int *sig, bool *prim) const {
    *b = a;
    b->x = -a.x;
    b->y = -a.y;
    b->z = -a.z;
    *sig = pow(-1, a.nx + a.ny + a.nz);
    *prim = true;    
  }
  string InvCent::str() const {
    return "i";
  }
  SymOp inv() {
    InvCent *ptr = new InvCent();
    return SymOp(ptr);
  }

  // ==== Symmetry operation class ====
  SymOpClass ClassMono(SymOp o) {
    SymOpClass x;
    x.push_back(o);
    return x;
  }

  // ==== Symmmetry Group ====
  SymmetryGroup::SymmetryGroup(int num_class, string name):
    name_(name),
    prod_table_(num_class * num_class * num_class) {

    character_table_ = MatrixXi::Zero(num_class, num_class);
    prod_table_.SetRange(0, num_class-1,
			 0, num_class-1,
			 0, num_class-1);

    int num(num_class);;
    for(int i = 0; i < num; i++)
      for(int j = 0; j < num; j++) {
	character_table_(i, j) = 1;
	for(int k = 0; k < num; k++)
	  prod_table_(i, j, k) = false;    
      }
    
  }
  void SymmetryGroup::CheckIrrep(Irrep a) {
    if(a < 0 || this->order() <= a) {
      std::string msg; SUB_LOCATION(msg);
      throw runtime_error(msg);
    }
  }
  bool SymmetryGroup::Non0_Scalar(Irrep a, Irrep b) {
    this->CheckIrrep(a);
    this->CheckIrrep(b);
    return this->prod_table_(a, b, this->irrep_s);
  }
  bool SymmetryGroup::Non0_Z(Irrep a, Irrep b) {
    this->CheckIrrep(a);
    this->CheckIrrep(b);
    return this->prod_table_(a, b, this->irrep_z);
  }
  bool SymmetryGroup::Non0_4(Irrep a, Irrep b, Irrep c, Irrep d) {
    this->CheckIrrep(a);
    this->CheckIrrep(b);
    this->CheckIrrep(c);
    this->CheckIrrep(d);

    bool find_non0(false);
    for(Irrep i = 0; i < this->order(); i++) {
      if(this->prod_table_(a, b, i) && this->prod_table_(c, d, i))
	find_non0 = true;
    }
    return find_non0;
  }
  void SymmetryGroup::CalcSymMatrix(const vector<PrimGTO>& gtos,
				    MatrixXi& a_Ii, MatrixXi& sig_Ii) {
    int n(gtos.size());
    int n_g(this->order());
    a_Ii   = MatrixXi::Zero(n_g, n);
    sig_Ii = MatrixXi::Zero(n_g, n);

    for(int I = 0; I < n_g; I++) {
      for(int i = 0; i < n; i++) {
	for(int j = 0; j < n; j++) {
	  int sig = this->sym_op_[I]->Op(gtos[i], gtos[j]);
	  if(sig != 0) {
	    a_Ii(I, i) = j;
	    sig_Ii(I, i) = sig;
	    break;
	  }
	}
      }
    }
  }
  void SymmetryGroup::setProdTable() {
    int n(this->num_class());

    // -- set default value --
    for(Irrep irrep = 0; irrep < n; irrep++) {
      for(Irrep jrrep = 0; jrrep < n; jrrep++) {
	for(Irrep krrep = 0; krrep < n; krrep++) {
	  this->prod_table_(irrep, jrrep, krrep) = false;
	}
      }
    }

    // -- search product matching --
    for(Irrep irrep = 0; irrep < n; irrep++) {
      for(Irrep jrrep = 0; jrrep < n; jrrep++) {
	VectorXi veci(this->character_table_.row(irrep));
	VectorXi vecj(this->character_table_.row(jrrep));
	VectorXi vec_ij = veci.array() * vecj.array();
	for(Irrep krrep = 0; krrep < n; krrep++) {
	  VectorXi veck(this->character_table_.row(krrep));
	  if(vec_ij.dot(veck) != 0) {
	    this->prod_table_(irrep, jrrep, krrep) = true;
	  }
	}
      }
    }
  }
  void SymmetryGroup::setSymOp() {
    typedef std::vector<SymOpClass>::iterator It;
    typedef std::vector<SymOp>::iterator ItOp;
    for(It it = sym_op_class_.begin(); it != sym_op_class_.end(); ++it) 
      for(ItOp itop = it->begin(); itop != it->end(); ++itop)
	sym_op_.push_back(*itop);
  }
  string SymmetryGroup::str() const {
    ostringstream oss; 
    typedef std::vector<SymOpClass>::const_iterator It;
    typedef std::vector<SymOp>::const_iterator ItOp;
    oss << "==== SymmetryGroup ====" << endl;
    oss << "name : " << name_ << endl;
    oss << "order: " << this->order() << endl;
    oss << "num_class: " << this->num_class() << endl;
    oss << "operators: " << endl;
    for(ItOp it = sym_op_.begin(); it != sym_op_.end(); ++it) {
      oss << (*it)->str() << endl;
    }
    oss << "operator class:" << endl;
    for(It it = sym_op_class_.begin(); it != sym_op_class_.end(); ++it) {
      oss << int(it->size()) << (*it)[0]->str() << endl;
    }

    return oss.str();
  }
  void SymmetryGroup::Display() const {
    cout << this->str(); 
  }
  Irrep SymmetryGroup::GetIrrep(std::string name) const {
    typedef vector<string>::const_iterator It;
    for(It it = irrep_name_.begin(); it != irrep_name_.end(); ++it) {
      if(*it == name)
	return distance(irrep_name_.begin(), it);
    }
    string msg; SUB_LOCATION(msg);
    msg += ": fail to find name. Input name is " + name;
    throw runtime_error(msg);
  }
  string SymmetryGroup::GetIrrepName(Irrep irrep) const {
    return irrep_name_[irrep];
  }

  // ---- Specific symmetry group ----
  pSymmetryGroup SymmetryGroup::C1() {
    pSymmetryGroup g(new SymmetryGroup(1, "C1"));
    g->sym_op_class_.push_back(ClassMono(id()));
    g->irrep_name_.push_back("A");
    g->character_table_(0, 0) = 1;

    g->setSymOp();
    g->setProdTable();

    g->irrep_s = 0;
    g->irrep_x = 0;
    g->irrep_y = 0;
    g->irrep_z = 0;
    return g;
  }
  pSymmetryGroup SymmetryGroup::Cs() {
    pSymmetryGroup g(new SymmetryGroup(2, "Cs"));

    g->sym_op_class_.push_back(ClassMono(id()));
    g->sym_op_class_.push_back(ClassMono(reflect(CoordZ)));

    g->irrep_name_.push_back("A'");
    g->irrep_name_.push_back("A''");

    g->character_table_<<
      1, 1,
      1, -1;

    g->setSymOp();
    g->setProdTable();

    /*
    g->prod_table_(Ap, Ap, Ap) = true;
    g->prod_table_(Ap, App, App) = true;
    g->prod_table_(App, Ap, App) = true;
    g->prod_table_(App, App, Ap) = true;
    */

    Irrep Ap = 0;
    Irrep App= 1;
    g->irrep_s = Ap;
    g->irrep_x = Ap;
    g->irrep_y = Ap;
    g->irrep_z = App;
    return g;
  }
  pSymmetryGroup SymmetryGroup::C2h() {
    pSymmetryGroup g(new SymmetryGroup(4, "C2h"));

    g->sym_op_class_.push_back(ClassMono(id()));
    g->sym_op_class_.push_back(ClassMono(cyclic(CoordZ, 2)));
    g->sym_op_class_.push_back(ClassMono(inv()));
    g->sym_op_class_.push_back(ClassMono(reflect(CoordZ)));

    g->irrep_name_.push_back("Ag"); int Ag = 0;
    g->irrep_name_.push_back("Bg"); //int Bg = 1;
    g->irrep_name_.push_back("Au"); int Au = 2;
    g->irrep_name_.push_back("Bu"); int Bu = 3;
    
    g->character_table_ <<
      1, 1, 1, 1,
      1,-1, 1,-1,
      1, 1,-1,-1,
      1,-1,-1, 1;

    g->setSymOp();
    g->setProdTable();

    /*
    g->prod_table_(Ag, Ag, Ag) = true;
    g->prod_table_(Ag, Bg, Bg) = true;
    g->prod_table_(Ag, Au, Bu) = true;
    g->prod_table_(Ag, Bu, Bu) = true;

    g->prod_table_(Bg, Ag, Bg) = true;
    g->prod_table_(Bg, Bg, Ag) = true;
    g->prod_table_(Bg, Au, Bu) = true;
    g->prod_table_(Bg, Bu, Au) = true;

    g->prod_table_(Au, Ag, Au) = true;
    g->prod_table_(Au, Bg, Bu) = true;
    g->prod_table_(Au, Au, Ag) = true;
    g->prod_table_(Au, Bu, Bg) = true;

    g->prod_table_(Bu, Ag, Bu) = true;
    g->prod_table_(Bu, Bg, Au) = true;
    g->prod_table_(Bu, Au, Bg) = true;
    g->prod_table_(Bu, Bu, Ag) = true;
    */

    g->irrep_s = Ag;
    g->irrep_x = Bu;
    g->irrep_y = Bu;
    g->irrep_z = Au;

    return g;
  }
  pSymmetryGroup SymmetryGroup::C2v() {
    pSymmetryGroup g(new SymmetryGroup(4, "C2v"));

    g->sym_op_class_.push_back(ClassMono(id()));
    g->sym_op_class_.push_back(ClassMono(cyclic(CoordZ, 2)));
    g->sym_op_class_.push_back(ClassMono(reflect(CoordY)));
    g->sym_op_class_.push_back(ClassMono(reflect(CoordX)));

    g->irrep_name_.push_back("A1"); int A1 = 0;
    g->irrep_name_.push_back("A2"); //int Bg = 1;
    g->irrep_name_.push_back("B1"); int B1 = 2;
    g->irrep_name_.push_back("B2"); int B2 = 3;
    
    g->character_table_ <<
      1, 1, 1, 1,
      1, 1,-1,-1,
      1,-1, 1,-1,
      1,-1,-1, 1;

    g->setSymOp();
    g->setProdTable();

    g->irrep_s = A1;
    g->irrep_x = B1;
    g->irrep_y = B2;
    g->irrep_z = A1;

    return g;
  }
  pSymmetryGroup SymmetryGroup::D2h() {
    pSymmetryGroup g(new SymmetryGroup(8, "D2h"));
    
    g->sym_op_class_.push_back(ClassMono(id()));
    g->sym_op_class_.push_back(ClassMono(cyclic(CoordZ, 2)));
    g->sym_op_class_.push_back(ClassMono(cyclic(CoordY, 2)));
    g->sym_op_class_.push_back(ClassMono(cyclic(CoordX, 2)));
    g->sym_op_class_.push_back(ClassMono(inv()));
    g->sym_op_class_.push_back(ClassMono(reflect(CoordZ)));
    g->sym_op_class_.push_back(ClassMono(reflect(CoordY)));
    g->sym_op_class_.push_back(ClassMono(reflect(CoordX)));

    g->irrep_name_.push_back("Ag");  int Ag = 0;
    g->irrep_name_.push_back("B1g"); // int B1g = 1;
    g->irrep_name_.push_back("B2g"); // int B2g = 2;
    g->irrep_name_.push_back("B3g"); // int B3g = 3;
    g->irrep_name_.push_back("Au");  // int Au = 4;
    g->irrep_name_.push_back("B1u"); int B1u = 5;
    g->irrep_name_.push_back("B2u"); int B2u = 6;
    g->irrep_name_.push_back("B3u"); int B3u = 7;
    
    g->character_table_ <<
      1, 1, 1, 1, 1, 1, 1, 1,
      1, 1,-1,-1, 1, 1,-1,-1,
      1,-1, 1,-1, 1,-1, 1,-1,
      1,-1,-1, 1, 1,-1,-1, 1,
      1, 1, 1, 1,-1,-1,-1,-1,
      1, 1,-1,-1,-1,-1,+1,+1,
      1,-1, 1,-1,-1,+1,-1,+1,
      1,-1,-1,+1,-1,+1,+1,-1;

    g->setSymOp();
    g->setProdTable();

    g->irrep_s = Ag;
    g->irrep_x = B3u;
    g->irrep_y = B2u;
    g->irrep_z = B1u;

    return g;
  }
  pSymmetryGroup SymmetryGroup::C4() {
    pSymmetryGroup g(new SymmetryGroup(3, "C4"));
    
    g->sym_op_class_.push_back(ClassMono(id()));
    g->sym_op_class_.push_back(ClassMono(cyclic(CoordZ, 2)));

    SymOpClass C4;
    C4.push_back(cyclic(CoordZ, 4));
    C4.push_back(mult(cyclic(CoordZ, 4), 3));
    g->sym_op_class_.push_back(C4);

    g->irrep_name_.push_back("A");
    g->irrep_name_.push_back("B");
    g->irrep_name_.push_back("E");

    g->character_table_ <<
      1, 1, 1,
      1,-1, 1,
      2, 0,-2;

    g->setSymOp();
    g->setProdTable();

    g->irrep_s = 0;
    g->irrep_x = 2;
    g->irrep_y = 2;
    g->irrep_z = 0;
    return g;
  }
}
