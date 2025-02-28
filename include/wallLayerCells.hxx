#pragma once
#ifndef _wallLayerCells_Header
#define _wallLayerCells_Header

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
    tnbLib::wallLayerCells

Description
    After construction is a list of \<refineCell\> of cells within a certain
    distance to the wall and with the wall normal direction.

SourceFiles
    wallLayerCells.C

\*---------------------------------------------------------------------------*/

#include <edgeVertex.hxx>
#include <refineCell.hxx>
#include <typeInfo.hxx>

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace tnbLib
{

    /*---------------------------------------------------------------------------*\
                               Class wallLayerCells Declaration
    \*---------------------------------------------------------------------------*/

    class wallLayerCells
        :
        public edgeVertex,
        public List<refineCell>
    {
        // Private Member Functions

            //- Check if any of the faces of celli is on processorPatch
        FoamDynamicMesh_EXPORT bool usesCoupledPatch(const label celli) const;

    public:

        //- Runtime type information
        /*ClassName("wallLayerCells");*/
        static const char* typeName_() { return "wallLayerCells"; }
        static FoamDynamicMesh_EXPORT const ::tnbLib::word typeName;
        static FoamDynamicMesh_EXPORT int debug;


        // Constructors

            //- Construct from components
        FoamDynamicMesh_EXPORT wallLayerCells
        (
            const polyMesh& mesh,
            const List<word>& patchNames,
            const label nLayers
        );
    };


    // * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace tnbLib

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif // !_wallLayerCells_Header

