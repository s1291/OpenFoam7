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

Application
    modifyMesh

Description
    Manipulates mesh elements.

    Actions are:
        (boundary)points:
            - move

        (boundary)edges:
            - split and move introduced point

        (boundary)faces:
            - split(triangulate) and move introduced point

        edges:
            - collapse

        cells:
            - split into polygonal base pyramids around newly introduced mid
              point

    Is a bit of a loose collection of mesh change drivers.

\*---------------------------------------------------------------------------*/

#include "cellSplitter.hxx"

#include <argList.hxx>
#include <Time.hxx>
#include <polyMesh.hxx>
#include <polyTopoChange.hxx>
#include <mapPolyMesh.hxx>
#include <boundaryCutter.hxx>
#include <edgeCollapser.hxx>
#include <meshTools.hxx>
#include <Pair.hxx>
#include <globalIndex.hxx>
#include <IOdictionary.hxx>

using namespace tnbLib;

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

// Locate point on patch. Returns (mesh) point label.
label findPoint(const primitivePatch& pp, const point& nearPoint)
{
    const pointField& points = pp.points();
    const labelList& meshPoints = pp.meshPoints();

    // Find nearest and next nearest
    scalar minDistSqr = great;
    label minI = -1;

    scalar almostMinDistSqr = great;
    label almostMinI = -1;

    forAll(meshPoints, i)
    {
        label pointi = meshPoints[i];

        scalar distSqr = magSqr(nearPoint - points[pointi]);

        if (distSqr < minDistSqr)
        {
            almostMinDistSqr = minDistSqr;
            almostMinI = minI;

            minDistSqr = distSqr;
            minI = pointi;
        }
        else if (distSqr < almostMinDistSqr)
        {
            almostMinDistSqr = distSqr;
            almostMinI = pointi;
        }
    }


    // Decide if nearPoint unique enough.
    Info << "Found to point " << nearPoint << nl
        << "    nearest point      : " << minI
        << " distance " << tnbLib::sqrt(minDistSqr)
        << " at " << points[minI] << nl
        << "    next nearest point : " << almostMinI
        << " distance " << tnbLib::sqrt(almostMinDistSqr)
        << " at " << points[almostMinI] << endl;

    if (almostMinDistSqr < 4 * minDistSqr)
    {
        Info << "Next nearest too close to nearest. Aborting" << endl;

        return -1;
    }
    else
    {
        return minI;
    }
}


// Locate edge on patch. Return mesh edge label.
label findEdge
(
    const primitiveMesh& mesh,
    const primitivePatch& pp,
    const point& nearPoint
)
{
    const pointField& localPoints = pp.localPoints();
    const pointField& points = pp.points();
    const labelList& meshPoints = pp.meshPoints();
    const edgeList& edges = pp.edges();

    // Find nearest and next nearest
    scalar minDist = great;
    label minI = -1;

    scalar almostMinDist = great;
    label almostMinI = -1;

    forAll(edges, edgeI)
    {
        const edge& e = edges[edgeI];

        pointHit pHit(e.line(localPoints).nearestDist(nearPoint));

        if (pHit.hit())
        {
            if (pHit.distance() < minDist)
            {
                almostMinDist = minDist;
                almostMinI = minI;

                minDist = pHit.distance();
                minI = meshTools::findEdge
                (
                    mesh,
                    meshPoints[e[0]],
                    meshPoints[e[1]]
                );
            }
            else if (pHit.distance() < almostMinDist)
            {
                almostMinDist = pHit.distance();
                almostMinI = meshTools::findEdge
                (
                    mesh,
                    meshPoints[e[0]],
                    meshPoints[e[1]]
                );
            }
        }
    }

    if (minI == -1)
    {
        Info << "Did not find edge close to point " << nearPoint << endl;

        return -1;
    }


    // Decide if nearPoint unique enough.
    Info << "Found to point " << nearPoint << nl
        << "    nearest edge      : " << minI
        << " distance " << minDist << " endpoints "
        << mesh.edges()[minI].line(points) << nl
        << "    next nearest edge : " << almostMinI
        << " distance " << almostMinDist << " endpoints "
        << mesh.edges()[almostMinI].line(points) << nl
        << endl;

    if (almostMinDist < 2 * minDist)
    {
        Info << "Next nearest too close to nearest. Aborting" << endl;

        return -1;
    }
    else
    {
        return minI;
    }
}


