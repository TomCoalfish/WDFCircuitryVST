/*
  ==============================================================================

    FilterObjects.h
    Created: 27 Oct 2020 8:04:53pm
    Author:  Richie Haynes

  ==============================================================================
*/
#pragma once


class IAudioSignalProcessor
{
public:
    float kPi = 3.14159265;
    // --- pure virtual, derived classes must implement or will not compile
    //     also means this is a pure abstract base class and is incomplete,
    //     so it can only be used as a base class
    //

    /** initialize the object with the new sample rate */
    virtual bool reset(double _sampleRate) = 0;

    /** process one sample in and out */
    virtual double processAudioSample(double xn) = 0;

    /** return true if the derived object can process a frame, false otherwise */
    virtual bool canProcessAudioFrame() = 0;

    /** set or change the sample rate; normally this is done during reset( ) but may be needed outside of initialzation */
    virtual void setSampleRate(double _sampleRate) {}

    /** switch to enable/disable the aux input */
    virtual void enableAuxInput(bool enableAuxInput) {}

    /** for processing objects with a sidechain input or other necessary aux input
            the return value is optional and will depend on the subclassed object */
    virtual double processAuxInputAudioSample(double xn)
    {
        // --- do nothing
        return xn;
    }

    /** for processing objects with a sidechain input or other necessary aux input
    --- optional processing function
        e.g. does not make sense for some objects to implement this such as inherently mono objects like Biquad
             BUT a processor that must use both left and right channels (ping-pong delay) would require it */
    virtual bool processAudioFrame(const float* inputFrame,        /* ptr to one frame of data: pInputFrame[0] = left, pInputFrame[1] = right, etc...*/
                                   float* outputFrame,
                                   uint32_t inputChannels,
                                   uint32_t outputChannels)
    {
        // --- do nothing
        return false; // NOT handled
    }
    virtual ~IAudioSignalProcessor() {}

};

// ------------------------------------------------------------------ //
// --- WDF LIBRARY -------------------------------------------------- //
// ------------------------------------------------------------------ //

/**
\class IComponentAdaptor
\ingroup Interfaces
\brief
Use this interface for objects in the WDF Ladder Filter library; see book for more information.

\author Will Pirkle http://www.willpirkle.com
\remark This object is included in Designing Audio Effects Plugins in C++ 2nd Ed. by Will Pirkle
\version Revision : 1.0
\date Date : 2018 / 09 / 7
*/
class IComponentAdaptor
{
public:
    /** initialize with source resistor R1 */
    virtual void initialize(double _R1) {}

    /** initialize all downstream adaptors in the chain */
    virtual void initializeAdaptorChain() {}

    /** set input value into component port  */
    virtual void setInput(double _in) {}

    /** get output value from component port  */
    virtual double getOutput() { return 0.0; }

    // --- for adaptors
    /** ADAPTOR: set input port 1  */
    virtual void setInput1(double _in1) = 0;

    /** ADAPTOR: set input port 2  */
    virtual void setInput2(double _in2) = 0;

    /** ADAPTOR: set input port 3 */
    virtual void setInput3(double _in3) = 0;

    /** ADAPTOR: get output port 1 value */
    virtual double getOutput1() = 0;

    /** ADAPTOR: get output port 2 value */
    virtual double getOutput2() = 0;

    /** ADAPTOR: get output port 3 value */
    virtual double getOutput3() = 0;

    /** reset the object with new sample rate */
    virtual void reset(double _sampleRate) {}

    /** get the commponent resistance from the attached object at Port3 */
    virtual double getComponentResistance() { return 0.0; }

    /** get the commponent conductance from the attached object at Port3 */
    virtual double getComponentConductance() { return 0.0; }

    /** update the commponent resistance at Port3 */
    virtual void updateComponentResistance() {}

    /** set an individual component value (may be R, L, or C */
    virtual void setComponentValue(double _componentValue) { }

    /** set LC combined values */
    virtual void setComponentValue_LC(double componentValue_L, double componentValue_C) { }

    /** set RL combined values */
    virtual void setComponentValue_RL(double componentValue_R, double componentValue_L) { }

    /** set RC combined values */
    virtual void setComponentValue_RC(double componentValue_R, double componentValue_C) { }

    /** get a component value */
    virtual double getComponentValue() { return 0.0; }
    
    virtual ~IComponentAdaptor() {}

};

// ------------------------------------------------------------------ //
// --- WDF COMPONENTS & COMMBO COMPONENTS --------------------------- //
// ------------------------------------------------------------------ //
/**
\class WdfResistor
\ingroup WDF-Objects
\brief
The WdfResistor object implements the reflection coefficient and signal flow through
a WDF simulated resistor.

\author Will Pirkle http://www.willpirkle.com
\remark This object is included in Designing Audio Effects Plugins in C++ 2nd Ed. by Will Pirkle
\version Revision : 1.0
\date Date : 2018 / 09 / 7
*/
class WdfResistor : public IComponentAdaptor
{
public:
    WdfResistor(double _componentValue) { componentValue = _componentValue; }
    WdfResistor() { }
    virtual ~WdfResistor() {}

    /** set sample rate and update component */
    void setSampleRate(double _sampleRate)
    {
        sampleRate = _sampleRate;
        updateComponentResistance();
    }

    /** get component's value as a resistance */
    virtual double getComponentResistance() { return componentResistance; }

    /** get component's value as a conducatance (or admittance) */
    virtual double getComponentConductance() { return 1.0 / componentResistance; }

    /** get the component value */
    virtual double getComponentValue() { return componentValue; }

    /** set the component value */
    virtual void setComponentValue(double _componentValue)
    {
        componentValue = _componentValue;
        updateComponentResistance();
    }

    /** change the resistance of component */
    virtual void updateComponentResistance() { componentResistance = componentValue; }

    /** reset the component; clear registers */
    virtual void reset(double _sampleRate) { setSampleRate(_sampleRate);  zRegister = 0.0; }

    /** set input value into component; NOTE: resistor is dead-end energy sink so this function does nothing */
    virtual void setInput(double in) {}

    /** get output value; NOTE: a WDF resistor produces no reflected output */
    virtual double getOutput() { return 0.0; }

    /** get output1 value; only one resistor output (not used) */
    virtual double getOutput1() { return  getOutput(); }

    /** get output2 value; only one resistor output (not used) */
    virtual double getOutput2() { return  getOutput(); }

    /** get output3 value; only one resistor output (not used) */
    virtual double getOutput3() { return  getOutput(); }

    /** set input1 value; not used for components */
    virtual void setInput1(double _in1) {}

    /** set input2 value; not used for components */
    virtual void setInput2(double _in2) {}

    /** set input3 value; not used for components */
    virtual void setInput3(double _in3) {}

protected:
    double zRegister = 0.0;            ///< storage register (not used with resistor)
    double componentValue = 0.0;    ///< component value in electronic form (ohm, farad, henry)
    double componentResistance = 0.0;///< simulated resistance
    double sampleRate = 0.0;        ///< sample rate
};

/**
\class WdfDiode
\ingroup WDF-Objects
\brief
*/

class WdfGZ34Diode : public IComponentAdaptor
{
public:
    
    WdfGZ34Diode(double _componentValue) { componentValue = _componentValue; }
    WdfGZ34Diode() { }
    
    virtual ~WdfGZ34Diode() {}

    /** set sample rate and update component */
    void setSampleRate(double _sampleRate)
    {
        sampleRate = _sampleRate;
        updateComponentResistance();
    }
    
    /** get component's value as a resistance */
    virtual double getComponentResistance() { return componentResistance; }

    /** get component's value as a conducatance (or admittance) */
    virtual double getComponentConductance() { return 1.0 / componentResistance; }

    /** get the component value */
    virtual double getComponentValue() { return componentValue; }

    /** set the component value */
    virtual void setComponentValue(double _componentValue)
    {
        componentValue = _componentValue;
        updateComponentResistance();
    }

    /** change the resistance of component */ // This is where we do maths on reflected output
    virtual void updateComponentResistance()
    {
        
        
        // For a diode ::
        // V = (a + b)/2, I = (a - b)/(2*Rp)
        // where a = input1 (incident wave), b = input 2 (reflected wave)
        // b = a + 2(Rp*Is) - 2(nD*VT*(W((RpIs/ndVT)*e^((RpIs+a)/nDVT)))
        
        // nD = diodde ideality factor
        // Rp = port resistance
        // VT = thermal Voltage (K)
        // W = lambert function??
        
        
        
        x = (Rp*Is)/(nD*Vt) * pow(e, (((Rp*Is) + zRegister)/(nD*Vt)));
        
        
        componentResistance /* (b) */ = zRegister /* a */ + 2*(componentValue * Is);
        
        outValue = componentResistance + Rdiode;
    }


