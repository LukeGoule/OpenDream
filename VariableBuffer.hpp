#ifndef VARIABLEBUFFER_HPP
#define VARIABLEBUFFER_HPP

#ifdef CPP_INCLUDE

#define _struct(name) struct name
#define _float(name, val) float name = val;

#endif

#ifdef HLSL_INCLUDE

#define _struct(name) cbuffer name : register(b0)
#define _float(name, val) float name;

#endif

// MUST BE ALIGNED TO 16 BYTES. PADDING REQUIRED! //
_struct(VariableBuffer)
{
	_float(time, 0.f);
	_float(forced_intensity, 1.f);
	_float(red_mod, 1.f);
	_float(green_mod, 1.f);

	_float(blue_mod, 1.f);
	_float(mousex, 0.f);
	_float(mousey, 0.f);
	_float(flick_rate, 1.5f);
};

#endif