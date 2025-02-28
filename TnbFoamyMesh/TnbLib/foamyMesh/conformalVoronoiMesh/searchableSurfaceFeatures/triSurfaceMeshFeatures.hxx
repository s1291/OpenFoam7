#pragma once
#ifndef _triSurfaceMeshFeatures_Header
#define _triSurfaceMeshFeatures_Header

/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     | Website:  https://openfoam.org
    \\  /    A nd           | Copyright (C) 2013-2019 OpenFOAM Foundation
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
    tnbLib::triSurfaceMeshFeatures

Description

SourceFiles
    triSurfaceMeshFeatures.C

\*---------------------------------------------------------------------------*/

#include <searchableSurfaceFeatures.hxx>

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace tnbLib
{

    /*---------------------------------------------------------------------------*\
                       Class triSurfaceMeshFeatures Declaration
    \*---------------------------------------------------------------------------*/

    class triSurfaceMeshFeatures
        :
        public searchableSurfaceFeatures
    {
    private:

        // Private Member Data

        const scalar includedAngle_;

        //- Which side of the surface to mesh
        const extendedFeatureEdgeMesh::sideVolumeType mode_;


    public:

        //- Runtime type information
        TypeName("triSurfaceMeshFeatures");


        // Constructors

            //- Construct from dictionary (used by searchableSurface)
        triSurfaceMeshFeatures
        (
            const searchableSurface& surface,
            const dictionary& dict
        );

        //- Disallow default bitwise copy construction
        triSurfaceMeshFeatures(const triSurfaceMeshFeatures&) = delete;


        //- Destructor
        virtual ~triSurfaceMeshFeatures();


        // Member Functions

            //- Return true for a triSurfaceMesh having features
        virtual bool hasFeatures() const
        {
            return true;
        }

        //- Return an extendedFeatureEdgeMesh containing the features
        virtual autoPtr<extendedFeatureEdgeMesh> features() const;


        // Member Operators

            //- Disallow default bitwise assignment
        void operator=(const triSurfaceMeshFeatures&) = delete;
    };


    // * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace tnbLib

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //
#endif // !_triSurfaceMeshFeatures_Header
