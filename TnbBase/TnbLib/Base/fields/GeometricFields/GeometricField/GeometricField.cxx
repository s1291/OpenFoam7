#include <GeometricField.hxx>

#include <Time.hxx>
#include <demandDrivenData.hxx>
#include <dictionary.hxx>
#include <localIOdictionary.hxx>
#include <data.hxx>

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#define checkField(gf1, gf2, op)                                    \
if ((gf1).mesh() != (gf2).mesh())                                   \
{                                                                   \
    FatalErrorInFunction                                            \
        << "different mesh for fields "                             \
        << (gf1).name() << " and " << (gf2).name()                  \
        << " during operatrion " <<  op                             \
        << abort(FatalError);                                       \
}


// * * * * * * * * * * * * * Private Member Functions * * * * * * * * * * * * //

template<class Type, template<class> class PatchField, class GeoMesh>
void tnbLib::GeometricField<Type, PatchField, GeoMesh>::readFields
(
	const dictionary& dict
)
{
	Internal::readField(dict, "internalField");

	boundaryField_.readField(*this, dict.subDict("boundaryField"));

	if (dict.found("referenceLevel"))
	{
		Type fieldAverage(pTraits<Type>(dict.lookup("referenceLevel")));

		Field<Type>::operator+=(fieldAverage);

		forAll(boundaryField_, patchi)
		{
			boundaryField_[patchi] == boundaryField_[patchi] + fieldAverage;
		}
	}
}


template<class Type, template<class> class PatchField, class GeoMesh>
void tnbLib::GeometricField<Type, PatchField, GeoMesh>::readFields()
{
	const localIOdictionary dict
	(
		IOobject
		(
			this->name(),
			this->instance(),
			this->local(),
			this->db(),
			IOobject::MUST_READ,
			IOobject::NO_WRITE,
			false
		),
		typeName
	);

	this->close();

	readFields(dict);
}


template<class Type, template<class> class PatchField, class GeoMesh>
bool tnbLib::GeometricField<Type, PatchField, GeoMesh>::readIfPresent()
{
	if
		(
			this->readOpt() == IOobject::MUST_READ
			|| this->readOpt() == IOobject::MUST_READ_IF_MODIFIED
			)
	{
		WarningInFunction
			<< "read option IOobject::MUST_READ or MUST_READ_IF_MODIFIED"
			<< " suggests that a read constructor for field " << this->name()
			<< " would be more appropriate." << endl;
	}
	else if
		(
			this->readOpt() == IOobject::READ_IF_PRESENT
			&& this->template typeHeaderOk<GeometricField<Type, PatchField, GeoMesh>>
			(
				true
				)
			)
	{
		readFields();

		// Check compatibility between field and mesh
		if (this->size() != GeoMesh::size(this->mesh()))
		{
			FatalIOErrorInFunction(this->readStream(typeName))
				<< "   number of field elements = " << this->size()
				<< " number of mesh elements = "
				<< GeoMesh::size(this->mesh())
				<< exit(FatalIOError);
		}

		readOldTimeIfPresent();

		return true;
	}

	return false;
}


template<class Type, template<class> class PatchField, class GeoMesh>
bool tnbLib::GeometricField<Type, PatchField, GeoMesh>::readOldTimeIfPresent()
{
	// Read the old time field if present
	IOobject field0
	(
		this->name() + "_0",
		this->time().timeName(),
		this->db(),
		IOobject::READ_IF_PRESENT,
		IOobject::AUTO_WRITE,
		this->registerObject()
	);

	if
		(
			field0.template typeHeaderOk<GeometricField<Type, PatchField, GeoMesh>>
			(
				true
				)
			)
	{
		if (debug)
		{
			InfoInFunction << "Reading old time level for field"
				<< endl << this->info() << endl;
		}

		field0Ptr_ = new GeometricField<Type, PatchField, GeoMesh>
			(
				field0,
				this->mesh()
				);

		field0Ptr_->timeIndex_ = timeIndex_ - 1;

		if (!field0Ptr_->readOldTimeIfPresent())
		{
			field0Ptr_->oldTime();
		}

		return true;
	}

	return false;
}


// * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * * //

template<class Type, template<class> class PatchField, class GeoMesh>
tnbLib::GeometricField<Type, PatchField, GeoMesh>::GeometricField
(
	const IOobject& io,
	const Mesh& mesh,
	const dimensionSet& ds,
	const word& patchFieldType
)
	:
	Internal(io, mesh, ds, false),
	timeIndex_(this->time().timeIndex()),
	field0Ptr_(nullptr),
	fieldPrevIterPtr_(nullptr),
	boundaryField_(mesh.boundary(), *this, patchFieldType)
{
	if (debug)
	{
		InfoInFunction << "Creating temporary" << endl << this->info() << endl;
	}

	readIfPresent();
}


template<class Type, template<class> class PatchField, class GeoMesh>
tnbLib::GeometricField<Type, PatchField, GeoMesh>::GeometricField
(
	const IOobject& io,
	const Mesh& mesh,
	const dimensionSet& ds,
	const wordList& patchFieldTypes,
	const wordList& actualPatchTypes
)
	:
	Internal(io, mesh, ds, false),
	timeIndex_(this->time().timeIndex()),
	field0Ptr_(nullptr),
	fieldPrevIterPtr_(nullptr),
	boundaryField_(mesh.boundary(), *this, patchFieldTypes, actualPatchTypes)
{
	if (debug)
	{
		InfoInFunction << "Creating temporary" << endl << this->info() << endl;
	}

	readIfPresent();
}


