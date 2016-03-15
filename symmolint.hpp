#ifndef SYMMOLINT_H
#define SYMMOLINT_H

#include "math_utils.hpp"
#include <Eigen/Core>
#include <string>
#include <map>

namespace l2func {

  // ==== Each integrate ====

  // ==== Type def ====
  typedef int Irrep;
  typedef std::map<std::pair<Irrep, Irrep>, Eigen::MatrixXcd> BMat;
  typedef std::map<std::string, BMat> BMatMap;

  void swap(BMat& a, BMat& b);
  void swap(BMatMap& a, BMatMap& b);

  // ==== Symmetry Group ====
  // ---- Class ----
  class SymmetryGroup {
  private:
    int order_;
    std::string name_;
  public:
    SymmetryGroup(int order, std::string name);
    int order()  const { return order_; }
    std::string name() const { return name_; }
    std::string str() const;
    void Display() const;
    void CheckIrrep(Irrep a) const;
    // Irrep GetIrrep(int n) const;
    bool IncludeScalar_2(Irrep a, Irrep b) const;
    // bool IncludeScalar_3(const Irrep& a, const Irrep& b, const Irrep& c) const;
    bool IncludeZ_2(Irrep a, Irrep b) const;
  };
  SymmetryGroup SymmetryGroup_Cs();
  Irrep Cs_Ap(); 
  Irrep Cs_App();
  SymmetryGroup SymmetryGroup_C1();

  // ==== BlockMatrixSets ==== 
  class BMatSets {
  private:
    SymmetryGroup sym_;
    BMatMap mat_map_;
  public:
    BMatSets(SymmetryGroup _sym);
    void SetMatrix(std::string name, Irrep i, Irrep j, Eigen::MatrixXcd& a);
    const Eigen::MatrixXcd& GetMatrix(std::string name, Irrep i, Irrep j);
  };
  
  // ==== AO Reduction Sets ====
  struct ReductionSets {
    Irrep sym;
    Eigen::MatrixXcd coef_iat_ipn;

    // ---- for calculation  ----
    Eigen::VectorXcd coef_iz;
    int offset;

    //    ReductionSets() {}
    ReductionSets(int _sym, Eigen::MatrixXcd _coef):
      sym(_sym), coef_iat_ipn(_coef), offset(0) {}
    std::string str() const;
    void Display() const;
    Irrep irrep() const { return sym; }
    const Eigen::MatrixXcd get_coef_iat_ipn() const { return coef_iat_ipn; }
    void set_zs_size(int num_zs) {
      coef_iz = Eigen::VectorXcd::Zero(num_zs);
    }
  };

  // ==== Sub sets of SymGTOs ====
  struct SubSymGTOs {
    // ---- data ----
    Eigen::MatrixXcd xyz_iat;
    Eigen::MatrixXi  ns_ipn;
    std::vector<ReductionSets> rds;
    Eigen::VectorXcd zeta_iz;

    // ---- for calculation  ----
    int maxn;

    int size_at() const { return xyz_iat.cols();}
    int size_pn() const { return ns_ipn.cols(); }
    int size_cont() const { return rds.size(); }
    int size_zeta() const { return zeta_iz.rows(); }
    Eigen::MatrixXcd get_xyz_iat() { return xyz_iat; }
    Eigen::MatrixXi  get_ns_ipn() const { return  ns_ipn; }
    ReductionSets  get_rds(int i) const { return  rds[i]; }
    Eigen::VectorXcd get_zeta_iz() const { return  zeta_iz; }
    SubSymGTOs(Eigen::MatrixXcd xyz, Eigen::MatrixXi ns,
	       std::vector<ReductionSets> cs, Eigen::VectorXcd zs);
    std::string str() const;
    void Display() const;
  };

  // ---- Helper ----
  SubSymGTOs Sub_s(Irrep sym, Eigen::Vector3cd xyz, Eigen::VectorXcd zs);
  SubSymGTOs Sub_pz(Irrep sym, Eigen::Vector3cd xyz, Eigen::VectorXcd zs);
  SubSymGTOs Sub_TwoSGTO(SymmetryGroup sym, Irrep irrep,
			 Eigen::Vector3cd xyz, Eigen::VectorXcd zs);

  // ==== SymGTOs ====
  class SymGTOs {
  public:
    SymmetryGroup sym_group;
    std::vector<SubSymGTOs> subs;
    Eigen::MatrixXcd xyzq_iat;
    bool setupq;
  public:
    // ---- Constructors ----
    SymGTOs(SymmetryGroup _sym_group);

    // ---- Accessors ----    
    int size_atom() const;
    int size_basis_isym(Irrep isym) const;
    std::string str() const;

    // ---- Add information ----
    void SetAtoms(Eigen::MatrixXcd _xyzq_iat);
    void AddSub(SubSymGTOs);
    void SetUp();
  private:
    void SetOffset();
    void Normalize();
  public:
    // ---- Calculation ----
    void loop();
    void CalcMat(BMatMap* res);
    void STVMat(BMatMap* res);
    void ZMat(BMatMap* res);

    void AtR_Ylm_add_center(int L, int M, const Eigen::VectorXcd& rs,
			    const Eigen::MatrixXcd& cs_irrep_ibasis,  
			    Eigen::VectorXcd* vs,
			    std::vector<SubSymGTOs>::const_iterator it);
    void AtR_Ylm(int L, int M,  int irrep,
		 const Eigen::VectorXcd& cs_ibasis,
		 const Eigen::VectorXcd& rs, Eigen::VectorXcd* vs );
  };
}

#endif
