#pragma once
#ifndef _nonUniformField_Header
#define _nonUniformField_Header

/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     | Website:  https://openfoam.org
    \\  /    A nd           | Copyright (C) 2012-2019 OpenFOAM Foundation
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
    tnbLib::nonUniformField

Description

SourceFiles
    nonUniformField.C

\*---------------------------------------------------------------------------*/

#include <FoamyMesh_Module.hxx>

#include <triSurfaceFields.hxx>
#include <PrimitivePatchInterpolation.hxx>
#include <surfaceCellSizeFunction.hxx>
#include <cellSizeCalculationType.hxx>

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace tnbLib
{

    class triSurfaceMesh;
    class searchableSurface;

    /*---------------------------------------------------------------------------*\
                               Class nonUniformField Declaration
    \*---------------------------------------------------------------------------*/

    class nonUniformField
        :
        public surfaceCellSizeFunction
    {

    protected:

        // Private Typedefs

        typedef PrimitivePatchInterpolation
            <
            PrimitivePatch<List<labelledTri>, pointField>
            >
            primitivePatchInterpolation;


        // Private Data

        const triSurfaceMesh& surfaceTriMesh_;

        autoPtr<cellSizeCalculationType> cellSizeCalculationType_;

        triSurfacePointScalarField pointCellSize_;


    public:

        //- Runtime type information
        TypeName("nonUniformField");


        // Constructors

            //- Construct from components
        nonUniformField
        (
            const dictionary& cellSizeFunctionDict,
            const searchableSurface& surface,
            const scalar& defaultCellSize
        );


        //- Destructor
        virtual ~nonUniformField()
        {}


        // Member Functions

            // Query

                //- Return the interpolated cell size for a point in the given
                //  surface triangle
        virtual scalar interpolate
        (
            const point& pt,
            const label index
        ) const;
    };


    // * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace tnbLib

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif // !_nonUniformField_Header
