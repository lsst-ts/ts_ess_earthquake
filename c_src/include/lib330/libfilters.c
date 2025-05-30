/*   Lib330 Filter Routines
     Copyright 2006 Certified Software Corporation

    This file is part of Lib330

    Lib330 is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    Lib330 is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Lib330; if not, write to the Free Software
    Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA

Edit History:
   Ed Date       By  Changes
   -- ---------- --- ---------------------------------------------------
    0 2006-10-11 rdr Created
    1 2007-08-04 rdr Some foolishness to get around gcc-avr32 optimizer bugs.
                     Add underflow detection for multi_section_filter for platforms
                     not corrected configured for "float-to-zero".
}
*/
#ifndef libfilters_h
#include "libfilters.h"
#endif

#ifndef OMIT_SEED
#ifndef libsampcfg_h
#include "libsampcfg.h"
#endif
#ifndef libdetect_h
#include "libdetect.h"
#endif
#ifndef libsupport_h
#include "libsupport.h"
#endif
#ifndef libmsgs_h
#include "libmsgs.h"
#endif
#ifndef libseed_h
#include "libseed.h"
#endif

typedef double tdec10[200] ;
typedef double tvlp389[389] ;
typedef double tulp379[379] ;
const tdec10 dec10 = {
   -1.28828092E-09,  9.14503405E-09,  2.87476904E-08,  7.11241767E-08,  1.51310061E-07,
    2.91624161E-07,  5.23189498E-07,  8.87843213E-07,  1.44005162E-06,  2.24865971E-06,
    3.39826488E-06,  4.98999634E-06,  7.14144197E-06,  9.98547874E-06,  1.36677381E-05,
    1.83425054E-05,  2.41668694E-05,  3.12930642E-05,  3.98589945E-05,  4.99771267E-05,
    6.17220211E-05,  7.51170082E-05,  9.01205857E-05,  1.06613370E-04,  1.24386483E-04,
    1.43132449E-04,  1.62439595E-04,  1.81791096E-04,  2.00569513E-04,  2.18067856E-04,
    2.33507438E-04,  2.46062962E-04,  2.54894577E-04,  2.59186228E-04,  2.58189102E-04,
    2.51268851E-04,  2.37953820E-04,  2.17982247E-04,  1.91345360E-04,  1.58323091E-04,
    1.19509517E-04,  7.58249953E-05,  2.85122951E-05, -2.08851106E-05, -7.05638885E-05,
   -1.18520940E-04, -1.62631870E-04, -2.00745591E-04, -2.30791746E-04, -2.50896090E-04,
   -2.59498571E-04, -2.55466904E-04, -2.38199442E-04, -2.07709323E-04, -1.64683297E-04,
   -1.10509143E-04, -4.72663596E-05,  2.23230290E-05,  9.49848763E-05,  1.67023521E-04,
    2.34488980E-04,  2.93374469E-04,  3.39835649E-04,  3.70420748E-04,  3.82298807E-04,
    3.73472488E-04,  3.42961138E-04,  2.90940603E-04,  2.18827306E-04,  1.29296604E-04,
    2.62285666E-05, -8.54221797E-05, -1.99831178E-04, -3.10577452E-04, -4.10974869E-04,
   -4.94446256E-04, -5.54921105E-04, -5.87234622E-04, -5.87504298E-04, -5.53458347E-04,
   -4.84693009E-04, -3.82836529E-04, -2.51602934E-04, -9.67231389E-05,  7.42515621E-05,
    2.52271566E-04,  4.27226506E-04,  5.88479800E-04,  7.25475081E-04,  8.28381569E-04,
    8.88742855E-04,  9.00087353E-04,  8.58461483E-04,  7.62846262E-04,  6.15422613E-04,
    4.21659147E-04,  1.90203253E-04, -6.74309048E-05, -3.37372737E-04, -6.04086031E-04,
   -8.51209566E-04, -1.06250588E-03, -1.22286635E-03, -1.31931377E-03, -1.34193967E-03,
   -1.28471490E-03, -1.14611199E-03, -9.29490662E-04, -6.43205188E-04, -3.00409156E-04,
    8.14499435E-05,  4.81434777E-04,  8.76190140E-04,  1.24123308E-03,  1.55239471E-03,
    1.78733305E-03,  1.92702783E-03,  1.95716135E-03,  1.86929386E-03,  1.66174676E-03,
    1.34011928E-03,  9.17386088E-04,  4.13541769E-04, -1.45211466E-04, -7.27711361E-04,
   -1.29946659E-03, -1.82459201E-03, -2.26794835E-03, -2.59736110E-03, -2.78579164E-03,
   -2.81331502E-03, -2.66877771E-03, -2.35100835E-03, -1.86948047E-03, -1.24435790E-03,
   -5.05877833E-04,  3.06923466E-04,  1.14810548E-03,  1.96725572E-03,  2.71234126E-03,
    3.33282724E-03,  3.78288654E-03,  4.02450701E-03,  4.03029890E-03,  3.78581485E-03,
    3.29120969E-03,  2.56210216E-03,  1.62953662E-03,  5.39001426E-04, -6.51497160E-04,
   -1.87423802E-03, -3.05534713E-03, -4.11894545E-03, -4.99166501E-03, -5.60728460E-03,
   -5.91120961E-03, -5.86451776E-03, -5.44730760E-03, -4.66110836E-03, -3.53014865E-03,
   -2.10134988E-03, -4.42970165E-04,  1.35810394E-03,  3.20033333E-03,  4.97261155E-03,
    6.56023062E-03,  7.85143300E-03,  8.74420720E-03,  9.15295816E-03,  9.01466794E-03,
    8.29416885E-03,  6.98819290E-03,  5.12791099E-03,  2.77973572E-03,  4.42597047E-05,
   -2.94670439E-03, -6.03491859E-03, -9.04218107E-03, -1.17781861E-02, -1.40494322E-02,
   -1.56687591E-02, -1.64650455E-02, -1.62925646E-02, -1.50395213E-02, -1.26352794E-02,
   -9.05589386E-03, -4.32758638E-03,  1.47205416E-03,  8.21529421E-03,  1.57279000E-02,
    2.37950831E-02,  3.21695320E-02,  4.05811667E-02,  4.87481765E-02,  5.63888475E-02,
    6.32336214E-02,  6.90367594E-02,  7.35871717E-02,  7.67176896E-02,  7.83125534E-02} ;
