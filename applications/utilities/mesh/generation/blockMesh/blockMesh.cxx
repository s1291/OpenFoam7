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

Application
	blockMesh

Description
	A multi-block mesh generator.

	Uses the block mesh description found in
	  - \c system/blockMeshDict
	  - \c system/\<region\>/blockMeshDict
	  - \c constant/polyMesh/blockMeshDict
	  - \c constant/\<region\>/polyMesh/blockMeshDict

Usage
	\b blockMesh [OPTION]

	Options:
	  - \par -blockTopology
		Write the topology as a set of edges in OBJ format.

	  - \par -region \<name\>
		Specify an alternative mesh region.

	  - \par -dict \<filename\>
		Specify alternative dictionary for the block mesh description.

\*---------------------------------------------------------------------------*/

#include <Base_Module.hxx>
#include <DynamicMesh_Module.hxx>
#include <FiniteVolume_Module.hxx>
#include <FvMesh_Module.hxx>

#include <Time.hxx>
#include <IOdictionary.hxx>
#include <IOPtrList.hxx>

#include <blockMesh.hxx>
#include <attachPolyTopoChanger.hxx>
#include <polyTopoChange.hxx>
#include <emptyPolyPatch.hxx>
#include <cyclicPolyPatch.hxx>

#include <argList.hxx>
#include <OSspecific.hxx>
#include <OFstream.hxx>

#include <Pair.hxx>
#include <slidingInterface.hxx>

#include <negEntry.hxx>

using namespace tnbLib;

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

