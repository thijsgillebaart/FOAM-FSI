
/*
 * Author
 *   David Blom, TU Delft. All rights reserved.
 */

#include "gtest/gtest.h"
#include <limits.h>

#include "RBFinterpolation.H"
#include "TwoDCorrector.H"

TEST( RBFinterpolationTest, oneD_small )
{
    // Initialize variables

    RBF::RBFinterpolation rbf = RBF::RBFinterpolation();

    int dim = 1;
    int n_A = 7;
    int n_B = 6;

    Eigen::VectorXd x( n_A ), y( n_A ), xnew( n_B ), ynew( n_B );

    // Location of original values

    x( 0 ) = 0;
    x( 1 ) = 1;
    x( 2 ) = 2;
    x( 3 ) = 3;
    x( 4 ) = 4;
    x( 5 ) = 5;
    x( 6 ) = 6;

    xnew( 0 ) = 0.5;
    xnew( 1 ) = 1.5;
    xnew( 2 ) = 2.5;
    xnew( 3 ) = 3.5;
    xnew( 4 ) = 4.5;
    xnew( 5 ) = 5.5;

    // Values

    y( 0 ) = 0;
    y( 1 ) = 8.414709848078965e-01;
    y( 2 ) = 9.092974268256817e-01;
    y( 3 ) = 1.411200080598672e-01;
    y( 4 ) = -7.568024953079282e-01;
    y( 5 ) = -9.589242746631385e-01;
    y( 6 ) = -2.794154981989259e-01;

    rbf.compute( x, xnew, dim );
    rbf.interpolate( y, ynew, dim );

    ASSERT_NEAR( 0.44626242465329269, ynew( 0 ), 1.0e-14 );
    ASSERT_NEAR( 0.98177874060900239, ynew( 1 ), 1.0e-14 );
    ASSERT_NEAR( 0.58634017875503497, ynew( 2 ), 1.0e-14 );
    ASSERT_NEAR( -0.34305440473301307, ynew( 3 ), 1.0e-14 );
    ASSERT_NEAR( -0.96344050977208218, ynew( 4 ), 1.0e-14 );
    ASSERT_NEAR( -0.65619975149611487, ynew( 5 ), 1.0e-14 );
    ASSERT_EQ( 1, ynew.cols() );
    ASSERT_EQ( 6, ynew.rows() );
    ASSERT_EQ( xnew.cols(), ynew.cols() );
    ASSERT_EQ( xnew.rows(), ynew.rows() );
}

TEST( RBFinterpolationTest, oneD_smallSamePosition )
{
    // Initialize variables

    RBF::RBFinterpolation rbf = RBF::RBFinterpolation();

    int dim = 1;
    int n_A = 7;
    int n_B = n_A;

    Eigen::VectorXd x( n_A ), y( n_A ), xnew( n_B ), ynew( n_B );

    x << 0, 1, 2, 3, 4, 5, 6;
    xnew = x;

    y( 0 ) = 0;
    y( 1 ) = 8.414709848078965e-01;
    y( 2 ) = 9.092974268256817e-01;
    y( 3 ) = 1.411200080598672e-01;
    y( 4 ) = -7.568024953079282e-01;
    y( 5 ) = -9.589242746631385e-01;
    y( 6 ) = -2.794154981989259e-01;

    rbf.compute( x, xnew, dim );
    rbf.interpolate( y, ynew, dim );

    for ( int i = 0; i < ynew.rows(); i++ )
        ASSERT_NEAR( y( i ), ynew( i ), 1.0e-14 );

    ASSERT_EQ( 1, ynew.cols() );
    ASSERT_EQ( 7, ynew.rows() );
    ASSERT_EQ( xnew.cols(), ynew.cols() );
    ASSERT_EQ( xnew.rows(), ynew.rows() );
}