const tvlp389 vlp389 = {
    1.552566394584E-08,   3.838396805256E-08,   8.362779588160E-08,   1.611856650765E-07,   2.855294744823E-07,
    4.745457226814E-07,   7.493373613481E-07,   1.133592677088E-06,   1.652402275925E-06,   2.330427428518E-06 ,
    3.189336982250E-06,   4.244466110163E-06,   5.500718985138E-06,   6.947785187742E-06,   8.554818995995E-06,
    1.026484432301E-05,   1.198920679926E-05,   1.360249119436E-05,   1.493842691710E-05,   1.578731698718E-05 ,
    1.589559564507E-05,   1.496809882671E-05,   1.267355442171E-05,   8.653767865499E-06,   2.536720154462E-06,
   -6.046302489149E-06,  -1.743978324789E-05,  -3.193970873875E-05,  -4.976679353047E-05,  -7.103832499508E-05 ,
   -9.574009270878E-05,  -1.237001281373E-04,  -1.545662057863E-04,  -1.877891685389E-04,  -2.226141463211E-04,
   -2.580816070830E-04,  -2.930398897929E-04,  -3.261704417989E-04,  -3.560263903179E-04,  -3.810843492955E-04 ,
   -3.998085424451E-04,  -4.107254096236E-04,  -4.125059559304E-04,  -4.040522153346E-04,  -3.845834135837E-04,
   -3.537168195160E-04,  -3.115378713577E-04,  -2.586540951013E-04,  -1.962275488594E-04,  -1.259811477109E-04 ,
   -5.017518265277E-05,   2.844831201260E-05,   1.067540572146E-04,   1.813337070203E-04,   2.486544777587E-04,
    3.052309010550E-04,   3.478123642856E-04,   3.735773180385E-04,   3.803236007367E-04,   3.666434036282E-04 ,
    3.320710779809E-04,   2.771923601331E-04,   2.037047230356E-04,   1.144204406806E-04,   1.320656090328E-05,
   -9.514078807411E-05,  -2.050866145468E-04,  -3.106066344345E-04,  -4.054869377349E-04,  -4.836605629994E-04 ,
   -5.395643227932E-04,  -5.684964442834E-04,  -5.669537730318E-04,  -5.329266106907E-04,  -4.661298988553E-04,
   -3.681514720867E-04,  -2.425015060503E-04,  -9.455197026659E-05,   6.863931335717E-05,   2.386187747019E-04 ,
    4.059330165016E-04,   5.606150098640E-04,   6.927381973863E-04,   7.930101501160E-04,   8.533727144001E-04,
    8.675725693010E-04,   8.316651094423E-04,   7.444158080483E-04,   6.075667962541E-04,   4.259422734851E-04 ,
    2.073743534022E-04,  -3.755930204741E-05,  -2.959836739051E-04,  -5.533463103089E-04,  -7.941831891721E-04,
   -1.002993788038E-03,  -1.165179186662E-03,  -1.267989808042E-03,  -1.301425007313E-03,  -1.259025639791E-03 ,
   -1.138503279404E-03,  -9.421560123941E-04,  -6.770305674248E-04,  -3.548035850963E-04,   8.629525406917E-06,
    3.938522550447E-04,   7.788870544391E-04,   1.140375685799E-03,   1.454921250676E-03,   1.700519338554E-03 ,
    1.857995436810E-03,   1.912359959705E-03,   1.853991651826E-03,   1.679565073285E-03,   1.392648421779E-03,
    1.003913787737E-03,   5.309224020114E-04,  -2.528464426306E-06,  -5.674838557405E-04,  -1.131294383117E-03 ,
   -1.659397590689E-03,  -2.117326771743E-03,  -2.472836888205E-03,  -2.698023247754E-03,  -2.771301493786E-03,
   -2.679118119865E-03,  -2.417269551530E-03,  -1.991724770718E-03,  -1.418870913453E-03,  -7.251321588955E-04 ,
    5.405199843135E-05,   8.758642868356E-04,   1.692298775680E-03,   2.452799616041E-03,   3.107213842280E-03,
    3.608895501699E-03,   3.917779673295E-03,   4.003236312754E-03,   3.846516408026E-03,   3.442617091426E-03 ,
    2.801417900821E-03,   1.947976354023E-03,   9.219157645641E-04,  -2.241104816572E-04,  -1.426838923738E-03,
   -2.615605374573E-03,  -3.716195765456E-03,  -4.655143758048E-03,  -5.364242712043E-03,  -5.785013832068E-03 ,
   -5.872860607418E-03,  -5.600643400193E-03,  -4.961427762851E-03,  -3.970195379824E-03,  -2.664356080425E-03 ,
   -1.102960913674E-03,   6.354132582377E-04,   2.456061522632E-03,   4.252708231777E-03,   5.913027807411E-03 ,
    7.324867533633E-03,   8.382857359878E-03,   8.995053809435E-03,   9.089245553003E-03,   8.618548933728E-03 ,
    7.565943496045E-03,   5.947439964943E-03,   3.813634534079E-03,   1.249480992454E-03,  -1.627797630402E-03 ,
   -4.672638210191E-03,  -7.716787115331E-03,  -1.057645210709E-02,  -1.306064905261E-02,  -1.498036021034E-02 ,
   -1.615806333373E-02,  -1.643715526114E-02,  -1.569078258614E-02,  -1.382960671136E-02,  -1.080807094477E-02 ,
   -6.628801743854E-03,  -1.344861825153E-03,   4.940324525219E-03,   1.207543915120E-02,   1.986607558565E-02 ,
    2.808170739344E-02,   3.646453116726E-02,   4.473979600604E-02,   5.262714980923E-02,   5.985247435163E-02 ,
    6.615964924099E-02,   7.132168148082E-02,   7.515066291015E-02,   7.750607115759E-02,   7.830100830038E-02 ,
    7.750607115759E-02,   7.515066291015E-02,   7.132168148082E-02,   6.615964924099E-02,   5.985247435163E-02 ,
    5.262714980923E-02,   4.473979600604E-02,   3.646453116726E-02,   2.808170739344E-02,   1.986607558565E-02 ,
    1.207543915120E-02,   4.940324525219E-03,  -1.344861825153E-03,  -6.628801743854E-03,  -1.080807094477E-02 ,
   -1.382960671136E-02,  -1.569078258614E-02,  -1.643715526114E-02,  -1.615806333373E-02,  -1.498036021034E-02 ,
   -1.306064905261E-02,  -1.057645210709E-02,  -7.716787115331E-03,  -4.672638210191E-03,  -1.627797630402E-03 ,
    1.249480992454E-03,   3.813634534079E-03,   5.947439964943E-03,   7.565943496045E-03,   8.618548933728E-03 ,
    9.089245553003E-03,   8.995053809435E-03,   8.382857359878E-03,   7.324867533633E-03,   5.913027807411E-03 ,
    4.252708231777E-03,   2.456061522632E-03,   6.354132582377E-04,  -1.102960913674E-03,  -2.664356080425E-03 ,
   -3.970195379824E-03,  -4.961427762851E-03,  -5.600643400193E-03,  -5.872860607418E-03,  -5.785013832068E-03 ,
   -5.364242712043E-03,  -4.655143758048E-03,  -3.716195765456E-03,  -2.615605374573E-03,  -1.426838923738E-03 ,
   -2.241104816572E-04,   9.219157645641E-04,   1.947976354023E-03,   2.801417900821E-03,   3.442617091426E-03 ,
    3.846516408026E-03,   4.003236312754E-03,   3.917779673295E-03,   3.608895501699E-03,   3.107213842280E-03 ,
    2.452799616041E-03,   1.692298775680E-03,   8.758642868356E-04,   5.405199843135E-05,  -7.251321588955E-04 ,
   -1.418870913453E-03,  -1.991724770718E-03,  -2.417269551530E-03,  -2.679118119865E-03,  -2.771301493786E-03 ,
   -2.698023247754E-03,  -2.472836888205E-03,  -2.117326771743E-03,  -1.659397590689E-03,  -1.131294383117E-03 ,
   -5.674838557405E-04,  -2.528464426306E-06,   5.309224020114E-04,   1.003913787737E-03,   1.392648421779E-03 ,
    1.679565073285E-03,   1.853991651826E-03,   1.912359959705E-03,   1.857995436810E-03,   1.700519338554E-03 ,
    1.454921250676E-03,   1.140375685799E-03,   7.788870544391E-04,   3.938522550447E-04,   8.629525406917E-06 ,
   -3.548035850963E-04,  -6.770305674248E-04,  -9.421560123941E-04,  -1.138503279404E-03,  -1.259025639791E-03 ,
   -1.301425007313E-03,  -1.267989808042E-03,  -1.165179186662E-03,  -1.002993788038E-03,  -7.941831891721E-04 ,
   -5.533463103089E-04,  -2.959836739051E-04,  -3.755930204741E-05,   2.073743534022E-04,   4.259422734851E-04 ,
    6.075667962541E-04,   7.444158080483E-04,   8.316651094423E-04,   8.675725693010E-04,   8.533727144001E-04 ,
    7.930101501160E-04,   6.927381973863E-04,   5.606150098640E-04,   4.059330165016E-04,   2.386187747019E-04 ,
    6.863931335717E-05,  -9.455197026659E-05,  -2.425015060503E-04,  -3.681514720867E-04,  -4.661298988553E-04 ,
   -5.329266106907E-04,  -5.669537730318E-04,  -5.684964442834E-04,  -5.395643227932E-04,  -4.836605629994E-04 ,
   -4.054869377349E-04,  -3.106066344345E-04,  -2.050866145468E-04,  -9.514078807411E-05,   1.320656090328E-05 ,
    1.144204406806E-04,   2.037047230356E-04,   2.771923601331E-04,   3.320710779809E-04,   3.666434036282E-04 ,
    3.803236007367E-04,   3.735773180385E-04,   3.478123642856E-04,   3.052309010550E-04,   2.486544777587E-04 ,
    1.813337070203E-04,   1.067540572146E-04,   2.844831201260E-05,  -5.017518265277E-05,  -1.259811477109E-04 ,
   -1.962275488594E-04,  -2.586540951013E-04,  -3.115378713577E-04,  -3.537168195160E-04,  -3.845834135837E-04 ,
   -4.040522153346E-04,  -4.125059559304E-04,  -4.107254096236E-04,  -3.998085424451E-04,  -3.810843492955E-04 ,
   -3.560263903179E-04,  -3.261704417989E-04,  -2.930398897929E-04,  -2.580816070830E-04,  -2.226141463211E-04 ,
   -1.877891685389E-04,  -1.545662057863E-04,  -1.237001281373E-04,  -9.574009270878E-05,  -7.103832499508E-05 ,
   -4.976679353047E-05,  -3.193970873875E-05,  -1.743978324789E-05,  -6.046302489149E-06,   2.536720154462E-06 ,
    8.653767865499E-06,   1.267355442171E-05,   1.496809882671E-05,   1.589559564507E-05,   1.578731698718E-05 ,
    1.493842691710E-05,   1.360249119436E-05,   1.198920679926E-05,   1.026484432301E-05,   8.554818995995E-06 ,
    6.947785187742E-06,   5.500718985138E-06,   4.244466110163E-06,   3.189336982250E-06,   2.330427428518E-06 ,
    1.652402275925E-06,   1.133592677088E-06,   7.493373613481E-07,   4.745457226814E-07,   2.855294744823E-07 ,
    1.611856650765E-07,   8.362779588160E-08,   3.838396805256E-08,   1.552566394584E-08} ;
