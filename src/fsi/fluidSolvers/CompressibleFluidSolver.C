
/*
 * Class
 *   CompressibleFluidSolver
 *
 * Author
 *   David Blom, TU Delft. All rights reserved.
 */

#include "CompressibleFluidSolver.H"

CompressibleFluidSolver::CompressibleFluidSolver(
    string name,
    shared_ptr<argList> args,
    shared_ptr<Time> runTime
    )
    :
    foamFluidSolver( name, args, runTime ),
    pThermo
    (
    basicPsiThermo::New( mesh )
    ),
    thermo( pThermo() ),
    p( thermo.p() ),
    h( thermo.h() ),
    T( thermo.T() ),
    psi( thermo.psi() ),
    rho
    (
    IOobject
    (
        "rho",
        runTime->timeName(),
        mesh,
        IOobject::NO_READ,
        IOobject::AUTO_WRITE
    ),
    thermo.rho()
    ),
    U
    (
    IOobject
    (
        "U",
        runTime->timeName(),
        mesh,
        IOobject::MUST_READ,
        IOobject::AUTO_WRITE
    ),
    mesh
    ),
    phi
    (
    IOobject
    (
        "phi",
        runTime->timeName(),
        mesh,
        IOobject::READ_IF_PRESENT,
        IOobject::NO_WRITE
    ),
    linearInterpolate( rho * U ) & mesh.Sf()
    ),
    turbulence
    (
    compressible::RASModel::New
    (
        rho,
        U,
        phi,
        thermo
    )
    ),
    Up
    (
    IOobject
    (
        "Up",
        runTime->timeName(),
        mesh,
        IOobject::NO_READ,
        IOobject::NO_WRITE
    ),
    mesh,
    dimensionedVector4( "zero", dimless, vector4::zero )
    ),
    DpDt(
    fvc::DDt( surfaceScalarField( "phiU", phi / fvc::interpolate( rho ) ), p ) ),
    cumulativeContErr( 0 ),
    convergenceTolerance( readScalar( mesh.solutionDict().subDict( "blockSolver" ).lookup( "convergenceTolerance" ) ) ),
    nOuterCorr( readLabel( mesh.solutionDict().subDict( "blockSolver" ).lookup( "nOuterCorrectors" ) ) ),
    acousticsPatchName( couplingProperties.lookup( "acousticsPatch" ) ),
    acousticsPatchID( mesh.boundaryMesh().findPatchID( acousticsPatchName ) )
{
    assert( nOuterCorr > 0 );
    assert( convergenceTolerance < 1 );
    assert( convergenceTolerance > 0 );

    rho.oldTime();

    // Ensure that the absolute tolerance of the linear solver is less than the
    // used convergence tolerance for the non-linear system.
    scalar absTolerance = readScalar( mesh.solutionDict().subDict( "solvers" ).subDict( "Up" ).lookup( "tolerance" ) );
    assert( absTolerance < convergenceTolerance );
}

CompressibleFluidSolver::~CompressibleFluidSolver()
{}

void CompressibleFluidSolver::continuityErrs()
{
    dimensionedScalar totalMass = fvc::domainIntegrate( rho );

    scalar sumLocalContErr =
        (fvc::domainIntegrate( mag( rho - thermo.rho() ) ) / totalMass).value();

    scalar globalContErr =
        (fvc::domainIntegrate( rho - thermo.rho() ) / totalMass).value();

    cumulativeContErr += globalContErr;

    Info << "time step continuity errors : sum local = " << sumLocalContErr
         << ", global = " << globalContErr
         << ", cumulative = " << cumulativeContErr
         << endl;
}

void CompressibleFluidSolver::getAcousticsPressureLocal( matrix & data )
{
    // Use linear interpolation to interpolate the pressure field from the cell
    // centers to the cell faces.
    // Return the pressure defined at the cell faces.

    tmp<surfaceScalarField> pface = fvc::interpolate( p );

    scalarField pressureField = pface->boundaryField()[acousticsPatchID];

    data.resize( pressureField.size(), 1 );

    for ( int i = 0; i < data.rows(); i++ )
        data( i, 0 ) = pressureField[i];
}

