#ifndef EOSLIB_UNIAXIAL_H
#define EOSLIB_UNIAXIAL_H

#define VonMisesElasticPlastic_vers "VonMisesElasticPlasticv2.0.1"
#define VonMisesElasticPlastic_date "Jan. 5, 2013"
#undef  BugReport
#define BugReport "Please report bugs to rtm@lanl.gov"

extern const char *VonMisesElasticPlasticlib_vers;
extern const char *VonMisesElasticPlasticlib_date;

extern "C" { // DataBase functions
    char *EOS_VonMisesElasticPlastic_Init();
    char *EOS_VonMisesElasticPlastic_Info();
    void *EOS_VonMisesElasticPlastic_Constructor();
}

class UniaxialEOS : public EOS		// hydrostatic pressure + shear stress
{
	friend class UniaxialIsotherm;
private:
	void operator=(const UniaxialEOS&);	// disallowed
	UniaxialEOS(const UniaxialEOS&);	// disallowed
	UniaxialEOS *operator &();		// disallowed, use Duplicate()
protected:
	EOS *eos;
	UniaxialEOS(const char *eos_type) : EOS(eos_type), eos(NULL) { }
    virtual int ConvertParams(Convert &convert);    
public:
	virtual ~UniaxialEOS();
	double P(double V, double e); 		// Pressure
	double T(double V, double e); 		// Temperature
	double S(double V, double e);		// Entropy
	
	double c2(double V, double e);		// sound speed squared
	double Gamma(double V, double e);	// Gruneisen coefficient
	double CV(double V, double e);		// Specific heat

	virtual int NotInDomain(double V, double e);
	//Isentrope *isentrope(const HydroState &state);
	//Hugoniot  *shock(const HydroState &state);
	virtual Isotherm  *isotherm(const HydroState &state);
	//int PT(double P, double T, HydroState &state);

  // Specialized functions
	virtual int         IsElastic(double V, double e) = 0;
	virtual double          Yield(double V, double e) = 0;
	virtual double   ShearModulus(double V, double e) = 0;
	virtual double    ShearEnergy(double V, double e) = 0;
	virtual double     stress_dev(double V, double e) = 0;
  // V on yield surface starting from state (V,e)
	virtual double Vy_compression(double V, double e) = 0;
	virtual double   Vy_expansion(double V, double e) = 0;
  //
	EOS *solid() { return eos ? eos->Duplicate() : NULL; }	
	double     P_solid(double V, double e) {return eos->P(V,e);    }
	double     T_solid(double V, double e) {return eos->T(V,e);    }
	double     S_solid(double V, double e) {return eos->S(V,e);    }
	double    c2_solid(double V, double e) {return eos->c2(V,e);   }
	double Gamma_solid(double V, double e) {return eos->Gamma(V,e);}
	double    CV_solid(double V, double e) {return eos->CV(V,e);   }
	double    FD_solid(double V, double e) {return eos->FD(V,e);   }

};

// Warning: treats plasticity as phase transition, no hysteresis
//          not strictly thermodynamically consistent
class VonMisesElasticPlastic : public UniaxialEOS
{
private:
	void operator=(const VonMisesElasticPlastic&);		// disallowed
	VonMisesElasticPlastic(const VonMisesElasticPlastic&);	// disallowed
	VonMisesElasticPlastic *operator &();	// disallowed, use Duplicate()
protected:
	double G;
	double Y;
	
	double Vy_comp;		// V at yield in compression
	double Vy_exp;		// V at yield in expansion

    int InitParams(Parameters &, Calc &, DataBase *db=NULL);
    void PrintParams(ostream &out);
    int ConvertParams(Convert &convert);    
public:
	VonMisesElasticPlastic();
	~VonMisesElasticPlastic();
    ostream &PrintComponents(ostream &out);
  // Specialized functions
	int         IsElastic(double V, double e);
	double          Yield(double V, double e);
	double   ShearModulus(double V, double e);
	double    ShearEnergy(double V, double e);
	double     stress_dev(double V, double e);
  // V on yield surface starting from state (V,e)
	double Vy_compression(double V, double e);
	double   Vy_expansion(double V, double e);
};

#endif // EOSLIB_UNIAXIAL_H