const tulp379 ulp379 = {
    1.026451784988E-08,   1.180827438263E-08,   1.222270072841E-08,   1.784569043697E-09,  -3.171089438061E-08 ,
   -1.077291467270E-07,  -2.554953559066E-07,  -5.168406471238E-07,  -9.492918609426E-07,  -1.629243835370E-06,
   -2.654991487515E-06,  -4.149339231814E-06,  -6.261453784789E-06,  -9.167581524892E-06,  -1.307022327112E-05,
   -1.819535922437E-05,  -2.478735261764E-05,  -3.310122477396E-05,  -4.339209432774E-05,  -5.590172080761E-05 ,
   -7.084228458081E-05,  -8.837774901527E-05,  -1.086033867683E-04,  -1.315243117997E-04,  -1.570341146662E-04,
   -1.848949188490E-04,  -2.147203583052E-04,  -2.459631087719E-04,  -2.779086352362E-04,  -3.096767386907E-04,
   -3.402323135044E-04,  -3.684064187493E-04,  -3.929283049387E-04,  -4.124684933983E-04,  -4.256923594327E-04 ,
   -4.313229018453E-04,  -4.282106262745E-04,  -4.154077269900E-04,  -3.922429908564E-04,  -3.583933008473E-04,
   -3.139471951601E-04,  -2.594556781198E-04,  -1.959656613106E-04,  -1.250316495320E-04,  -4.870211218377E-05,
    3.052211243450E-05,   1.097586447595E-04,   1.858433664932E-04,   2.554571595010E-04,   3.152757039169E-04 ,
    3.621366034745E-04,   3.932163833021E-04,   4.062084327728E-04,   3.994918660218E-04,   3.722806580722E-04,
    3.247423843257E-04,   2.580765172940E-04,   1.745435403532E-04,   7.743812787763E-05,  -2.899773516609E-05,
   -1.397226672272E-04,  -2.491022698159E-04,  -3.511676454733E-04,  -4.399159776456E-04,  -5.096395949121E-04 ,
   -5.552669179358E-04,  -5.726963932934E-04,  -5.591031729212E-04,  -5.131980615309E-04,  -4.354192598725E-04,
   -3.280397408896E-04,  -1.951766613779E-04,  -4.269394694667E-05,   1.220050996668E-04,   2.902919216533E-04,
    4.527235473565E-04,   5.995334002354E-04,   7.211790815244E-04,   8.089187416563E-04,   8.553836106716E-04 ,
    8.551118755616E-04,   8.050086636132E-04,   7.046986521457E-04,   5.567417551315E-04,   3.666884759904E-04,
    1.429595323193E-04,  -1.034559890014E-04,  -3.594789754338E-04,  -6.105789107604E-04,  -8.415454811974E-04,
   -1.037355654481E-03,  -1.184090091542E-03,  -1.269846355483E-03,  -1.285592795306E-03,  -1.225906639409E-03 ,
   -1.089542985518E-03,  -8.797880768168E-04,  -6.045603413930E-04,  -2.762357484243E-04,   8.881051943921E-05,
    4.709370574344E-04,   8.482172520524E-04,   1.197624514067E-03,   1.496362338092E-03,   1.723266866410E-03,
    1.860200354469E-03,   1.893349127932E-03,   1.814339889434E-03,   1.621093889591E-03,   1.318349511609E-03 ,
    9.177999012550E-04,   4.378127047711E-04,  -9.727724235811E-05,  -6.582855701364E-04,  -1.212663227346E-03,
   -1.726279974542E-03,  -2.165415224119E-03,  -2.498846476193E-03,  -2.699912806241E-03,  -2.748424901329E-03,
   -2.632294775655E-03,  -2.348767885561E-03,  -1.905157797142E-03,  -1.319008184603E-03,  -6.176376947216E-04 ,
    1.629414948190E-04,   9.797026440298E-04,   1.784813789684E-03,   2.528273803602E-03,   3.160839294628E-03,
    3.637080725446E-03,   3.918388793951E-03,   3.975744765012E-03,   3.792072035007E-03,   3.364001152506E-03,
    2.702906508667E-03,   1.835108966987E-03,   8.011831689895E-04,  -3.456411119405E-04,  -1.541940144637E-03 ,
   -2.717322124106E-03,  -3.798268482339E-03,  -4.712396050860E-03,  -5.392909415564E-03,  -5.782988553657E-03,
   -5.839846472553E-03,  -5.538196386724E-03,  -4.872888431900E-03,  -3.860511584446E-03,  -2.539805943971E-03 ,
   -9.707916038829E-04,   7.674100682562E-04,   2.580012466451E-03,   4.361131629765E-03,   5.999288918806E-03,
    7.383587747446E-03,   8.410251879376E-03,   8.989176473957E-03,   9.050124884710E-03,   8.548205984557E-03 ,
    7.468289206184E-03,   5.828057093995E-03,   3.679456341875E-03,   1.108385277773E-03,  -1.767455172892E-03,
   -4.802523432728E-03,  -7.829089530977E-03,  -1.066435913962E-02,  -1.311876335018E-02,  -1.500503421626E-02 ,
   -1.614763018295E-02,  -1.639204126750E-02,  -1.561349373078E-02,  -1.372458920009E-02,  -1.068145360293E-02,
   -6.488035284000E-03,  -1.198276407116E-03,   5.084016923480E-03,   1.220766753683E-02,   1.997892624731E-02 ,
    2.816840227991E-02,   3.651983914056E-02,   4.476035176393E-02,   5.261166083662E-02,   5.980180304551E-02,
    6.607676494603E-02,   7.121148510555E-02,   7.501969457590E-02,   7.736211848923E-02,   7.815263905001E-02 ,
    7.736211848923E-02,   7.501969457590E-02,   7.121148510555E-02,   6.607676494603E-02,   5.980180304551E-02,
    5.261166083662E-02,   4.476035176393E-02,   3.651983914056E-02,   2.816840227991E-02,   1.997892624731E-02 ,
    1.220766753683E-02,   5.084016923480E-03,  -1.198276407116E-03,  -6.488035284000E-03,  -1.068145360293E-02,
   -1.372458920009E-02,  -1.561349373078E-02,  -1.639204126750E-02,  -1.614763018295E-02,  -1.500503421626E-02 ,
   -1.311876335018E-02,  -1.066435913962E-02,  -7.829089530977E-03,  -4.802523432728E-03,  -1.767455172892E-03 ,
    1.108385277773E-03,   3.679456341875E-03,   5.828057093995E-03,   7.468289206184E-03,   8.548205984557E-03 ,
    9.050124884710E-03,   8.989176473957E-03,   8.410251879376E-03,   7.383587747446E-03,   5.999288918806E-03 ,
    4.361131629765E-03,   2.580012466451E-03,   7.674100682562E-04,  -9.707916038829E-04,  -2.539805943971E-03 ,
   -3.860511584446E-03,  -4.872888431900E-03,  -5.538196386724E-03,  -5.839846472553E-03,  -5.782988553657E-03 ,
   -5.392909415564E-03,  -4.712396050860E-03,  -3.798268482339E-03,  -2.717322124106E-03,  -1.541940144637E-03 ,
   -3.456411119405E-04,   8.011831689895E-04,   1.835108966987E-03,   2.702906508667E-03,   3.364001152506E-03 ,
    3.792072035007E-03,   3.975744765012E-03,   3.918388793951E-03,   3.637080725446E-03,   3.160839294628E-03 ,
    2.528273803602E-03,   1.784813789684E-03,   9.797026440298E-04,   1.629414948190E-04,  -6.176376947216E-04 ,
   -1.319008184603E-03,  -1.905157797142E-03,  -2.348767885561E-03,  -2.632294775655E-03,  -2.748424901329E-03 ,
   -2.699912806241E-03,  -2.498846476193E-03,  -2.165415224119E-03,  -1.726279974542E-03,  -1.212663227346E-03 ,
   -6.582855701364E-04,  -9.727724235811E-05,   4.378127047711E-04,   9.177999012550E-04,   1.318349511609E-03 ,
    1.621093889591E-03,   1.814339889434E-03,   1.893349127932E-03,   1.860200354469E-03,   1.723266866410E-03 ,
    1.496362338092E-03,   1.197624514067E-03,   8.482172520524E-04,   4.709370574344E-04,   8.881051943921E-05 ,
   -2.762357484243E-04,  -6.045603413930E-04,  -8.797880768168E-04,  -1.089542985518E-03,  -1.225906639409E-03 ,
   -1.285592795306E-03,  -1.269846355483E-03,  -1.184090091542E-03,  -1.037355654481E-03,  -8.415454811974E-04 ,
   -6.105789107604E-04,  -3.594789754338E-04,  -1.034559890014E-04,   1.429595323193E-04,   3.666884759904E-04 ,
    5.567417551315E-04,   7.046986521457E-04,   8.050086636132E-04,   8.551118755616E-04,   8.553836106716E-04 ,
    8.089187416563E-04,   7.211790815244E-04,   5.995334002354E-04,   4.527235473565E-04,   2.902919216533E-04 ,
    1.220050996668E-04,  -4.269394694667E-05,  -1.951766613779E-04,  -3.280397408896E-04,  -4.354192598725E-04 ,
   -5.131980615309E-04,  -5.591031729212E-04,  -5.726963932934E-04,  -5.552669179358E-04,  -5.096395949121E-04 ,
   -4.399159776456E-04,  -3.511676454733E-04,  -2.491022698159E-04,  -1.397226672272E-04,  -2.899773516609E-05 ,
    7.743812787763E-05,   1.745435403532E-04,   2.580765172940E-04,   3.247423843257E-04,   3.722806580722E-04 ,
    3.994918660218E-04,   4.062084327728E-04,   3.932163833021E-04,   3.621366034745E-04,   3.152757039169E-04 ,
    2.554571595010E-04,   1.858433664932E-04,   1.097586447595E-04,   3.052211243450E-05,  -4.870211218377E-05 ,
   -1.250316495320E-04,  -1.959656613106E-04,  -2.594556781198E-04,  -3.139471951601E-04,  -3.583933008473E-04 ,
   -3.922429908564E-04,  -4.154077269900E-04,  -4.282106262745E-04,  -4.313229018453E-04,  -4.256923594327E-04 ,
   -4.124684933983E-04,  -3.929283049387E-04,  -3.684064187493E-04,  -3.402323135044E-04,  -3.096767386907E-04 ,
   -2.779086352362E-04,  -2.459631087719E-04,  -2.147203583052E-04,  -1.848949188490E-04,  -1.570341146662E-04 ,
   -1.315243117997E-04,  -1.086033867683E-04,  -8.837774901527E-05,  -7.084228458081E-05,  -5.590172080761E-05 ,
   -4.339209432774E-05,  -3.310122477396E-05,  -2.478735261764E-05,  -1.819535922437E-05,  -1.307022327112E-05 ,
   -9.167581524892E-06,  -6.261453784789E-06,  -4.149339231814E-06,  -2.654991487515E-06,  -1.629243835370E-06 ,
   -9.492918609426E-07,  -5.168406471238E-07,  -2.554953559066E-07,  -1.077291467270E-07,  -3.171089438061E-08 ,
    1.784569043697E-09,   1.222270072841E-08,   1.180827438263E-08,   1.026451784988E-08} ;