template<class Type, template<class> class PatchField, class GeoMesh>
tnbLib::GeometricField<Type, PatchField, GeoMesh>::GeometricField
(
	const IOobject& io,
	const Mesh& mesh,
	const dimensioned<Type>& dt,
	const word& patchFieldType
)
	:
	Internal(io, mesh, dt, false),
	timeIndex_(this->time().timeIndex()),
	field0Ptr_(nullptr),
	fieldPrevIterPtr_(nullptr),
	boundaryField_(mesh.boundary(), *this, patchFieldType)
{
	if (debug)
	{
		InfoInFunction << "Creating temporary" << endl << this->info() << endl;
	}

	boundaryField_ == dt.value();

	readIfPresent();
}


template<class Type, template<class> class PatchField, class GeoMesh>
tnbLib::GeometricField<Type, PatchField, GeoMesh>::GeometricField
(
	const IOobject& io,
	const Mesh& mesh,
	const dimensioned<Type>& dt,
	const wordList& patchFieldTypes,
	const wordList& actualPatchTypes
)
	:
	Internal(io, mesh, dt, false),
	timeIndex_(this->time().timeIndex()),
	field0Ptr_(nullptr),
	fieldPrevIterPtr_(nullptr),
	boundaryField_(mesh.boundary(), *this, patchFieldTypes, actualPatchTypes)
{
	if (debug)
	{
		InfoInFunction << "Creating temporary" << endl << this->info() << endl;
	}

	boundaryField_ == dt.value();

	readIfPresent();
}


template<class Type, template<class> class PatchField, class GeoMesh>
tnbLib::GeometricField<Type, PatchField, GeoMesh>::GeometricField
(
	const IOobject& io,
	const Internal& diField,
	const PtrList<PatchField<Type>>& ptfl
)
	:
	Internal(io, diField),
	timeIndex_(this->time().timeIndex()),
	field0Ptr_(nullptr),
	fieldPrevIterPtr_(nullptr),
	boundaryField_(this->mesh().boundary(), *this, ptfl)
{
	if (debug)
	{
		InfoInFunction
			<< "Constructing from components" << endl << this->info() << endl;
	}

	readIfPresent();
}


template<class Type, template<class> class PatchField, class GeoMesh>
tnbLib::GeometricField<Type, PatchField, GeoMesh>::GeometricField
(
	const IOobject& io,
	const Mesh& mesh,
	const dimensionSet& ds,
	const Field<Type>& iField,
	const PtrList<PatchField<Type>>& ptfl
)
	:
	Internal(io, mesh, ds, iField),
	timeIndex_(this->time().timeIndex()),
	field0Ptr_(nullptr),
	fieldPrevIterPtr_(nullptr),
	boundaryField_(mesh.boundary(), *this, ptfl)
{
	if (debug)
	{
		InfoInFunction
			<< "Constructing from components" << endl << this->info() << endl;
	}

	readIfPresent();
}


template<class Type, template<class> class PatchField, class GeoMesh>
tnbLib::GeometricField<Type, PatchField, GeoMesh>::GeometricField
(
	const IOobject& io,
	const Mesh& mesh
)
	:
	Internal(io, mesh, dimless, false),
	timeIndex_(this->time().timeIndex()),
	field0Ptr_(nullptr),
	fieldPrevIterPtr_(nullptr),
	boundaryField_(mesh.boundary())
{
	readFields();

	// Check compatibility between field and mesh

	if (this->size() != GeoMesh::size(this->mesh()))
	{
		FatalIOErrorInFunction(this->readStream(typeName))
			<< "   number of field elements = " << this->size()
			<< " number of mesh elements = " << GeoMesh::size(this->mesh())
			<< exit(FatalIOError);
	}

	readOldTimeIfPresent();

	if (debug)
	{
		InfoInFunction
			<< "Finishing read-construction of" << endl << this->info() << endl;
	}
}


template<class Type, template<class> class PatchField, class GeoMesh>
tnbLib::GeometricField<Type, PatchField, GeoMesh>::GeometricField
(
	const IOobject& io,
	const Mesh& mesh,
	const dictionary& dict
)
	:
	Internal(io, mesh, dimless, false),
	timeIndex_(this->time().timeIndex()),
	field0Ptr_(nullptr),
	fieldPrevIterPtr_(nullptr),
	boundaryField_(mesh.boundary())
{
	readFields(dict);

	// Check compatibility between field and mesh

	if (this->size() != GeoMesh::size(this->mesh()))
	{
		FatalErrorInFunction
			<< "   number of field elements = " << this->size()
			<< " number of mesh elements = " << GeoMesh::size(this->mesh())
			<< exit(FatalIOError);
	}

	if (debug)
	{
		InfoInFunction
			<< "Finishing dictionary-construct of "
			<< endl << this->info() << endl;
	}
}


template<class Type, template<class> class PatchField, class GeoMesh>
tnbLib::GeometricField<Type, PatchField, GeoMesh>::GeometricField
(
	const GeometricField<Type, PatchField, GeoMesh>& gf
)
	:
	Internal(gf),
	timeIndex_(gf.timeIndex()),
	field0Ptr_(nullptr),
	fieldPrevIterPtr_(nullptr),
	boundaryField_(*this, gf.boundaryField_)
{
	if (debug)
	{
		InfoInFunction
			<< "Constructing as copy" << endl << this->info() << endl;
	}

	if (gf.field0Ptr_)
	{
		field0Ptr_ = new GeometricField<Type, PatchField, GeoMesh>
			(
				*gf.field0Ptr_
				);
	}

	this->writeOpt() = IOobject::NO_WRITE;
}


