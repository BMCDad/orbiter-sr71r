//	Tools - bco Orbiter Library
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

#pragma once

#include "Orbitersdk.h"
#include "bco.h"

namespace bc_orbiter
{
	/**
	Contains information needed to draw a font onto a surface.
	*/
	struct FontInfo
	{
		SURFHANDLE surfSource;		// SURFHANDLE to the texture that contains the font image.
		int charWidth;				// Width of each character (fixed width).
		int charHeight;				// Hight of each character.
		int sourceX;				// Horizontal start of font image.
		int sourceY;				// Vertical start of font image.
		int blankX;					// Horizontal start of blank.
		int blankY;					// Vertical start of blank.
	};

	/**
	Contains the place values for a number.  Useful for things like altimeters.
	*/
	struct TensParts
	{
		double Hundredths;
		double Tenths;
		double Tens;
		double Hundreds;
		double Thousands;
		double TenThousands;
		double HundredThousands;
		double Millions;
	};

	// Indicates how to draw text.  See DrawSurfaceText.
	enum DrawTextFormat { Left, Right, Center };

    inline double AngleToState(double angle)
    {
        auto state = angle / PI2;
        if (state < 0.0) state = state + 1.0;
        return state;
    }

	/**
	Breaks a number into its component 10s parts.
	@param number The input number
	@param out Returns the number's tens parts.
	*/
	inline void BreakTens(double number, TensParts& out)
	{
		out.Millions = number / 1000000;
		double remainder = number - ((int)out.Millions * 1000000);

		out.HundredThousands = remainder / 100000;
		remainder = remainder - ((int)out.HundredThousands * 100000);

		out.TenThousands = remainder / 10000;
		remainder = remainder - ((int)out.TenThousands * 10000);

		out.Thousands = remainder / 1000;
		remainder = remainder - ((int)out.Thousands * 1000);

		out.Hundreds = remainder / 100;
		remainder = remainder - ((int)out.Hundreds * 100);

		out.Tens = remainder / 10;
	}

	/**
	Breaks the number into its tenparts components.  Each
	part will be a whole number plus a hundredths part.  This is useful
	for 'barrel' numbers.
	*/
	inline void GetDigits(double number, TensParts& out)
	{
        // fmod(num, den) returns the remainder of num/den rounded to 0.
        // so 123, for example, is .3.

        // Start by multiplying by 100.  This moves hundreths into
        // the whole number range.  So, for example, the incoming number
        // is 234.56, that becomes 23456.
		double numberIn = number * 100;	// Move hundreths into the whole number.

        // Now get fmod.  In our example, this is '6'.  23456 / 10 = 2345.[6]
		//out.Hundredths = fmod(numberIn, 10.0);
        modf(fmod(numberIn, 10.0), &out.Hundredths);

        // Divide by 10 and keep going.
		numberIn /= 10;
		//out.Tenths = fmod(numberIn, 10.0);
        modf(fmod(numberIn, 10.0), &out.Tenths);

		numberIn /= 10;
		//out.Tens = fmod(numberIn, 10.0);
        modf(fmod(numberIn, 10.0), &out.Tens);
		
        numberIn /= 10;
		//out.Hundreds = fmod(numberIn, 10.0);
        modf(fmod(numberIn, 10.0), &out.Hundreds);

		numberIn /= 10;
		//out.Thousands = fmod(numberIn, 10.0);
        modf(fmod(numberIn, 10.0), &out.Thousands);

		numberIn /= 10;
		modf(fmod(numberIn, 10.0), &out.TenThousands);

		numberIn /= 10;
		modf(fmod(numberIn, 10.0), &out.HundredThousands);
		
        numberIn /= 10;
		modf(fmod(numberIn, 10.0), &out.Millions);
	}