void load_firfilters (pq330 q330, paqstruc paqs)
begin
  pfilter ic ;
  integer i, cnt, flen ;

/* standard filter DEC10 */
  getthrbuf (q330, addr(paqs->firchain), sizeof(tfilter)) ;
  ic = paqs->firchain ;
  ic->link = NIL ;
  strcpy(addr(ic->fname), "DEC10") ; ;
  ic->len = 400 ;
  flen = 400 ;
  ic->gain = 1.0 ;
  ic->dec = 10 ;
  ic->fir_num = 0 ;
  cnt = (integer)ic->len div 2 ;
  ic->dly = (ic->len - 1) / 2.0 ;
  for (i = 1 ; i <= cnt ; i++)
    begin
      ic->coef[i - 1] = dec10[i - 1] ;
      ic->coef[flen - i] = dec10[i - 1] ;
    end
/* standard filter VLP389 */
  getthrbuf (q330, addr(ic->link), sizeof(tfilter)) ;
  ic = ic->link ;
  ic->link = NIL ;
  strcpy(addr(ic->fname), "VLP389") ;
  ic->len = 389 ;
  ic->gain = 1.0 ;
  ic->dec = 10 ;
  ic->fir_num = 1 ;
  cnt = (integer)ic->len ;
  ic->dly = 194 ;
  for (i = 1 ; i <= cnt ; i++)
    ic->coef[i - 1] = vlp389[i - 1] ;
