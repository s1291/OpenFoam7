#pragma once
#ifndef _directionInfo_Header
#define _directionInfo_Header

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
    tnbLib::directionInfo

Description
    Holds direction in which to split cell (in fact a local coordinate axes).
    Information is a label and a direction.

    The direction is the normal
    direction to cut in. The label's meaning depends on whether the info
    is on a cell or on a face:
        - in cell: edge that is being cut. (determines for hex how cut is)
        - in face: local face point that is being cut or -1.
            -# (-1)  : cut is tangential to plane
            -# (>= 0): edge fp..fp+1 is cut

            (has to be facepoint, not vertex since vertex not valid across
             processors whereas f[0] should correspond to f[0] on other side)

    The rule is that if the label is set (-1 or higher) it is used
    (topological information only), otherwise the vector is used. This makes
    sure that we use topological information as much as possible and so a
    hex mesh is cut purely topologically. All other shapes are cut
    geometrically.

SourceFiles
    directionInfoI.H
    directionInfo.C

\*---------------------------------------------------------------------------*/

#include <DynamicMesh_Module.hxx>
#include <point.hxx>
#include <labelList.hxx>
#include <tensor.hxx>


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace tnbLib
{
    class polyPatch;
    class polyMesh;
    class primitiveMesh;
    class edge;
    class face;
    class polyMesh;


    // Forward declaration of friend functions and operators

    class directionInfo;

    FoamDynamicMesh_EXPORT Istream& operator>>(Istream&, directionInfo&);
    FoamDynamicMesh_EXPORT Ostream& operator<<(Ostream&, const directionInfo&);


    /*---------------------------------------------------------------------------*\
                               Class directionInfo Declaration
    \*---------------------------------------------------------------------------*/

    class directionInfo
    {
        // Private Data

            // Either mesh edge or face point
        label index_;

        // Local n axis
        vector n_;


        // Private Member Functions

            //- Find edge among edgeLabels that uses v0 and v1
        static FoamDynamicMesh_EXPORT label findEdge
        (
            const primitiveMesh& mesh,
            const labelList& edgeLabels,
            const label v1,
            const label v0
        );

        //- Return 'lowest' of a,b in face of size.
        static FoamDynamicMesh_EXPORT label lowest
        (
            const label size,
            const label a,
            const label b
        );

    public:

        // Static Functions

            //- Given edge on hex cell find corresponding edge on face. Is either
            //  index in face or -1 (cut tangential to face). Public since is
            //  needed to fill in seed faces in meshWave.
        static FoamDynamicMesh_EXPORT label edgeToFaceIndex
        (
            const primitiveMesh& mesh,
            const label celli,
            const label facei,
            const label edgeI
        );

        // Constructors

            //- Construct null
        inline directionInfo();

        //- Construct from components
        inline directionInfo
        (
            const label,
            const vector& n
        );


        // Member Functions

            // Access

        inline label index() const
        {
            return index_;
        }

        inline const vector& n() const
        {
            return n_;
        }

        // Needed by FaceCellWave

            //- Check whether origin has been changed at all or
            //  still contains original (invalid) value.
        template<class TrackingData>
        inline bool valid(TrackingData& td) const;

        //- Check for identical geometrical data. Used for cyclics checking.
        template<class TrackingData>
        inline bool sameGeometry
        (
            const polyMesh&,
            const directionInfo&,
            const scalar,
            TrackingData& td
        ) const;

        //- Convert any absolute coordinates into relative to (patch)face
        //  centre
        template<class TrackingData>
        inline void leaveDomain
        (
            const polyMesh&,
            const polyPatch&,
            const label patchFacei,
            const point& faceCentre,
            TrackingData& td
        );

        //- Reverse of leaveDomain
        template<class TrackingData>
        inline void enterDomain
        (
            const polyMesh&,
            const polyPatch&,
            const label patchFacei,
            const point& faceCentre,
            TrackingData& td
        );

        //- Apply rotation matrix to any coordinates
        template<class TrackingData>
        inline void transform
        (
            const polyMesh&,
            const tensor&,
            TrackingData& td
        );

        //- Influence of neighbouring face.
        template<class TrackingData>
        inline bool updateCell
        (
            const polyMesh&,
            const label thisCelli,
            const label neighbourFacei,
            const directionInfo& neighbourInfo,
            const scalar tol,
            TrackingData& td
        );

        //- Influence of neighbouring cell.
        template<class TrackingData>
        inline bool updateFace
        (
            const polyMesh&,
            const label thisFacei,
            const label neighbourCelli,
            const directionInfo& neighbourInfo,
            const scalar tol,
            TrackingData& td
        );

        //- Influence of different value on same face.
        template<class TrackingData>
        inline bool updateFace
        (
            const polyMesh&,
            const label thisFacei,
            const directionInfo& neighbourInfo,
            const scalar tol,
            TrackingData& td
        );

        //- Same (like operator==)
        template<class TrackingData>
        inline bool equal(const directionInfo&, TrackingData& td) const;

        // Member Operators

            // Needed for List IO
        inline bool operator==(const directionInfo&) const;

        inline bool operator!=(const directionInfo&) const;


        // IOstream Operators

        friend FoamDynamicMesh_EXPORT Ostream& operator<<(Ostream&, const directionInfo&);
        friend FoamDynamicMesh_EXPORT Istream& operator>>(Istream&, directionInfo&);
    };


    //- Data associated with directionInfo type are contiguous
    template<>
    inline bool contiguous<directionInfo>()
    {
        return true;
    }


    // * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace tnbLib

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#include <directionInfoI.hxx>

#endif // !_directionInfo_Header
