// generated from file './/gain.dsp' by dsp2cc:
// Code generated with Faust 2.15.11 (https://faust.grame.fr)


namespace gain {

class Dsp {
private:
	uint32_t fSamplingFreq;
	double fConst0;
	double power0;
	double db_zero;
	float fVslider0;
	float *fVslider0_;
	double fRec3[2];
	double fRec0[2];
	int iRec1[2];
	double fRec2[2];
	float fVbargraph0;
	float *fVbargraph0_;
	float bypass;
	float *bypass_;

	void connect(uint32_t port,void* data);
	void clear_state_f();
	void init(uint32_t samplingFreq);
	void compute(int count, float *input0, float *output0);

public:
	static void clear_state_f_static(Dsp*);
	static void init_static(uint32_t samplingFreq, Dsp*);
	static void compute_static(int count, float *input0, float *output0, Dsp*);
	static void del_instance(Dsp *p);
	static void connect_static(uint32_t port,void* data, Dsp *p);
	Dsp() {};
	~Dsp() {};
};

inline void Dsp::clear_state_f()
{
	for (int l0 = 0; (l0 < 2); l0 = (l0 + 1)) fRec3[l0] = 0.0;
	for (int l1 = 0; (l1 < 2); l1 = (l1 + 1)) fRec0[l1] = 0.0;
	for (int l2 = 0; (l2 < 2); l2 = (l2 + 1)) iRec1[l2] = 0;
	for (int l3 = 0; (l3 < 2); l3 = (l3 + 1)) fRec2[l3] = 0.0;
}

void Dsp::clear_state_f_static(Dsp *p)
{
	p->clear_state_f();
}

inline void Dsp::init(uint32_t samplingFreq)
{
	fSamplingFreq = samplingFreq;
	fConst0 = (1.0 / std::min<double>(192000.0, std::max<double>(1.0, double(fSamplingFreq))));
	fVslider0 = 0.0;
	db_zero = 20.*log10(0.0000003); // -130db
	power0 = 20.*log10(0.0000003); // -130db
	clear_state_f();
}

void Dsp::init_static(uint32_t samplingFreq, Dsp *p)
{
	p->init(samplingFreq);
}

void Dsp::compute(int count, float *input0, float *output0)
{
#define bypass (*bypass_)
#define fVslider0 (*fVslider0_)
#define fVbargraph0 (*fVbargraph0_)
	double fSlow0 = (0.0010000000000000009 * std::pow(10.0, (0.050000000000000003 * double(fVslider0))));
	for (int i = 0; (i < count); i = (i + 1)) {
		int iTemp0 = (iRec1[1] < 4096);
		fRec3[0] = (fSlow0 + (0.999 * fRec3[1]));
		double fTemp1 = (fRec3[0] * double(input0[i]));
		double fTemp2 = std::max<double>(fConst0, std::fabs(fTemp1));
		fRec0[0] = (iTemp0?std::max<double>(fRec0[1], fTemp2):fTemp2);
		iRec1[0] = (iTemp0?(iRec1[1] + 1):1);
		fRec2[0] = (iTemp0?fRec2[1]:fRec0[1]);
		power0 = fRec2[0];
		output0[i] = fTemp1;
		fRec3[1] = fRec3[0];
		fRec0[1] = fRec0[0];
		iRec1[1] = iRec1[0];
		fRec2[1] = fRec2[0];
	}
    fVbargraph0 = (bypass? 20.*log10(power0) : db_zero);
    
#undef bypass
#undef fVslider0
#undef fVbargraph0
}

void Dsp::compute_static(int count, float *input0, float *output0, Dsp *p)
{
	p->compute(count, input0, output0);
}


void Dsp::connect(uint32_t port,void* data)
{
	switch ((PortIndex)port)
	{
	case BYPASS: 
		bypass_ = (float*)data; // , 0.0, 0.0, 1.0, 1.0 
		break;
	case GAIN: 
		fVslider0_ = (float*)data; // , 0.0, -40.0, 40.0, 0.25 
		break;
	case METER: 
		fVbargraph0_ = (float*)data; // , -70.0, -70.0, 4.0, 0.00001 
		break;
	default:
		break;
	}
}

void Dsp::connect_static(uint32_t port,void* data, Dsp *p)
{
	p->connect(port, data);
}


Dsp *plugin() {
	return new Dsp();
}

void Dsp::del_instance(Dsp *p)
{
	delete p;
}

/*
typedef enum
{
   GAIN, 
} PortIndex;
*/

} // end namespace gain