/* standard filter ULP379 */
  getthrbuf (q330, addr(ic->link), sizeof(tfilter)) ;
  ic = ic->link ;
  ic->link = NIL ;
  strcpy(addr(ic->fname), "ULP379") ;
  ic->len = 379 ;
  ic->gain = 1.0 ;
  ic->dec = 10 ;
  ic->fir_num = 2 ;
  cnt = (integer)ic->len ;
  ic->dly = 189 ;
  for (i = 1 ; i <= cnt ; i++)
    ic->coef[i - 1] = ulp379[i - 1] ;
end

/* append client defined filters to standard list */
void append_firfilters (pq330 q330, paqstruc paqs, pfilter src)
begin
  pfilter dest ;

  dest = paqs->firchain ;
  /* find last of the standard bunch */
  while (dest->link)
    dest = dest->link ;
  while (src)
    begin
      getthrbuf (q330, addr(dest->link), sizeof(tfilter)) ;
      memcpy(dest->link, src, sizeof(tfilter)) ; /* make copy */
      dest = dest->link ;
      dest->link = NIL ;
      src = src->link ;
    end
end

pfilter find_fir (paqstruc paqs, byte num)
begin
  pfilter src ;

  src = paqs->firchain ;
  while (src)
    if (num == src->fir_num)
      then
        return src ;
      else
        src = src->link ;
  return NIL ;
