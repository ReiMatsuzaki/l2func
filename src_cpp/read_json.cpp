#include <fstream>
#include "../utils/macros.hpp"
#include "read_json.hpp"

using namespace std;
using namespace picojson;
using namespace Eigen;

namespace cbasis {

  template<> void CheckValue<object>(picojson::value& val, int n, int m) {
    
    if(not val.is<object>()) {
      throw(runtime_error("value is not object. "));
    }
    if(n > 0) {
      if((int)val.get<object>().size() != n)
	throw(runtime_error("invalid size object."));
    }
  }
  template<> void CheckValue<string>(picojson::value& val, int n, int m) {
    if(not val.is<string>()) {
      throw(runtime_error("value is not string. "));
    }    
  }
  template<> void CheckValue<double>(picojson::value& val, int n, int m) {
    
    if(not val.is<double>()) {
      throw(runtime_error("value is not double."));
    }    
  }
  template<> void CheckValue<dcomplex>(picojson::value& val, int n, int m) {
    string msg = "value is not compelx (double or two element array of double).";
    if(val.is<array>()) {
      array& ary = val.get<array>();
      if(ary.size() != 2) {
	throw(runtime_error(msg));
      }
      if(not ary[0].is<double>())
	throw(runtime_error(msg));
      if(not ary[1].is<double>())
	throw(runtime_error(msg));
    } else if(not val.is<double>()) {
      throw runtime_error(msg);
    }
  }
  template<> void CheckValue<int>(picojson::value& val, int n, int m) {
    string msg = "value is not int. ";
    if(not val.is<double>()) {
      throw runtime_error(msg);
    }
    double x(val.get<double>());
    double y((double)(int)x);
    double eps(0.0000000001);
    if(abs(x-y) > eps) {
      throw runtime_error(msg);
    }
  }
  template<> void CheckValue<array>(picojson::value& val, int n, int m) {
    if(not val.is<array>()) {
      throw(runtime_error("value  is not array. "));
    }

    if(n > 0) {
      if((int)val.get<array>().size() != n) {
	throw(runtime_error("invalid size of array. "));
      }
    }
  }
  template<> void CheckValue<VectorXcd>(picojson::value& val, int n, int m) {

    if(val.is<array>()) {
      try {
	CheckValue<array>(val, n);
      } catch(exception& e) {
	throw(runtime_error("value is not array for VectorXcd."));
      }
      array& ary = val.get<array>();
      for(array::iterator it = ary.begin(); it != ary.end(); ++it) {
	try {
	  CheckValue<dcomplex>(*it);
	} catch(...) {
	  throw(runtime_error("element of value is not dcomplex for VectorXcd."));
	}
      }
      
    } else if(val.is<object>()) {
    } else {
      throw runtime_error("value must be array or object");
    }
  }
  template<> void CheckValue<VectorXi>(picojson::value& val, int n, int m) {

    if(val.is<array>()) {
      array& ary = val.get<array>();
      for(array::iterator it = ary.begin(); it != ary.end(); ++it) {
	try {
	  CheckValue<int>(*it);
	} catch(...) {
	  throw(runtime_error("element is not int for VectorXi."));
	}
      }
    } else {
      throw(runtime_error("value is not array for VectorXi."));
    }
    
  }
  template<> void CheckValue<MatrixXi>(picojson::value& val, int n, int m) {
    
    try {
      CheckValue<array>(val, -1);
    } catch(exception& e) {
      throw(runtime_error("value is not array for MatrixXi"));
    }
    array& ary = val.get<array>();
    
    if(n > 0) 
      if(m != (int)ary.size()) 
	throw(runtime_error("invalid rows size for MatrixXi"));
	
    for(array::iterator it = ary.begin(); it != ary.end(); ++it) {
      try {
	CheckValue<array>(*it, n);
      } catch(...) {
	throw(runtime_error("element of value is not array for MatrixXi."));
      }
      array& aryary = it->get<array>();
      if(m > 0) {
	if(m != (int)aryary.size()) {
	  throw(runtime_error("invalid cols size of element for MatrixXi."));
	}
      }
      for(array::iterator jt = aryary.begin(); jt != aryary.end(); ++jt) {
	try {
	  CheckValue<int>(*jt);
	} catch(...) {
	  throw(runtime_error("element of element of value is not int for MatrixXi"));
	}
      }
    }
        
  }
  template<> void CheckValue<MatrixXcd>(picojson::value& val, int n, int m) {
    
    try {
      CheckValue<array>(val, -1);
    } catch(exception& e) {
      throw(runtime_error("value is not array for MatrixXcd"));
    }
    array& ary = val.get<array>();
    for(array::iterator it = ary.begin(); it != ary.end(); ++it) {
      try {
	CheckValue<array>(*it, n);
      } catch(...) {
	throw(runtime_error("element of value is not array for MatrixXcd."));
      }
      array& aryary = it->get<array>();
      if(n > 0) {
	if(n != (int)aryary.size()) {
	  throw(runtime_error("invalid size of element for MatrixXcd."));
	}
      }
      for(array::iterator jt = aryary.begin(); jt != aryary.end(); ++jt) {
	try {
	  CheckValue<dcomplex>(*jt);
	} catch(...) {
	  throw(runtime_error("element of element of value is not dcomplex for MatrixXcd"));
	}
      }
    }  
  }
  template<class T>
  void CheckObject(picojson::object& obj, std::string k, int n, int m) {
    
    string key = "key \"" + k + "\"";
    if(obj.find(k) == obj.end()) {
      throw(runtime_error(key + " not found."));
    }
    
    try {
      CheckValue<T>(obj[k], n, m);
    } catch(exception& e) {
      string msg = "error on parsing value of " + key + "\n";
      msg += e.what();
      throw(runtime_error(msg));
    }
  }
  template void CheckObject<object>(object&, string, int , int);