template<class Type, template<class> class PatchField, class GeoMesh>
tnbLib::GeometricField<Type, PatchField, GeoMesh>::GeometricField
(
	GeometricField<Type, PatchField, GeoMesh>&& gf
)
	:
	Internal(move(gf)),
	timeIndex_(gf.timeIndex()),
	field0Ptr_(nullptr),
	fieldPrevIterPtr_(nullptr),
	boundaryField_(move(gf.boundaryField_))
{
	if (debug)
	{
		InfoInFunction
			<< "Constructing by moving" << endl << this->info() << endl;
	}

	if (gf.field0Ptr_)
	{
		field0Ptr_ = gf.field0Ptr_;
		gf.field0Ptr_ = nullptr;
	}

	this->writeOpt() = IOobject::NO_WRITE;
}


template<class Type, template<class> class PatchField, class GeoMesh>
tnbLib::GeometricField<Type, PatchField, GeoMesh>::GeometricField
(
	const tmp<GeometricField<Type, PatchField, GeoMesh>>& tgf
)
	:
	Internal
	(
		const_cast<GeometricField<Type, PatchField, GeoMesh>&>(tgf()),
		tgf.isTmp()
	),
	timeIndex_(tgf().timeIndex()),
	field0Ptr_(nullptr),
	fieldPrevIterPtr_(nullptr),
	boundaryField_(*this, tgf().boundaryField_)
{
	if (debug)
	{
		InfoInFunction
			<< "Constructing from tmp" << endl << this->info() << endl;
	}

	this->writeOpt() = IOobject::NO_WRITE;

	tgf.clear();
}


template<class Type, template<class> class PatchField, class GeoMesh>
tnbLib::GeometricField<Type, PatchField, GeoMesh>::GeometricField
(
	const IOobject& io,
	const GeometricField<Type, PatchField, GeoMesh>& gf
)
	:
	Internal(io, gf),
	timeIndex_(gf.timeIndex()),
	field0Ptr_(nullptr),
	fieldPrevIterPtr_(nullptr),
	boundaryField_(*this, gf.boundaryField_)
{
	if (debug)
	{
		InfoInFunction
			<< "Constructing as copy resetting IO params"
			<< endl << this->info() << endl;
	}

	if (!readIfPresent() && gf.field0Ptr_)
	{
		field0Ptr_ = new GeometricField<Type, PatchField, GeoMesh>
			(
				io.name() + "_0",
				*gf.field0Ptr_
				);
	}
}


template<class Type, template<class> class PatchField, class GeoMesh>
tnbLib::GeometricField<Type, PatchField, GeoMesh>::GeometricField
(
	const IOobject& io,
	const tmp<GeometricField<Type, PatchField, GeoMesh>>& tgf
)
	:
	Internal
	(
		io,
		const_cast<GeometricField<Type, PatchField, GeoMesh>&>(tgf()),
		tgf.isTmp()
	),
	timeIndex_(tgf().timeIndex()),
	field0Ptr_(nullptr),
	fieldPrevIterPtr_(nullptr),
	boundaryField_(*this, tgf().boundaryField_)
{
	if (debug)
	{
		InfoInFunction
			<< "Constructing from tmp resetting IO params"
			<< endl << this->info() << endl;
	}

	tgf.clear();

	readIfPresent();
}


template<class Type, template<class> class PatchField, class GeoMesh>
tnbLib::GeometricField<Type, PatchField, GeoMesh>::GeometricField
(
	const word& newName,
	const GeometricField<Type, PatchField, GeoMesh>& gf
)
	:
	Internal(newName, gf),
	timeIndex_(gf.timeIndex()),
	field0Ptr_(nullptr),
	fieldPrevIterPtr_(nullptr),
	boundaryField_(*this, gf.boundaryField_)
{
	if (debug)
	{
		InfoInFunction
			<< "Constructing as copy resetting name"
			<< endl << this->info() << endl;
	}

	if (!readIfPresent() && gf.field0Ptr_)
	{
		field0Ptr_ = new GeometricField<Type, PatchField, GeoMesh>
			(
				newName + "_0",
				*gf.field0Ptr_
				);
	}
}


template<class Type, template<class> class PatchField, class GeoMesh>
tnbLib::GeometricField<Type, PatchField, GeoMesh>::GeometricField
(
	const word& newName,
	const tmp<GeometricField<Type, PatchField, GeoMesh>>& tgf
)
	:
	Internal
	(
		newName,
		const_cast<GeometricField<Type, PatchField, GeoMesh>&>(tgf()),
		tgf.isTmp()
	),
	timeIndex_(tgf().timeIndex()),
	field0Ptr_(nullptr),
	fieldPrevIterPtr_(nullptr),
	boundaryField_(*this, tgf().boundaryField_)
{
	if (debug)
	{
		InfoInFunction
			<< "Constructing from tmp resetting name"
			<< endl << this->info() << endl;
	}

	tgf.clear();
}


template<class Type, template<class> class PatchField, class GeoMesh>
tnbLib::GeometricField<Type, PatchField, GeoMesh>::GeometricField
(
	const IOobject& io,
	const GeometricField<Type, PatchField, GeoMesh>& gf,
	const word& patchFieldType
)
	:
	Internal(io, gf),
	timeIndex_(gf.timeIndex()),
	field0Ptr_(nullptr),
	fieldPrevIterPtr_(nullptr),
	boundaryField_(this->mesh().boundary(), *this, patchFieldType)
{
	if (debug)
	{
		InfoInFunction
			<< "Constructing as copy resetting IO params"
			<< endl << this->info() << endl;
	}

	boundaryField_ == gf.boundaryField_;

	if (!readIfPresent() && gf.field0Ptr_)
	{
		field0Ptr_ = new GeometricField<Type, PatchField, GeoMesh>
			(
				io.name() + "_0",
				*gf.field0Ptr_
				);
	}
}