end

pfir_packet create_fir (pq330 q330, pfilter src)
begin
  pfir_packet pf ;

  getbuf (q330, addr(pf), sizeof(tfir_packet)) ;
  getbuf (q330, addr(pf->fbuf), src->len * sizeof (tfloat)) ;
  pf->f = pf->fbuf ;
  pf->fcoef = addr(src->coef) ;
  pf->flen = src->len ;
  pf->fdec = src->dec ;
  pf->fcount = 0 ;
  while (pf->fcount < (pf->flen - 1)) /* fill to len-1 */
    begin
      *(pf->f) = 0.0 ;
      inc(pf->f) ;
      inc(pf->fcount) ;
    end
  return pf ;
end

piirfilter create_iir (pq330 q330, piirdef src, integer points)
begin
  piirfilter pf ;
  word extra ;
  integer i, j ;

  if (points > 2)
    then
      extra = (points - 1) * sizeof(tfloat) ;
    else
      extra = 0 ;
  getbuf (q330, addr(pf), sizeof (tiirfilter) + extra) ;
  for (i = 1 ; i <= src->sects ; i++)
    begin
      pf->filt[i].poles = src->filt[i].poles ;
      for (j = 0 ; j <= src->filt[i].poles ; j++)
        begin
          pf->filt[i].x[j] = 0.0 ;
          pf->filt[i].y[j] = 0.0 ;
          if (i == 1)
            then
              pf->filt[i].a[j] = src->filt[i].a[j] * src->gain ;
            else
              pf->filt[i].a[j] = src->filt[i].a[j] ;
          pf->filt[i].b[j] = src->filt[i].b[j] ;
        end
    end
  pf->packet_size = sizeof(tiirfilter) + extra ;
  pf->link = NIL ;
  pf->def = src ;
  pf->sects = src->sects ;
  return pf ;