	inline VECTOR3 RotateVector(const VECTOR3 &input, double angle, const VECTOR3 &rotationaxis)
	{
		// To rotate a vector in 3D space we'll need to build a matrix, these are the variables required to do so.
		// NOTE: this version of the function takes it's input angle in Radians

		double c = cos(angle);
		double s = sin(angle);
		double t = 1.0 - c;
		double x = rotationaxis.x;
		double y = rotationaxis.y;
		double z = rotationaxis.z;

		// Build rotation matrix
		MATRIX3 rMatrix;
		rMatrix.m11 = (t * x * x + c);
		rMatrix.m12 = (t * x * y - s * z);
		rMatrix.m13 = (t * x * z + s * y);
		rMatrix.m21 = (t * x * y + s * z);
		rMatrix.m22 = (t * y * y + c);
		rMatrix.m23 = (t * y * z - s * x);
		rMatrix.m31 = (t * x * z - s * y);
		rMatrix.m32 = (t * y * z + s * x);
		rMatrix.m33 = (t * z * z + c);

		// Perform Rotation
		VECTOR3 output = mul(rMatrix, input); // multiply the input vector by our rotation matrix to get our output vector
		return output; // Return rotated vector

	} // End "RotateVector"
	/**
	Transforms the x and y members of the source NTVERTEX array based on the translate and angle parameters
	and returns the transformed array in the vectors parameter.  This is useful for translating or rotating a mesh for
	animating a panel surface.
	@param source The NTVERTEX array to transform.
	@param vectors The resulting NTVERTEX array.
	@param numVectors The number of vectors in both source and vectors.
	@param translate A vector whose x and y members indicate the translation to apply.
	@param angle The angle in radians to rotate the x and y coordinates.  The center is calculated average
	of the x and y values in source.
	*/
	inline void TransformXY2d(const NTVERTEX* source, NTVERTEX* vectors, int numVectors, const VECTOR3& translate, double angle)
	{
		double sumx = 0;
		double sumy = 0;

		for (int i = 0; i < numVectors; i++)
		{
			vectors[i] = source[i];
			sumx += source[i].x;
			sumy += source[i].y;
		}

		float centerX = (float)((sumx / numVectors) + translate.x);
		float centerY = (float)((sumy / numVectors) + translate.y);

		// Apply translation.
		for (int i = 0; i < numVectors; i++)
		{
			vectors[i].x += (float)translate.x;
			vectors[i].y += (float)translate.y;
		}

		// Move to origin.
		for (int i = 0; i < numVectors; i++)
		{
			vectors[i].x -= centerX;
			vectors[i].y -= centerY;
		}

		double sina = sin(angle), cosa = cos(angle);

		// Rotate.
		for (int i = 0; i < numVectors; i++)
		{
			float y = (float)(vectors[i].y*cosa - vectors[i].x*sina);
			float x = (float)(vectors[i].y*sina + vectors[i].x*cosa);
			vectors[i].y = y;
			vectors[i].x = x;
		}

		// Move back.
		for (int i = 0; i < numVectors; i++)
		{
			vectors[i].x += centerX;
			vectors[i].y += centerY;
		}
	}

	/**
	Transforms the tu and tv members of the source NTVERTEX array based on the translate and angle parameters
	and returns the transformed array in the vectors parameter.  This is useful for translating or rotating a mesh's
	texture for	animating a panel surface.
	@param source The NTVERTEX array to transform.
	@param vectors The resulting NTVERTEX array.
	@param numVectors The number of vectors in both source and vectors.
	@param translate A vector whose x and y members indicate the translation to apply.
	@param angle The angle in radians to rotate the x and y coordinates.  The center is calculated average
	of the x and y values in source.
	*/
	inline void TransformUV2d(const NTVERTEX* source, NTVERTEX* vectors, int numVectors, const VECTOR3& translate, double angle)
	{
		double sumu = 0;
		double sumv = 0;

		for (int i = 0; i < numVectors; i++)
		{
			vectors[i] = source[i];
			sumu += source[i].tu;
			sumv += source[i].tv;
		}

		float centerU = (float)((sumu / numVectors) + translate.x);
		float centerV = (float)((sumv / numVectors) + translate.y);

		// Apply translation.
		for (int i = 0; i < numVectors; i++)
		{
			vectors[i].tu += (float)translate.x;
			vectors[i].tv += (float)translate.y;
		}

		// Move to origin.
		for (int i = 0; i < numVectors; i++)
		{
			vectors[i].tu -= (float)centerU;
			vectors[i].tv -= (float)centerV;
		}

		double sina = sin(angle), cosa = cos(angle);

		// Rotate.
		for (int i = 0; i < numVectors; i++)
		{
			float v = (float)(vectors[i].tv*cosa - vectors[i].tu*sina);
			float u = (float)(vectors[i].tv*sina + vectors[i].tu*cosa);
			vectors[i].tv = v;
			vectors[i].tu = u;
		}

		// Move back.
		for (int i = 0; i < numVectors; i++)
		{
			vectors[i].tu += centerU;
			vectors[i].tv += centerV;
		}
	}

