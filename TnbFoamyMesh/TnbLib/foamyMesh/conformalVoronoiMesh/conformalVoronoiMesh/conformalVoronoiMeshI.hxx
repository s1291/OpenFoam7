#pragma once
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

\*---------------------------------------------------------------------------*/

#include <indexedVertexOps.hxx>
#include <indexedCellOps.hxx>

// * * * * * * * * * * * * * Private Member Functions  * * * * * * * * * * * //

inline tnbLib::scalar tnbLib::conformalVoronoiMesh::defaultCellSize() const
{
    return foamyHexMeshControls().defaultCellSize();
}


inline tnbLib::scalar tnbLib::conformalVoronoiMesh::targetCellSize
(
    const tnbLib::point& pt
) const
{
    return cellShapeControls().cellSize(pt);
}


inline tnbLib::scalar tnbLib::conformalVoronoiMesh::averageAnyCellSize
(
    const Vertex_handle& vA,
    const Vertex_handle& vB
) const
{
    if
        (
            (!vA->internalOrBoundaryPoint() || vA->referred())
            && (!vB->internalOrBoundaryPoint() || vB->referred())
            )
    {
        // There are no internalOrBoundaryPoints available, determine
        // size from scratch

        // Geometric mean
        return sqrt
        (
            targetCellSize(topoint(vA->point()))
            * targetCellSize(topoint(vB->point()))
        );
    }
    else if (!vB->internalOrBoundaryPoint() || vB->referred())
    {
        return vA->targetCellSize();
    }
    else if (!vA->internalOrBoundaryPoint() || vA->referred())
    {
        return vB->targetCellSize();
    }

    return CGAL::indexedVertexOps::averageCellSize(vA, vB);
}


inline tnbLib::scalar tnbLib::conformalVoronoiMesh::averageAnyCellSize
(
    const Delaunay::Finite_facets_iterator& fit
) const
{
    // Arithmetic mean

    scalar sizeSum = 0;

    label nProducts = 0;

    const Cell_handle c(fit->first);
    const label oppositeVertex = fit->second;

    for (label i = 0; i < 3; i++)
    {
        Vertex_handle v = c->vertex(vertex_triple_index(oppositeVertex, i));

        if (v->internalOrBoundaryPoint() && !v->referred())
        {

            sizeSum += v->targetCellSize();

            nProducts++;
        }
    }

    if (nProducts < 1)
    {
        // There are no internalOrBoundaryPoints available, determine
        // size from scratch

        for (label i = 0; i < 3; i++)
        {
            Vertex_handle v = c->vertex(vertex_triple_index(oppositeVertex, i));

            sizeSum += targetCellSize(topoint(v->point()));
        }

        nProducts = 3;
    }

    if (sizeSum < 0)
    {
        WarningInFunction
            << "sizeSum = " << sizeSum
            << endl;

        return 0;
    }

    return pow(sizeSum, (1.0 / nProducts));
}


inline tnbLib::scalar tnbLib::conformalVoronoiMesh::pointPairDistance
(
    const tnbLib::point& pt
) const
{
    return targetCellSize(pt) * foamyHexMeshControls().pointPairDistanceCoeff();
}


inline tnbLib::scalar tnbLib::conformalVoronoiMesh::mixedFeaturePointDistance
(
    const tnbLib::point& pt
) const
{
    return
        pointPairDistance(pt)
        * foamyHexMeshControls().mixedFeaturePointPPDistanceCoeff();
}


inline tnbLib::scalar tnbLib::conformalVoronoiMesh::featurePointExclusionDistanceSqr
(
    const tnbLib::point& pt
) const
{
    return
        sqr
        (
            targetCellSize(pt)
            * foamyHexMeshControls().featurePointExclusionDistanceCoeff()
        );
}


inline tnbLib::scalar tnbLib::conformalVoronoiMesh::featureEdgeExclusionDistanceSqr
(
    const tnbLib::point& pt
) const
{
    return
        sqr
        (
            targetCellSize(pt)
            * foamyHexMeshControls().featureEdgeExclusionDistanceCoeff()
        );
}


inline tnbLib::scalar tnbLib::conformalVoronoiMesh::surfacePtExclusionDistanceSqr
(
    const tnbLib::point& pt
) const
{
    return
        sqr
        (
            targetCellSize(pt)
            * foamyHexMeshControls().surfacePtExclusionDistanceCoeff()
        );
}


inline tnbLib::scalar tnbLib::conformalVoronoiMesh::surfaceSearchDistanceSqr
(
    const tnbLib::point& pt
) const
{
    return
        sqr
        (
            targetCellSize(pt)
            * foamyHexMeshControls().surfaceSearchDistanceCoeff()
        );
}


