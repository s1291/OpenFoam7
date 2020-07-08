#pragma once
#ifndef _pyramid_Header
#define _pyramid_Header

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
	tnbLib::pyramid

Description
	A geometric pyramid primitive with a base of 'n' sides:
	i.e. a parametric pyramid. A pyramid is constructed from
	a base polygon and an apex point.

SourceFiles
	pyramidI.H

\*---------------------------------------------------------------------------*/



// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace tnbLib
{

	// Forward declaration of friend functions and operators
	class Istream; // added by amir
	class Ostream; // added by amir

	template<class Point, class PointRef, class polygonRef>
	class pyramid;

	template<class Point, class PointRef, class polygonRef>
	inline Istream& operator>>
		(
			Istream&,
			pyramid<Point, PointRef, polygonRef>&
			);

	template<class Point, class PointRef, class polygonRef>
	inline Ostream& operator<<
		(
			Ostream&,
			const pyramid<Point, PointRef, polygonRef>&
			);


	/*---------------------------------------------------------------------------*\
							   Class pyramid Declaration
	\*---------------------------------------------------------------------------*/

	template<class Point, class PointRef, class polygonRef>
	class pyramid
	{
		// Private Data

		polygonRef base_;
		PointRef apex_;


	public:

		// Constructors

			//- Construct from base polygon and apex point
		inline pyramid(polygonRef base, const Point& apex);

		//- Construct from Istream
		inline pyramid(Istream&);


		// Member Functions

			// Access

				//- Return apex point
		inline const Point& apex() const;

		//- Return base polygon
		inline polygonRef base() const;


		// Properties

			//- Return centre (centroid)
		inline Point centre(const pointField& points) const;

		//- Return height vector
		inline vector height(const pointField& points) const;

		//- Return scalar magnitude - returns volume of pyramid
		inline scalar mag(const pointField& points) const;


		// IOstream Operators

		friend Istream& operator>> <Point, PointRef, polygonRef>
			(
				Istream&,
				pyramid&
				);

		friend Ostream& operator<< <Point, PointRef, polygonRef>
			(
				Ostream&,
				const pyramid&
				);
	};


	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace tnbLib

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#include <pyramidI.hxx>

#endif // !_pyramid_Header