template<class Type, template<class> class PatchField, class GeoMesh>
tnbLib::GeometricField<Type, PatchField, GeoMesh>::GeometricField
(
	const IOobject& io,
	const GeometricField<Type, PatchField, GeoMesh>& gf,
	const wordList& patchFieldTypes,
	const wordList& actualPatchTypes

)
	:
	Internal(io, gf),
	timeIndex_(gf.timeIndex()),
	field0Ptr_(nullptr),
	fieldPrevIterPtr_(nullptr),
	boundaryField_
	(
		this->mesh().boundary(),
		*this,
		patchFieldTypes,
		actualPatchTypes
	)
{
	if (debug)
	{
		InfoInFunction
			<< "Constructing as copy resetting IO params and patch types"
			<< endl << this->info() << endl;
	}

	boundaryField_ == gf.boundaryField_;

	if (!readIfPresent() && gf.field0Ptr_)
	{
		field0Ptr_ = new GeometricField<Type, PatchField, GeoMesh>
			(
				io.name() + "_0",
				*gf.field0Ptr_
				);
	}
}


template<class Type, template<class> class PatchField, class GeoMesh>
tnbLib::GeometricField<Type, PatchField, GeoMesh>::GeometricField
(
	const IOobject& io,
	const tmp<GeometricField<Type, PatchField, GeoMesh>>& tgf,
	const wordList& patchFieldTypes,
	const wordList& actualPatchTypes
)
	:
	Internal
	(
		io,
		const_cast<GeometricField<Type, PatchField, GeoMesh>&>(tgf()),
		tgf.isTmp()
	),
	timeIndex_(tgf().timeIndex()),
	field0Ptr_(nullptr),
	fieldPrevIterPtr_(nullptr),
	boundaryField_
	(
		this->mesh().boundary(),
		*this,
		patchFieldTypes,
		actualPatchTypes
	)
{
	if (debug)
	{
		InfoInFunction
			<< "Constructing from tmp resetting IO params and patch types"
			<< endl << this->info() << endl;
	}

	boundaryField_ == tgf().boundaryField_;

	tgf.clear();
}


template<class Type, template<class> class PatchField, class GeoMesh>
tnbLib::tmp<tnbLib::GeometricField<Type, PatchField, GeoMesh>>
tnbLib::GeometricField<Type, PatchField, GeoMesh>::clone() const
{
	return tmp<GeometricField<Type, PatchField, GeoMesh>>
		(
			new GeometricField<Type, PatchField, GeoMesh>(*this)
			);
}


template<class Type, template<class> class PatchField, class GeoMesh>
tnbLib::tmp<tnbLib::GeometricField<Type, PatchField, GeoMesh>>
tnbLib::GeometricField<Type, PatchField, GeoMesh>::New
(
	const word& name,
	const Mesh& mesh,
	const dimensionSet& ds,
	const word& patchFieldType
)
{
	return tmp<GeometricField<Type, PatchField, GeoMesh>>
		(
			new GeometricField<Type, PatchField, GeoMesh>
			(
				IOobject
				(
					name,
					mesh.thisDb().time().timeName(),
					mesh.thisDb(),
					IOobject::NO_READ,
					IOobject::NO_WRITE,
					false
				),
				mesh,
				ds,
				patchFieldType
				)
			);
}


template<class Type, template<class> class PatchField, class GeoMesh>
tnbLib::tmp<tnbLib::GeometricField<Type, PatchField, GeoMesh>>
tnbLib::GeometricField<Type, PatchField, GeoMesh>::New
(
	const word& name,
	const Mesh& mesh,
	const dimensioned<Type>& dt,
	const word& patchFieldType
)
{
	return tmp<GeometricField<Type, PatchField, GeoMesh>>
		(
			new GeometricField<Type, PatchField, GeoMesh>
			(
				IOobject
				(
					name,
					mesh.thisDb().time().timeName(),
					mesh.thisDb(),
					IOobject::NO_READ,
					IOobject::NO_WRITE,
					false
				),
				mesh,
				dt,
				patchFieldType
				)
			);
}



template<class Type, template<class> class PatchField, class GeoMesh>
tnbLib::tmp<tnbLib::GeometricField<Type, PatchField, GeoMesh>>
tnbLib::GeometricField<Type, PatchField, GeoMesh>::New
(
	const word& name,
	const Mesh& mesh,
	const dimensioned<Type>& dt,
	const wordList& patchFieldTypes,
	const wordList& actualPatchTypes
)
{
	return tmp<GeometricField<Type, PatchField, GeoMesh>>
		(
			new GeometricField<Type, PatchField, GeoMesh>
			(
				IOobject
				(
					name,
					mesh.thisDb().time().timeName(),
					mesh.thisDb(),
					IOobject::NO_READ,
					IOobject::NO_WRITE,
					false
				),
				mesh,
				dt,
				patchFieldTypes,
				actualPatchTypes
				)
			);
}


template<class Type, template<class> class PatchField, class GeoMesh>
tnbLib::tmp<tnbLib::GeometricField<Type, PatchField, GeoMesh>>
tnbLib::GeometricField<Type, PatchField, GeoMesh>::New
(
	const word& newName,
	const tmp<GeometricField<Type, PatchField, GeoMesh>>& tgf
)
{
	return tmp<GeometricField<Type, PatchField, GeoMesh>>
		(
			new GeometricField<Type, PatchField, GeoMesh>
			(
				IOobject
				(
					newName,
					tgf().instance(),
					tgf().local(),
					tgf().db(),
					IOobject::NO_READ,
					IOobject::NO_WRITE,
					false
				),
				tgf
				)
			);
}


