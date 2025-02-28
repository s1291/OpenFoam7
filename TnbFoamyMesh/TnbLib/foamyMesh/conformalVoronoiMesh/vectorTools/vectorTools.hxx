#pragma once
#ifndef _vectorTools_Header
#define _vectorTools_Header
/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     | Website:  https://openfoam.org
    \\  /    A nd           | Copyright (C) 2011-2018 OpenFOAM Foundation
     \\/     M anipulation  |
-------------------------------------------------------------------------------
License
    This file is part of OpenFOAM.

    OpenFOAM is free software: you can redistribute it and/or modify it
    under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    OpenFOAM is distributed in the hope that it will be useful, but WITHOUT
    ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
    FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
    for more details.

    You should have received a copy of the GNU General Public License
    along with OpenFOAM.  If not, see <http://www.gnu.org/licenses/>.

Class
    tnbLib::vectorTools

Description
    Functions for analysing the relationships between vectors

\*---------------------------------------------------------------------------*/

#include <FoamyMesh_Module.hxx>

#include <vector.hxx>
#include <unitConversion.hxx>

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace tnbLib
{

    /*---------------------------------------------------------------------------*\
                          Namespace vectorTools Declaration
    \*---------------------------------------------------------------------------*/

    //- Collection of functions for testing relationships between two vectors.
    namespace vectorTools
    {
        //- Test if a and b are parallel: a^b = 0
        //  Uses the cross product, so the tolerance is proportional to
        //  the sine of the angle between a and b in radians
        template<class T>
        bool areParallel
        (
            const Vector<T>& a,
            const Vector<T>& b,
            const T& tolerance = small
        )
        {
            return (mag(a ^ b) < tolerance) ? true : false;
            //        return ( mag( mag(a & b)/(mag(a)*mag(b)) - 1.0 ) < tolerance )
            //               ? true
            //               : false;
        }

        //- Test if a and b are orthogonal: a.b = 0
        //  Uses the dot product, so the tolerance is proportional to
        //  the cosine of the angle between a and b in radians
        template<class T>
        bool areOrthogonal
        (
            const Vector<T>& a,
            const Vector<T>& b,
            const T& tolerance = small
        )
        {
            return (mag(a & b) < tolerance) ? true : false;
        }

        //- Test if angle between a and b is acute: a.b > 0
        template<class T>
        bool areAcute
        (
            const Vector<T>& a,
            const Vector<T>& b
        )
        {
            return ((a & b) > 0) ? true : false;
        }

        //- Test if angle between a and b is obtuse: a.b < 0
        template<class T>
        bool areObtuse
        (
            const Vector<T>& a,
            const Vector<T>& b
        )
        {
            return ((a & b) < 0) ? true : false;
        }

        //- Calculate angle between a and b in radians
        template<class T>
        T cosPhi
        (
            const Vector<T>& a,
            const Vector<T>& b,
            const T& tolerance = small
        )
        {
            scalar cosPhi = (a & b) / (mag(a) * mag(b) + tolerance);

            // Enforce bounding between -1 and 1
            return min(max(cosPhi, -1), 1);
        }

        //- Calculate angle between a and b in radians
        template<class T>
        T radAngleBetween
        (
            const Vector<T>& a,
            const Vector<T>& b,
            const T& tolerance = small
        )
        {
            scalar cosPhi = (a & b) / (mag(a) * mag(b) + tolerance);

            // Enforce bounding between -1 and 1
            return acos(min(max(cosPhi, -1), 1));
        }

        //- Calculate angle between a and b in degrees
        template<class T>
        T degAngleBetween
        (
            const Vector<T>& a,
            const Vector<T>& b,
            const T& tolerance = small
        )
        {
            return radToDeg(radAngleBetween(a, b, tolerance));
        }

    } // End namespace vectorTools


    // * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace tnbLib

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