end

void allocate_lcq_filters (paqstruc paqs, plcq q)
begin
  integer points ;
  pdet_packet pdp ;
  piirfilter pi ;
  pq330 q330 ;

  q330 = paqs->owner ;
  if (q->rate > 0)
    then
      points = q->rate ;
    else
      points = 1 ;
  if (q->avg_source)
    then
      begin
        q->avg_filt = create_iir (q330, q->avg_source, points) ;
        q->stream_iir = q->avg_filt ;
      end
  pdp = q->det ;
  while (pdp)
    begin
      inc(paqs->total_detectors) ;
      if (pdp->detector_def->detfilt)
        then
          begin /* need an IIR filter */
            pi = q->stream_iir ;
            while (pi)
              if (pi->def == pdp->detector_def->detfilt)
                then
                  break ;
                else
                  pi = pi->link ;
            if (pi == NIL)
              then
                begin
                  pi = create_iir (q330, pdp->detector_def->detfilt, points) ;
                  if (pi)
                    then
                      q->stream_iir = extend_link (q->stream_iir, pi) ;
                end
            initialize_detector (q330, pdp, pi) ;
          end
        else
          initialize_detector (q330, pdp, NIL) ;
      pdp = pdp->link ;
    end
  if (q->source_fir)
    then
      q->fir = create_fir (q330, q->source_fir) ;
end

void average (paqstruc paqs, pavg_packet pavg, tfloat s, tfloat samp, plcq q)
begin
  string95 s1 ;
  string15 ss ;
  pq330 q330 ;

  q330 = paqs->owner ;
  pavg->signed_sum = pavg->signed_sum + s ; /* update sum */
  pavg->sqr_sum = pavg->sqr_sum + samp * samp ; /* update mean-sqr */
  if (fabs(samp) > pavg->peak_abs)
    then
      pavg->peak_abs = fabs(samp) ;
  inc(pavg->avg_count) ;
  if (pavg->avg_count >= q->avg_length)
    then
      begin
        pavg->running_avg = pavg->signed_sum / q->avg_length ;
        seed2string(q->location, q->seedname, addr(ss)) ;
        sprintf(s1, "%s:%12.3f%12.3f%12.3f", ss,
                pavg->running_avg, sqrt(pavg->sqr_sum / q->avg_length), pavg->peak_abs) ;
        libdatamsg(q330, LIBMSG_AVG, addr(s1)) ;
        pavg->avg_count = 0 ;
        pavg->signed_sum = 0.0 ;
        pavg->sqr_sum = 0.0 ;
        pavg->peak_abs = 0.0 ;
      end
end

tfloat mac_and_shift (pfir_packet pf)
begin
  longint i ;
  tfloat accum ;
  pfloat pv, pc ;

  accum = 0.0 ;
  pv = pf->fbuf ;
  pc = pf->fcoef ;
  for (i = 0 ; i <= pf->flen - 1 ; i++)
    begin
      accum = (*pv) * (*pc) + accum ;
      inc(pv) ;
      inc(pc) ;
    end
  pc = pf->fbuf ;
  pv = pf->fbuf ;
  incn(pv, pf->fdec) ;
  for (i = pf->fdec ; i <= pf->flen - 1 ; i++)
    begin
      *pc = *pv ;
      inc(pc) ;
      inc(pv) ;
    end
  return accum ;
end

piirdef find_iir (paqstruc paqs, byte num)
begin
  piirdef src ;

  src = paqs->iirchain ;
  while (src)
    if (num == src->iir_num)
      then
        return src ;
      else
        src = src->link ;
  return NIL ;
end

static tfloat scalar_product (tvector *a, tvector *b, integer vector_length, integer offset)
begin
  integer counter ;
  double multiply_accum ;

  multiply_accum = 0 ;
  for (counter = offset ; counter <= (vector_length + offset - 1) ; counter++)
    begin
#ifdef CHK_IIR_UNDERFLOW
    if (fabs((*b)[counter]) < 1.0E-20)
      (*b)[counter] = 0.0 ;
#endif
      multiply_accum = multiply_accum + (*a)[counter] * (*b)[counter] ;
    end
  return multiply_accum ;
end