template<class Type, template<class> class PatchField, class GeoMesh>
tnbLib::tmp<tnbLib::GeometricField<Type, PatchField, GeoMesh>>
tnbLib::GeometricField<Type, PatchField, GeoMesh>::New
(
	const word& newName,
	const tmp<GeometricField<Type, PatchField, GeoMesh>>& tgf,
	const wordList& patchFieldTypes,
	const wordList& actualPatchTypes
)
{
	return tmp<GeometricField<Type, PatchField, GeoMesh>>
		(
			new GeometricField<Type, PatchField, GeoMesh>
			(
				IOobject
				(
					newName,
					tgf().instance(),
					tgf().local(),
					tgf().db(),
					IOobject::NO_READ,
					IOobject::NO_WRITE,
					false
				),
				tgf,
				patchFieldTypes,
				actualPatchTypes
				)
			);
}


// * * * * * * * * * * * * * * * Destructor * * * * * * * * * * * * * * * * * //

template<class Type, template<class> class PatchField, class GeoMesh>
tnbLib::GeometricField<Type, PatchField, GeoMesh>::~GeometricField()
{
	deleteDemandDrivenData(field0Ptr_);
	deleteDemandDrivenData(fieldPrevIterPtr_);
}


// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

template<class Type, template<class> class PatchField, class GeoMesh>
typename
tnbLib::GeometricField<Type, PatchField, GeoMesh>::Internal&
tnbLib::GeometricField<Type, PatchField, GeoMesh>::ref()
{
	this->setUpToDate();
	storeOldTimes();
	return *this;
}


template<class Type, template<class> class PatchField, class GeoMesh>
typename
tnbLib::GeometricField<Type, PatchField, GeoMesh>::Internal::FieldType&
tnbLib::GeometricField<Type, PatchField, GeoMesh>::primitiveFieldRef()
{
	this->setUpToDate();
	storeOldTimes();
	return *this;
}


template<class Type, template<class> class PatchField, class GeoMesh>
typename
tnbLib::GeometricField<Type, PatchField, GeoMesh>::Boundary&
tnbLib::GeometricField<Type, PatchField, GeoMesh>::boundaryFieldRef()
{
	this->setUpToDate();
	storeOldTimes();
	return boundaryField_;
}


template<class Type, template<class> class PatchField, class GeoMesh>
void tnbLib::GeometricField<Type, PatchField, GeoMesh>::storeOldTimes() const
{
	if
		(
			field0Ptr_
			&& timeIndex_ != this->time().timeIndex()
			&& !(
				this->name().size() > 2
				&& this->name()(this->name().size() - 2, 2) == "_0"
				)
			)
	{
		storeOldTime();
	}

	// Correct time index
	timeIndex_ = this->time().timeIndex();
}


template<class Type, template<class> class PatchField, class GeoMesh>
void tnbLib::GeometricField<Type, PatchField, GeoMesh>::storeOldTime() const
{
	if (field0Ptr_)
	{
		field0Ptr_->storeOldTime();

		if (debug)
		{
			InfoInFunction
				<< "Storing old time field for field" << endl
				<< this->info() << endl;
		}

		*field0Ptr_ == *this;
		field0Ptr_->timeIndex_ = timeIndex_;

		if (field0Ptr_->field0Ptr_)
		{
			field0Ptr_->writeOpt() = this->writeOpt();
		}
	}
}


template<class Type, template<class> class PatchField, class GeoMesh>
tnbLib::label tnbLib::GeometricField<Type, PatchField, GeoMesh>::nOldTimes() const
{
	if (field0Ptr_)
	{
		return field0Ptr_->nOldTimes() + 1;
	}
	else
	{
		return 0;
	}
}


template<class Type, template<class> class PatchField, class GeoMesh>
const tnbLib::GeometricField<Type, PatchField, GeoMesh>&
tnbLib::GeometricField<Type, PatchField, GeoMesh>::oldTime() const
{
	if (!field0Ptr_)
	{
		field0Ptr_ = new GeometricField<Type, PatchField, GeoMesh>
			(
				IOobject
				(
					this->name() + "_0",
					this->time().timeName(),
					this->db(),
					IOobject::NO_READ,
					IOobject::NO_WRITE,
					this->registerObject()
				),
				*this
				);
	}
	else
	{
		storeOldTimes();
	}

	return *field0Ptr_;
}


template<class Type, template<class> class PatchField, class GeoMesh>
tnbLib::GeometricField<Type, PatchField, GeoMesh>&
tnbLib::GeometricField<Type, PatchField, GeoMesh>::oldTime()
{
	static_cast<const GeometricField<Type, PatchField, GeoMesh>&>(*this)
		.oldTime();

	return *field0Ptr_;
}


template<class Type, template<class> class PatchField, class GeoMesh>
void tnbLib::GeometricField<Type, PatchField, GeoMesh>::storePrevIter() const
{
	if (!fieldPrevIterPtr_)
	{
		if (debug)
		{
			InfoInFunction
				<< "Allocating previous iteration field" << endl
				<< this->info() << endl;
		}

		fieldPrevIterPtr_ = new GeometricField<Type, PatchField, GeoMesh>
			(
				this->name() + "PrevIter",
				*this
				);
	}
	else
	{
		*fieldPrevIterPtr_ == *this;
	}
}


