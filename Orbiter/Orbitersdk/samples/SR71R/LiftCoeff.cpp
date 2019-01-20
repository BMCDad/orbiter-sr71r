//	LiftCoeff - SR-71r Orbiter Addon
//	Copyright(C) 2015  Blake Christensen
//
//	This program is free software : you can redistribute it and / or modify
//	it under the terms of the GNU General Public License as published by
//	the Free Software Foundation, either version 3 of the License, or
//	(at your option) any later version.
//
//	This program is distributed in the hope that it will be useful,
//	but WITHOUT ANY WARRANTY; without even the implied warranty of
//	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.See the
//	GNU General Public License for more details.
//
//	You should have received a copy of the GNU General Public License
//	along with this program.If not, see <http://www.gnu.org/licenses/>.

#include "StdAfx.h"

#include "LiftCoeff.h"
#include "ShipMets.h"

// Borrowed from DeltaGlider.
void VLiftCoeff(VESSEL *v, double aoa, double M, double Re, void *context, double *cl, double *cm, double *cd)
{
	int i;
	const int nabsc = 9;
	static const double AOA[nabsc] = { -180 * RAD, -60 * RAD, -30 * RAD, -2 * RAD, 15 * RAD, 20 * RAD, 25 * RAD, 60 * RAD, 180 * RAD };
	static const double CL[nabsc] = { 0, 0, -0.4, 0, 0.7, 1, 0.8, 0, 0 };
	static const double CM[nabsc] = { 0, 0, 0.014, 0.0039, -0.006, -0.008, -0.010, 0, 0 };
	for (i = 0; i < nabsc - 1 && AOA[i + 1] < aoa; i++);
	double f = (aoa - AOA[i]) / (AOA[i + 1] - AOA[i]);
	*cl = CL[i] + (CL[i + 1] - CL[i]) * f;  // aoa-dependent lift coefficient
	*cm = CM[i] + (CM[i + 1] - CM[i]) * f;  // aoa-dependent moment coefficient
	double saoa = sin(aoa);
	double pd = 0.015 + 0.4*saoa*saoa;  // profile drag
	*cd = pd + oapiGetInducedDrag(*cl, VERT_WING_AR, VERT_WIND_EFFICIENCY) + oapiGetWaveDrag(M, 0.75, 1.0, 1.1, 0.04);
	// profile drag + (lift-)induced drag + transonic/supersonic wave (compressibility) drag
}

// Borrowed from DeltaGlider.
void HLiftCoeff(VESSEL *v, double beta, double M, double Re, void *context, double *cl, double *cm, double *cd)
{
	int i;
	const int nabsc = 8;
	static const double BETA[nabsc] = { -180 * RAD, -135 * RAD, -90 * RAD, -45 * RAD, 45 * RAD, 90 * RAD, 135 * RAD, 180 * RAD };
	static const double CL[nabsc] = { 0, +0.3, 0, -0.3, +0.3, 0, -0.3, 0 };
	for (i = 0; i < nabsc - 1 && BETA[i + 1] < beta; i++);
	*cl = CL[i] + (CL[i + 1] - CL[i]) * (beta - BETA[i]) / (BETA[i + 1] - BETA[i]);
	*cm = 0.0;
	*cd = 0.015 + oapiGetInducedDrag(*cl, HORZ_WING_AR, HORZ_WING_EFFICIENCY) + oapiGetWaveDrag(M, 0.75, 1.0, 1.1, 0.04);
}
