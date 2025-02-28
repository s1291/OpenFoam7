#pragma once
#ifndef _hexCellLooper_Header
#define _hexCellLooper_Header

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
    tnbLib::hexCellLooper

Description
    Implementation of cellLooper.

    This one walks hexes in a topological way:
      - cross edge to other face
      - cross face by walking edge-point-edge across to reach the other side.
    (edges are always cut through the middle)

    For anything else (tet, prism, .. poly) it will use geomCellLooper
    (which does a purely geometric cut using a plane through cell centre)

SourceFiles
    hexCellLooper.C

\*---------------------------------------------------------------------------*/

#include <geomCellLooper.hxx>
#include <typeInfo.hxx>

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace tnbLib
{

    // Forward declaration of classes
    class cellModel;

    /*---------------------------------------------------------------------------*\
                               Class hexCellLooper Declaration
    \*---------------------------------------------------------------------------*/

    class hexCellLooper
        :
        public geomCellLooper
    {

    protected:

        // Protected data

            //- Reference to hex cell shape.
        const cellModel& hex_;

    private:

        // Private Member Functions

            //- Walk across faces of hex. Update loop/loopWeights with edges cut.
            //  Return true if successful walk. (should be always!)
        FoamDynamicMesh_EXPORT bool walkHex
        (
            const label celli,
            const label startFacei,
            const label startEdgeI,

            labelList& loop,
            scalarField& loopWeights
        ) const;

        //- Convert loop into face and points
        FoamDynamicMesh_EXPORT void makeFace
        (
            const labelList& loop,
            const scalarField& loopWeights,

            labelList& faceVerts,
            pointField& facePoints
        ) const;


    public:

        //- Runtime type information
        /*TypeName("hexCellLooper");*/
        static const char* typeName_() { return "hexCellLooper"; }
        static FoamDynamicMesh_EXPORT const ::tnbLib::word typeName;
        static FoamDynamicMesh_EXPORT int debug;
        virtual const word& type() const { return typeName; };


        // Constructors

            //- Construct from components
        FoamDynamicMesh_EXPORT hexCellLooper(const polyMesh& mesh);

        //- Disallow default bitwise copy construction
        hexCellLooper(const hexCellLooper&) = delete;


        //- Destructor
        FoamDynamicMesh_EXPORT virtual ~hexCellLooper();


        // Member Functions

            //- Create cut along circumference of celli. Gets current mesh cuts.
            //  Cut along circumference is expressed as loop of cuts plus weights
            //  for cuts along edges (only valid for edge cuts).
            //  Return true if successful cut.
        FoamDynamicMesh_EXPORT virtual bool cut
        (
            const vector& refDir,
            const label celli,
            const boolList& vertIsCut,
            const boolList& edgeIsCut,
            const scalarField& edgeWeight,

            labelList& loop,
            scalarField& loopWeights
        ) const;


        //- Same but now also base point of cut provided (instead of always
        //  cell centre)
        FoamDynamicMesh_EXPORT virtual bool cut
        (
            const plane& cutPlane,
            const label celli,
            const boolList& vertIsCut,
            const boolList& edgeIsCut,
            const scalarField& edgeWeight,

            labelList& loop,
            scalarField& loopWeights
        ) const;


        // Member Operators

            //- Disallow default bitwise assignment
        void operator=(const hexCellLooper&) = delete;
    };


    // * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace tnbLib

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //


#endif // !_hexCellLooper_Header