  template<> string ReadJson<string>(value& json, int n, int m) {
    CheckValue<string>(json);
    return json.get<string>();
  }
  template<> double ReadJson<double>(value& json, int n, int m) {
    CheckValue<double>(json);
    return json.get<double>();
  }  
  template<> int ReadJson<int>(value& json, int n, int m) {
    CheckValue<int>(json, n, m);
    return (int)json.get<double>();;
  }  
  template<> dcomplex ReadJson<dcomplex>(value& json, int n, int m) {
    CheckValue<dcomplex>(json);
    dcomplex x(0);
    if(json.is<double>()) {
      x = json.get<double>();
    } else if(json.is<array>()) {
      array& ary = json.get<array>();
      x = dcomplex(ary[0].get<double>(), ary[1].get<double>());
    }   
    return x;
  }  
  template<> VectorXcd ReadJson<VectorXcd>(value& json, int n, int m) {

    CheckValue<VectorXcd>(json, n);

    if(json.is<array>()) {
      array& ary = json.get<array>();
      VectorXcd vec = VectorXcd::Zero(ary.size());
      
      for(int i = 0; i <(int) ary.size(); i++) {
	vec.coeffRef(i) = ReadJson<dcomplex>(ary[i]);
      }
      return vec;

      
    } else if(json.is<object>()) {
      object& obj = json.get<object>();
      string type = ReadJson<string>(obj, "type");
      if(type == "lin") {
	dcomplex x0 = ReadJson<dcomplex>(obj, "x0");
	dcomplex dx = ReadJson<dcomplex>(obj, "dx");
	int N = ReadJson<int>(obj, "N");
	VectorXcd vec(N);
	//	dcomplex dx = (xN-x0)/(N-1);
	for(int i =0; i<N; i++) {
	  vec(i) = x0 + dx * dcomplex(i);
	}
	return vec;
      } else if(type == "geo") {
	dcomplex x0 = ReadJson<dcomplex>(obj, "x0");
	dcomplex r = ReadJson<dcomplex>(obj, "r");
	int N = ReadJson<int>(obj, "N");
	VectorXcd vec(N);
	//	dcomplex r = (xN-x0)/(N-1);
	for(int i =0; i<N; i++) {
	  vec(i) = x0 * pow(r, i);
	}
	return vec;
      } else {
	throw runtime_error("type must be [lin or geo]");
      }
    } else {
      throw runtime_error("value must be array or object for VectorXcd");
    }
    
  }
  template<> VectorXd ReadJson<VectorXd>(value& json, int n, int m) {

    try {
      CheckValue<array>(json, n);
      array& ary = json.get<array>();
      VectorXd vec = VectorXd::Zero(ary.size());
      for(int i = 0; i <(int) ary.size(); i++) {
	vec.coeffRef(i) = ReadJson<double>(ary[i]);
      }
      return vec;
    } catch(exception& e) {
      throw runtime_error("error on parsing VectorXd");
    }
    
  }  
  template<> MatrixXcd ReadJson<MatrixXcd>(value& json, int _n, int _m) {
    CheckValue<MatrixXcd>(json, _n, _m);
    array& ary = json.get<array>();
    int n(0), m(0);
    n = ary.size();
    m = ary[0].get<array>().size();

    MatrixXcd mat = MatrixXcd::Zero(n, m);

    for(int i = 0; i < n; i++) {
      array& aryary = ary[i].get<array>();
      for(int j = 0; j < m; j++) {
	mat.coeffRef(i, j) = ReadJson<dcomplex>(aryary[j]);
      }
    }
    
    return mat;
  }
  template<> MatrixXd ReadJson<MatrixXd>(value& json, int _n, int _m) {
    CheckValue<array>(json, _n);
    array& ary = json.get<array>();
    int n = ary.size();

    if(n == 0) {
      throw runtime_error("no element for MatrixXd");
    }

    CheckValue<array>(ary[0], _m);
    array& aryary0 = ary[0].get<array>();
    int m = aryary0.size();

    MatrixXd mat = MatrixXd::Zero(n, m);    
    for(int i = 0; i < n; i++) {
      CheckValue<array>(ary[i], m);
      array& aryary = ary[i].get<array>();
      for(int j = 0; j < m; j++) {
	mat.coeffRef(i, j) = ReadJson<double>(aryary[j]);
      }
    }
    
    return mat;
  }  
  template<> VectorXi ReadJson<VectorXi>(value& json, int n, int m) {
    
    CheckValue<VectorXi>(json, n);

    if(json.is<array>()) {
      array& ary = json.get<array>();
      VectorXi vec = VectorXi::Zero(ary.size());
      
      for(int i = 0; i < (int)ary.size(); i++) {
	int x(ary[i].get<double>());
	vec.coeffRef(i) = x;
      }
      return vec;
    } else {
      throw runtime_error("value is not array for VectorXi");
    }
    
  }  
  template<> MatrixXi ReadJson<MatrixXi>(value& json, int _n, int _m) {
    CheckValue<MatrixXi>(json, _n, _m);
    array& ary = json.get<array>();
    int n(0), m(0);
    n = ary.size();
    m = ary[0].get<array>().size();
    MatrixXi mat = MatrixXi::Zero(n, m);

    for(int i = 0; i < n; i++) {
      array& aryary = ary[i].get<array>();
      for(int j = 0; j < m; j++) {
	mat.coeffRef(i, j) = (int)aryary[j].get<double>();
      }
    }
    
    return mat;
  }
  