    /** reset the component; clear registers */
    virtual void reset(double _sampleRate) { setSampleRate(_sampleRate);  zRegister = 0.0; }
    
    /** set input value into component; NOTE: diode is dead-end energy sink so this function does nothing */
    virtual void setInput(double in) {
        
        zRegister = in;
    }
    

    /** get output value; NOTE: a WDF diode produces no reflected output */
    virtual double getOutput() { return outValue; }

    
    
protected:
    double zRegister = 0.0;            ///< storage register (not used with resistor)
    double nextRegister = 0.0;
    double outValue;
    double componentValue = 0.0;    ///< component value in electronic form (ohm, farad, henry)
    double componentResistance = 0.0;///< simulated resistance
    double sampleRate = 0.0;        ///< sample rate
    // accurate simulation of GZ34 valve diode.
    double Is = 4.35e-9;    // reverse saturation current
    double Vt = 0.7;    // thermal voltage
    double nD = 1.906;    // Ideality factor
    // initial value for the voltage over the diode (n-1 memory)
    double Vdiode = 0.0;
    double b;
    double Rdiode = 1000; // Enough resistance for diode voltage drop
    double Rp = 100;
    
    double x;
    double e = 2.7182;
};


/**
\class WdfCapacitor
\ingroup WDF-Objects
\brief
The WdfCapacitor object implements the reflection coefficient and signal flow through
a WDF simulated capacitor.

\author Will Pirkle http://www.willpirkle.com
\remark This object is included in Designing Audio Effects Plugins in C++ 2nd Ed. by Will Pirkle
\version Revision : 1.0
\date Date : 2018 / 09 / 7
*/
class WdfCapacitor : public IComponentAdaptor
{
public:
    WdfCapacitor(double _componentValue) { componentValue = _componentValue; }
    WdfCapacitor() { }
    virtual ~WdfCapacitor() {}

    /** set sample rate and update component */
    void setSampleRate(double _sampleRate)
    {
        sampleRate = _sampleRate;
        updateComponentResistance();
    }

    /** get component's value as a resistance */
    virtual double getComponentResistance() { return componentResistance; }

    /** get component's value as a conducatance (or admittance) */
    virtual double getComponentConductance() { return 1.0 / componentResistance; }

    /** get the component value */
    virtual double getComponentValue() { return componentValue; }

    /** set the component value */
    virtual void setComponentValue(double _componentValue)
    {
        componentValue = _componentValue;
        updateComponentResistance();
    }

    /** change the resistance of component */
    virtual void updateComponentResistance()
    {
        componentResistance = 1.0 / (2.0*componentValue*sampleRate);
    }

    /** reset the component; clear registers */
    virtual void reset(double _sampleRate) { setSampleRate(_sampleRate); zRegister = 0.0; }

    /** set input value into component; NOTE: capacitor sets value into register*/
    virtual void setInput(double in) { zRegister = in; }

    /** get output value; NOTE: capacitor produces reflected output */
    virtual double getOutput() { return zRegister; }    // z^-1

    /** get output1 value; only one capacitor output (not used) */
    virtual double getOutput1() { return  getOutput(); }

    /** get output2 value; only one capacitor output (not used) */
    virtual double getOutput2() { return  getOutput(); }

    /** get output3 value; only one capacitor output (not used) */
    virtual double getOutput3() { return  getOutput(); }

    /** set input1 value; not used for components */
    virtual void setInput1(double _in1) {}

    /** set input2 value; not used for components */
    virtual void setInput2(double _in2) {}

    /** set input3 value; not used for components */
    virtual void setInput3(double _in3) {}

protected:
    double zRegister = 0.0;            ///< storage register (not used with resistor)
    double componentValue = 0.0;    ///< component value in electronic form (ohm, farad, henry)
    double componentResistance = 0.0;///< simulated resistance
    double sampleRate = 0.0;        ///< sample rate
};








/**
\class WdfInductor
\ingroup WDF-Objects
\brief
The WdfInductor object implements the reflection coefficient and signal flow through
a WDF simulated inductor.

\author Will Pirkle http://www.willpirkle.com
\remark This object is included in Designing Audio Effects Plugins in C++ 2nd Ed. by Will Pirkle
\version Revision : 1.0
\date Date : 2018 / 09 / 7
*/
class WdfInductor : public IComponentAdaptor
{
public:
    WdfInductor(double _componentValue) { componentValue = _componentValue; }
    WdfInductor() { }
    virtual ~WdfInductor() {}

    /** set sample rate and update component */
    void setSampleRate(double _sampleRate)
    {
        sampleRate = _sampleRate;
        updateComponentResistance();
    }

    /** get component's value as a resistance */
    virtual double getComponentResistance() { return componentResistance; }

    /** get component's value as a conducatance (or admittance) */
    virtual double getComponentConductance() { return 1.0 / componentResistance; }

    /** get the component value */
    virtual double getComponentValue() { return componentValue; }

    /** set the component value */
    virtual void setComponentValue(double _componentValue)
    {
        componentValue = _componentValue;
        updateComponentResistance();
    }

    /** change the resistance of component R(L) = 2Lfs */
    virtual void updateComponentResistance(){ componentResistance = 2.0*componentValue*sampleRate;}

    /** reset the component; clear registers */
    virtual void reset(double _sampleRate) { setSampleRate(_sampleRate); zRegister = 0.0; }

    /** set input value into component; NOTE: inductor sets value into storage register */
    virtual void setInput(double in) { zRegister = in; }

    /** get output value; NOTE: a WDF inductor produces reflected output that is inverted */
    virtual double getOutput() { return -zRegister; } // -z^-1

    /** get output1 value; only one resistor output (not used) */
    virtual double getOutput1() { return  getOutput(); }

    /** get output2 value; only one resistor output (not used) */
    virtual double getOutput2() { return  getOutput(); }

    /** get output3 value; only one resistor output (not used) */
    virtual double getOutput3() { return  getOutput(); }

    /** set input1 value; not used for components */
    virtual void setInput1(double _in1) {}

    /** set input2 value; not used for components */
    virtual void setInput2(double _in2) {}

    /** set input3 value; not used for components */
    virtual void setInput3(double _in3) {}

protected:
    double zRegister = 0.0;            ///< storage register (not used with resistor)
    double componentValue = 0.0;    ///< component value in electronic form (ohm, farad, henry)
    double componentResistance = 0.0;///< simulated resistance
    double sampleRate = 0.0;        ///< sample rate
};


/**
\class WdfSeriesLC
\ingroup WDF-Objects
\brief
The WdfSeriesLC object implements the reflection coefficient and signal flow through
a WDF simulated series LC pair.

\author Will Pirkle http://www.willpirkle.com
\remark This object is included in Designing Audio Effects Plugins in C++ 2nd Ed. by Will Pirkle
SEE: p143 "Design of Wave Digital Filters" Psenicka, Ugalde, Romero M.
\version Revision : 1.0
\date Date : 2018 / 09 / 7
*/
class WdfSeriesLC : public IComponentAdaptor
{
public:
    WdfSeriesLC() {}
    WdfSeriesLC(double _componentValue_L, double _componentValue_C)
    {
        componentValue_L = _componentValue_L;
        componentValue_C = _componentValue_C;
    }
    virtual ~WdfSeriesLC() {}

    /** set sample rate and update component */
    void setSampleRate(double _sampleRate)
    {
        sampleRate = _sampleRate;
        updateComponentResistance();
    }

    /** get component's value as a resistance */
    virtual double getComponentResistance() { return componentResistance; }

    /** get component's value as a conducatance (or admittance) */
    virtual double getComponentConductance() { return 1.0 / componentResistance; }

    /** change the resistance of component; see FX book for details */
    virtual void updateComponentResistance()
    {
        RL = 2.0*componentValue_L*sampleRate;
        RC = 1.0 / (2.0*componentValue_C*sampleRate);
        componentResistance = RL + (1.0 / RC);
    }

    /** set both LC components at once */
    virtual void setComponentValue_LC(double _componentValue_L, double _componentValue_C)
    {
        componentValue_L = _componentValue_L;
        componentValue_C = _componentValue_C;
        updateComponentResistance();
    }