template<class Type, template<class> class PatchField, class GeoMesh>
const tnbLib::GeometricField<Type, PatchField, GeoMesh>&
tnbLib::GeometricField<Type, PatchField, GeoMesh>::prevIter() const
{
	if (!fieldPrevIterPtr_)
	{
		FatalErrorInFunction
			<< "previous iteration field" << endl << this->info() << endl
			<< "  not stored."
			<< "  Use field.storePrevIter() at start of iteration."
			<< abort(FatalError);
	}

	return *fieldPrevIterPtr_;
}


template<class Type, template<class> class PatchField, class GeoMesh>
void tnbLib::GeometricField<Type, PatchField, GeoMesh>::
correctBoundaryConditions()
{
	this->setUpToDate();
	storeOldTimes();
	boundaryField_.evaluate();
}


template<class Type, template<class> class PatchField, class GeoMesh>
bool tnbLib::GeometricField<Type, PatchField, GeoMesh>::needReference() const
{
	// Search all boundary conditions, if any are
	// fixed-value or mixed (Robin) do not set reference level for solution.

	bool needRef = true;

	forAll(boundaryField_, patchi)
	{
		if (boundaryField_[patchi].fixesValue())
		{
			needRef = false;
			break;
		}
	}

	reduce(needRef, andOp<bool>());

	return needRef;
}


template<class Type, template<class> class PatchField, class GeoMesh>
void tnbLib::GeometricField<Type, PatchField, GeoMesh>::relax(const scalar alpha)
{
	if (debug)
	{
		InfoInFunction
			<< "Relaxing" << endl << this->info() << " by " << alpha << endl;
	}

	operator==(prevIter() + alpha * (*this - prevIter()));
}


template<class Type, template<class> class PatchField, class GeoMesh>
void tnbLib::GeometricField<Type, PatchField, GeoMesh>::relax()
{
	if
		(
			this->mesh().data::template lookupOrDefault<bool>
			(
				"finalIteration",
				false
				)
			&& this->mesh().relaxField(this->name() + "Final")
			)
	{
		relax(this->mesh().fieldRelaxationFactor(this->name() + "Final"));
	}
	else if (this->mesh().relaxField(this->name()))
	{
		relax(this->mesh().fieldRelaxationFactor(this->name()));
	}
}


template<class Type, template<class> class PatchField, class GeoMesh>
tnbLib::word tnbLib::GeometricField<Type, PatchField, GeoMesh>::select
(
	bool final
) const
{
	if (final)
	{
		return this->name() + "Final";
	}
	else
	{
		return this->name();
	}
}


template<class Type, template<class> class PatchField, class GeoMesh>
void tnbLib::GeometricField<Type, PatchField, GeoMesh>::writeMinMax
(
	Ostream& os
) const
{
	os << "min/max(" << this->name() << ") = "
		<< tnbLib::min(*this).value() << ", "
		<< tnbLib::max(*this).value()
		<< endl;
}


template<class Type, template<class> class PatchField, class GeoMesh>
bool tnbLib::GeometricField<Type, PatchField, GeoMesh>::
writeData(Ostream& os) const
{
	os << *this;
	return os.good();
}


// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

template<class Type, template<class> class PatchField, class GeoMesh>
tnbLib::tmp<tnbLib::GeometricField<Type, PatchField, GeoMesh>>
tnbLib::GeometricField<Type, PatchField, GeoMesh>::T() const
{
	tmp<GeometricField<Type, PatchField, GeoMesh>> result
	(
		GeometricField<Type, PatchField, GeoMesh>::New
		(
			this->name() + ".T()",
			this->mesh(),
			this->dimensions()
		)
	);

	tnbLib::T(result.ref().primitiveFieldRef(), primitiveField());
	tnbLib::T(result.ref().boundaryFieldRef(), boundaryField());

	return result;
}


template<class Type, template<class> class PatchField, class GeoMesh>
tnbLib::tmp
<
	tnbLib::GeometricField
	<
	typename tnbLib::GeometricField<Type, PatchField, GeoMesh>::cmptType,
	PatchField,
	GeoMesh
	>
>
tnbLib::GeometricField<Type, PatchField, GeoMesh>::component
(
	const direction d
) const
{
	tmp<GeometricField<cmptType, PatchField, GeoMesh>> Component
	(
		GeometricField<cmptType, PatchField, GeoMesh>::New
		(
			this->name() + ".component(" + tnbLib::name(d) + ')',
			this->mesh(),
			this->dimensions()
		)
	);

	tnbLib::component(Component.ref().primitiveFieldRef(), primitiveField(), d);
	tnbLib::component(Component.ref().boundaryFieldRef(), boundaryField(), d);

	return Component;
}


template<class Type, template<class> class PatchField, class GeoMesh>
void tnbLib::GeometricField<Type, PatchField, GeoMesh>::replace
(
	const direction d,
	const GeometricField
	<
	typename GeometricField<Type, PatchField, GeoMesh>::cmptType,
	PatchField,
	GeoMesh
	>& gcf
)
{
	primitiveFieldRef().replace(d, gcf.primitiveField());
	boundaryFieldRef().replace(d, gcf.boundaryField());
}


template<class Type, template<class> class PatchField, class GeoMesh>
void tnbLib::GeometricField<Type, PatchField, GeoMesh>::replace
(
	const direction d,
	const dimensioned<cmptType>& ds
)
{
	primitiveFieldRef().replace(d, ds.value());
	boundaryFieldRef().replace(d, ds.value());
}


