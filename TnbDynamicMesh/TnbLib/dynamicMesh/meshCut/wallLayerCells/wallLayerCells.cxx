#include <wallLayerCells.hxx>

#include <DynamicList.hxx>
#include <MeshWave.hxx>
#include <wallNormalInfo.hxx>
#include <OFstream.hxx>

// * * * * * * * * * * * * * * Static Data Members * * * * * * * * * * * * * //

namespace tnbLib
{
    defineTypeNameAndDebug(wallLayerCells, 0);
}


// * * * * * * * * * * * * * Private Member Functions  * * * * * * * * * * * //

bool tnbLib::wallLayerCells::usesCoupledPatch(const label celli) const
{
    const polyBoundaryMesh& patches = mesh().boundaryMesh();

    const cell& cFaces = mesh().cells()[celli];

    forAll(cFaces, cFacei)
    {
        label facei = cFaces[cFacei];

        label patchID = patches.whichPatch(facei);

        if ((patchID >= 0) && (patches[patchID].coupled()))
        {
            return true;
        }
    }
    return false;
}

// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

tnbLib::wallLayerCells::wallLayerCells
(
    const polyMesh& mesh,
    const List<word>& patchNames,
    const label nLayers
)
    :
    edgeVertex(mesh),
    List<refineCell>()
{
    // Find out cells connected to walls.

    const polyPatchList& patches = mesh.boundaryMesh();

    // Make map from name to local patch ID
    HashTable<label> patchNameToIndex(patches.size());

    forAll(patches, patchi)
    {
        patchNameToIndex.insert(patches[patchi].name(), patchi);
    }


    // Count size of walls to set
    label nWalls = 0;

    forAll(patchNames, patchNameI)
    {
        const word& name = patchNames[patchNameI];

        if (patchNameToIndex.found(name))
        {
            label patchi = patchNameToIndex[name];

            nWalls += patches[patchi].size();
        }
    }

    // Allocate storage for start of wave on faces
    List<wallNormalInfo> changedFacesInfo(nWalls);
    labelList changedFaces(nWalls);

    // Fill changedFaces info
    label nChangedFaces = 0;

    forAll(patchNames, patchNameI)
    {
        const word& name = patchNames[patchNameI];

        if (patchNameToIndex.found(name))
        {
            label patchi = patchNameToIndex[name];

            const polyPatch& pp = patches[patchi];

            forAll(pp, patchFacei)
            {
                label meshFacei = pp.start() + patchFacei;

                changedFaces[nChangedFaces] = meshFacei;

                // Set transported information to the wall normal.
                const vector& norm = pp.faceNormals()[patchFacei];

                changedFacesInfo[nChangedFaces] = wallNormalInfo(norm);

                nChangedFaces++;
            }
        }
    }


    // Do a wave of nLayers, transporting the index in patchNames
    // (cannot use local patchIDs since we might get info from neighbouring
    //  processor)

    MeshWave<wallNormalInfo> regionCalc
    (
        mesh,
        changedFaces,
        changedFacesInfo,
        0
    );

    regionCalc.iterate(nLayers);


    // Now regionCalc should hold info on faces that are reachable from
    // changedFaces within nLayers iterations. We use face info since that is
    // guaranteed to be consistent across processor boundaries.

    const List<wallNormalInfo>& faceInfo = regionCalc.allFaceInfo();

    if (debug)
    {
        InfoInFunction << "Dumping selected faces to selectedFaces.obj" << endl;

        OFstream fcStream("selectedFaces.obj");

        label vertI = 0;

        forAll(faceInfo, facei)
        {
            const wallNormalInfo& info = faceInfo[facei];

            if (info.valid(regionCalc.data()))
            {
                const face& f = mesh.faces()[facei];

                point mid(0.0, 0.0, 0.0);

                forAll(f, fp)
                {
                    mid += mesh.points()[f[fp]];
                }
                mid /= f.size();

                fcStream
                    << "v " << mid.x() << ' ' << mid.y() << ' ' << mid.z()
                    << endl;
                vertI++;

                point end(mid + info.normal());

                fcStream
                    << "v " << end.x() << ' ' << end.y() << ' ' << end.z()
                    << endl;
                vertI++;

                fcStream << "l " << vertI << ' ' << vertI - 1 << endl;
            }
        }
    }


    //
    // Copy meshWave information to List<refineCell>
    //

    // Estimate size

    DynamicList<refineCell> refineCells(3 * nWalls);

    const List<wallNormalInfo>& cellInfo = regionCalc.allCellInfo();

    forAll(cellInfo, celli)
    {
        const wallNormalInfo& info = cellInfo[celli];

        if (info.valid(regionCalc.data()) && !usesCoupledPatch(celli))
        {
            refineCells.append(refineCell(celli, info.normal()));
        }
    }

    // Transfer refineCells storage to this.
    transfer(refineCells);
}


// ************************************************************************* //