TEST( RBFinterpolationTest, oneD_smallDoublePositions )
{
    // Initialize variables

    RBF::RBFinterpolation rbf = RBF::RBFinterpolation();

    int dim = 1;
    int n_A = 17;
    int n_B = 11;

    Eigen::VectorXd x( n_A ), y( n_A ), xnew( n_B ), ynew( n_B );

    // Location of original values

    x( 0 ) = 0;
    x( 1 ) = 0;
    x( 2 ) = 1;
    x( 3 ) = 1;
    x( 4 ) = 2;
    x( 5 ) = 2;
    x( 6 ) = 3;
    x( 7 ) = 3;
    x( 8 ) = 4;
    x( 9 ) = 4;
    x( 10 ) = 5;
    x( 11 ) = 5;
    x( 12 ) = 6;
    x( 13 ) = 6;
    x( 14 ) = 6;
    x( 15 ) = 6;
    x( 16 ) = 6;

    xnew( 0 ) = 0.5;
    xnew( 1 ) = 0.5;
    xnew( 2 ) = 1.5;
    xnew( 3 ) = 1.5;
    xnew( 4 ) = 2.5;
    xnew( 5 ) = 2.5;
    xnew( 6 ) = 3.5;
    xnew( 7 ) = 3.5;
    xnew( 8 ) = 4.5;
    xnew( 9 ) = 4.5;
    xnew( 10 ) = 5.5;

    // Values

    y( 0 ) = 0;
    y( 1 ) = 0;
    y( 2 ) = 8.414709848078965e-01;
    y( 3 ) = 8.414709848078965e-01;
    y( 4 ) = 9.092974268256817e-01;
    y( 5 ) = 9.092974268256817e-01;
    y( 6 ) = 1.411200080598672e-01;
    y( 7 ) = 1.411200080598672e-01;
    y( 8 ) = -7.568024953079282e-01;
    y( 9 ) = -7.568024953079282e-01;
    y( 10 ) = -9.589242746631385e-01;
    y( 11 ) = -9.589242746631385e-01;
    y( 12 ) = -2.794154981989259e-01;
    y( 13 ) = -2.794154981989259e-01;
    y( 14 ) = -2.794154981989259e-01;
    y( 15 ) = -2.794154981989259e-01;
    y( 16 ) = -2.794154981989259e-01;

    rbf.compute( x, xnew, dim );
    rbf.interpolate( y, ynew, dim );

    ASSERT_NEAR( 0.44626242465329269, ynew( 0 ), 1.0e-14 );
    ASSERT_NEAR( 0.44626242465329269, ynew( 1 ), 1.0e-14 );
    ASSERT_NEAR( 0.98177874060900239, ynew( 2 ), 1.0e-14 );
    ASSERT_NEAR( 0.98177874060900239, ynew( 3 ), 1.0e-14 );
    ASSERT_NEAR( 0.58634017875503497, ynew( 4 ), 1.0e-14 );
    ASSERT_NEAR( 0.58634017875503497, ynew( 5 ), 1.0e-14 );
    ASSERT_NEAR( -0.34305440473301307, ynew( 6 ), 1.0e-14 );
    ASSERT_NEAR( -0.34305440473301307, ynew( 7 ), 1.0e-14 );
    ASSERT_NEAR( -0.96344050977208218, ynew( 8 ), 1.0e-14 );
    ASSERT_NEAR( -0.96344050977208218, ynew( 9 ), 1.0e-14 );
    ASSERT_NEAR( -0.65619975149611487, ynew( 10 ), 1.0e-14 );
    ASSERT_EQ( 1, ynew.cols() );
    ASSERT_EQ( 11, ynew.rows() );
    ASSERT_EQ( xnew.cols(), ynew.cols() );
    ASSERT_EQ( xnew.rows(), ynew.rows() );
}

