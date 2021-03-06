
/*
 * Author
 *   David Blom, TU Delft. All rights reserved.
 */

#include "SDC.H"
#include "QuadratureRules.H"

namespace sdc
{
    SDC::SDC(
        std::shared_ptr<SDCSolver> solver,
        std::string rule,
        int nbNodes,
        double tol
        )
        :
        solver( solver ),
        nodes(),
        smat(),
        qmat(),
        dsdc(),
        dt( solver->getTimeStep() ),
        N( solver->getDOF() ),
        k( 0 ),
        tol( tol )
    {
        assert( solver );
        assert( nbNodes > 1 );
        assert( nbNodes < 15 );
        assert( dt > 0 );
        assert( tol > 0 );
        assert( tol < 1 );
        assert( rule == "gauss-radau" || rule == "gauss-lobatto" );

        quadrature::rules( rule, nbNodes, nodes, smat, qmat );

        k = nodes.rows();

        dsdc.resize( nodes.rows() - 1 );

        for ( int i = 0; i < dsdc.rows(); i++ )
            dsdc( i ) = nodes( i + 1 ) - nodes( i );

        solver->setNumberOfStages( k );
    }

    SDC::~SDC()
    {}

    void SDC::run()
    {
        int i = 0;

        while ( solver->isRunning() )
        {
            solveTimeStep( dt * i );
            i++;
        }
    }

    void SDC::solveTimeStep( const double t0 )
    {
        solver->nextTimeStep();

        Eigen::VectorXd dtsdc = this->dt * dsdc;
        Eigen::MatrixXd solStages( k, N ), F( k, N );

        Eigen::VectorXd sol( N ), f( N );
        solver->getSolution( sol );
        solStages.row( 0 ) = sol;

        double t = t0;

        solver->evaluateFunction( 0, sol, t, f );
        F.row( 0 ) = f;

        for ( int j = 0; j < k - 1; j++ )
        {
            double dt = dtsdc( j );
            t += dt;

            Eigen::VectorXd rhs( N ), result( N ), qold( N );
            f.setZero();
            rhs.setZero();
            result.setZero();
            qold = solStages.row( j );

            Info << "\nTime = " << t << ", SDC sweep = 0, SDC substep = " << j + 1 << nl << endl;

            solver->initTimeStep();
            solver->implicitSolve( false, j, t, dt, qold, rhs, f, result );
            solver->finalizeTimeStep();

            solStages.row( j + 1 ) = result;
            F.row( j + 1 ) = f;
        }

        // Compute successive corrections

        bool convergence = false;

        for ( int j = 0; j < 10 * k; j++ )
        {
            t = t0;
            Eigen::MatrixXd Fold = F;

            Eigen::VectorXd rhs( N ), result( N ), qold( N );
            f.setZero();
            rhs.setZero();
            result.setZero();

            Eigen::MatrixXd Sj = this->dt * (smat * F);

            // SDC sweep
            for ( int p = 0; p < k - 1; p++ )
            {
                double dt = dtsdc( p );
                t += dt;
                f.setZero();
                result.setZero();

                Info << "\nTime = " << t << ", SDC sweep = " << j + 1 << ", SDC substep = " << p + 1 << nl << endl;

                qold = solStages.row( p );

                // Form right hand side
                rhs = -dt * Fold.row( p + 1 ) + Sj.row( p );

                solver->initTimeStep();
                solver->implicitSolve( true, p, t, dt, qold, rhs, f, result );
                solver->finalizeTimeStep();

                solStages.row( p + 1 ) = result;
                F.row( p + 1 ) = f;
            }

            // Compute the SDC residual

            Eigen::MatrixXd Qj = dt * (qmat * F);
            Eigen::MatrixXd residual = solStages.row( 0 ) + Qj.row( k - 2 ) - solStages.row( k - 1 );

            scalarList squaredNorm( Pstream::nProcs() );
            squaredNorm[Pstream::myProcNo()] = residual.squaredNorm();
            reduce( squaredNorm, sumOp<scalarList>() );
            double error = std::sqrt( sum( squaredNorm ) / solver->getNbCells() );
            error /= solver->getScalingFactor();
            convergence = error < tol && j >= k - 1;

            Info << "SDC residual = " << error;
            Info << ", tol = " << tol;
            Info << ", time = " << t;
            Info << ", sweep = " << j + 1;
            Info << ", convergence = ";

            if ( convergence )
                Info << "true";
            else
                Info << "false";

            Info << endl;

            if ( convergence )
                break;
        }

        solver->setDeltaT( this->dt );
    }
}