	/**
	Calculates the KIAS for the given vessel.
	@param vessel Vessel to get KIAS for.
	*/
	inline double GetVesselKias(const VESSEL3* vessel)
	{
		auto result = 0.0;

		// From Heilor, Orbiter forum
		// speed of sound at sea level
		double speedOfSound = 340.29;   // m/s

										// use Orbiter's constant for earth sea level pressure
		double seaLevelPres = ATMP;
		double ias = 0;
		double statPres, dynPres, mach;
		const ATMCONST *atmConst;
		OBJHANDLE atmRef = vessel->GetAtmRef();
		if (atmRef != NULL)
		{
			// Freestream static pressure
			statPres = vessel->GetAtmPressure();

			// Retrieve the ratio of specific heats
			atmConst = oapiGetPlanetAtmConstants(atmRef);
			double gamma = atmConst->gamma;

			// Mach number
			mach = vessel->GetMachNumber();

			// Determine the dynamic pressure using the
			// thermal definition for stagnation pressure
			dynPres = (gamma - 1) * pow(mach, 2.0) / 2 + 1;
			dynPres = pow(dynPres, gamma / (gamma - 1));
			// Convert stagnation pressure to dynamic pressure
			dynPres = dynPres * statPres - statPres;

			// Following is the equation from the Orbiter manual, page 62
			ias = dynPres / seaLevelPres + 1;
			ias = pow(ias, ((gamma - 1) / gamma)) - 1.0;
			ias = ias * 2 / (gamma - 1);
			ias = sqrt(ias) * speedOfSound;
		}

		result = ias * 1.94384; // To knots.

		return result;
	}

	/**
	Calculate the KEAS for the given vessel.
	@param vessel Vessel to get KEAS for.
	*/
	inline double GetVesselKeas(const VESSEL3* vessel)
	{
		double speedOfSound = 340.29;   // m/s

		return (vessel->GetMachNumber() * speedOfSound * sqrt(vessel->GetAtmPressure() / ATMP)) * 1.94384;
	}

	/**
	Calculate the G force on the vessel.
	@param vessel Vessel to calculate G forces for.
	*/
	inline double GetVesselGs(const VESSEL3* vessel)
	{
		VECTOR3 vW, vF;
		vessel->GetWeightVector(vW);
		vessel->GetForceVector(vF);
		auto m = vessel->GetMass();

		auto vA = (vF - vW) / m;
		return length(vA) / 9.81;
	}

	/**
	Calculate the vessel vertical speed.
	*/
	inline double GetVerticalSpeedFPM(const VESSEL3* vessel)
	{
		VECTOR3 V;
		double vspdFPM = 0.0;
		if (vessel->GetAirspeedVector(FRAME_HORIZON, V))
		{
			vspdFPM = (V.y * 3.28084) * 60; // his gives us feet per minute.
		}

		return vspdFPM;
	}

	/**
	Returns the vessel vertical speed.
	*/
	inline double GetVerticalSpeedRaw(const VESSEL3* vessel)
	{
		VECTOR3 V;
		double vspd = 0.0;
		if (vessel->GetAirspeedVector(FRAME_HORIZON, V))
		{
			vspd = V.y;
		}

		return vspd;
	}

	/**
	Blank surface text.  Clears out MFD text.
	*/
	inline void DrawBlankText(int x, int y, SURFHANDLE surfTarget, FontInfo& font)
	{
		int xCurrent = x;

		oapiBlt(
			surfTarget,
			font.surfSource,
			xCurrent, y,
			font.blankX, font.blankY,
			3 * font.charWidth,
			font.charHeight);
	}

	/**
	Draws the text string in 'text' onto a SURFHANDLE target at x, y position using font info from the 'font'.
	@param x The horizontal location for the text.
	@param y The veritcal location for the text.
	@param text The text string to draw on the surface.
	@param format How to align the text: left|center|right.
	@param surfTarget The SURFHANDLE to draw the string on.
	@param font The FontInfo used for drawing.
	*/
	inline void DrawSurfaceText(int x, int y, const char* text, DrawTextFormat format, SURFHANDLE surfTarget, FontInfo& font)
	{
		int len = strlen(text);
		int blockLen = len * font.charWidth;
		int xCurrent = x;

		switch (format)
		{
		case DrawTextFormat::Right:
			xCurrent = x - blockLen;
			break;

		case DrawTextFormat::Center:
			xCurrent = x - (blockLen / 2);
			break;
		}

		oapiBlt(
			surfTarget,
			font.surfSource,
			xCurrent, y,
			font.blankX, font.blankY,
			len * font.charWidth,
			font.charHeight);

		for (int i = 0; i < len; i++)
		{
			char l = text[i] - 32;

			if (l >= 0)
			{
				int fo = (l * font.charWidth) + font.sourceX;
				oapiBlt(
					surfTarget,
					font.surfSource,
					xCurrent, y,
					fo, font.sourceY,
					font.charWidth, font.charHeight);

				xCurrent += font.charWidth;
			}
		}
	}

