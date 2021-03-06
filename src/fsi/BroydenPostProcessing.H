
/*
 * Author
 *   David Blom, TU Delft. All rights reserved.
 */

#ifndef BroydenPostProcessing_H
#define BroydenPostProcessing_H

#include "PostProcessing.H"

namespace fsi
{
    class BroydenPostProcessing : public PostProcessing
    {
public:

        BroydenPostProcessing(
            shared_ptr<MultiLevelFsiSolver> fsi,
            int maxIter,
            double initialRelaxation,
            int maxUsedIterations,
            int nbReuse,
            double singularityLimit,
            int reuseInformationStartingFromTimeIndex
            );

        virtual ~BroydenPostProcessing();

        virtual void finalizeTimeStep();

        void performPostProcessing(
            const vector & x0,
            vector & xk
            );

        void performPostProcessing(
            const vector & y,
            const vector & x0,
            vector & xk
            );

        void performPostProcessing(
            const vector & y,
            const vector & x0,
            vector & xk,
            bool residualCriterium
            );

        matrix J;
        double singularityLimit;
    };
}

#endif
