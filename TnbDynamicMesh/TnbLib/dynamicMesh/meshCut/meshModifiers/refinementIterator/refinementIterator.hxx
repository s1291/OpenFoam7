#pragma once
#ifndef _refinementIterator_Header
#define _refinementIterator_Header

/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     | Website:  https://openfoam.org
    \\  /    A nd           | Copyright (C) 2011-2019 OpenFOAM Foundation
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
    tnbLib::refinementIterator

Description
    Utility class to do iterating meshCutter until all requests satisfied.

    Needed since cell cutting can only cut cell once in one go so if
    refinement pattern is not compatible on a cell by cell basis it will
    refuse to cut.

    Parallel: communicates. All decisions done on 'reduce'd variable.

SourceFiles
    refinementIterator.C

\*---------------------------------------------------------------------------*/

#include <edgeVertex.hxx>
#include <labelList.hxx>
#include <Map.hxx>
#include <typeInfo.hxx>

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace tnbLib
{

    // Forward declaration of classes
    class polyMesh;
    class refineCell;
    class undoableMeshCutter;
    class cellLooper;

    /*---------------------------------------------------------------------------*\
                               Class refinementIterator Declaration
    \*---------------------------------------------------------------------------*/

    class refinementIterator
        :
        public edgeVertex
    {
        // Private Data

            //- Reference to mesh
        polyMesh& mesh_;

        //- Reference to refinementEngine
        undoableMeshCutter& meshRefiner_;

        //- Reference to object to walk individual cells
        const cellLooper& cellWalker_;

        //- Whether to write intermediate meshes
        bool writeMesh_;


    public:

        //- Runtime type information
        /*ClassName("refinementIterator");*/
        static const char* typeName_() { return "refinementIterator"; }
        static FoamDynamicMesh_EXPORT const ::tnbLib::word typeName;
        static FoamDynamicMesh_EXPORT int debug;


        // Constructors

            //- Construct from mesh, refinementEngine and cell walking routine.
            //  If writeMesh = true increments runTime and writes intermediate
            //  meshes.
        FoamDynamicMesh_EXPORT refinementIterator
        (
            polyMesh& mesh,
            undoableMeshCutter& meshRefiner,
            const cellLooper& cellWalker,
            const bool writeMesh = false
        );


        //- Destructor
        FoamDynamicMesh_EXPORT ~refinementIterator();


        // Member Functions

            //- Try to refine cells in given direction. Constructs intermediate
            //  meshes. Returns map from old to added cells.
        FoamDynamicMesh_EXPORT Map<label> setRefinement(const List<refineCell>&);

    };


    // * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace tnbLib

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif // !_refinementIterator_Header