inline tnbLib::scalar tnbLib::conformalVoronoiMesh::maxSurfaceProtrusion
(
    const tnbLib::point& pt
) const
{
    return
        targetCellSize(pt)
        * foamyHexMeshControls().maxSurfaceProtrusionCoeff();
}


inline void tnbLib::conformalVoronoiMesh::createPointPair
(
    const scalar ppDist,
    const tnbLib::point& surfPt,
    const vector& n,
    const bool ptPair,
    DynamicList<Vb>& pts
) const
{
    vector ppDistn = ppDist * n;

    //    const tnbLib::point internalPt = surfPt - ppDistn;
    //    const tnbLib::point externalPt = surfPt + ppDistn;

    //    bool internalInside = geometryToConformTo_.inside(internalPt);
    //    bool externalOutside = geometryToConformTo_.outside(externalPt);

    //    if (internalInside && externalOutside)
    {
        pts.append
        (
            Vb
            (
                surfPt - ppDistn,
                vertexCount() + pts.size(),
                Vb::vtInternalSurface,
                Pstream::myProcNo()
            )
        );

        pts.append
        (
            Vb
            (
                surfPt + ppDistn,
                vertexCount() + pts.size(),
                Vb::vtExternalSurface,
                Pstream::myProcNo()
            )
        );

        if (ptPair)
        {
            ptPairs_.addPointPair
            (
                pts[pts.size() - 2].index(),
                pts[pts.size() - 1].index() // external 0 -> slave
            );
        }
    }
    //    else
    //    {
    //        Info<< "Warning: point pair not inside/outside" << nl
    //            << "    surfPt = " << surfPt << nl
    //            << "    internal = " << internalPt << " " << internalInside << nl
    //            << "    external = " << externalPt << " " << externalOutside
    //            << endl;
    //    }
}


inline tnbLib::point tnbLib::conformalVoronoiMesh::perturbPoint
(
    const tnbLib::point& pt
) const
{
    tnbLib::point perturbedPt(pt);

    //    vector delta(xR/ni, yR/nj, zR/nk);
    //    scalar pert = randomPerturbationCoeff*cmptMin(delta);

    scalar pert = 1e-12 * defaultCellSize();

    perturbedPt.x() += pert * (rndGen_.scalar01() - 0.5);
    perturbedPt.y() += pert * (rndGen_.scalar01() - 0.5);
    perturbedPt.z() += pert * (rndGen_.scalar01() - 0.5);

    return perturbedPt;
}


inline void tnbLib::conformalVoronoiMesh::createBafflePointPair
(
    const scalar ppDist,
    const tnbLib::point& surfPt,
    const vector& n,
    const bool ptPair,
    DynamicList<Vb>& pts
) const
{
    vector ppDistn = ppDist * n;

    pts.append
    (
        Vb
        (
            surfPt - ppDistn,
            vertexCount() + pts.size(),
            Vb::vtInternalSurfaceBaffle,
            Pstream::myProcNo()
        )
    );

    pts.append
    (
        Vb
        (
            surfPt + ppDistn,
            vertexCount() + pts.size(),
            Vb::vtExternalSurfaceBaffle,
            Pstream::myProcNo()
        )
    );

    if (ptPair)
    {
        ptPairs_.addPointPair
        (
            pts[pts.size() - 2].index(), // external 0 -> slave
            pts[pts.size() - 1].index()
        );
    }
}


inline bool tnbLib::conformalVoronoiMesh::internalPointIsInside
(
    const tnbLib::point& pt
) const
{
    if
        (
            !geometryToConformTo_.globalBounds().contains(pt)
            || !geometryToConformTo_.inside(pt)
            )
    {
        return false;
    }

    return true;
}


inline bool tnbLib::conformalVoronoiMesh::isBoundaryDualFace
(
    const Delaunay::Finite_edges_iterator& eit
) const
{
    Cell_handle c = eit->first;
    Vertex_handle vA = c->vertex(eit->second);
    Vertex_handle vB = c->vertex(eit->third);

    //    if (vA->internalBoundaryPoint() && vB->externalBoundaryPoint())
    //    {
    //        if (vA->index() == vB->index() - 1)
    //        {
    //            return true;
    //        }
    //    }
    //    else if (vA->externalBoundaryPoint() && vB->internalBoundaryPoint())
    //    {
    //        if (vA->index() == vB->index() + 1)
    //        {
    //            return true;
    //        }
    //    }
    //
    //    return false;

        // A dual face on the boundary will result from one Dv inside and
        // one outside
    return
        (
            (
                (vA->internalOrBoundaryPoint() && !vA->referred())
                || (vB->internalOrBoundaryPoint() && !vB->referred())
                )
            && (
                !vA->internalOrBoundaryPoint()
                || !vB->internalOrBoundaryPoint()
                )
            );
}