    /** set L component */
    virtual void setComponentValue_L(double _componentValue_L)
    {
        componentValue_L = _componentValue_L;
        updateComponentResistance();
    }

    /** set C component */
    virtual void setComponentValue_C(double _componentValue_C)
    {
        componentValue_C = _componentValue_C;
        updateComponentResistance();
    }

    /** get L component value */
    virtual double getComponentValue_L() { return componentValue_L; }

    /** get C component value */
    virtual double getComponentValue_C() { return componentValue_C; }

    /** reset the component; clear registers */
    virtual void reset(double _sampleRate) { setSampleRate(_sampleRate); zRegister_L = 0.0; zRegister_C = 0.0; }

    /** set input value into component; NOTE: K is calculated here */
    virtual void setInput(double in)
    {
        double YC = 1.0 / RC;
        double K = (1.0 - RL*YC) / (1.0 + RL*YC);
        double N1 = K*(in - zRegister_L);
        zRegister_L = N1 + zRegister_C;
        zRegister_C = in;
    }

    /** get output value; NOTE: utput is located in zReg_L */
    virtual double getOutput(){ return zRegister_L; }

    /** get output1 value; only one resistor output (not used) */
    virtual double getOutput1() { return  getOutput(); }

    /** get output2 value; only one resistor output (not used) */
    virtual double getOutput2() { return  getOutput(); }

    /** get output3 value; only one resistor output (not used) */
    virtual double getOutput3() { return  getOutput(); }

    /** set input1 value; not used for components */
    virtual void setInput1(double _in1) {}

    /** set input2 value; not used for components */
    virtual void setInput2(double _in2) {}

    /** set input3 value; not used for components */
    virtual void setInput3(double _in3) {}

protected:
    double zRegister_L = 0.0; ///< storage register for L
    double zRegister_C = 0.0; ///< storage register for C

    double componentValue_L = 0.0; ///< component value L
    double componentValue_C = 0.0; ///< component value C

    double RL = 0.0; ///< RL value
    double RC = 0.0; ///< RC value
    double componentResistance = 0.0; ///< equivalent resistance of pair of components
    double sampleRate = 0.0; ///< sample rate
};

/**
\class WdfParallelLC
\ingroup WDF-Objects
\brief
The WdfParallelLC object implements the reflection coefficient and signal flow through
a WDF simulated parallel LC pair.

\author Will Pirkle http://www.willpirkle.com
\remark This object is included in Designing Audio Effects Plugins in C++ 2nd Ed. by Will Pirkle
SEE: p143 "Design of Wave Digital Filters" Psenicka, Ugalde, Romero M.
\version Revision : 1.0
\date Date : 2018 / 09 / 7
*/
class WdfParallelLC : public IComponentAdaptor
{
public:
    WdfParallelLC() {}
    WdfParallelLC(double _componentValue_L, double _componentValue_C)
    {
        componentValue_L = _componentValue_L;
        componentValue_C = _componentValue_C;
    }
    virtual ~WdfParallelLC() {}

    /** set sample rate and update component */
    void setSampleRate(double _sampleRate)
    {
        sampleRate = _sampleRate;
        updateComponentResistance();
    }

    /** get component's value as a resistance */
    virtual double getComponentResistance() { return componentResistance; }

    /** get component's value as a conducatance (or admittance) */
    virtual double getComponentConductance() { return 1.0 / componentResistance; }

    /** change the resistance of component; see FX book for details */
    virtual void updateComponentResistance()
    {
        RL = 2.0*componentValue_L*sampleRate;
        RC = 1.0 / (2.0*componentValue_C*sampleRate);
        componentResistance = (RC + 1.0 / RL);
    }

    /** set both LC components at once */
    virtual void setComponentValue_LC(double _componentValue_L, double _componentValue_C)
    {
        componentValue_L = _componentValue_L;
        componentValue_C = _componentValue_C;
        updateComponentResistance();
    }

    /** set L component */
    virtual void setComponentValue_L(double _componentValue_L)
    {
        componentValue_L = _componentValue_L;
        updateComponentResistance();
    }

    /** set C component */
    virtual void setComponentValue_C(double _componentValue_C)
    {
        componentValue_C = _componentValue_C;
        updateComponentResistance();
    }

    /** get L component value */
    virtual double getComponentValue_L() { return componentValue_L; }

    /** get C component value */
    virtual double getComponentValue_C() { return componentValue_C; }

    /** reset the component; clear registers */
    virtual void reset(double _sampleRate) { setSampleRate(_sampleRate); zRegister_L = 0.0; zRegister_C = 0.0; }

    /** set input value into component; NOTE: K is calculated here */
    virtual void setInput(double in)
    {
        double YL = 1.0 / RL;
        double K = (YL*RC - 1.0) / (YL*RC + 1.0);
        double N1 = K*(in - zRegister_L);
        zRegister_L = N1 + zRegister_C;
        zRegister_C = in;
    }

    /** get output value; NOTE: output is located in -zReg_L */
    virtual double getOutput(){ return -zRegister_L; }

    /** get output1 value; only one resistor output (not used) */
    virtual double getOutput1() { return  getOutput(); }

    /** get output2 value; only one resistor output (not used) */
    virtual double getOutput2() { return  getOutput(); }

    /** get output3 value; only one resistor output (not used) */
    virtual double getOutput3() { return  getOutput(); }

    /** set input1 value; not used for components */
    virtual void setInput1(double _in1) {}

    /** set input2 value; not used for components */
    virtual void setInput2(double _in2) {}

    /** set input3 value; not used for components */
    virtual void setInput3(double _in3) {}

protected:
    double zRegister_L = 0.0; ///< storage register for L
    double zRegister_C = 0.0; ///< storage register for C

    double componentValue_L = 0.0; ///< component value L
    double componentValue_C = 0.0; ///< component value C

    double RL = 0.0; ///< RL value
    double RC = 0.0; ///< RC value
    double componentResistance = 0.0; ///< equivalent resistance of pair of components
    double sampleRate = 0.0; ///< sample rate
};


/**
\class WdfSeriesRL
\ingroup WDF-Objects
\brief
The WdfSeriesRL object implements the reflection coefficient and signal flow through
a WDF simulated series RL pair.

\author Will Pirkle http://www.willpirkle.com
\remark This object is included in Designing Audio Effects Plugins in C++ 2nd Ed. by Will Pirkle
SEE: p143 "Design of Wave Digital Filters" Psenicka, Ugalde, Romero M.
\version Revision : 1.0
\date Date : 2018 / 09 / 7
*/
class WdfSeriesRL : public IComponentAdaptor
{
public:
    WdfSeriesRL() {}
    WdfSeriesRL(double _componentValue_R, double _componentValue_L)
    {
        componentValue_L = _componentValue_L;
        componentValue_R = _componentValue_R;
    }
    virtual ~WdfSeriesRL() {}

    /** set sample rate and update component */
    void setSampleRate(double _sampleRate)
    {
        sampleRate = _sampleRate;
        updateComponentResistance();
    }

    /** get component's value as a resistance */
    virtual double getComponentResistance() { return componentResistance; }

    /** get component's value as a conducatance (or admittance) */
    virtual double getComponentConductance() { return 1.0 / componentResistance; }

    /** change the resistance of component; see FX book for details */
    virtual void updateComponentResistance()
    {
        RR = componentValue_R;
        RL = 2.0*componentValue_L*sampleRate;
        componentResistance = RR + RL;
        K = RR / componentResistance;
    }

    /** set both RL components at once */
    virtual void setComponentValue_RL(double _componentValue_R, double _componentValue_L)
    {
        componentValue_L = _componentValue_L;
        componentValue_R = _componentValue_R;
        updateComponentResistance();
    }

    /** set L component */
    virtual void setComponentValue_L(double _componentValue_L)
    {
        componentValue_L = _componentValue_L;
        updateComponentResistance();
    }

    /** set R component */
    virtual void setComponentValue_R(double _componentValue_R)
    {
        componentValue_R = _componentValue_R;
        updateComponentResistance();
    }

    /** get L component value */
    virtual double getComponentValue_L() { return componentValue_L; }

    /** get R component value */
    virtual double getComponentValue_R() { return componentValue_R; }

    /** reset the component; clear registers */
    virtual void reset(double _sampleRate) { setSampleRate(_sampleRate); zRegister_L = 0.0; zRegister_C = 0.0; }

    /** set input value into component */
    virtual void setInput(double in){ zRegister_L = in; }

