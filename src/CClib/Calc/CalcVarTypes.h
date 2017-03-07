#ifndef CCLIB_CALC_VAR_TYPES_H
#define CCLIB_CALC_VAR_TYPES_H

class CalcVar                           // abstract base class
{
public:
    enum TYPE { UNDEF, DOUBLE, INT, STRING };
protected:
    TYPE type;    
public:
    CalcVar() : type(UNDEF)       {}    // needed for NameArray<CalcVar>
    CalcVar(TYPE typ) : type(typ) {}
    virtual ~CalcVar();
    TYPE Type() const    {return type;}
    
	virtual int get(double &value);
	virtual int set(double  value);
    
	virtual int get(const char *&value);
	virtual int set(const char *value);
};


//  derived types

class CVdouble : public CalcVar
{
private:
    double x;
public:
    CVdouble(double value);
    ~CVdouble();
    
	int get(double &value);
	int set(double value);
};

class CVpdouble : public CalcVar
{
private:
    double *x;
public:
    CVpdouble(double *value);
    ~CVpdouble();
    
	int get(double &value);
	int set(double value);
};


class CVpinvdouble : public CalcVar
{
private:
    double *x;
public:
    CVpinvdouble(double *value);
    ~CVpinvdouble();
    
	int get(double &value);
	int set(double value);
};



class CVint : public CalcVar
{
private:
    int x;
public:
    CVint(int value);
    ~CVint();
    
	int get(double &value);
	int set(double value);
};

class CVpint : public CalcVar
{
private:
    int *x;
public:
    CVpint(int *value);
    ~CVpint();
    
	int get(double &value);
	int set(double value);
};

// ToDo
class CVpstring : public CalcVar
{
    char **string;
public:
    CVpstring(char **value);
    ~CVpstring();
    
	int get(const char *&value);
	int set(const char *value);
};

class CVstring : public CalcVar
{
    char *string;
public:
    CVstring(const char *value);
    ~CVstring();
    
	int get(const char *&value);
	int set(const char *value);
};

#endif // CCLIB_CALC_VAR_TYPES_H