inline tnbLib::List<bool> tnbLib::conformalVoronoiMesh::dualFaceBoundaryPoints
(
    const Delaunay::Finite_edges_iterator& eit
) const
{
    Cell_circulator ccStart = incident_cells(*eit);
    Cell_circulator cc1 = ccStart;
    Cell_circulator cc2 = cc1;

    // Advance the second circulator so that it always stays on the next
    // cell around the edge;
    cc2++;

    DynamicList<bool> tmpFaceBoundaryPoints;

    do
    {
        label cc1I = cc1->cellIndex();

        label cc2I = cc2->cellIndex();

        if (cc1I != cc2I)
        {
            if (cc1->boundaryDualVertex())
            {
                tmpFaceBoundaryPoints.append(true);
            }
            else
            {
                tmpFaceBoundaryPoints.append(false);
            }
        }

        cc1++;

        cc2++;

    } while (cc1 != ccStart);

    return tnbLib::move(tmpFaceBoundaryPoints);
}


inline tnbLib::List<tnbLib::label> tnbLib::conformalVoronoiMesh::processorsAttached
(
    const Delaunay::Finite_facets_iterator& fit
) const
{
    DynamicList<label> procsAttached(8);

    const Cell_handle c1(fit->first);
    const label oppositeVertex = fit->second;
    const Cell_handle c2(c1->neighbor(oppositeVertex));

    FixedList<label, 4> c1Procs(CGAL::indexedCellOps::processorsAttached(c1));

    FixedList<label, 4> c2Procs(CGAL::indexedCellOps::processorsAttached(c2));

    forAll(c1Procs, aPI)
    {
        if (findIndex(procsAttached, c1Procs[aPI] == -1))
        {
            procsAttached.append(c1Procs[aPI]);
        }

        if (findIndex(procsAttached, c2Procs[aPI] == -1))
        {
            procsAttached.append(c2Procs[aPI]);
        }
    }

    return List<label>(procsAttached);
}


inline bool tnbLib::conformalVoronoiMesh::isParallelDualEdge
(
    const Delaunay::Finite_facets_iterator& fit
) const
{
    const Cell_handle c1(fit->first);
    const label oppositeVertex = fit->second;

    return
        (
            c1->vertex(vertex_triple_index(oppositeVertex, 0))->referred()
            || c1->vertex(vertex_triple_index(oppositeVertex, 1))->referred()
            || c1->vertex(vertex_triple_index(oppositeVertex, 2))->referred()
            );
}


inline bool tnbLib::conformalVoronoiMesh::isProcBoundaryEdge
(
    const Delaunay::Finite_edges_iterator& eit
) const
{
    bool isProcBoundaryEdge = false;

    Cell_handle c = eit->first;
    Vertex_handle vA = c->vertex(eit->second);
    Vertex_handle vB = c->vertex(eit->third);

    if
        (
            (
                (vA->referred() && !vB->referred())
                || (vB->referred() && !vA->referred())
                )
            && vA->internalOrBoundaryPoint()
            && vB->internalOrBoundaryPoint()
            )
    {
        isProcBoundaryEdge = true;
    }

    return isProcBoundaryEdge;
}


// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

inline const tnbLib::Time& tnbLib::conformalVoronoiMesh::time() const
{
    return runTime_;
}


inline tnbLib::Random& tnbLib::conformalVoronoiMesh::rndGen() const
{
    return rndGen_;
}


inline const tnbLib::searchableSurfaces&
tnbLib::conformalVoronoiMesh::allGeometry() const
{
    return allGeometry_;
}


inline const tnbLib::conformationSurfaces&
tnbLib::conformalVoronoiMesh::geometryToConformTo() const
{
    return geometryToConformTo_;
}


inline const tnbLib::backgroundMeshDecomposition&
tnbLib::conformalVoronoiMesh::decomposition() const
{
    if (!Pstream::parRun())
    {
        FatalErrorInFunction
            << "The backgroundMeshDecomposition cannot be asked for in serial."
            << exit(FatalError) << endl;
    }

    return decomposition_();
}


inline const tnbLib::cellShapeControl&
tnbLib::conformalVoronoiMesh::cellShapeControls() const
{
    return cellShapeControl_;
}


inline const tnbLib::cvControls&
tnbLib::conformalVoronoiMesh::foamyHexMeshControls() const
{
    return foamyHexMeshControls_;
}


// ************************************************************************* //