    /** get output value; NOTE: see FX book for details */
    virtual double getOutput()
    {
        double NL = -zRegister_L;
        double out = NL*(1.0 - K) - K*zRegister_C;
        zRegister_C = out;

        return out;
    }

    /** get output1 value; only one resistor output (not used) */
    virtual double getOutput1() { return  getOutput(); }

    /** get output2 value; only one resistor output (not used) */
    virtual double getOutput2() { return  getOutput(); }

    /** get output3 value; only one resistor output (not used) */
    virtual double getOutput3() { return  getOutput(); }

    /** set input1 value; not used for components */
    virtual void setInput1(double _in1) {}

    /** set input2 value; not used for components */
    virtual void setInput2(double _in2) {}

    /** set input3 value; not used for components */
    virtual void setInput3(double _in3) {}

protected:
    double zRegister_L = 0.0; ///< storage register for L
    double zRegister_C = 0.0;///< storage register for C (not used)
    double K = 0.0;

    double componentValue_L = 0.0;///< component value L
    double componentValue_R = 0.0;///< component value R

    double RL = 0.0; ///< RL value
    double RC = 0.0; ///< RC value
    double RR = 0.0; ///< RR value

    double componentResistance = 0.0; ///< equivalent resistance of pair of componen
    double sampleRate = 0.0; ///< sample rate
};

/**
\class WdfParallelRL
\ingroup WDF-Objects
\brief
The WdfParallelRL object implements the reflection coefficient and signal flow through
a WDF simulated parallel RL pair.

\author Will Pirkle http://www.willpirkle.com
\remark This object is included in Designing Audio Effects Plugins in C++ 2nd Ed. by Will Pirkle
\version Revision : 1.0
\date Date : 2018 / 09 / 7
*/
class WdfParallelRL : public IComponentAdaptor
{
public:
    WdfParallelRL() {}
    WdfParallelRL(double _componentValue_R, double _componentValue_L)
    {
        componentValue_L = _componentValue_L;
        componentValue_R = _componentValue_R;
    }
    virtual ~WdfParallelRL() {}

    /** set sample rate and update component */
    void setSampleRate(double _sampleRate)
    {
        sampleRate = _sampleRate;
        updateComponentResistance();
    }

    /** get component's value as a resistance */
    virtual double getComponentResistance() { return componentResistance; }

    /** get component's value as a conducatance (or admittance) */
    virtual double getComponentConductance() { return 1.0 / componentResistance; }

    /** change the resistance of component; see FX book for details */
    virtual void updateComponentResistance()
    {
        RR = componentValue_R;
        RL = 2.0*componentValue_L*sampleRate;
        componentResistance = 1.0 / ((1.0 / RR) + (1.0 / RL));
        K = componentResistance / RR;
    }


    /** set both RL components at once */
    virtual void setComponentValue_RL(double _componentValue_R, double _componentValue_L)
    {
        componentValue_L = _componentValue_L;
        componentValue_R = _componentValue_R;
        updateComponentResistance();
    }

    /** set L component */
    virtual void setComponentValue_L(double _componentValue_L)
    {
        componentValue_L = _componentValue_L;
        updateComponentResistance();
    }

    /** set R component */
    virtual void setComponentValue_R(double _componentValue_R)
    {
        componentValue_R = _componentValue_R;
        updateComponentResistance();
    }

    /** get L component value */
    virtual double getComponentValue_L() { return componentValue_L; }

    /** get R component value */
    virtual double getComponentValue_R() { return componentValue_R; }

    /** reset the component; clear registers */
    virtual void reset(double _sampleRate) { setSampleRate(_sampleRate); zRegister_L = 0.0; zRegister_C = 0.0; }

    /** set input value into component */
    virtual void setInput(double in){ zRegister_L = in; }

    /** get output value; NOTE: see FX book for details */
    virtual double getOutput()
    {
        double NL = -zRegister_L;
        double out = NL*(1.0 - K) + K*zRegister_C;
        zRegister_C = out;
        return out;
    }

    /** get output1 value; only one resistor output (not used) */
    virtual double getOutput1() { return  getOutput(); }

    /** get output2 value; only one resistor output (not used) */
    virtual double getOutput2() { return  getOutput(); }

    /** get output3 value; only one resistor output (not used) */
    virtual double getOutput3() { return  getOutput(); }

    /** set input1 value; not used for components */
    virtual void setInput1(double _in1) {}

    /** set input2 value; not used for components */
    virtual void setInput2(double _in2) {}

    /** set input3 value; not used for components */
    virtual void setInput3(double _in3) {}

protected:
    double zRegister_L = 0.0;    ///< storage register for L
    double zRegister_C = 0.0;    ///< storage register for L
    double K = 0.0;                ///< K value

    double componentValue_L = 0.0;    ///< component value L
    double componentValue_R = 0.0;    ///< component value R

    double RL = 0.0;    ///< RL value
    double RC = 0.0;    ///< RC value
    double RR = 0.0;    ///< RR value

    double componentResistance = 0.0; ///< equivalent resistance of pair of components
    double sampleRate = 0.0; ///< sample rate
};

/**
\class WdfSeriesRC
\ingroup WDF-Objects
\brief
The WdfSeriesRC object implements the reflection coefficient and signal flow through
a WDF simulated series RC pair.

\author Will Pirkle http://www.willpirkle.com
\remark This object is included in Designing Audio Effects Plugins in C++ 2nd Ed. by Will Pirkle
SEE: p143 "Design of Wave Digital Filters" Psenicka, Ugalde, Romero M.
\version Revision : 1.0
\date Date : 2018 / 09 / 7
*/
class WdfSeriesRC : public IComponentAdaptor
{
public:
    WdfSeriesRC() {}
    WdfSeriesRC(double _componentValue_R, double _componentValue_C)
    {
        componentValue_C = _componentValue_C;
        componentValue_R = _componentValue_R;
    }
    virtual ~WdfSeriesRC() {}

    /** set sample rate and update component */
    void setSampleRate(double _sampleRate)
    {
        sampleRate = _sampleRate;
        updateComponentResistance();
    }

    /** get component's value as a resistance */
    virtual double getComponentResistance() { return componentResistance; }

    /** get component's value as a conducatance (or admittance) */
    virtual double getComponentConductance() { return 1.0 / componentResistance; }

    /** change the resistance of component; see FX book for details */
    virtual void updateComponentResistance()
    {
        RR = componentValue_R;
        RC = 1.0 / (2.0*componentValue_C*sampleRate);
        componentResistance = RR + RC;
        K = RR / componentResistance;
    }

    /** set both RC components at once */
    virtual void setComponentValue_RC(double _componentValue_R, double _componentValue_C)
    {
        componentValue_R = _componentValue_R;
        componentValue_C = _componentValue_C;
        updateComponentResistance();
    }

    /** set R component */
    virtual void setComponentValue_R(double _componentValue_R)
    {
        componentValue_R = _componentValue_R;
        updateComponentResistance();
    }

    /** set C component */
    virtual void setComponentValue_C(double _componentValue_C)
    {
        componentValue_C = _componentValue_C;
        updateComponentResistance();
    }

    /** get R component value */
    virtual double getComponentValue_R() { return componentValue_R; }

    /** get C component value */
    virtual double getComponentValue_C() { return componentValue_C; }

    /** reset the component; clear registers */
    virtual void reset(double _sampleRate) { setSampleRate(_sampleRate); zRegister_L = 0.0; zRegister_C = 0.0; }

    /** set input value into component */
    virtual void setInput(double in){ zRegister_L = in; }

    /** get output value; NOTE: see FX book for details */
    virtual double getOutput()
    {
        double NL = zRegister_L;
        double out = NL*(1.0 - K) + K*zRegister_C;
        zRegister_C = out;
        return out;
    }

    /** get output1 value; only one resistor output (not used) */
    virtual double getOutput1() { return  getOutput(); }

    /** get output2 value; only one resistor output (not used) */
    virtual double getOutput2() { return  getOutput(); }

    /** get output3 value; only one resistor output (not used) */
    virtual double getOutput3() { return  getOutput(); }

    /** set input1 value; not used for components */
    virtual void setInput1(double _in1) {}

    /** set input2 value; not used for components */
    virtual void setInput2(double _in2) {}

    /** set input3 value; not used for components */
    virtual void setInput3(double _in3) {}

protected:
    double zRegister_L = 0.0; ///< storage register for L
    double zRegister_C = 0.0; ///< storage register for C
    double K = 0.0;