  template<class T>
  T ReadJson(object& json, string key, int n, int m) {
    if(json.find(key) == json.end()) {
      throw runtime_error("key \"" + key + "\" not found");
    }
    T t;
    try {
      t = ReadJson<T>(json[key], n, m);
    } catch(exception& e) {
      ostringstream oss;
      oss << "error on parsing value of key \"" << key << "\"" << endl;
      oss << e.what() << endl;
      throw runtime_error(oss.str());
    }
    return t;
  }
  template int ReadJson<int>(object& o, string k, int n, int m);
  template VectorXd ReadJson<VectorXd>(object& o, string k, int n, int m);
  
  template<>
  value ToJson<dcomplex>(dcomplex& x) {
    double eps(0.000000000000001);
    if(abs(x.imag()) < eps ) {
      double re_x(x.real());
      return value(re_x);
    } else {
      array xs(2);
      xs[0] = value(x.real());
      xs[1] = value(x.imag());
      return value(xs);
    }
  }
  template<>
  value ToJson<int>(int& x) {
    double y(x);
    return value(y);
  }  
  
  template<> SymmetryGroup ReadJson<SymmetryGroup>( value& v, int n, int m) {
    string sym;
    try {
      sym = ReadJson<string>(v);
    } catch(exception& e) {
      string msg = "error on parsing SymmetryGroup.\n";
      msg += e.what();
      throw(runtime_error(msg));
    }
    
    if(sym == "C1")
      return SymmetryGroup_C1();
    else if(sym == "Cs")
      return SymmetryGroup_Cs();
    else if(sym == "C2h")
      return SymmetryGroup_Cs();
    else if(sym == "D2h")
      return SymmetryGroup_D2h();
    else
      throw(runtime_error("error on parsing SymmetryGroup.\nsym must be C1,Cs,C2h or D2h"));
  }
  template<> SymmetryGroup ReadJson<SymmetryGroup>(object& o, string k, int n, int m){
    
    if(o.find(k) == o.end()) {
      string msg = "key \"" + k + "\" not found in parsing SymmetryGroup";
      throw runtime_error(msg);
    }
    return ReadJson<SymmetryGroup>(o[k]);
  }
  void ReadJson_Molecule(value& v, Molecule mole) {

    try {
      CheckValue<array>(v);
    }catch(exception& e) {
      string msg = "error on parsing Molecule\n";
      msg += e.what();
      throw(runtime_error(msg));
    }

    string key;
    array& atoms = v.get<array>();
    for(array::iterator it = atoms.begin(); it != atoms.end(); ++it) {

      try {
	// -- build --
	CheckValue<object>(*it);
	object& obj_atom = it->get<object>();
	key = "atom"; string name = ReadJson<string>(obj_atom, "atom");
	key = "q";    dcomplex q = ReadJson<dcomplex>(obj_atom, "q");
	key = "xyz";  MatrixXcd xyz_mat = ReadJson<MatrixXcd>(obj_atom, "xyz", -1, 3);
	vector<Vector3cd> xyz0;
	for(int i = 0 ; i < xyz_mat.rows(); i++) {
	  xyz0.push_back(Vector3cd(xyz_mat(i, 0), xyz_mat(i, 1), xyz_mat(i, 2)));
	}
	vector<Vector3cd> xyz1;
	mole->sym_group()->CalcSymPosList(xyz0, &xyz1);

	Atom atom = NewAtom(name, q);
	for(int i = 0 ; i < (int)xyz1.size(); i++) {
	  atom->Add(xyz1[i][0], xyz1[i][1], xyz1[i][2]);
	}
	mole->Add(atom);
      } catch(exception& e) {
	int i = distance(atoms.begin(), it);
	ostringstream oss;
	oss << "error on parsing value of "
	    << i << "th element of molecule" << endl;
	oss << e.what();
	throw(runtime_error(oss.str()));
      }
    }

  }
  void ReadJson_Molecule(picojson::object& obj, std::string k, Molecule mole) {
    try {
      CheckObject<array>(obj, k);
      ReadJson_Molecule(obj[k], mole);
    } catch(exception& e) {
      string msg = "error on parsing Molecule\n";
      msg += e.what();
      throw runtime_error(msg);
    }
    
  }
  Reduction ReadJson_Reduction(value& json, SymmetryGroup sym){

    try {
      CheckValue<object>(json);
      object& obj = json.get<object>();
      string str_irrep = ReadJson<string>(obj, "irrep");
      MatrixXcd coef = ReadJson<MatrixXcd>(obj, "coef");
      int irrep = sym->GetIrrep(str_irrep);
      Reduction rds(irrep, coef);
      return rds;

    } catch(exception& e) {
      string msg = "error on parsing Reduction.\n";
      msg += e.what();
      throw(runtime_error(msg));      
    }
    
  }
  void ReadJson_SymGTOs_Subs_cart( object& basis, SymGTOs gtos) {

    try {
      
      // -- pn --
      Vector3i ns(ReadJson<VectorXi>(basis, "ns", -1, 3));

      // -- atom --
      string atom_name = ReadJson<string>(basis["atom"]);
      Molecule mole = gtos->molecule();
      if(not mole->exist_atom(atom_name))  {
	throw runtime_error("atom name \"" + atom_name + "\" not found");
      }
      Atom atom = mole->atom(atom_name);

      // -- zeta --
      VectorXcd zeta =  ReadJson<VectorXcd>(basis, "zeta");

      // -- build --
      SymmetryGroup sym = gtos->sym_group();
      SubSymGTOs sub(gtos->sym_group(), atom);
      gtos->AddSub(Sub_Mono(sym, atom, 0, ns, zeta));

    } catch(exception& e) {
      string msg = "error on parsing SymGTOs_Subs_cart.\n";
      msg += e.what();
      throw(runtime_error(msg));      
    }    

  }
  void ReadJson_SymGTOs_Subs_full( object& basis, SymGTOs gtos) {
    
    try {

      // -- atom --
      string atom_name = ReadJson<string>(basis, "atom");
      Molecule mole = gtos->molecule();
      Atom atom = mole->atom(atom_name);

      // -- sub --
      SymmetryGroup sym = gtos->sym_group();
      SubSymGTOs sub(sym, atom);

      // -- ns --
      MatrixXi ns = ReadJson<MatrixXi>(basis, "ns", -1, 3);
      for(int i = 0; i < ns.rows(); i++) {
	sub.AddNs(ns(i, 0), ns(i, 1), ns(i, 2));
      }
      
      // -- reduction --
      CheckObject<array>(basis, "rds");
      array& rds_list = basis["rds"].get<array>();
      for(array::iterator it = rds_list.begin(); it != rds_list.end(); ++it) {
	Reduction rds = ReadJson_Reduction(*it, sym);
	sub.AddRds(rds);
      }

      // -- zeta --
      VectorXcd zeta = ReadJson<VectorXcd>(basis["zeta"]);
      sub.AddZeta(zeta);

      gtos->AddSub(sub);
      
    } catch(exception& e) {
      string msg = "error on parsing SymGTOs_Subs_cart.\n";
      msg += e.what();
      throw(runtime_error(msg));      
    }    
  }
  void ReadJson_SymGTOs_Subs_SolidSH( object& basis, SymGTOs gtos) {

    try {

      // -- atom --
      string atom_name = ReadJson<string>(basis, "atom");
      Molecule mole = gtos->molecule();
      if(not mole->exist_atom(atom_name))  {
	throw runtime_error("atom name \"" + atom_name + "\" not found");
      }
      Atom atom = mole->atom(atom_name);

      // -- L --
      int L = ReadJson<int>(basis, "L");

      // -- M --
      VectorXi Ms = ReadJson<VectorXi>(basis, "Ms");
      
      // -- zeta --
      VectorXcd zeta =  ReadJson<VectorXcd>(basis, "zeta");

      // -- build --
      SymmetryGroup sym = gtos->sym_group();
      SubSymGTOs sub(Sub_SolidSH_Ms(sym, atom, L, Ms, zeta));
      gtos->AddSub(sub);

    } catch(exception& e) {
      string msg = "error on parsing SymGTOs_Subs_SolidSH.\n";
      msg += e.what();
      throw(runtime_error(msg));      
    }    

  }
  void ReadJson_SymGTOs_Subs(picojson::value& json, SymGTOs gtos) {

    try {
      /*
      if(json.is<string>()) {
	string path = ReadJson<string>(json);
	fstream f; f.open(path.c_str(), ios::in);
	value new_json; f >> new_json;
	cout << "new_json: " << new_json.to_str() << endl;
	ReadJson_SymGTOs_Subs(new_json, gtos);
	return;
      } 
      */
      CheckValue<array>(json);
      array& ary = json.get<array>();
      
      for(array::iterator it = ary.begin(); it != ary.end(); ++it) {
	
	CheckValue<object>(*it);
	object& basis = it->get<object>();
	
	CheckObject<string>(basis, "type");
	string type = ReadJson<string>(basis, "type");
	if(type == "cart")
	  ReadJson_SymGTOs_Subs_cart(basis, gtos);
	else if(type == "full")
	  ReadJson_SymGTOs_Subs_full(basis, gtos);
	else if(type == "solid_sh")
	  ReadJson_SymGTOs_Subs_SolidSH(basis, gtos);
	else 
	  throw(runtime_error("unsupported type: " + type));
      }
    } catch(exception& e) {
      string msg = "error on parsing SymGTOs_Subs\n";
      msg += e.what();
      throw runtime_error(msg);
    }
  }
  void ReadJson_SymGTOs_Subs(picojson::object& obj, std::string k, SymGTOs gtos) {
    if(obj.find(k) == obj.end()) {
      throw runtime_error("key \"" + k + "\" not found.");
    }
    ReadJson_SymGTOs_Subs(obj[k], gtos);
  }

