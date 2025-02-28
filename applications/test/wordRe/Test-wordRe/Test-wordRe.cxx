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

Description

\*---------------------------------------------------------------------------*/

#include <includeAllModules.hxx>

#include <IOstreams.hxx>
#include <IOobject.hxx>
#include <IFstream.hxx>
#include <List.hxx>
#include <Tuple2.hxx>
#include <wordRe.hxx>

using namespace tnbLib;

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //
// Main program:

int main(int argc, char* argv[])
{
    wordRe wre;
    std::string s1("this .* file");
    tnbLib::string s2("this .* file");
    const char* s3 = "this .* file";

    wordRe(s1, wordRe::compOption::detect).info(Info) << endl;
    wordRe(s2).info(Info) << endl;
    wordRe(s2, wordRe::compOption::detect).info(Info) << endl;
    wordRe(s3, wordRe::compOption::regExp).info(Info) << endl;

    wre = "this .* file";
    wre.info(Info) << endl;
    wre = s1;
    wre.info(Info) << endl;
    wre.uncompile();
    wre.info(Info) << endl;

    wre = "something";
    wre.info(Info) << " before" << endl;
    wre.uncompile();
    wre.info(Info) << " uncompiled" << endl;
    wre.compile(wordRe::compOption::detect);
    wre.info(Info) << " after compOption::detect" << endl;
    wre.compile(wordRe::compOption::noCase);
    wre.info(Info) << " after noCase" << endl;
    wre.compile(wordRe::compOption::detectNoCase);
    wre.info(Info) << " after compOption::detectNoCase" << endl;

    wre = "something .* value";
    wre.info(Info) << " before" << endl;
    wre.uncompile();
    wre.info(Info) << " uncompiled" << endl;
    wre.compile(wordRe::compOption::detect);
    wre.info(Info) << " after compOption::detect" << endl;
    wre.uncompile();
    wre.info(Info) << " uncompiled" << endl;
    wre.recompile();
    wre.info(Info) << " recompiled" << endl;

    wre.set("something .* value", wordRe::compOption::literal);
    wre.info(Info) << " set as LITERAL" << endl;

    IOobject::writeDivider(Info);

    List<Tuple2<wordRe, string>> rawList(IFstream("testRegexps")());
    Info << "input list:" << rawList << endl;
    IOobject::writeDivider(Info) << endl;

    forAll(rawList, elemI)
    {
        const wordRe& wre = rawList[elemI].first();
        const string& str = rawList[elemI].second();

        wre.info(Info)
            << " equals:" << (wre == str)
            << "(" << wre.match(str, true) << ")"
            << " match:" << wre.match(str)
            << "  str=" << str
            << endl;

        wordRe wre2;
        wre2.set(wre, wordRe::compOption::noCase);

        wre2.info(Info)
            << " match:" << wre2.match(str)
            << "  str=" << str
            << endl;

    }

    Info << endl;

    return 0;
}


// ************************************************************************* //