// Find face on patch. Return mesh face label.
label findFace
(
    const primitiveMesh& mesh,
    const primitivePatch& pp,
    const point& nearPoint
)
{
    const pointField& points = pp.points();

    // Find nearest and next nearest
    scalar minDist = great;
    label minI = -1;

    scalar almostMinDist = great;
    label almostMinI = -1;

    forAll(pp, patchFacei)
    {
        pointHit pHit(pp[patchFacei].nearestPoint(nearPoint, points));

        if (pHit.hit())
        {
            if (pHit.distance() < minDist)
            {
                almostMinDist = minDist;
                almostMinI = minI;

                minDist = pHit.distance();
                minI = patchFacei + mesh.nInternalFaces();
            }
            else if (pHit.distance() < almostMinDist)
            {
                almostMinDist = pHit.distance();
                almostMinI = patchFacei + mesh.nInternalFaces();
            }
        }
    }

    if (minI == -1)
    {
        Info << "Did not find face close to point " << nearPoint << endl;

        return -1;
    }


    // Decide if nearPoint unique enough.
    Info << "Found to point " << nearPoint << nl
        << "    nearest face      : " << minI
        << " distance " << minDist
        << " to face centre " << mesh.faceCentres()[minI] << nl
        << "    next nearest face : " << almostMinI
        << " distance " << almostMinDist
        << " to face centre " << mesh.faceCentres()[almostMinI] << nl
        << endl;

    if (almostMinDist < 2 * minDist)
    {
        Info << "Next nearest too close to nearest. Aborting" << endl;

        return -1;
    }
    else
    {
        return minI;
    }
}


// Find cell with cell centre close to given point.
label findCell(const primitiveMesh& mesh, const point& nearPoint)
{
    label celli = mesh.findCell(nearPoint);

    if (celli != -1)
    {
        scalar distToCcSqr = magSqr(nearPoint - mesh.cellCentres()[celli]);

        const labelList& cPoints = mesh.cellPoints()[celli];

        label minI = -1;
        scalar minDistSqr = great;

        forAll(cPoints, i)
        {
            label pointi = cPoints[i];

            scalar distSqr = magSqr(nearPoint - mesh.points()[pointi]);

            if (distSqr < minDistSqr)
            {
                minDistSqr = distSqr;
                minI = pointi;
            }
        }

        // Decide if nearPoint unique enough.
        Info << "Found to point " << nearPoint << nl
            << "    nearest cell       : " << celli
            << " distance " << tnbLib::sqrt(distToCcSqr)
            << " to cell centre " << mesh.cellCentres()[celli] << nl
            << "    nearest mesh point : " << minI
            << " distance " << tnbLib::sqrt(minDistSqr)
            << " to " << mesh.points()[minI] << nl
            << endl;

        if (minDistSqr < 4 * distToCcSqr)
        {
            Info << "Mesh point too close to nearest cell centre. Aborting"
                << endl;

            celli = -1;
        }
    }

    return celli;
}