TEST( RBFinterpolationTest, radialBasisFunction )
{
    RBF::RBFinterpolation rbf = RBF::RBFinterpolation();

    ASSERT_EQ( rbf.linear( 5.6 ), 5.6 );
    ASSERT_EQ( rbf.linear( 1.532 ), 1.532 );
    ASSERT_EQ( rbf.linear( 0 ), 0 );

    ASSERT_NEAR( rbf.TPS( 2.6 ), 2.80521983228, 1.0e-11 );
    ASSERT_EQ( rbf.TPS( 0 ), 0 );

    ASSERT_NEAR( rbf.MQB( 2.6, 1.0 ), 2.78567765544, 1.0e-11 );
    ASSERT_EQ( rbf.MQB( 0, 1.0 ), 1 );

    ASSERT_NEAR( rbf.IMQB( 2.6, 1.0 ), 0.3589790793, 1.0e-11 );
    ASSERT_EQ( rbf.IMQB( 0, 1.0 ), 1 );

    ASSERT_NEAR( rbf.QB( 2.6 ), 7.76, 1.0e-15 );
    ASSERT_EQ( rbf.QB( 0 ), 1 );

    ASSERT_NEAR( rbf.IQB( 2.6 ), 0.12886597938, 1.0e-11 );
    ASSERT_EQ( rbf.IQB( 0 ), 1 );

    ASSERT_NEAR( rbf.Gauss( 2.6 ), 0.00115922917, 1.0e-11 );
    ASSERT_EQ( rbf.Gauss( 0 ), 1 );
}

TEST( RBFinterpolationTest, oneD_large )
{
    // Initialize variables

    RBF::RBFinterpolation rbf = RBF::RBFinterpolation();

    int dim = 1;
    int n_A = 7;
    int n_B = 11;

    Eigen::VectorXd x( n_A ), y( n_A ), xnew( n_B ), ynew( n_B );

    // Location of original values

    x( 0 ) = 0;
    x( 1 ) = 1;
    x( 2 ) = 2;
    x( 3 ) = 3;
    x( 4 ) = 4;
    x( 5 ) = 5;
    x( 6 ) = 6;

    xnew( 0 ) = 0.5;
    xnew( 1 ) = 1.5;
    xnew( 2 ) = 2.5;
    xnew( 3 ) = 3.5;
    xnew( 4 ) = 4.5;
    xnew( 5 ) = 5.5;
    xnew( 6 ) = 6.5;
    xnew( 7 ) = 7.5;
    xnew( 8 ) = 8.5;
    xnew( 9 ) = 9.5;
    xnew( 10 ) = 10.5;

    // Values

    y( 0 ) = 0;
    y( 1 ) = 8.414709848078965e-01;
    y( 2 ) = 9.092974268256817e-01;
    y( 3 ) = 1.411200080598672e-01;
    y( 4 ) = -7.568024953079282e-01;
    y( 5 ) = -9.589242746631385e-01;
    y( 6 ) = -2.794154981989259e-01;

    rbf.compute( x, xnew, dim );
    rbf.interpolate( y, ynew, dim );

    ASSERT_NEAR( 0.44626242465329269, ynew( 0 ), 1.0e-14 );
    ASSERT_NEAR( 0.98177874060900239, ynew( 1 ), 1.0e-14 );
    ASSERT_NEAR( 0.58634017875503497, ynew( 2 ), 1.0e-14 );
    ASSERT_NEAR( -0.34305440473301307, ynew( 3 ), 1.0e-14 );
    ASSERT_NEAR( -0.96344050977208218, ynew( 4 ), 1.0e-14 );
    ASSERT_NEAR( -0.65619975149611487, ynew( 5 ), 1.0e-14 );
    ASSERT_NEAR( -0.034247633730382976, ynew( 6 ), 1.0e-14 );
    ASSERT_NEAR( 0.30887921699372267, ynew( 7 ), 1.0e-14 );
    ASSERT_NEAR( 0.58723653375009, ynew( 8 ), 1.0e-14 );
    ASSERT_NEAR( 0.83915589224001175, ynew( 9 ), 1.0e-14 );
    ASSERT_NEAR( 1.0775427583214592, ynew( 10 ), 1.0e-14 );
    ASSERT_EQ( 1, ynew.cols() );
    ASSERT_EQ( 11, ynew.rows() );
    ASSERT_EQ( xnew.cols(), ynew.cols() );
    ASSERT_EQ( xnew.rows(), ynew.rows() );
}

