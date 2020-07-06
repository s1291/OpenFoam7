#include <masterCoarsestGAMGProcAgglomeration.hxx>

#include <addToRunTimeSelectionTable.hxx>
#include <GAMGAgglomeration.hxx>

#include <IOstreams.hxx> // added by amir

// * * * * * * * * * * * * * * Static Data Members * * * * * * * * * * * * * //

namespace tnbLib
{
	defineTypeNameAndDebug(masterCoarsestGAMGProcAgglomeration, 0);

	addToRunTimeSelectionTable
	(
		GAMGProcAgglomeration,
		masterCoarsestGAMGProcAgglomeration,
		GAMGAgglomeration
	);
}


// * * * * * * * * * * * * * Private Member Functions  * * * * * * * * * * * //


// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

tnbLib::masterCoarsestGAMGProcAgglomeration::masterCoarsestGAMGProcAgglomeration
(
	GAMGAgglomeration& agglom,
	const dictionary& controlDict
)
	:
	GAMGProcAgglomeration(agglom, controlDict)
{}


// * * * * * * * * * * * * * * * * Destructor  * * * * * * * * * * * * * * * //

tnbLib::masterCoarsestGAMGProcAgglomeration::
~masterCoarsestGAMGProcAgglomeration()
{
	forAllReverse(comms_, i)
	{
		if (comms_[i] != -1)
		{
			UPstream::freeCommunicator(comms_[i]);
		}
	}
}


// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

bool tnbLib::masterCoarsestGAMGProcAgglomeration::agglomerate()
{
	if (debug)
	{
		Pout << nl << "Starting mesh overview" << endl;
		printStats(Pout, agglom_);
	}

	if (agglom_.size() >= 1)
	{
		// Agglomerate one but last level (since also agglomerating
		// restrictAddressing)
		label fineLevelIndex = agglom_.size() - 1;

		if (agglom_.hasMeshLevel(fineLevelIndex))
		{
			// Get the fine mesh
			const lduMesh& levelMesh = agglom_.meshLevel(fineLevelIndex);
			label levelComm = levelMesh.comm();
			label nProcs = UPstream::nProcs(levelComm);

			if (nProcs > 1)
			{
				// Processor restriction map: per processor the coarse processor
				labelList procAgglomMap(nProcs, 0);

				// Master processor
				labelList masterProcs;
				// Local processors that agglomerate. agglomProcIDs[0] is in
				// masterProc.
				List<label> agglomProcIDs;
				GAMGAgglomeration::calculateRegionMaster
				(
					levelComm,
					procAgglomMap,
					masterProcs,
					agglomProcIDs
				);

				// Allocate a communicator for the processor-agglomerated matrix
				comms_.append
				(
					UPstream::allocateCommunicator
					(
						levelComm,
						masterProcs
					)
				);

				// Use procesor agglomeration maps to do the actual collecting.
				if (Pstream::myProcNo(levelComm) != -1)
				{
					GAMGProcAgglomeration::agglomerate
					(
						fineLevelIndex,
						procAgglomMap,
						masterProcs,
						agglomProcIDs,
						comms_.last()
					);
				}
			}
		}
	}

	// Print a bit
	if (debug)
	{
		Pout << nl << "Agglomerated mesh overview" << endl;
		printStats(Pout, agglom_);
	}

	return true;
}


// ************************************************************************* //