void CompressibleFluidSolver::getAcousticsDensityLocal( matrix & data )
{
    // Use linear interpolation to interpolate the density field from the cell
    // centers to the cell faces.
    // Return the density defined at the cell faces.

    tmp<surfaceScalarField> rhoface = fvc::interpolate( rho );

    scalarField densityField = rhoface->boundaryField()[acousticsPatchID];

    data.resize( densityField.size(), 1 );

    for ( int i = 0; i < data.rows(); i++ )
        data( i, 0 ) = densityField[i];
}

void CompressibleFluidSolver::getAcousticsVelocityLocal( matrix & data )
{
    // Use linear interpolation to interpolate the velocity field from the cell
    // centers to the cell faces.
    // Return the velocity defined at the cell faces.

    tmp<surfaceVectorField> Uface = fvc::interpolate( U );

    vectorField velocityField = Uface->boundaryField()[acousticsPatchID];

    data.resize( velocityField.size(), mesh.nGeometricD() );

    for ( int i = 0; i < data.rows(); i++ )
        for ( int j = 0; j < data.cols(); j++ )
            data( i, j ) = velocityField[i][j];
}

void CompressibleFluidSolver::getTractionLocal( matrix & traction )
{
    vectorField tractionField( getInterfaceSizeLocal(), Foam::vector::zero );

    int offset = 0;

    forAll( movingPatchIDs, patchI )
    {
        int size = mesh.boundaryMesh()[movingPatchIDs[patchI]].faceCentres().size();

        vectorField tractionFieldPatchI = -thermo.mu()
            * U.boundaryField()[movingPatchIDs[patchI]].snGrad()
            + p.boundaryField()[movingPatchIDs[patchI]]
            * mesh.boundary()[movingPatchIDs[patchI]].nf();

        forAll( tractionFieldPatchI, i )
        {
            tractionField[i + offset] = tractionFieldPatchI[i];
        }

        offset += size;
    }

    assert( tractionField.size() == nGlobalCenters[Pstream::myProcNo()] );

    traction.resize( tractionField.size(), mesh.nGeometricD() );

    for ( int i = 0; i < traction.rows(); i++ )
        for ( int j = 0; j < traction.cols(); j++ )
            traction( i, j ) = tractionField[i][j];
}

void CompressibleFluidSolver::getWritePositionsLocalAcoustics( matrix & writePositions )
{
    const vectorField faceCentres( mesh.boundaryMesh()[acousticsPatchID].faceCentres() );

    writePositions.resize( faceCentres.size(), mesh.nGeometricD() );

    for ( int i = 0; i < writePositions.rows(); i++ )
        for ( int j = 0; j < writePositions.cols(); j++ )
            writePositions( i, j ) = faceCentres[i][j];
}

void CompressibleFluidSolver::initTimeStep()
{
    assert( !init );

    timeIndex++;
    t = timeIndex * runTime->deltaT().value();

    Info << "\nTime = " << runTime->timeName() << endl;

    init = true;
}

bool CompressibleFluidSolver::isRunning()
{
    runTime->write();

    Info << "ExecutionTime = " << runTime->elapsedCpuTime() << " s"
         << "  ClockTime = " << runTime->elapsedClockTime() << " s"
         << endl << endl;

    return runTime->loop();
}

void CompressibleFluidSolver::resetSolution()
{
    // U == U.oldTime();
    // p == p.oldTime();
    // phi == phi.oldTime();
    // h == h.oldTime();
    // rho == rho.oldTime();
}