TEST( RBFinterpolationTest, oneD_sin )
{
    // Initialize variables

    RBF::RBFinterpolation rbf = RBF::RBFinterpolation();

    int dim = 1;
    Eigen::VectorXd x( 9 ), y( 9 ), xnew( 101 ), ynew( 101 );
    Eigen::VectorXd fexpected( 101 );

    x << 0., 1.25, 2.5, 3.75, 5., 6.25, 7.5, 8.75, 10.;
    y << 0., 0.9489846193555862, 0.5984721441039564,
    -0.5715613187423437, -0.9589242746631385, -0.0331792165475568,
    0.9379999767747389, 0.6247239537541924, -0.5440211108893698;
    xnew << 0., 0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.7, 0.8, 0.9, 1., 1.1, 1.2,
    1.3, 1.4, 1.5, 1.6, 1.7, 1.8, 1.9, 2., 2.1, 2.2, 2.3, 2.4, 2.5, 2.6,
    2.7, 2.8, 2.9, 3., 3.1, 3.2, 3.3, 3.4, 3.5, 3.6, 3.7, 3.8, 3.9, 4.,
    4.1, 4.2, 4.3, 4.4, 4.5, 4.6, 4.7, 4.8, 4.9, 5., 5.1, 5.2, 5.3, 5.4,
    5.5, 5.6, 5.7, 5.8, 5.9, 6., 6.1, 6.2, 6.3, 6.4, 6.5, 6.6, 6.7, 6.8,
    6.9, 7., 7.1, 7.2, 7.3, 7.4, 7.5, 7.6, 7.7, 7.8, 7.9, 8., 8.1, 8.2,
    8.3, 8.4, 8.5, 8.6, 8.7, 8.8, 8.9, 9., 9.1, 9.2, 9.3, 9.4, 9.5, 9.6,
    9.7, 9.8, 9.9, 10.;

    fexpected << 0., 0.0759187695484469, 0.1518375390968947,
    0.2277563086453407, 0.3036750781937876, 0.379593847742234,
    0.4555126172906814, 0.5314313868391283, 0.6073501563875747,
    0.6832689259360221, 0.7591876954844685, 0.8351064650329159,
    0.9110252345813636, 0.9349641203455201, 0.9069231223253906,
    0.8788821243052602, 0.8508411262851294, 0.8228001282649999,
    0.7947591302448687, 0.7667181322247387, 0.7386771342046079,
    0.7106361361844775, 0.6825951381643476, 0.6545541401442172,
    0.6265131421240864, 0.5984721441039569, 0.5048694670762521,
    0.4112667900485483, 0.317664113020844, 0.2240614359931397,
    0.1304587589654362, 0.0368560819377319, -0.0567465950899719,
    -0.1503492721176762, -0.2439519491453801, -0.337554626173084,
    -0.4311573032007878, -0.5247599802284921, -0.5870558369791761,
    -0.6180448734528396, -0.6490339099265028, -0.6800229464001664,
    -0.7110119828738299, -0.7420010193474935, -0.7729900558211571,
    -0.8039790922948207, -0.8349681287684843, -0.8659571652421478,
    -0.8969462017158119, -0.9279352381894754, -0.9589242746631381,
    -0.8848646700138918, -0.8108050653646455, -0.7367454607153987,
    -0.6626858560661519, -0.5886262514169061, -0.5145666467676597,
    -0.440507042118413, -0.3664474374691666, -0.2923878328199199,
    -0.2183282281706738, -0.1442686235214272, -0.0702090188721807,
    0.0056679511853348, 0.0833622866511183, 0.1610566221169014,
    0.2387509575826854, 0.3164452930484689, 0.3941396285142531,
    0.4718339639800365, 0.5495282994458197, 0.6272226349116037,
    0.7049169703773872, 0.7826113058431711, 0.8603056413089549,
    0.9379999767747382, 0.9129378949330944, 0.8878758130914507,
    0.8628137312498069, 0.8377516494081637, 0.8126895675665196,
    0.787627485724876, 0.7625654038832328, 0.7375033220415884,
    0.7124412401999451, 0.6873791583583013, 0.6623170765166577,
    0.6372549946750136, 0.577974151168449, 0.4844745459969648,
    0.3909749408254795, 0.2974753356539959, 0.203975730482509,
    0.110476125311024, 0.01697652013954, -0.076523085031944,
    -0.1700226902034308, -0.2635222953749153, -0.3570219005464004,
    -0.4505215057178851, -0.5440211108893696;

    rbf.setFunction( "linear" );
    rbf.compute( x, xnew, dim );
    rbf.interpolate( y, ynew, dim );

    for ( int i = 0; i < fexpected.cols(); i++ )
        ASSERT_NEAR( fexpected( i ), ynew( i ), 1.0e-14 );

    ASSERT_EQ( xnew.cols(), ynew.cols() );
    ASSERT_EQ( xnew.rows(), ynew.rows() );

    fexpected << -8.8817841970012523e-15, 1.0667679590089563e-01, 2.0995773774815696e-01,
    3.0928375764320926e-01, 4.0408493576127213e-01, 4.9378647520253693e-01,
    5.7781527344405781e-01, 6.5560701593064330e-01, 7.2661370130280289e-01,
    7.9031149305178694e-01, 8.4620877932212224e-01, 8.9385431136892812e-01,
    9.3284528200488381e-01, 9.6283519835932019e-01, 9.8354139855451095e-01,
    9.9475205962094826e-01, 9.9633254429559948e-01, 9.8823093751890490e-01,
    9.7048262972985988e-01, 9.4321381376998836e-01, 9.0664377565274279e-01,
    8.6108587687625970e-01, 8.0694714750318575e-01, 7.4472643486142687e-01,
    6.7501108216771044e-01, 5.9847214410392979e-01, 5.1585818154126706e-01,
    4.2798771407626468e-01, 3.3574044543133752e-01, 2.4004741148691977e-01,
    1.4188023208589762e-01, 4.2239674101852032e-02, -5.7856246925840793e-02,
    -1.5738438729165516e-01, -2.5532857517214858e-01, -3.5069145348986375e-01,
    -4.4250584377141955e-01, -5.2984543251473326e-01, -6.1183461219951063e-01,
    -6.8765734536085787e-01, -7.5656496012077312e-01, -8.1788282701285553e-01,
    -8.7101590751607638e-01, -9.1545320215772641e-01, -9.5077115827340464e-01,
    -9.7663612278090639e-01, -9.9280594232588903e-01, -9.9913082111900664e-01,
    -9.9555354550221864e-01, -9.8210917413828902e-01, -9.5892427466313546e-01,
    -9.2621576313254828e-01, -8.8428937351485448e-01, -8.3353775304101418e-01,
    -7.7443814781811326e-01, -7.0754961421594675e-01, -6.3350966752037241e-01,
    -5.5303026238458219e-01, -4.6689299148723773e-01, -3.7594339084839667e-01,
    -2.8108425318476105e-01, -1.8326787457305294e-01, -8.3487193891388145e-02,
    1.7234172312154072e-02, 1.1785294673121527e-01, 2.1731815266346288e-01,
    3.1458385658960797e-01, 4.0862247475413582e-01, 4.9843837784211154e-01,
    5.8308148767837409e-01, 6.6166053109943856e-01, 7.3335560107932807e-01,
    7.9742967541262022e-01, 8.5323875948764716e-01, 9.0024035170780436e-01,
    9.3799997677469893e-01, 9.6619559127974330e-01, 9.8461973503690903e-01,
    9.9317937690921898e-01, 9.9189348173840752e-01, 9.8088840146500278e-01,
    9.6039126482106152e-01, 9.3072160267079873e-01, 8.9228149730292206e-01,
    8.4554458164132207e-01, 7.9104423715997818e-01, 7.2936134688661980e-01,
    6.6111195273317946e-01, 5.8693514575961636e-01, 5.0748148577275565e-01,
    4.2340220527940264e-01, 3.3533940495584691e-01, 2.4391739626552322e-01,
    1.4973529439788980e-01, 5.3360913818104372e-02, -4.4674028442939218e-02,
    -1.4387743835702071e-01, -2.4379917101312909e-01, -3.4403225671937676e-01,
    -4.4421314670994239e-01, -5.4402111088941041e-01;

    rbf = RBF::RBFinterpolation();
    rbf.setFunction( "multiquadric" );
    rbf.compute( x, xnew, dim );
    rbf.interpolate( y, ynew, dim );

    for ( int i = 0; i < fexpected.cols(); i++ )
        ASSERT_NEAR( fexpected( i ), ynew( i ), 1.0e-13 );

    ASSERT_EQ( xnew.cols(), ynew.cols() );
    ASSERT_EQ( xnew.rows(), ynew.rows() );

    fexpected << 3.5527136788005009e-15, 9.5265685623708407e-02, 1.9087472346790157e-01,
    2.8517990071119215e-01, 3.7716213375333751e-01, 4.6599330957092988e-01,
    5.5092149090730480e-01, 6.3121343457188317e-01, 7.0611222608313540e-01,
    7.7479293705624919e-01, 8.3629943098655168e-01, 8.8942515660724197e-01,
    9.3238945225965431e-01, 9.6105336697118382e-01, 9.7527920784249744e-01,
    9.7892704928691820e-01, 9.7348789609497643e-01, 9.5985958328990417e-01,
    9.3866029827249964e-01, 9.1033433257783969e-01, 8.7519615684356555e-01,
    8.3344542534863031e-01, 7.8515994954467327e-01, 7.3025864897693182e-01,
    6.6839277869148628e-01, 5.9847214410395821e-01, 5.1820275319291298e-01,
    4.3083948860956589e-01, 3.3844552264003003e-01, 2.4236770496700899e-01,
    1.4372549507809484e-01, 4.3538632685883982e-02, -5.7210510106510348e-02,
    -1.5753341252612962e-01, -2.5639309339964012e-01, -3.5264766618458587e-01,
    -4.4494841449540878e-01, -5.3146948969580388e-01, -6.0843263514255597e-01,
    -6.7535125739540591e-01, -7.3502395062961989e-01, -7.8827696908557021e-01,
    -8.3542442673542627e-01, -8.7652043364483800e-01, -9.1143752794070343e-01,
    -9.3989048480546056e-01, -9.6142959141787410e-01, -9.7540311356985354e-01,
    -9.8086686756246122e-01, -9.7635475304028652e-01, -9.5892427466313768e-01,
    -9.2322584596354806e-01, -8.7494480421076237e-01, -8.1736389251310637e-01,
    -7.5229880832559881e-01, -6.8105831992517618e-01, -6.0468076969801965e-01,
    -5.2403392466725851e-01, -4.3986609757952611e-01, -3.5283420960109901e-01,
    -2.6351738675692382e-01, -1.7241576626916089e-01, -7.9915372381090366e-02,
    1.3964088159838961e-02, 1.0862500128912489e-01, 2.0288394669823628e-01,
    2.9589950559322997e-01, 3.8690165342400595e-01, 4.7513444374485836e-01,
    5.5982608805492573e-01, 6.4016260299780015e-01, 7.1525578295608572e-01,
    7.8409489831348222e-01, 8.4545895263249449e-01, 8.9771431388107170e-01,
    9.3799997677474023e-01, 9.6143430361350868e-01, 9.7250228978180953e-01,
    9.7366536070787724e-01, 9.6615150739320543e-01, 9.5076649116250744e-01,
    9.2809195734793970e-01, 8.9856770287659926e-01, 8.6253239692659645e-01,
    8.2024415275112939e-01, 7.7188747241464639e-01, 7.1756420280781674e-01,
    6.5724364596354357e-01, 5.9044342156966023e-01, 5.1739839273244959e-01,
    4.3906507855081567e-01, 3.5596192191448162e-01, 2.6849880303887763e-01,
    1.7703956537653223e-01, 8.1927602193171245e-02, -1.6497819426557821e-02,
    -1.1788675883475125e-01, -2.2186025755566363e-01, -3.2798031993241406e-01,
    -4.3568382923553006e-01, -5.4402111088938165e-01;

    rbf = RBF::RBFinterpolation();
    rbf.setFunction( "thin_plate" );
    rbf.compute( x, xnew, dim );
    rbf.interpolate( y, ynew, dim );

    for ( int i = 0; i < fexpected.cols(); i++ )
        ASSERT_NEAR( fexpected( i ), ynew( i ), 1.0e-14 );

    ASSERT_EQ( xnew.cols(), ynew.cols() );
    ASSERT_EQ( xnew.rows(), ynew.rows() );
}