template<class Type, template<class> class PatchField, class GeoMesh>
void tnbLib::GeometricField<Type, PatchField, GeoMesh>::max
(
	const dimensioned<Type>& dt
)
{
	tnbLib::max(primitiveFieldRef(), primitiveField(), dt.value());
	tnbLib::max(boundaryFieldRef(), boundaryField(), dt.value());
}


template<class Type, template<class> class PatchField, class GeoMesh>
void tnbLib::GeometricField<Type, PatchField, GeoMesh>::min
(
	const dimensioned<Type>& dt
)
{
	tnbLib::min(primitiveFieldRef(), primitiveField(), dt.value());
	tnbLib::min(boundaryFieldRef(), boundaryField(), dt.value());
}


template<class Type, template<class> class PatchField, class GeoMesh>
void tnbLib::GeometricField<Type, PatchField, GeoMesh>::maxMin
(
	const dimensioned<Type>& minDt,
	const dimensioned<Type>& maxDt
)
{
	tnbLib::max(primitiveFieldRef(), primitiveField(), minDt.value());
	tnbLib::max(boundaryFieldRef(), boundaryField(), minDt.value());
	tnbLib::min(primitiveFieldRef(), primitiveField(), maxDt.value());
	tnbLib::min(boundaryFieldRef(), boundaryField(), maxDt.value());
}


template<class Type, template<class> class PatchField, class GeoMesh>
void tnbLib::GeometricField<Type, PatchField, GeoMesh>::negate()
{
	primitiveFieldRef().negate();
	boundaryFieldRef().negate();
}


// * * * * * * * * * * * * * * * Member Operators  * * * * * * * * * * * * * //

template<class Type, template<class> class PatchField, class GeoMesh>
void tnbLib::GeometricField<Type, PatchField, GeoMesh>::operator=
(
	const GeometricField<Type, PatchField, GeoMesh>& gf
	)
{
	if (this == &gf)
	{
		FatalErrorInFunction
			<< "attempted assignment to self"
			<< abort(FatalError);
	}

	checkField(*this, gf, "=");

	// Only assign field contents not ID

	ref() = gf();
	boundaryFieldRef() = gf.boundaryField();
}


template<class Type, template<class> class PatchField, class GeoMesh>
void tnbLib::GeometricField<Type, PatchField, GeoMesh>::operator=
(
	GeometricField<Type, PatchField, GeoMesh>&& gf
	)
{
	if (this == &gf)
	{
		FatalErrorInFunction
			<< "attempted assignment to self"
			<< abort(FatalError);
	}

	checkField(*this, gf, "=");

	// Only assign field contents not ID

	ref() = move(gf());
	boundaryFieldRef() = move(gf.boundaryField());
}


template<class Type, template<class> class PatchField, class GeoMesh>
void tnbLib::GeometricField<Type, PatchField, GeoMesh>::operator=
(
	const tmp<GeometricField<Type, PatchField, GeoMesh>>& tgf
	)
{
	if (this == &(tgf()))
	{
		FatalErrorInFunction
			<< "attempted assignment to self"
			<< abort(FatalError);
	}

	const GeometricField<Type, PatchField, GeoMesh>& gf = tgf();

	checkField(*this, gf, "=");

	// Only assign field contents not ID

	this->dimensions() = gf.dimensions();

	if (tgf.isTmp())
	{
		// Transfer the storage from the tmp
		primitiveFieldRef().transfer
		(
			const_cast<Field<Type>&>(gf.primitiveField())
		);
	}
	else
	{
		primitiveFieldRef() = gf.primitiveField();
	}

	boundaryFieldRef() = gf.boundaryField();

	tgf.clear();
}


template<class Type, template<class> class PatchField, class GeoMesh>
void tnbLib::GeometricField<Type, PatchField, GeoMesh>::operator=
(
	const dimensioned<Type>& dt
	)
{
	ref() = dt;
	boundaryFieldRef() = dt.value();
}


template<class Type, template<class> class PatchField, class GeoMesh>
void tnbLib::GeometricField<Type, PatchField, GeoMesh>::operator=
(
	const zero&
	)
{
	ref() = Zero;
	boundaryFieldRef() = Zero;
}


template<class Type, template<class> class PatchField, class GeoMesh>
void tnbLib::GeometricField<Type, PatchField, GeoMesh>::operator==
(
	const tmp<GeometricField<Type, PatchField, GeoMesh>>& tgf
	)
{
	const GeometricField<Type, PatchField, GeoMesh>& gf = tgf();

	checkField(*this, gf, "==");

	// Only assign field contents not ID

	ref() = gf();
	boundaryFieldRef() == gf.boundaryField();

	tgf.clear();
}


template<class Type, template<class> class PatchField, class GeoMesh>
void tnbLib::GeometricField<Type, PatchField, GeoMesh>::operator==
(
	const dimensioned<Type>& dt
	)
{
	ref() = dt;
	boundaryFieldRef() == dt.value();
}


template<class Type, template<class> class PatchField, class GeoMesh>
void tnbLib::GeometricField<Type, PatchField, GeoMesh>::operator==
(
	const zero&
	)
{
	ref() = Zero;
	boundaryFieldRef() == Zero;
}


#define COMPUTED_ASSIGNMENT(TYPE, op)                                          \
                                                                               \