void CompressibleFluidSolver::solve()
{
    Info << "Solve fluid domain" << endl;

    int oCorr;

    // Outer correction loop to solve the non-linear system
    for ( oCorr = 0; oCorr < nOuterCorr; oCorr++ )
    {
        // Make the fluxes relative to the mesh-motion
        fvc::makeRelative( phi, rho, U );

        p.storePrevIter();

        // Initialize the Up block system (matrix, source and reference to Up)
        fvBlockMatrix<vector4> UpEqn( Up );

        scalar residual = 1;

        {
            // Solve the enthalpy equation
            T.storePrevIter();

            // Calculate face velocity from flux
            surfaceScalarField faceU
            (
                "faceU",
                phi / fvc::interpolate( rho )
            );

            DpDt = fvc::DDt( surfaceScalarField( "phiU", phi / fvc::interpolate( rho ) ), p );

            fvScalarMatrix hEqn
            (
                fvm::ddt( rho, h )
                + fvm::div( phi, h )
                - fvm::laplacian( turbulence->alphaEff(), h )
                + fvm::SuSp( ( fvc::div( faceU, p, "div(U,p)" ) - p * fvc::div( faceU ) ) / h, h )
                ==

                // ddt(p) term removed: steady-state.  HJ, 27/Apr/2010
                // Viscous heating: note sign (devRhoReff has a minus in it)
                DpDt
                - ( turbulence->devRhoReff() && fvc::grad( U ) )
            );

            hEqn.relax();
            residual = hEqn.solve().initialResidual();

            // Bound the enthalpy using TMin and TMax
            volScalarField Cp = thermo.Cp();

            // h = Foam::min(h, TMax*Cp);
            // h = Foam::max(h, TMin*Cp);
            h.correctBoundaryConditions();

            thermo.correct();
        }

        // Momentum equation
        tmp<fvVectorMatrix> UEqn
        (
            fvm::ddt( rho, U )
            + fvm::div( phi, U )
            + turbulence->divDevRhoReff( U )
        );

        UEqn().relax();

        UpEqn.insertEquation( 0, UEqn() );


        // Pressure parts of the continuity equation
        surfaceScalarField rUAf
        (
            "rUAf",
            fvc::interpolate( rho / UEqn().A() )
        );

        // Flux for p
        surfaceScalarField phid
        (
            "phid",
            phi * fvc::interpolate( psi / rho )
        );

        // Rhie-Chow correction
        surfaceScalarField presSource
        (
            "presSource",
            rUAf * ( fvc::interpolate( fvc::grad( p, "grad(pSource)" ) ) & mesh.Sf() )
        );

        tmp<fvScalarMatrix> pEqn
        (
            fvm::ddt( psi, p )
            + fvm::div( phid, p )
            + fvc::div( -phi )
            - fvm::laplacian( rUAf, p )
            ==
            -fvc::div( presSource )
        );

        UpEqn.insertEquation( 3, pEqn() );

        {
            // Calculate grad p coupling matrix. Needs to be here if one uses
            // gradient schemes with limiters. VV, 9/June/2014
            BlockLduSystem<Foam::vector, Foam::vector> pInU( fvm::grad( p ) );
            BlockLduSystem<Foam::vector, scalar> UInp( fvm::UDiv( fvc::interpolate( rho ), U ) );

            // Last argument in insertBlockCoupling says if the column direction
            // should be incremented. This is needed for arbitrary positioning
            // of U and p in the system. This could be better. VV, 30/April/2014
            UpEqn.insertBlockCoupling( 0, 3, pInU, true );
            UpEqn.insertBlockCoupling( 3, 0, UInp, false );
        }

        // Solve the block matrix

        Foam::VectorN<double, 4> initialResidual = UpEqn.solve().initialResidual();
        forAll( initialResidual, j )
        {
            residual = std::max( initialResidual[j], residual );
        }

        // Retrieve solution
        UpEqn.retrieveSolution( 0, U.internalField() );
        UpEqn.retrieveSolution( 3, p.internalField() );

        U.correctBoundaryConditions();
        p.correctBoundaryConditions();

        phi = ( fvc::interpolate( rho * U ) & mesh.Sf() ) + pEqn().flux() + presSource - phi;

        pEqn.clear();

        p.relax();

        {
            // Calculate density from pressure
            rho.storePrevIter();
            rho = thermo.rho();

            rho.relax();
        }

        // Correct turbulence
        turbulence->correct();

        if ( residual < convergenceTolerance )
            break;
    }

    continuityErrs();
}