	/**
	Returns distance and direction from point one on a globe to point two given the longitude and
	latitude of each point.

	From Orbiter DeltaGlider code.

	@param lng1 Longitude for point 1.
	@param lat1 Latitude for point 1.
	@parma lng2 Longitude for point 2.
	@param lat2 Latitude for point 2.
	@param dist Returns the dist between the points.
	@param dir Returns the direction in radians.
	*/
	inline void Orthodome(double lng1, double lat1, double lng2, double lat2, double &dist, double &dir)
	{
		double A = lng2 - lng1;
		double sinA = sin(A), cosA = cos(A);
		double slat1 = sin(lat1), clat1 = cos(lat1);
		double slat2 = sin(lat2), clat2 = cos(lat2);
		double cosa = slat2*slat1 + clat2*clat1*cosA;
		dist = acos(cosa);
		dir = asin(clat2*sinA / sin(dist));
		if (lat2 < lat1) dir = PI - dir; // point 2 further south than point 1
		if (dir < 0.0) dir += PI2;     // quadrant 4
	}

	/** CalcLaunchHeading
		@param lat Launch latitude.
		@param tgtInc Target inclination in radian.
		@param direct Returns the direct heading.
		@param alt Returns the alternate heading.
		Calculates the direct launch heading given a launch
		latitude and target inclination.
	*/
	inline bool CalcLaunchHeading(double lat, double tgtInc, double &direct, double &alt)
	{
		/*
		 Target inclincation is measured from due EAST and must be greater than the
		 launch latitude.  So for a direct heading, if latitude is 20deg then the
		 target inclination must be between 20 and 90.  Values greater than 90 can
		 be used for retrograde or southern departure inclinations.  In that case,
		 the value must be between 90 and 160 (180 - 20 <example lat).  So for 20deg
		 the valid inclination range is 20 -> 160
		 */


		if (lat > tgtInc) return false;				// Target too shallow in direct.
		if (tgtInc > (PI - lat)) return false;		// Retro target too shallow.

		// asin returns (in deg) -90 to 90.  Negative values being retrograde orbit.
		direct = asin(cos(tgtInc) / cos(lat));

		// Adjust for negative values and find 'alt'.
		alt = PI + direct;	// Happens before adjustment of direct.
		if (direct < 0.0) direct = PI2 + direct;
		
		return true;
	}

	inline void RotateMesh(MESHHANDLE mesh, const UINT group, const NTVERTEX* verts, const VECTOR3& trans)
	{
		if (NULL == mesh)
		{
			return;
		}

		GROUPEDITSPEC change{};
		NTVERTEX delta[4];

		TransformXY2d(verts, delta, 4, trans, 0.0);

		change.flags = GRPEDIT_VTXCRD;
		change.nVtx = 4;
		change.vIdx = NULL; //Just use the mesh order
		change.Vtx = delta;
		oapiEditMeshGroup(mesh, group, &change);
	}

	inline void RotateMesh(MESHHANDLE mesh, const UINT group, const NTVERTEX* verts, const double angle)
	{
		if (NULL == mesh)
		{
			return;
		}

		GROUPEDITSPEC change {};
		NTVERTEX delta[4];

		TransformXY2d(verts, delta, 4, _V(0.0, 0.0, 0.0), angle);

		change.flags = GRPEDIT_VTXCRD;
		change.nVtx = 4;
		change.vIdx = NULL; //Just use the mesh order
		change.Vtx = delta;
		oapiEditMeshGroup(mesh, group, &change);
	}

	inline void DrawPanelOnOff(MESHHANDLE mesh, UINT group, const NTVERTEX* verts, bool isOn, double offset)
	{
		auto grp = oapiMeshGroup(mesh, group);

		double trans = isOn ? offset : 0.0;
		grp->Vtx[0].tu = verts[0].tu + trans;
		grp->Vtx[1].tu = verts[1].tu + trans;
		grp->Vtx[2].tu = verts[2].tu + trans;
		grp->Vtx[3].tu = verts[3].tu + trans;
	}
}