    double componentValue_R = 0.0;///< component value R
    double componentValue_C = 0.0;///< component value C

    double RL = 0.0;    ///< RL value
    double RC = 0.0;    ///< RC value
    double RR = 0.0;    ///< RR value

    double componentResistance = 0.0; ///< equivalent resistance of pair of components
    double sampleRate = 0.0; ///< sample rate
};

/**
\class WdfParallelRC
\ingroup WDF-Objects
\brief
The WdfParallelRC object implements the reflection coefficient and signal flow through
a WDF simulated parallal RC pair.

\author Will Pirkle http://www.willpirkle.com
\remark This object is included in Designing Audio Effects Plugins in C++ 2nd Ed. by Will Pirkle
SEE: p143 "Design of Wave Digital Filters" Psenicka, Ugalde, Romero M.
\version Revision : 1.0
\date Date : 2018 / 09 / 7
*/
class WdfParallelRC : public IComponentAdaptor
{
public:
    WdfParallelRC() {}
    WdfParallelRC(double _componentValue_R, double _componentValue_C)
    {
        componentValue_C = _componentValue_C;
        componentValue_R = _componentValue_R;
    }
    virtual ~WdfParallelRC() {}

    /** set sample rate and update component */
    void setSampleRate(double _sampleRate)
    {
        sampleRate = _sampleRate;
        updateComponentResistance();
    }

    /** get component's value as a resistance */
    virtual double getComponentResistance() { return componentResistance; }

    /** get component's value as a conducatance (or admittance) */
    virtual double getComponentConductance() { return 1.0 / componentResistance; }

    /** change the resistance of component; see FX book for details */
    virtual void updateComponentResistance()
    {
        RR = componentValue_R;
        RC = 1.0 / (2.0*componentValue_C*sampleRate);
        componentResistance = 1.0 / ((1.0 / RR) + (1.0 / RC));
        K = componentResistance / RR;
    }

    /** set both RC components at once */
    virtual void setComponentValue_RC(double _componentValue_R, double _componentValue_C)
    {
        componentValue_R = _componentValue_R;
        componentValue_C = _componentValue_C;
        updateComponentResistance();
    }

    /** set R component */
    virtual void setComponentValue_R(double _componentValue_R)
    {
        componentValue_R = _componentValue_R;
        updateComponentResistance();
    }

    /** set C component */
    virtual void setComponentValue_C(double _componentValue_C)
    {
        componentValue_C = _componentValue_C;
        updateComponentResistance();
    }

    /** get R component value */
    virtual double getComponentValue_R() { return componentValue_R; }

    /** get C component value */
    virtual double getComponentValue_C() { return componentValue_C; }

    /** reset the component; clear registers */
    virtual void reset(double _sampleRate) { setSampleRate(_sampleRate); zRegister_L = 0.0; zRegister_C = 0.0; }

    /** set input value into component; */
    virtual void setInput(double in){ zRegister_L = in; }

    /** get output value; NOTE: output is located in zRegister_C */
    virtual double getOutput()
    {
        double NL = zRegister_L;
        double out = NL*(1.0 - K) - K*zRegister_C;
        zRegister_C = out;
        return out;
    }

    /** get output1 value; only one resistor output (not used) */
    virtual double getOutput1() { return  getOutput(); }

    /** get output2 value; only one resistor output (not used) */
    virtual double getOutput2() { return  getOutput(); }

    /** get output3 value; only one resistor output (not used) */
    virtual double getOutput3() { return  getOutput(); }

    /** set input1 value; not used for components */
    virtual void setInput1(double _in1) {}

    /** set input2 value; not used for components */
    virtual void setInput2(double _in2) {}

    /** set input3 value; not used for components */
    virtual void setInput3(double _in3) {}

protected:
    double zRegister_L = 0.0; ///< storage register for L
    double zRegister_C = 0.0; ///< storage register for C
    double K = 0.0;

    double componentValue_C = 0.0;    ///< component value C
    double componentValue_R = 0.0;    ///< component value R

    double RL = 0.0; ///< RL value
    double RC = 0.0; ///< RC value
    double RR = 0.0; ///< RR value

    double componentResistance = 0.0; ///< equivalent resistance of pair of components
    double sampleRate = 0.0; ///< sample rate
};


// ------------------------------------------------------------------ //
// --- WDF ADAPTORS ------------------------------------------------- //
// ------------------------------------------------------------------ //

/**
\enum wdfComponent
\ingroup Constants-Enums
\brief
Use this strongly typed enum to easily set the wdfComponent type

- enum class wdfComponent { R, L, C, seriesLC, parallelLC, seriesRL, parallelRL, seriesRC, parallelRC };

\author Will Pirkle http://www.willpirkle.com
\remark This object is included in Designing Audio Effects Plugins in C++ 2nd Ed. by Will Pirkle
\version Revision : 1.0
\date Date : 2018 / 09 / 7
*/
enum class wdfComponent { R, L, C, D, seriesLC, parallelLC, seriesRL, parallelRL, seriesRC, parallelRC };

/**
\struct WdfComponentInfo
\ingroup WDF-Objects
\brief
Custom structure to hold component information.

\author Will Pirkle http://www.willpirkle.com
\remark This object is included in Designing Audio Effects Plugins in C++ 2nd Ed. by Will Pirkle
\version Revision : 1.0
\date Date : 2018 / 09 / 7
*/
struct WdfComponentInfo
{
    WdfComponentInfo() { }

    WdfComponentInfo(wdfComponent _componentType, double value1 = 0.0, double value2 = 0.0)
    {
        componentType = _componentType;
        if (componentType == wdfComponent::R)
            R = value1;
        else if (componentType == wdfComponent::L)
            L = value1;
        else if (componentType == wdfComponent::C)
            C = value1;
        else if (componentType == wdfComponent::D)
            D = value1;
        else if (componentType == wdfComponent::seriesLC || componentType == wdfComponent::parallelLC)
        {
            L = value1;
            C = value2;
        }
        else if (componentType == wdfComponent::seriesRL || componentType == wdfComponent::parallelRL)
        {
            R = value1;
            L = value2;
        }
        else if (componentType == wdfComponent::seriesRC || componentType == wdfComponent::parallelRC)
        {
            R = value1;
            C = value2;
        }
    }

    double R = 0.0; ///< value of R component
    double L = 0.0;    ///< value of L component
    double C = 0.0;    ///< value of C component
    double D = 0.0;
    wdfComponent componentType = wdfComponent::R; ///< selected component type
};


/**
\class WdfAdaptorBase
\ingroup WDF-Objects
\brief
The WdfAdaptorBase object acts as the base class for all WDF Adaptors; the static members allow
for simplified connection of components. See the FX book for more details.

\author Will Pirkle http://www.willpirkle.com
\remark This object is included in Designing Audio Effects Plugins in C++ 2nd Ed. by Will Pirkle
\version Revision : 1.0
\date Date : 2018 / 09 / 7
*/
class WdfAdaptorBase : public IComponentAdaptor
{
public:
    WdfAdaptorBase() {}
    virtual ~WdfAdaptorBase() {}

    /** set the termainal (load) resistance for terminating adaptors */
    void setTerminalResistance(double _terminalResistance) { terminalResistance = _terminalResistance; }

    /** set the termainal (load) resistance as open circuit for terminating adaptors */
    void setOpenTerminalResistance(bool _openTerminalResistance = true)
    {
        // --- flag overrides value
        openTerminalResistance = _openTerminalResistance;
        terminalResistance = 1.0e+34; // avoid /0.0
    }

    /** set the input (source) resistance for an input adaptor */
    void setSourceResistance(double _sourceResistance) { sourceResistance = _sourceResistance; }

    /** set the component or connected adaptor at port 1; functions is generic and allows extending the functionality of the WDF Library */
    void setPort1_CompAdaptor(IComponentAdaptor* _port1CompAdaptor) { port1CompAdaptor = _port1CompAdaptor; }

    /** set the component or connected adaptor at port 2; functions is generic and allows extending the functionality of the WDF Library */
    void setPort2_CompAdaptor(IComponentAdaptor* _port2CompAdaptor) { port2CompAdaptor = _port2CompAdaptor; }

    /** set the component or connected adaptor at port 3; functions is generic and allows extending the functionality of the WDF Library */
    void setPort3_CompAdaptor(IComponentAdaptor* _port3CompAdaptor) { port3CompAdaptor = _port3CompAdaptor; }

