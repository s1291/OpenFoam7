#pragma once
#ifndef _symmTensorField_Header
#define _symmTensorField_Header

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

Typedef
	tnbLib::symmTensorField

Description
	Specialisation of Field\<T\> for symmTensor.

SourceFiles
	symmTensorField.C

\*---------------------------------------------------------------------------*/

#include <scalarField.hxx>
#include <vectorField.hxx>
#include <sphericalTensor.hxx>
#include <symmTensor.hxx>
#include <tensor.hxx>

//#define TEMPLATE
//#include <FieldFunctionsM.hxx>

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace tnbLib
{

	typedef Field<symmTensor> symmTensorField;

	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

	 
	void sqr(Field<symmTensor>& res, const UList<vector>& f);
	tmp<Field<symmTensor>> sqr(const UList<vector>& f);
	tmp<Field<symmTensor>> sqr(const tmp<Field<vector>>& tf);
		
	void innerSqr(Field<symmTensor>& res, const UList<symmTensor>& f);
	tmp<Field<symmTensor>> innerSqr(const UList<symmTensor>& f);
	tmp<Field<symmTensor>> innerSqr(const tmp<Field<symmTensor>>& tf);

		
	void tr(Field<scalar>& res, const UList<symmTensor>& f);
	tmp<Field<scalar>> tr(const UList<symmTensor>& f);
	tmp<Field<scalar>> tr(const tmp<Field<symmTensor>>& tf);
		
	void sph(Field<sphericalTensor>& res, const UList<symmTensor>& f);
	tmp<Field<sphericalTensor>> sph(const UList<symmTensor>& f);
	tmp<Field<sphericalTensor>> sph(const tmp<Field<symmTensor>>& tf);
		
	void symm(Field<symmTensor>& res, const UList<symmTensor>& f);
	tmp<Field<symmTensor>> symm(const UList<symmTensor>& f);
	tmp<Field<symmTensor>> symm(const tmp<Field<symmTensor>>& tf);
		
	void twoSymm(Field<symmTensor>& res, const UList<symmTensor>& f);
	tmp<Field<symmTensor>> twoSymm(const UList<symmTensor>& f);
	tmp<Field<symmTensor>> twoSymm(const tmp<Field<symmTensor>>& tf);
		
	void dev(Field<symmTensor>& res, const UList<symmTensor>& f);
	tmp<Field<symmTensor>> dev(const UList<symmTensor>& f);
	tmp<Field<symmTensor>> dev(const tmp<Field<symmTensor>>& tf);
		
	void dev2(Field<symmTensor>& res, const UList<symmTensor>& f);
	tmp<Field<symmTensor>> dev2(const UList<symmTensor>& f);
	tmp<Field<symmTensor>> dev2(const tmp<Field<symmTensor>>& tf);
		
	void det(Field<scalar>& res, const UList<symmTensor>& f);
	tmp<Field<scalar>> det(const UList<symmTensor>& f);
	tmp<Field<scalar>> det(const tmp<Field<symmTensor>>& tf);
		
	void cof(Field<symmTensor>& res, const UList<symmTensor>& f);
	tmp<Field<symmTensor>> cof(const UList<symmTensor>& f);
	tmp<Field<symmTensor>> cof(const tmp<Field<symmTensor>>& tf);
		
	void inv(Field<symmTensor>& res, const UList<symmTensor>& f);
	tmp<Field<symmTensor>> inv(const UList<symmTensor>& f);
	tmp<Field<symmTensor>> inv(const tmp<Field<symmTensor>>& tf);


		// * * * * * * * * * * * * * * * global operators  * * * * * * * * * * * * * //

		
	void hdual(Field<vector>& res, const UList<symmTensor>& f);
	tmp<Field<vector>> operator *(const UList<symmTensor>& f);
	tmp<Field<vector>> operator *(const tmp<Field<symmTensor>>& tf);

		
	void dot(Field<tensor>& f, const UList<symmTensor>& f1, const UList<symmTensor>& f2);
	tmp<Field<tensor>> operator &(const UList<symmTensor>& f1, const UList<symmTensor>& f2);
	tmp<Field<tensor>> operator &(const UList<symmTensor>& f1, const tmp<Field<symmTensor>>& tf2);
	tmp<Field<tensor>> operator &(const tmp<Field<symmTensor>>& tf1, const UList<symmTensor>& f2);
	tmp<Field<tensor>> operator &(const tmp<Field<symmTensor>>& tf1, const tmp<Field<symmTensor>>& tf2);
		
	void dot(Field<tensor>& f, const symmTensor& s1, const UList<symmTensor>& f2);
	tmp<Field<tensor>> operator &(const symmTensor& s1, const UList<symmTensor>& f2);
	tmp<Field<tensor>> operator &(const symmTensor& s1, const tmp<Field<symmTensor>>& tf2);
	void dot(Field<tensor>& f, const UList<symmTensor>& f1, const symmTensor& s2);
	tmp<Field<tensor>> operator &(const UList<symmTensor>& f1, const symmTensor& s2);
	tmp<Field<tensor>> operator &(const tmp<Field<symmTensor>>& tf1, const symmTensor& s2);


		// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace tnbLib

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

//#include <undefFieldFunctionsM.hxx>

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif // !_symmTensorField_Header
