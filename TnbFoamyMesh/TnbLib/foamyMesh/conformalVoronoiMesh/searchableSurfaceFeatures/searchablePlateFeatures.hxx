#pragma once
#ifndef _searchablePlateFeatures_Header
#define _searchablePlateFeatures_Header

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
    tnbLib::searchablePlateFeatures

Description

SourceFiles
    searchablePlateFeatures.C

\*---------------------------------------------------------------------------*/

#include <searchableSurfaceFeatures.hxx>

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace tnbLib
{

    /*---------------------------------------------------------------------------*\
                       Class searchablePlateFeatures Declaration
    \*---------------------------------------------------------------------------*/

    class searchablePlateFeatures
        :
        public searchableSurfaceFeatures
    {
    private:

        //- To initialise edges.
        static edgeList calcEdges(const label[4][2]);


        // Private Member Data

            //- Which side of the plate to mesh
        const extendedFeatureEdgeMesh::sideVolumeType mode_;


    public:

        //- Runtime type information
        TypeName("searchablePlateFeatures");


        // Static Data Members

            //- Edge to point addressing
        static const edgeList edges;


        // Constructors

            //- Construct from searchable surface and dictionary
        searchablePlateFeatures
        (
            const searchableSurface& surface,
            const dictionary& dict
        );

        //- Disallow default bitwise copy construction
        searchablePlateFeatures(const searchablePlateFeatures&) = delete;


        //- Destructor
        virtual ~searchablePlateFeatures();


        // Member Functions

            //- Return true for a searchable plate having features
        virtual bool hasFeatures() const
        {
            return true;
        }

        //- Return an extendedFeatureEdgeMesh containing the features
        virtual autoPtr<extendedFeatureEdgeMesh> features() const;


        // Member Operators

            //- Disallow default bitwise assignment
        void operator=(const searchablePlateFeatures&) = delete;
    };


    // * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace tnbLib

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //
#endif // !_searchablePlateFeatures_Header