    /** reset the connected component */
    virtual void reset(double _sampleRate)
    {
        if (wdfComponent)
            wdfComponent->reset(_sampleRate);
    }

    /** creates a new WDF component and connects it to Port 3 */
    void setComponent(wdfComponent componentType, double value1 = 0.0, double value2 = 0.0)
    {
        // --- decode and set
        if (componentType == wdfComponent::R)
        {
            wdfComponent = new WdfResistor;
            wdfComponent->setComponentValue(value1);
            port3CompAdaptor = wdfComponent;
        }
        else if (componentType == wdfComponent::L)
        {
            wdfComponent = new WdfInductor;
            wdfComponent->setComponentValue(value1);
            port3CompAdaptor = wdfComponent;
        }
        else if (componentType == wdfComponent::C)
        {
            wdfComponent = new WdfCapacitor;
            wdfComponent->setComponentValue(value1);
            port3CompAdaptor = wdfComponent;
        }
        
        else if (componentType == wdfComponent::seriesLC)
        {
            wdfComponent = new WdfSeriesLC;
            wdfComponent->setComponentValue_LC(value1, value2);
            port3CompAdaptor = wdfComponent;
        }
        else if (componentType == wdfComponent::parallelLC)
        {
            wdfComponent = new WdfParallelLC;
            wdfComponent->setComponentValue_LC(value1, value2);
            port3CompAdaptor = wdfComponent;
        }
        else if (componentType == wdfComponent::seriesRL)
        {
            wdfComponent = new WdfSeriesRL;
            wdfComponent->setComponentValue_RL(value1, value2);
            port3CompAdaptor = wdfComponent;
        }
        else if (componentType == wdfComponent::parallelRL)
        {
            wdfComponent = new WdfParallelRL;
            wdfComponent->setComponentValue_RL(value1, value2);
            port3CompAdaptor = wdfComponent;
        }
        else if (componentType == wdfComponent::seriesRC)
        {
            wdfComponent = new WdfSeriesRC;
            wdfComponent->setComponentValue_RC(value1, value2);
            port3CompAdaptor = wdfComponent;
        }
        else if (componentType == wdfComponent::parallelRC)
        {
            wdfComponent = new WdfParallelRC;
            wdfComponent->setComponentValue_RC(value1, value2);
            port3CompAdaptor = wdfComponent;
        }
    }

    /** connect two adapters together upstreamAdaptor --> downstreamAdaptor */
    static void connectAdaptors(WdfAdaptorBase* upstreamAdaptor, WdfAdaptorBase* downstreamAdaptor)
    {
        upstreamAdaptor->setPort2_CompAdaptor(downstreamAdaptor);
        downstreamAdaptor->setPort1_CompAdaptor(upstreamAdaptor);
    }

    /** initialize the chain of adaptors from upstreamAdaptor --> downstreamAdaptor */
    virtual void initializeAdaptorChain()
    {
        initialize(sourceResistance);
    }

    /** set value of single-component adaptor */
    virtual void setComponentValue(double _componentValue)
    {
        if (wdfComponent)
            wdfComponent->setComponentValue(_componentValue);
    }

    /** set LC value of mjulti-component adaptor */
    virtual void setComponentValue_LC(double componentValue_L, double componentValue_C)
    {
        if (wdfComponent)
            wdfComponent->setComponentValue_LC(componentValue_L, componentValue_C);
    }

    /** set RL value of mjulti-component adaptor */
    virtual void setComponentValue_RL(double componentValue_R, double componentValue_L)
    {
        if (wdfComponent)
            wdfComponent->setComponentValue_RL(componentValue_R, componentValue_L);
    }

    /** set RC value of mjulti-component adaptor */
    virtual void setComponentValue_RC(double componentValue_R, double componentValue_C)
    {
        if (wdfComponent)
            wdfComponent->setComponentValue_RC(componentValue_R, componentValue_C);
    }

    /** get adaptor connected at port 1: for extended functionality; not used in WDF ladder filter library */
    IComponentAdaptor* getPort1_CompAdaptor() { return port1CompAdaptor; }

    /** get adaptor connected at port 2: for extended functionality; not used in WDF ladder filter library */
    IComponentAdaptor* getPort2_CompAdaptor() { return port2CompAdaptor; }

    /** get adaptor connected at port 3: for extended functionality; not used in WDF ladder filter library */
    IComponentAdaptor* getPort3_CompAdaptor() { return port3CompAdaptor; }

protected:
    // --- can in theory connect any port to a component OR adaptor;
    //     though this library is setup with a convention R3 = component
    IComponentAdaptor* port1CompAdaptor = nullptr;    ///< componant or adaptor connected to port 1
    IComponentAdaptor* port2CompAdaptor = nullptr;    ///< componant or adaptor connected to port 2
    IComponentAdaptor* port3CompAdaptor = nullptr;    ///< componant or adaptor connected to port 3
    IComponentAdaptor* wdfComponent = nullptr;        ///< WDF componant connected to port 3 (default operation)

    // --- These hold the input (R1), component (R3) and output (R2) resistances
    double R1 = 0.0; ///< input port resistance
    double R2 = 0.0; ///< output port resistance
    double R3 = 0.0; ///< component resistance

    // --- these are input variables that are stored;
    //     not used in this implementation but may be required for extended versions
    double in1 = 0.0;    ///< stored port 1 input;  not used in this implementation but may be required for extended versions
    double in2 = 0.0;    ///< stored port 2 input;  not used in this implementation but may be required for extended versions
    double in3 = 0.0;    ///< stored port 3 input;  not used in this implementation but may be required for extended versions

    // --- these are output variables that are stored;
    //     currently out2 is the only one used as it is y(n) for this library
    //     out1 and out2 are stored; not used in this implementation but may be required for extended versions
    double out1 = 0.0;    ///< stored port 1 output; not used in this implementation but may be required for extended versions
    double out2 = 0.0;    ///< stored port 2 output; it is y(n) for this library
    double out3 = 0.0;    ///< stored port 3 output; not used in this implementation but may be required for extended versions

    // --- terminal impedance
    double terminalResistance = 600.0; ///< value of terminal (load) resistance
    bool openTerminalResistance = false; ///< flag for open circuit load

    // --- source impedance, OK for this to be set to 0.0 for Rs = 0
    double sourceResistance = 600.0; ///< source impedance; OK for this to be set to 0.0 for Rs = 0
};

/**
\class WdfSeriesAdaptor
\ingroup WDF-Objects
\brief
The WdfSeriesAdaptor object implements the series reflection-free (non-terminated) adaptor

\author Will Pirkle http://www.willpirkle.com
\remark This object is included in Designing Audio Effects Plugins in C++ 2nd Ed. by Will Pirkle
\version Revision : 1.0
\date Date : 2018 / 09 / 7
*/
class WdfSeriesAdaptor : public WdfAdaptorBase
{
public:
    WdfSeriesAdaptor() {}
    virtual ~WdfSeriesAdaptor() {}

    /** get the resistance at port 2; R2 = R1 + component (series)*/
    virtual double getR2()
    {
        double componentResistance = 0.0;
        if (getPort3_CompAdaptor())
            componentResistance = getPort3_CompAdaptor()->getComponentResistance();

        R2 = R1 + componentResistance;
        return R2;
    }

    /** initialize adaptor with input resistance */
    virtual void initialize(double _R1)
    {
        // --- R1 is source resistance for this adaptor
        R1 = _R1;

        double componentResistance = 0.0;
        if (getPort3_CompAdaptor())
            componentResistance = getPort3_CompAdaptor()->getComponentResistance();

        // --- calculate B coeff
        B = R1 / (R1 + componentResistance);

        // --- init downstream adaptor
        if (getPort2_CompAdaptor())
            getPort2_CompAdaptor()->initialize(getR2());

        // --- not used in this implementation but saving for extended use
        R3 = componentResistance;
    }

    /** push audio input sample into incident wave input*/
    virtual void setInput1(double _in1)
    {
        // --- save
        in1 = _in1;

        // --- read component value
        N2 = 0.0;
        if (getPort3_CompAdaptor())
            N2 = getPort3_CompAdaptor()->getOutput();

        // --- form output
        out2 = -(in1 + N2);

        // --- deliver downstream
        if (getPort2_CompAdaptor())
            getPort2_CompAdaptor()->setInput1(out2);
    }