int main(int argc, char *argv[])
{
	argList::noParallel();
	argList::addBoolOption
	(
		"blockTopology",
		"write block edges and centres as .obj files"
	);
	argList::addBoolOption
	(
		"noClean",
		"keep the existing files in the polyMesh"
	);
	argList::addOption
	(
		"dict",
		"file",
		"specify alternative dictionary for the blockMesh description"
	);

	argList::addNote
	(
		"Block description\n"
		"\n"
		"  For a given block, the correspondence between the ordering of\n"
		"  vertex labels and face labels is shown below.\n"
		"  For vertex numbering in the sequence 0 to 7 (block, centre):\n"
		"    faces 0 (f0) and 1 are left and right, respectively;\n"
		"    faces 2 and 3 are front and back; \n"
		"    and faces 4 and 5 are bottom and top::\n"
		"\n"
		"                 7 ---- 6\n"
		"            f5   |\\     |\\   f3\n"
		"            |    | 4 ---- 5   \\\n"
		"            |    3 |--- 2 |    \\\n"
		"            |     \\|     \\|    f2\n"
		"            f4     0 ---- 1\n"
		"\n"
		"       Z         f0 ----- f1\n"
		"       |  Y\n"
		"       | /\n"
		"       O --- X\n"
	);

#include <addRegionOption.lxx>
#include <setRootCase.lxx>
#include <createTime.lxx>

	const word dictName("blockMeshDict");

	word regionName;
	word regionPath;

	// Check if the region is specified otherwise mesh the default region
	if (args.optionReadIfPresent("region", regionName, polyMesh::defaultRegion))
	{
		Info << nl << "Generating mesh for region " << regionName << endl;
		regionPath = regionName;
	}

	// Search for the appropriate blockMesh dictionary....

	fileName dictPath;
	// Check if the dictionary is specified on the command-line
	if (args.optionFound("dict"))
	{
		dictPath = args["dict"];

		dictPath =
			(
				isDir(dictPath)
				? dictPath / dictName
				: dictPath
				);
	}
	// Check if dictionary is present in the constant directory
	else if
		(
			exists
			(
				runTime.path() / runTime.constant()
				/ regionPath / polyMesh::meshSubDir / dictName
			)
			)
	{
		dictPath =
			runTime.constant()
			/ regionPath / polyMesh::meshSubDir / dictName;
	}
	// Otherwise assume the dictionary is present in the system directory
	else
	{
		dictPath = runTime.system() / regionPath / dictName;
	}

	if (!args.optionFound("noClean"))
	{
		fileName polyMeshPath
		(
			runTime.path() / runTime.constant() / regionPath / polyMesh::meshSubDir
		);

		if (exists(polyMeshPath))
		{
			if (exists(polyMeshPath / dictName))
			{
				Info << "Not deleting polyMesh directory " << nl
					<< "    " << polyMeshPath << nl
					<< "    because it contains " << dictName << endl;
			}
			else
			{
				Info << "Deleting polyMesh directory" << nl
					<< "    " << polyMeshPath << endl;
				rmDir(polyMeshPath);
			}
		}
	}

	IOobject meshDictIO
	(
		dictPath,
		runTime,
		IOobject::MUST_READ,
		IOobject::NO_WRITE,
		false
	);

	if (!meshDictIO.typeHeaderOk<IOdictionary>(true))
	{
		FatalErrorInFunction
			<< meshDictIO.objectPath()
			<< nl
			<< exit(FatalError);
	}

	Info << "Creating block mesh from\n    "
		<< meshDictIO.objectPath() << endl;

	IOdictionary meshDict(meshDictIO);

	auto keys = meshDict.keys();
	/*for (int i = 0; i < keys.size(); i++)
	{
		std::cout << keys[i] << std::endl;
	}*/

	blockMesh blocks(meshDict, regionName);

	if (args.optionFound("blockTopology"))
	{
		// Write mesh as edges.
		{
			fileName objMeshFile("blockTopology.obj");

			OFstream str(runTime.path() / objMeshFile);

			Info << nl << "Dumping block structure as Lightwave obj format"
				<< " to " << objMeshFile << endl;

			blocks.writeTopology(str);
		}

		// Write centres of blocks
		{
			fileName objCcFile("blockCentres.obj");

			OFstream str(runTime.path() / objCcFile);

			Info << nl << "Dumping block centres as Lightwave obj format"
				<< " to " << objCcFile << endl;

			const polyMesh& topo = blocks.topology();

			const pointField& cellCentres = topo.cellCentres();

			forAll(cellCentres, celli)
			{
				// point cc = b.blockShape().centre(b.points());
				const point& cc = cellCentres[celli];

				str << "v " << cc.x() << ' ' << cc.y() << ' ' << cc.z() << nl;
			}
		}

		Info << nl << "end" << endl;

		return 0;
	}


	Info << nl << "Creating polyMesh from blockMesh" << endl;

	word defaultFacesName = "defaultFaces";
	word defaultFacesType = emptyPolyPatch::typeName;
	polyMesh mesh
	(
		IOobject
		(
			regionName,
			runTime.constant(),
			runTime
		),
		clone(blocks.points()),           // could we re-use space?
		blocks.cells(),
		blocks.patches(),
		blocks.patchNames(),
		blocks.patchDicts(),
		defaultFacesName,
		defaultFacesType
	);


	// Read in a list of dictionaries for the merge patch pairs
	if (meshDict.found("mergePatchPairs"))
	{
		List<Pair<word>> mergePatchPairs
		(
			meshDict.lookup("mergePatchPairs")
		);

#include "mergePatchPairs.lxx"
	}
	else
	{
		Info << nl << "There are no merge patch pairs edges" << endl;
	}


	// Set any cellZones (note: cell labelling unaffected by above
	// mergePatchPairs)

	label nZones = blocks.numZonedBlocks();

	if (nZones > 0)
	{
		Info << nl << "Adding cell zones" << endl;

		// Map from zoneName to cellZone index
		HashTable<label> zoneMap(nZones);

		// Cells per zone.
		List<DynamicList<label>> zoneCells(nZones);

		// Running cell counter
		label celli = 0;

		// Largest zone so far
		label freeZoneI = 0;

		forAll(blocks, blockI)
		{
			const block& b = blocks[blockI];
			const List<FixedList<label, 8>> blockCells = b.cells();
			const word& zoneName = b.zoneName();

			if (zoneName.size())
			{
				HashTable<label>::const_iterator iter = zoneMap.find(zoneName);

				label zoneI;

				if (iter == zoneMap.end())
				{
					zoneI = freeZoneI++;

					Info << "    " << zoneI << '\t' << zoneName << endl;

					zoneMap.insert(zoneName, zoneI);
				}
				else
				{
					zoneI = iter();
				}

				forAll(blockCells, i)
				{
					zoneCells[zoneI].append(celli++);
				}
			}
			else
			{
				celli += b.cells().size();
			}
		}


		List<cellZone*> cz(zoneMap.size());

		forAllConstIter(HashTable<label>, zoneMap, iter)
		{
			label zoneI = iter();

			cz[zoneI] = new cellZone
			(
				iter.key(),
				zoneCells[zoneI].shrink(),
				zoneI,
				mesh.cellZones()
			);
		}

		mesh.pointZones().setSize(0);
		mesh.faceZones().setSize(0);
		mesh.cellZones().setSize(0);
		mesh.addZones(List<pointZone*>(0), List<faceZone*>(0), cz);
	}


	// Detect any cyclic patches and force re-ordering of the faces
	{
		const polyPatchList& patches = mesh.boundaryMesh();
		bool hasCyclic = false;
		forAll(patches, patchi)
		{
			if (isA<cyclicPolyPatch>(patches[patchi]))
			{
				hasCyclic = true;
				break;
			}
		}

		if (hasCyclic)
		{
			Info << nl << "Detected cyclic patches; ordering boundary faces"
				<< endl;
			const word oldInstance = mesh.instance();
			polyTopoChange meshMod(mesh);
			meshMod.changeMesh(mesh, false);
			mesh.setInstance(oldInstance);
		}
	}


	// Set the precision of the points data to 10
	IOstream::defaultPrecision(max(10u, IOstream::defaultPrecision()));

	Info << nl << "Writing polyMesh" << endl;
	mesh.removeFiles();
	if (!mesh.write())
	{
		FatalErrorInFunction
			<< "Failed writing polyMesh."
			<< exit(FatalError);
	}


	// Write summary
	{
		const polyPatchList& patches = mesh.boundaryMesh();

		Info << "----------------" << nl
			<< "Mesh Information" << nl
			<< "----------------" << nl
			<< "  " << "boundingBox: " << boundBox(mesh.points()) << nl
			<< "  " << "nPoints: " << mesh.nPoints() << nl
			<< "  " << "nCells: " << mesh.nCells() << nl
			<< "  " << "nFaces: " << mesh.nFaces() << nl
			<< "  " << "nInternalFaces: " << mesh.nInternalFaces() << nl;

		Info << "----------------" << nl
			<< "Patches" << nl
			<< "----------------" << nl;

		forAll(patches, patchi)
		{
			const polyPatch& p = patches[patchi];

			Info << "  " << "patch " << patchi
				<< " (start: " << p.start()
				<< " size: " << p.size()
				<< ") name: " << p.name()
				<< nl;
		}
	}

	Info << "\nEnd\n" << endl;
	return 0;
}


// ************************************************************************* //