  void ReadJson_Orbital_file(object& initstate, SymmetryGroup _sym, dcomplex *_E0,
			     VectorXcd *_c0, Irrep *_irrep0, int *_i0) {

  try {
    string irrep_str = ReadJson<string>(initstate, "irrep");
    *_irrep0 = _sym->GetIrrep(irrep_str);
    *_i0 = ReadJson<int>(initstate, "i0");
  } catch(exception& e) {
    string msg = "error on parsing Orbital_file.\n";
    msg += e.what();
    throw runtime_error(msg);
  }

  try {
    string eigvecs_file = ReadJson<string>(initstate, "eigvecs");
    BMat eigvecs;
    eigvecs.Read(eigvecs_file);
    *_c0 = eigvecs(*_irrep0, *_irrep0).col(*_i0);    
  } catch(exception& e) {
    string msg = "error on reading Orbital_file.\n";
    msg += e.what();
    throw runtime_error(msg);
  }

  try {
    string eigvals_file = ReadJson<string>(initstate, "eigvals");
    BVec eigvals;
    eigvals.Read(eigvals_file);
    *_E0 = eigvals(*_irrep0)(*_i0);
  } catch(exception& e) {
    string msg = "error on parsing Orbital_file.\n";
    msg += e.what();
    throw runtime_error(msg);
  }

}
  void ReadJson_Orbital(object& obj, string k, SymmetryGroup _sym,
			dcomplex *_E0, VectorXcd *_c0, Irrep *_irrep0, int *_i0) {
  
  try {
    CheckObject<object>(obj, k);
    object& initstate = obj[k].get<object>();
    string type = ReadJson<string>(initstate, "type");

    if(type == "file")
      ReadJson_Orbital_file(initstate, _sym, _E0, _c0, _irrep0, _i0);
    else
      throw runtime_error("type <- [file]");
    

  } catch(exception& e) {
    string msg = "error on parsing Orbital. key = \"" + k + "\"\n";
    msg += e.what();
    throw runtime_error(msg);
  }
}

}