    /** push audio input sample into reflected wave input */
    virtual void setInput2(double _in2)
    {
        // --- save
        in2 = _in2;

        // --- calc N1
        N1 = -(in1 - B*(in1 + N2 + in2) + in2);

        // --- calc out1
        out1 = in1 - B*(N2 + in2);

        // --- deliver upstream
        if (getPort1_CompAdaptor())
            getPort1_CompAdaptor()->setInput2(out1);

        // --- set component state
        if (getPort3_CompAdaptor())
            getPort3_CompAdaptor()->setInput(N1);
    }

    /** set input 3 always connects to component */
    virtual void setInput3(double _in3){ }

    /** get OUT1 = reflected output pin on Port 1 */
    virtual double getOutput1() { return out1; }

    /** get OUT2 = incident (normal) output pin on Port 2 */
    virtual double getOutput2() { return out2; }

    /** get OUT3 always connects to component */
    virtual double getOutput3() { return out3; }

private:
    double N1 = 0.0;    ///< node 1 value, internal use only
    double N2 = 0.0;    ///< node 2 value, internal use only
    double B = 0.0;        ///< B coefficient value
};

/**
\class WdfSeriesTerminatedAdaptor
\ingroup WDF-Objects
\brief
The WdfSeriesTerminatedAdaptor object implements the series terminated (non-reflection-free) adaptor

\author Will Pirkle http://www.willpirkle.com
\remark This object is included in Designing Audio Effects Plugins in C++ 2nd Ed. by Will Pirkle
\version Revision : 1.0
\date Date : 2018 / 09 / 7
*/
// --- Series terminated adaptor
class WdfSeriesTerminatedAdaptor : public WdfAdaptorBase
{
public:
    WdfSeriesTerminatedAdaptor() {}
    virtual ~WdfSeriesTerminatedAdaptor() {}

    /** get the resistance at port 2; R2 = R1 + component (series)*/
    virtual double getR2()
    {
        double componentResistance = 0.0;
        if (getPort3_CompAdaptor())
            componentResistance = getPort3_CompAdaptor()->getComponentResistance();

        R2 = R1 + componentResistance;
        return R2;
    }

    /** initialize adaptor with input resistance */
    virtual void initialize(double _R1)
    {
        // --- source impedance
        R1 = _R1;

        double componentResistance = 0.0;
        if (getPort3_CompAdaptor())
            componentResistance = getPort3_CompAdaptor()->getComponentResistance();

        B1 = (2.0*R1) / (R1 + componentResistance + terminalResistance);
        B3 = (2.0*terminalResistance) / (R1 + componentResistance + terminalResistance);

        // --- init downstream
        if (getPort2_CompAdaptor())
            getPort2_CompAdaptor()->initialize(getR2());

        // --- not used in this implementation but saving for extended use
        R3 = componentResistance;
    }

    /** push audio input sample into incident wave input*/
    virtual void setInput1(double _in1)
    {
        // --- save
        in1 = _in1;

        N2 = 0.0;
        if (getPort3_CompAdaptor())
            N2 = getPort3_CompAdaptor()->getOutput();

        double N3 = in1 + N2;

        // --- calc out2 y(n)
        out2 = -B3*N3;

        // --- form output1
        out1 = in1 - B1*N3;

        // --- form N1
        N1 = -(out1 + out2 + N3);

        // --- deliver upstream to input2
        if (getPort1_CompAdaptor())
            getPort1_CompAdaptor()->setInput2(out1);

        // --- set component state
        if (getPort3_CompAdaptor())
            getPort3_CompAdaptor()->setInput(N1);
    }

    /** push audio input sample into reflected wave input
        for terminated adaptor, this is dead end, just store it */
    virtual void setInput2(double _in2) { in2 = _in2;}

    /** set input 3 always connects to component */
    virtual void setInput3(double _in3) { in3 = _in3;}

    /** get OUT1 = reflected output pin on Port 1 */
    virtual double getOutput1() { return out1; }

    /** get OUT2 = incident (normal) output pin on Port 2 */
    virtual double getOutput2() { return out2; }

    /** get OUT3 always connects to component */
    virtual double getOutput3() { return out3; }

private:
    double N1 = 0.0;    ///< node 1 value, internal use only
    double N2 = 0.0;    ///< node 2 value, internal use only
    double B1 = 0.0;    ///< B1 coefficient value
    double B3 = 0.0;    ///< B3 coefficient value
};

/**
\class WdfParallelAdaptor
\ingroup WDF-Objects
\brief
The WdfParallelAdaptor object implements the parallel reflection-free (non-terminated) adaptor

\author Will Pirkle http://www.willpirkle.com
\remark This object is included in Designing Audio Effects Plugins in C++ 2nd Ed. by Will Pirkle
\version Revision : 1.0
\date Date : 2018 / 09 / 7
*/
class WdfParallelAdaptor : public WdfAdaptorBase
{
public:
    WdfParallelAdaptor() {}
    virtual ~WdfParallelAdaptor() {}

    /** get the resistance at port 2;  R2 = 1.0/(sum of admittances) */
    virtual double getR2()
    {
        double componentConductance = 0.0;
        if (getPort3_CompAdaptor())
            componentConductance = getPort3_CompAdaptor()->getComponentConductance();

        // --- 1 / (sum of admittances)
        R2 = 1.0 / ((1.0 / R1) + componentConductance);
        return R2;
    }

    /** initialize adaptor with input resistance */
    virtual void initialize(double _R1)
    {
        // --- save R1
        R1 = _R1;

        double G1 = 1.0 / R1;
        double componentConductance = 0.0;
        if (getPort3_CompAdaptor())
            componentConductance = getPort3_CompAdaptor()->getComponentConductance();

        // --- calculate B coeff
        A = G1 / (G1 + componentConductance);

        // --- now, do we init our downstream??
        if (getPort2_CompAdaptor())
            getPort2_CompAdaptor()->initialize(getR2());

        // --- not used in this implementation but saving for extended use
        R3 = 1.0/ componentConductance;
    }

    /** push audio input sample into incident wave input*/
    virtual void setInput1(double _in1)
    {
        // --- save
        in1 = _in1;

        // --- read component
        N2 = 0.0;
        if (getPort3_CompAdaptor())
            N2 = getPort3_CompAdaptor()->getOutput();

        // --- form output
        out2 = N2 - A*(-in1 + N2);

        // --- deliver downstream
        if (getPort2_CompAdaptor())
            getPort2_CompAdaptor()->setInput1(out2);
    }

    /** push audio input sample into reflected wave input*/
    virtual void setInput2(double _in2)
    {
        // --- save
        in2 = _in2;

        // --- calc N1
        N1 = in2 - A*(-in1 + N2);

        // --- calc out1
        out1 = -in1 + N2 + N1;

        // --- deliver upstream
        if (getPort1_CompAdaptor())
            getPort1_CompAdaptor()->setInput2(out1);

        // --- set component state
        if (getPort3_CompAdaptor())
            getPort3_CompAdaptor()->setInput(N1);
    }
    
    // For a diode ::
    // V = (a + b)/2, I = (a - b)/(2*Rp)
    // where a = input1 (incident wave), b = input 2 (reflected wave)
    

    /** set input 3 always connects to component */
    virtual void setInput3(double _in3) { }

    /** get OUT1 = reflected output pin on Port 1 */
    virtual double getOutput1() { return out1; }

    /** get OUT2 = incident (normal) output pin on Port 2 */
    virtual double getOutput2() { return out2; }

    /** get OUT3 always connects to component */
    virtual double getOutput3() { return out3; }

private:
    double N1 = 0.0;    ///< node 1 value, internal use only
    double N2 = 0.0;    ///< node 2 value, internal use only
    double A = 0.0;        ///< A coefficient value
};


/**
\class WdfParallelTerminatedAdaptor
\ingroup WDF-Objects
\brief
The WdfParallelTerminatedAdaptor object implements the parallel  terminated (non-reflection-free) adaptor

\author Will Pirkle http://www.willpirkle.com
\remark This object is included in Designing Audio Effects Plugins in C++ 2nd Ed. by Will Pirkle
\version Revision : 1.0
\date Date : 2018 / 09 / 7
*/
class WdfParallelTerminatedAdaptor : public WdfAdaptorBase
{
public:
    WdfParallelTerminatedAdaptor() {}
    virtual ~WdfParallelTerminatedAdaptor() {}

    /** get the resistance at port 2;  R2 = 1.0/(sum of admittances) */
    virtual double getR2()
    {
        double componentConductance = 0.0;
        if (getPort3_CompAdaptor())
            componentConductance = getPort3_CompAdaptor()->getComponentConductance();

        // --- 1 / (sum of admittances)
        R2 = 1.0 / ((1.0 / R1) + componentConductance);
        return R2;
    }