static void time_shift (tvector *filter_history, integer length, integer shift)
begin
  integer index ;

  length = length - shift ;
  index = length - 1 ;
  repeat
    (*filter_history)[index + shift] = (*filter_history)[index] ;
    dec(index) ;
    dec(length) ;
  until (length == 0)) ;
end

static double recursive_filter (tiirsection *f, double s)
begin

  f->x[0] = s ;
  f->y[0] = scalar_product (addr(f->x) , addr(f->a) , f->poles+1, 0)
            + scalar_product (addr(f->y) , addr(f->b) , f->poles, 1) ;
  time_shift (addr(f->x), f->poles + 1, 1) ;
  time_shift (addr(f->y), f->poles + 1, 1) ;
  return f->y[0] ;
end

double multi_section_filter (piirfilter resp, double s)
begin
  integer sect ;

  if (resp->sects > 0)
    then
      for (sect = 1 ; sect <= resp->sects ; sect++)
        s = recursive_filter (addr(resp->filt[sect]), s) ;
#ifdef CHK_IIR_UNDERFLOW
  if (fabs(s) < 1.0E-20)
    then
      if (s > 0)
        then
          s = 1.0E-20 ;
        else
          s = -1.0E-20 ;
#endif
  return s ;
end

static double factorial (integer npoles, integer index)
begin
  integer i, j, nmi ;
  double value, factn, facti, factnmi ;

  i = index - 1 ;
  value = 0.0 ;
  if (i <= npoles)
    then
      begin
        factn = 1.0 ;
        facti = 1.0 ;
        factnmi = 1.0 ;
        for (j = 1 ; j <= npoles ; j++)
          factn = j * factn ;
        if (i > 0)
          then
            for (j = 1 ; j <= i ; j++)
              facti = j * facti ;
        nmi = npoles - i;
        if (nmi > 0)
          then
            for (j = 1 ; j <= nmi ; j++)
              factnmi = j * factnmi ;
        value = factn / (facti * factnmi) ;
      end
  return value ;
end

#define PI 3.1415926535897932385

void bwsectdes (pdouble a, pdouble b, integer npoles, boolean high,
                     tfloat ratio)
begin
  double p1[6], p2[6] ;
  double hden[10], hnum[10] ;
  double p[4][5] ;
  integer i, j ;
  double nu ;
  double hzero, d, den, warp, warp2 ;
  integer nconjp, nterm, ind ;
  boolean iodd ;

  for (j = 1 ; j <= 4 ; j++)
    begin
      for (i = 1 ; i <= 3 ; i++)
        begin
          p[i][j] = 0.0 ;
          if (i == 1)
            then
              p[i][j] = 1.0 ;
        end
    end
  nconjp = npoles shr 1 ;
  iodd = (npoles and 1) ;
  nterm = (npoles + 1) shr 1 ;
  nu = PI * ratio ;
  warp = sin (nu) / cos (nu) ;
  warp2 = warp * warp ;
  hzero = 1.0 ;
  for (i = 0 ; i <= nconjp - 1 ; i++)
    begin
      d = -2.0 * cos((PI / 2.0) + (PI / (2.0 * npoles)) + (PI * i) / npoles) ;
      den = 1.0 + d * warp + warp2 ;
      hzero = hzero * warp2 / den ;
      p[2][i + 1] = 2.0 * (warp2 - 1.0) / den ;
      p[3][i + 1] = (1.0 - d * warp + warp2) / den ;
    end
  if (iodd)
    then
      begin
        den = 1.0 + warp ;
        hzero = hzero * warp / den ;
        p[2][nterm] = (warp - 1.0) / den ;
      end
  for (i = 1 ; i <= 5 ; i++)
    begin
      p1[i] = 0.0 ;
      p2[i] = 0.0 ;
    end
  for (i = 1 ; i <= 3 ; i++)
    begin
      for (j = 1 ; j <= 3 ; j++)
        begin
          ind = i + j - 1 ;
          p1[ind] = p1[ind] + p[i][1] * p[j][2] ;
          p2[ind] = p2[ind] + p[i][3] * p[j][4] ;
        end
    end
  for (i = 1 ; i <= 9 ; i++)
    begin
      hnum[i] = factorial (npoles, i) * hzero ;
      if (high)
        then
          hnum[i] = hnum[i] / pow(warp, npoles) ;
      if ((high) land ((i and 1) == 0))
        then
          hnum[i] = -hnum[i] ;
      hden[i] = 0.0 ;
    end
  for (i = 1 ; i <= 5 ; i++)
    for (j = 1 ; j <= 5 ; j++)
      begin
        ind = i + j - 1 ;
        hden[ind] = hden[ind] + p1[i] * p2[j] ;
      end
  for (i = 1 ; i <= 9 ; i++)
    begin
      if (i > 1)
        then
          hden[i] = -hden[i] ;
      *a = hnum[i] ;
      inc(a) ;
      *b = hden[i] ;
      inc(b) ;
    end
end

void calc_section (tsection_base *sect)
begin
  double a_coef[10], b_coef[10] ;
  integer i ;

  for (i = 1 ; i <= 9 ; i++)
    begin
      a_coef[i] = 0 ;
      b_coef[i] = 0 ;
    end
  bwsectdes (addr(a_coef[1]), addr(b_coef[1]), sect->poles, sect->highpass, sect->ratio) ;
  for (i = 0 ; i <= sect->poles ; i++)
    begin
      sect->a[i] = a_coef[i + 1] ;
      sect->b[i] = b_coef[i + 1] ;
    end
end

#endif
