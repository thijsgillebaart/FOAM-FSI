
/*
 * Author
 *   David Blom, TU Delft. All rights reserved.
 */

#include "ConvergenceMeasure.H"
#include "FsiSolver.H"
#include "RelativeConvergenceMeasure.H"
#include "TubeFlowFluidSolver.H"
#include "TubeFlowSolidSolver.H"
#include "gtest/gtest.h"

using namespace tubeflow;

class FsiSolverTest : public::testing::Test
{
protected:

    virtual void SetUp()
    {
        // Physical settings
        double r0 = 0.2;
        double a0 = M_PI * r0 * r0;
        double u0 = 0.1;
        double p0 = 0;
        double dt = 0.1;
        int N = 5;
        double L = 1;
        double T = 1;
        double dx = L / N;
        double rho = 1.225;
        double E = 490;
        double h = 1.0e-3;
        double cmk = std::sqrt( E * h / (2 * rho * r0) );
        double c0 = std::sqrt( cmk * cmk - p0 / (2 * rho) );
        double kappa = c0 / u0;
        double tau = u0 * dt / L;

        // Computational settings
        bool parallel = false;
        double tol = 1.0e-5;
        int extrapolationOrder = 0;

        ASSERT_NEAR( tau, 0.01, 1.0e-13 );
        ASSERT_NEAR( kappa, 10, 1.0e-13 );
        ASSERT_TRUE( dx > 0 );

        // Initialize solvers
        shared_ptr<TubeFlowFluidSolver> fluid( new TubeFlowFluidSolver( a0, u0, p0, dt, cmk, N, L, T, rho ) );
        shared_ptr<TubeFlowSolidSolver> solid( new TubeFlowSolidSolver( a0, cmk, p0, rho, L, N ) );

        // Convergence measures
        std::shared_ptr< std::list<std::shared_ptr<ConvergenceMeasure> > > convergenceMeasures;
        convergenceMeasures = std::shared_ptr<std::list<std::shared_ptr<ConvergenceMeasure> > >( new std::list<std::shared_ptr<ConvergenceMeasure> > );

        // Add convergence measure
        convergenceMeasures->push_back( std::shared_ptr<ConvergenceMeasure> ( new RelativeConvergenceMeasure( 0, tol ) ) );

        // Create the fsi solver
        fsi = new FsiSolver( fluid, solid, convergenceMeasures, parallel, extrapolationOrder );
    }

    virtual void TearDown()
    {
        delete fsi;
    }

    FsiSolver * fsi;
};

TEST_F( FsiSolverTest, object )
{
    ASSERT_TRUE( true );
}

TEST_F( FsiSolverTest, initTimeStep )
{
    ASSERT_FALSE( fsi->init );
    ASSERT_FALSE( fsi->fluid->init );
    ASSERT_FALSE( fsi->solid->init );

    fsi->initTimeStep();

    ASSERT_TRUE( fsi->init );
    ASSERT_TRUE( fsi->fluid->init );
    ASSERT_TRUE( fsi->solid->init );
}

TEST_F( FsiSolverTest, evaluate )
{
    fsi::vector input( 5 ), output( 5 ), R( 5 );
    input = fsi->solid->data.col( 0 );
    output.setZero();
    R.setZero();

    fsi->initTimeStep();

    fsi->evaluate( input, output, R );
}