template<class Type, template<class> class PatchField, class GeoMesh>          \
void tnbLib::GeometricField<Type, PatchField, GeoMesh>::operator op              \
(                                                                              \
    const GeometricField<TYPE, PatchField, GeoMesh>& gf                        \
)                                                                              \
{                                                                              \
    checkField(*this, gf, #op);                                                \
                                                                               \
    ref() op gf();            \
    boundaryFieldRef() op gf.boundaryField();                                  \
}                                                                              \
                                                                               \
template<class Type, template<class> class PatchField, class GeoMesh>          \
void tnbLib::GeometricField<Type, PatchField, GeoMesh>::operator op              \
(                                                                              \
    const tmp<GeometricField<TYPE, PatchField, GeoMesh>>& tgf                  \
)                                                                              \
{                                                                              \
    operator op(tgf());                                                        \
    tgf.clear();                                                               \
}                                                                              \
                                                                               \
template<class Type, template<class> class PatchField, class GeoMesh>          \
void tnbLib::GeometricField<Type, PatchField, GeoMesh>::operator op              \
(                                                                              \
    const dimensioned<TYPE>& dt                                                \
)                                                                              \
{                                                                              \
    ref() op dt;                                       \
    boundaryFieldRef() op dt.value();                                          \
}

template <class Type, template<class> class PatchField, class GeoMesh>
void tnbLib::GeometricField<Type, PatchField, GeoMesh>::operator +=(const GeometricField<Type, PatchField, GeoMesh>& gf)
{
	checkField(*this, gf, "+=");
	ref() += gf();
	boundaryFieldRef() += gf.boundaryField();
}

template <class Type, template<class> class PatchField, class GeoMesh>
void tnbLib::GeometricField<Type, PatchField, GeoMesh>::operator +=(
	const tmp<GeometricField<Type, PatchField, GeoMesh>>& tgf)
{
	operator +=(tgf());
	tgf.clear();
}

template <class Type, template<class> class PatchField, class GeoMesh>
void tnbLib::GeometricField<Type, PatchField, GeoMesh>::operator +=(const dimensioned<Type>& dt)
{
	ref() += dt;
	boundaryFieldRef() += dt.value();
}

template <class Type, template<class> class PatchField, class GeoMesh>
void tnbLib::GeometricField<Type, PatchField, GeoMesh>::operator -=(const GeometricField<Type, PatchField, GeoMesh>& gf)
{
	checkField(*this, gf, "-=");
	ref() -= gf();
	boundaryFieldRef() -= gf.boundaryField();
}

template <class Type, template<class> class PatchField, class GeoMesh>
void tnbLib::GeometricField<Type, PatchField, GeoMesh>::operator -=(
	const tmp<GeometricField<Type, PatchField, GeoMesh>>& tgf)
{
	operator -=(tgf());
	tgf.clear();
}

template <class Type, template<class> class PatchField, class GeoMesh>
void tnbLib::GeometricField<Type, PatchField, GeoMesh>::operator -=(const dimensioned<Type>& dt)
{
	ref() -= dt;
	boundaryFieldRef() -= dt.value();
}

template <class Type, template<class> class PatchField, class GeoMesh>
void tnbLib::GeometricField<Type, PatchField, GeoMesh>::operator *=(
	const GeometricField<scalar, PatchField, GeoMesh>& gf)
{
	checkField(*this, gf, "*=");
	ref() *= gf();
	boundaryFieldRef() *= gf.boundaryField();
}

template <class Type, template<class> class PatchField, class GeoMesh>
void tnbLib::GeometricField<Type, PatchField, GeoMesh>::operator *=(
	const tmp<GeometricField<scalar, PatchField, GeoMesh>>& tgf)
{
	operator *=(tgf());
	tgf.clear();
}

template <class Type, template<class> class PatchField, class GeoMesh>
void tnbLib::GeometricField<Type, PatchField, GeoMesh>::operator *=(const dimensioned<scalar>& dt)
{
	ref() *= dt;
	boundaryFieldRef() *= dt.value();
}

template <class Type, template<class> class PatchField, class GeoMesh>
void tnbLib::GeometricField<Type, PatchField, GeoMesh>::operator /=(
	const GeometricField<scalar, PatchField, GeoMesh>& gf)
{
	checkField(*this, gf, "/=");
	ref() /= gf();
	boundaryFieldRef() /= gf.boundaryField();
}

template <class Type, template<class> class PatchField, class GeoMesh>
void tnbLib::GeometricField<Type, PatchField, GeoMesh>::operator /=(
	const tmp<GeometricField<scalar, PatchField, GeoMesh>>& tgf)
{
	operator /=(tgf());
	tgf.clear();
}

template <class Type, template<class> class PatchField, class GeoMesh>
void tnbLib::GeometricField<Type, PatchField, GeoMesh>::operator /=(const dimensioned<scalar>& dt)
{
	ref() /= dt;
	boundaryFieldRef() /= dt.value();
}

#undef COMPUTED_ASSIGNMENT


// * * * * * * * * * * * * * * * IOstream Operators  * * * * * * * * * * * * //

template<class Type, template<class> class PatchField, class GeoMesh>
tnbLib::Ostream& tnbLib::operator<<
(
	Ostream& os,
	const GeometricField<Type, PatchField, GeoMesh>& gf
	)
{
	gf().writeData(os, "internalField");
	os << nl;
	gf.boundaryField().writeEntry("boundaryField", os);

	// Check state of IOstream
	os.check
	(
		"Ostream& operator<<(Ostream&, "
		"const GeometricField<Type, PatchField, GeoMesh>&)"
	);

	return (os);
}


template<class Type, template<class> class PatchField, class GeoMesh>
tnbLib::Ostream& tnbLib::operator<<
(
	Ostream& os,
	const tmp<GeometricField<Type, PatchField, GeoMesh>>& tgf
	)
{
	os << tgf();
	tgf.clear();
	return os;
}


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#undef checkField

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#include <GeometricBoundaryField.cxx>
#include <GeometricFieldFunctions.cxx>

// ************************************************************************* //