int main(int argc, char* argv[])
{
#include <addOverwriteOption.lxx>

#include <setRootCase.lxx>
#include <createTime.lxx>
    runTime.functionObjects().off();
#include <createPolyMesh.lxx>
    const word oldInstance = mesh.pointsInstance();

    const bool overwrite = args.optionFound("overwrite");

    Info << "Reading modifyMeshDict\n" << endl;

    IOdictionary dict
    (
        IOobject
        (
            "modifyMeshDict",
            runTime.system(),
            mesh,
            IOobject::MUST_READ_IF_MODIFIED,
            IOobject::NO_WRITE
        )
    );

    // Read all from the dictionary.
    List<Pair<point>> pointsToMove(dict.lookup("pointsToMove"));
    List<Pair<point>> edgesToSplit(dict.lookup("edgesToSplit"));
    List<Pair<point>> facesToTriangulate
    (
        dict.lookup("facesToTriangulate")
    );

    bool cutBoundary =
        (
            pointsToMove.size()
            || edgesToSplit.size()
            || facesToTriangulate.size()
            );

    List<Pair<point>> edgesToCollapse(dict.lookup("edgesToCollapse"));

    bool collapseEdge = edgesToCollapse.size();

    List<Pair<point>> cellsToPyramidise(dict.lookup("cellsToSplit"));

    bool cellsToSplit = cellsToPyramidise.size();

    // List<Tuple2<pointField,point>>
    //     cellsToCreate(dict.lookup("cellsToCreate"));

    Info << "Read from " << dict.name() << nl
        << "  Boundary cutting module:" << nl
        << "    points to move      :" << pointsToMove.size() << nl
        << "    edges to split      :" << edgesToSplit.size() << nl
        << "    faces to triangulate:" << facesToTriangulate.size() << nl
        << "  Cell splitting module:" << nl
        << "    cells to split      :" << cellsToPyramidise.size() << nl
        << "  Edge collapsing module:" << nl
        << "    edges to collapse   :" << edgesToCollapse.size() << nl
        //<< "    cells to create     :" << cellsToCreate.size() << nl
        << endl;

    if
        (
            (cutBoundary && collapseEdge)
            || (cutBoundary && cellsToSplit)
            || (collapseEdge && cellsToSplit)
            )
    {
        FatalErrorInFunction
            << "Used more than one mesh modifying module "
            << "(boundary cutting, cell splitting, edge collapsing)" << nl
            << "Please do them in separate passes." << exit(FatalError);
    }



    // Get calculating engine for all of outside
    const SubList<face> outsideFaces
    (
        mesh.faces(),
        mesh.nFaces() - mesh.nInternalFaces(),
        mesh.nInternalFaces()
    );

    primitivePatch allBoundary(outsideFaces, mesh.points());


    // Look up mesh labels and convert to input for boundaryCutter.

    bool validInputs = true;


    Info << nl << "Looking up points to move ..." << nl << endl;
    Map<point> pointToPos(pointsToMove.size());
    forAll(pointsToMove, i)
    {
        const Pair<point>& pts = pointsToMove[i];

        label pointi = findPoint(allBoundary, pts.first());

        if (pointi == -1 || !pointToPos.insert(pointi, pts.second()))
        {
            Info << "Could not insert mesh point " << pointi
                << " for input point " << pts.first() << nl
                << "Perhaps the point is already marked for moving?" << endl;
            validInputs = false;
        }
    }


    Info << nl << "Looking up edges to split ..." << nl << endl;
    Map<List<point>> edgeToCuts(edgesToSplit.size());
    forAll(edgesToSplit, i)
    {
        const Pair<point>& pts = edgesToSplit[i];

        label edgeI = findEdge(mesh, allBoundary, pts.first());

        if
            (
                edgeI == -1
                || !edgeToCuts.insert(edgeI, List<point>(1, pts.second()))
                )
        {
            Info << "Could not insert mesh edge " << edgeI
                << " for input point " << pts.first() << nl
                << "Perhaps the edge is already marked for cutting?" << endl;

            validInputs = false;
        }
    }


    Info << nl << "Looking up faces to triangulate ..." << nl << endl;
    Map<point> faceToDecompose(facesToTriangulate.size());
    forAll(facesToTriangulate, i)
    {
        const Pair<point>& pts = facesToTriangulate[i];

        label facei = findFace(mesh, allBoundary, pts.first());

        if (facei == -1 || !faceToDecompose.insert(facei, pts.second()))
        {
            Info << "Could not insert mesh face " << facei
                << " for input point " << pts.first() << nl
                << "Perhaps the face is already marked for splitting?" << endl;

            validInputs = false;
        }
    }



    Info << nl << "Looking up cells to convert to pyramids around"
        << " cell centre ..." << nl << endl;
    Map<point> cellToPyrCentre(cellsToPyramidise.size());
    forAll(cellsToPyramidise, i)
    {
        const Pair<point>& pts = cellsToPyramidise[i];

        label celli = findCell(mesh, pts.first());

        if (celli == -1 || !cellToPyrCentre.insert(celli, pts.second()))
        {
            Info << "Could not insert mesh cell " << celli
                << " for input point " << pts.first() << nl
                << "Perhaps the cell is already marked for splitting?" << endl;

            validInputs = false;
        }
    }


    Info << nl << "Looking up edges to collapse ..." << nl << endl;
    Map<point> edgeToPos(edgesToCollapse.size());
    forAll(edgesToCollapse, i)
    {
        const Pair<point>& pts = edgesToCollapse[i];

        label edgeI = findEdge(mesh, allBoundary, pts.first());

        if (edgeI == -1 || !edgeToPos.insert(edgeI, pts.second()))
        {
            Info << "Could not insert mesh edge " << edgeI
                << " for input point " << pts.first() << nl
                << "Perhaps the edge is already marked for collaping?" << endl;

            validInputs = false;
        }
    }



    if (!validInputs)
    {
        Info << nl << "There was a problem in one of the inputs in the"
            << " dictionary. Not modifying mesh." << endl;
    }
    else if (cellToPyrCentre.size())
    {
        Info << nl << "All input cells located. Modifying mesh." << endl;

        // Mesh change engine
        cellSplitter cutter(mesh);

        // Topo change container
        polyTopoChange meshMod(mesh);

        // Insert commands into meshMod
        cutter.setRefinement(cellToPyrCentre, meshMod);

        // Do changes
        autoPtr<mapPolyMesh> morphMap = meshMod.changeMesh(mesh, false);

        if (morphMap().hasMotionPoints())
        {
            mesh.movePoints(morphMap().preMotionPoints());
        }

        cutter.updateMesh(morphMap());

        if (!overwrite)
        {
            runTime++;
        }
        else
        {
            mesh.setInstance(oldInstance);
        }

        // Write resulting mesh
        Info << "Writing modified mesh to time " << runTime.timeName() << endl;
        mesh.write();
    }
    else if (edgeToPos.size())
    {
        Info << nl << "All input edges located. Modifying mesh." << endl;

        // Mesh change engine
        edgeCollapser cutter(mesh);

        const edgeList& edges = mesh.edges();
        const pointField& points = mesh.points();

        pointField newPoints(points);

        PackedBoolList collapseEdge(mesh.nEdges());
        Map<point> collapsePointToLocation(mesh.nPoints());

        // Get new positions and construct collapse network
        forAllConstIter(Map<point>, edgeToPos, iter)
        {
            label edgeI = iter.key();
            const edge& e = edges[edgeI];

            collapseEdge[edgeI] = true;
            collapsePointToLocation.set(e[1], points[e[0]]);

            newPoints[e[0]] = iter();
        }

        // Move master point to destination.
        mesh.movePoints(newPoints);

        List<pointEdgeCollapse> allPointInfo;
        const globalIndex globalPoints(mesh.nPoints());
        labelList pointPriority(mesh.nPoints(), 0);

        cutter.consistentCollapse
        (
            globalPoints,
            pointPriority,
            collapsePointToLocation,
            collapseEdge,
            allPointInfo
        );

        // Topo change container
        polyTopoChange meshMod(mesh);

        // Insert
        cutter.setRefinement(allPointInfo, meshMod);

        // Do changes
        autoPtr<mapPolyMesh> morphMap = meshMod.changeMesh(mesh, false);

        if (morphMap().hasMotionPoints())
        {
            mesh.movePoints(morphMap().preMotionPoints());
        }

        // Not implemented yet:
        // cutter.updateMesh(morphMap());


        if (!overwrite)
        {
            runTime++;
        }
        else
        {
            mesh.setInstance(oldInstance);
        }

        // Write resulting mesh
        Info << "Writing modified mesh to time " << runTime.timeName() << endl;
        mesh.write();
    }
    else
    {
        Info << nl << "All input points located. Modifying mesh." << endl;

        // Mesh change engine
        boundaryCutter cutter(mesh);

        // Topo change container
        polyTopoChange meshMod(mesh);

        // Insert commands into meshMod
        cutter.setRefinement
        (
            pointToPos,
            edgeToCuts,
            Map<labelPair>(0),  // Faces to split diagonally
            faceToDecompose,    // Faces to triangulate
            meshMod
        );

        // Do changes
        autoPtr<mapPolyMesh> morphMap = meshMod.changeMesh(mesh, false);

        if (morphMap().hasMotionPoints())
        {
            mesh.movePoints(morphMap().preMotionPoints());
        }

        cutter.updateMesh(morphMap());

        if (!overwrite)
        {
            runTime++;
        }
        else
        {
            mesh.setInstance(oldInstance);
        }

        // Write resulting mesh
        Info << "Writing modified mesh to time " << runTime.timeName() << endl;
        mesh.write();
    }


    Info << "\nEnd\n" << endl;
    return 0;
}


// ************************************************************************* //