    /** initialize adaptor with input resistance */
    virtual void initialize(double _R1)
    {
        // --- save R1
        R1 = _R1;

        double G1 = 1.0 / R1;
        if (terminalResistance <= 0.0)
            terminalResistance = 1e-15;

        double G2 = 1.0 / terminalResistance;
        double componentConductance = 0.0;
        if (getPort3_CompAdaptor())
            componentConductance = getPort3_CompAdaptor()->getComponentConductance();

        A1 = 2.0*G1 / (G1 + componentConductance + G2);
        A3 = openTerminalResistance ? 0.0 : 2.0*G2 / (G1 + componentConductance + G2);

        // --- init downstream
        if (getPort2_CompAdaptor())
            getPort2_CompAdaptor()->initialize(getR2());

        // --- not used in this implementation but saving for extended use
        R3 = 1.0 / componentConductance;
    }

    /** push audio input sample into incident wave input*/
    virtual void setInput1(double _in1)
    {
        // --- save
        in1 = _in1;

        N2 = 0.0;
        if (getPort3_CompAdaptor())
            N2 = getPort3_CompAdaptor()->getOutput();

        // --- form N1
        N1 = -A1*(-in1 + N2) + N2 - A3*N2;

        // --- form output1
        out1 = -in1 + N2 + N1;

        // --- deliver upstream to input2
        if (getPort1_CompAdaptor())
            getPort1_CompAdaptor()->setInput2(out1);

        // --- calc out2 y(n)
        out2 = N2 + N1;

        // --- set component state
        if (getPort3_CompAdaptor())
            getPort3_CompAdaptor()->setInput(N1);
    }

    /** push audio input sample into reflected wave input; this is a dead end for terminated adaptorsthis is a dead end for terminated adaptors  */
    virtual void setInput2(double _in2){ in2 = _in2;}

    /** set input 3 always connects to component */
    virtual void setInput3(double _in3) { }

    /** get OUT1 = reflected output pin on Port 1 */
    virtual double getOutput1() { return out1; }

    /** get OUT2 = incident (normal) output pin on Port 2 */
    virtual double getOutput2() { return out2; }

    /** get OUT3 always connects to component */
    virtual double getOutput3() { return out3; }

private:
    double N1 = 0.0;    ///< node 1 value, internal use only
    double N2 = 0.0;    ///< node 2 value, internal use only
    double A1 = 0.0;    ///< A1 coefficient value
    double A3 = 0.0;    ///< A3 coefficient value
};

// ------------------------------------------------------------------------------ //
// --- WDF Ladder Filter Design  Examples --------------------------------------- //
// ------------------------------------------------------------------------------ //
//
// --- 3rd order Butterworth LPF designed with Elsie www.TonneSoftware.comm
//
/*
    3rd Order Inductor-Leading LPF

    Rs = Rload = 600 ohms

    Series(L1) -> Parallel(C1) -> Series(L2)

    --L1-- | --L2--
           C1
           |

    fc = 1kHz
        L1 = 95.49e-3;
        C1 = 0.5305e-6;
        L2 = 95.49e-3;

    fc = 10kHz
        L1 = 9.549e-3;
        C1 = 0.05305e-6;
        L2 = 9.549e-3;
*/

class WDFPreGainDistortionCircuit : public IAudioSignalProcessor
{
public:
    WDFPreGainDistortionCircuit(void) { createWDF(); }    /* C-TOR */
    ~WDFPreGainDistortionCircuit(void) {}    /* D-TOR */
    
    /** reset members to initialized state */
    virtual bool reset(double _sampleRate)
    {
        // --- rest WDF components (flush state registers)
        seriesAdaptor_R3.reset(_sampleRate);
        seriesAdaptor_C23.reset(_sampleRate);

        // --- intialize the chain of adapters
        seriesAdaptor_R3.initializeAdaptorChain();
        return true;
    }
    
    virtual bool canProcessAudioFrame() { return false; }
    
    virtual double processAudioSample(double xn)
    {
        //std::cout << "incoming pre = " << xn << std::endl;
        // --- push audio sample into series L1
        seriesAdaptor_R3.setInput1(xn);
        //std::cout << "outgoing pre = " << seriesAdaptor_C23.getOutput1() << std::endl;
        // --- output is at terminated L2's output2
        return seriesAdaptor_C23.getOutput2();
    }
    
    void createWDF()
    {
        // --- actual component values fc = 34Hz
        double R3_value = 10000;
        double C23_value = 470e-9;
        
        

        // --- set adapter components
        seriesAdaptor_R3.setComponent(wdfComponent::R, R3_value);
        seriesAdaptor_C23.setComponent(wdfComponent::C, C23_value);
        

        // --- connect adapters
        WdfAdaptorBase::connectAdaptors(&seriesAdaptor_R3, &seriesAdaptor_C23);
        
        seriesAdaptor_R3.setSourceResistance(100);
        seriesAdaptor_C23.setOpenTerminalResistance();

    }
    
protected:
    WdfSeriesAdaptor seriesAdaptor_R3;
    WdfSeriesTerminatedAdaptor seriesAdaptor_C23;
};

class WDFPostGainDistortionCircuit : public IAudioSignalProcessor
{
public:
    
    // Make these standard null ptrs
    double tone = 5000.0;
    double volume = 10000.0;
    
    WDFPostGainDistortionCircuit(void) { createWDF(); }    /* C-TOR */
    ~WDFPostGainDistortionCircuit(void) {}    /* D-TOR */
    
    /** reset members to initialized state */
    virtual bool reset(double _sampleRate)
    {
        // --- rest WDF components (flush state registers)
        seriesAdaptor_C3.reset(_sampleRate);
        seriesAdaptor_Tone.reset(_sampleRate);
        parallelAdaptor_C29.reset(_sampleRate);
        parallelAdaptor_Volume.reset(_sampleRate);

        // --- intialize the chain of adapters
        seriesAdaptor_C3.initializeAdaptorChain();
        return true;
    }
    
    virtual bool canProcessAudioFrame() { return false; }
    
    virtual double processAudioSample(double xn)
    {
        
        
        // --- push audio sample into series L1
        seriesAdaptor_C3.setInput1(xn);

        
        //std::cout << "post = " << parallelAdaptor_C29.getOutput() << std::endl;
        // --- output is at terminated L2's output2
        return parallelAdaptor_Volume.getOutput2();
        
        
    }
    
    void createWDF()
    {

        // --- actual component values fc = 400Hz
        double C3_value = 1e-6;
        double Tone_value = getTone();
        double C29_value = 22e-9;
        double Volume_value = getVolume();

        // --- set adapter components
        // High pass filter from 100-8000Hz
        seriesAdaptor_C3.setComponent(wdfComponent::C, C3_value);
        seriesAdaptor_Tone.setComponent(wdfComponent::R, Tone_value);
        // Potential divider
        parallelAdaptor_C29.setComponent(wdfComponent::C, C29_value);
        parallelAdaptor_Volume.setComponent(wdfComponent::R, Volume_value);
        //seriesTerminatedAdaptor_outR.setComponent(wdfComponent::R, 0.0);
        
        seriesAdaptor_C3.setSourceResistance(100);
        //parallelAdaptor_Volume.setSourceResistance(1000);
        parallelAdaptor_Volume.setTerminalResistance(100);
        
        // --- connect adapters
        WdfAdaptorBase::connectAdaptors(&seriesAdaptor_C3, &seriesAdaptor_Tone);
        WdfAdaptorBase::connectAdaptors(&seriesAdaptor_Tone, &parallelAdaptor_C29);
        WdfAdaptorBase::connectAdaptors(&parallelAdaptor_C29, &parallelAdaptor_Volume);
       // WdfAdaptorBase::connectAdaptors(&parallelAdaptor_Volume, &seriesTerminatedAdaptor_outR);

    }
    
    void setTone (double toneValue) {
        tone = toneValue;
    }
    
    double getTone() {
        return tone;
    }
    
    void setVolume (double volumeValue) {
        volume = volumeValue;
    }
    
    double getVolume() {
        return volume;
    }
    
protected:
    WdfSeriesAdaptor seriesAdaptor_C3;
    WdfSeriesAdaptor seriesAdaptor_Tone;
    WdfParallelAdaptor parallelAdaptor_C29;
    WdfParallelTerminatedAdaptor parallelAdaptor_Volume;
};


