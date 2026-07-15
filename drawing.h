// =================================================================
// VECTOR IMAGE PATH SCRIPT: HIGH-DETAIL PORTRAIT SWEEP (SHIFTED DOWN 10cm)
// Scaled 20cm across | Centered horizontally | Shifted 100mm down on Y
// Layout Envelope Bounds: X: 210 to 410, Y: 400 to 600
test drawing - this is another update - this change made in the repo
// =================================================================

// --- LEFT EYE OUTLINE & DETAIL ---
startPath(273.00, 512.00);
drawTo(278.00, 508.00);
drawTo(285.00, 507.00);
drawTo(292.00, 510.00);
drawTo(295.00, 514.00);
drawTo(291.00, 518.00);
drawTo(283.00, 519.00);
drawTo(276.00, 517.00);
drawTo(273.00, 512.00);
endPath();

// Left Pupil/Iris Concentric Rings
startPath(281.00, 513.00);
drawTo(287.00, 513.00);
drawTo(287.00, 516.00);
drawTo(281.00, 516.00);
drawTo(281.00, 513.00);
endPath();

// Upper Eyelash Layer Thickener
startPath(271.00, 509.00);
drawTo(280.00, 505.00);
drawTo(291.00, 507.00);
drawTo(297.00, 512.00);
endPath();


// --- RIGHT EYE OUTLINE & DETAIL ---
startPath(325.00, 514.00);
drawTo(328.00, 510.00);
drawTo(335.00, 507.00);
drawTo(342.00, 508.00);
drawTo(347.00, 512.00);
drawTo(344.00, 517.00);
drawTo(337.00, 519.00);
drawTo(329.00, 518.00);
drawTo(325.00, 514.00);
endPath();

// Right Pupil/Iris Concentric Rings
startPath(333.00, 513.00);
drawTo(339.00, 513.00);
drawTo(339.00, 516.00);
drawTo(333.00, 516.00);
drawTo(333.00, 513.00);
endPath();

// Upper Eyelash Layer Thickener
startPath(323.00, 512.00);
drawTo(329.00, 507.00);
drawTo(340.00, 505.00);
drawTo(349.00, 509.00);
endPath();


// --- LEFT EYEBROW ---
startPath(266.00, 496.00);
drawTo(278.00, 491.00);
drawTo(294.00, 495.00);
endPath();
startPath(270.00, 494.00); drawTo(282.00, 492.00); endPath();

// --- RIGHT EYEBROW ---
startPath(326.00, 495.00);
drawTo(342.00, 491.00);
drawTo(354.00, 496.00);
endPath();
startPath(338.00, 492.00); drawTo(350.00, 494.00); endPath();


// --- NOSE DETAIL ---
startPath(304.00, 512.00);
drawTo(303.00, 538.00); 
drawTo(297.00, 544.00); 
drawTo(300.00, 548.00); 
drawTo(305.00, 547.00); 
drawTo(315.00, 547.00);
drawTo(320.00, 548.00);
drawTo(323.00, 544.00); 
drawTo(317.00, 538.00);
endPath();

// Nose bridge definition crease
startPath(302.00, 542.00);
drawTo(306.00, 540.00);
drawTo(314.00, 540.00);
drawTo(318.00, 542.00);
endPath();


// --- MOUTH, LIPS & TEETH OUTLINES ---
// Upper Lip Top Edge
startPath(286.00, 564.00);
drawTo(298.00, 567.00);
drawTo(310.00, 565.00); 
drawTo(322.00, 567.00);
drawTo(334.00, 564.00);
// Center Line Parting Edge
drawTo(310.00, 571.00);
drawTo(286.00, 564.00);
endPath();

// Lower Lip Profile
startPath(286.00, 564.00);
drawTo(298.00, 580.00);
drawTo(322.00, 580.00);
drawTo(334.00, 564.00);
endPath();

// Inside Open Smile / Exposed Teeth Partitions
startPath(292.00, 568.00);
drawTo(300.00, 568.00);
drawTo(300.00, 574.00); 
drawTo(292.00, 573.00);
endPath();

startPath(300.00, 568.00);
drawTo(310.00, 568.00);
drawTo(310.00, 575.00); 
drawTo(300.00, 574.00);
endPath();

startPath(310.00, 568.00);
drawTo(320.00, 568.00);
drawTo(320.00, 574.00); 
drawTo(310.00, 575.00);
endPath();

startPath(320.00, 568.00);
drawTo(328.00, 568.00);
drawTo(328.00, 573.00);
drawTo(320.00, 574.00);
endPath();


// --- JAWLINE, CHIN & SOFT CHEEK CONTOURS ---
startPath(246.00, 520.00);
drawTo(250.00, 548.00);
drawTo(266.00, 576.00);
drawTo(292.00, 594.00); 
drawTo(310.00, 596.00); 
drawTo(328.00, 594.00); 
drawTo(354.00, 576.00);
drawTo(370.00, 548.00);
drawTo(374.00, 520.00);
endPath();

// Dimples & Cheek definition arcs
startPath(262.00, 562.00); drawTo(268.00, 572.00); endPath();
startPath(358.00, 562.00); drawTo(352.00, 572.00); endPath();


// --- EXPOSED EAR REGIONS ---
// Left Ear Arc Detail
startPath(246.00, 520.00);
drawTo(240.00, 516.00);
drawTo(238.00, 532.00);
drawTo(244.00, 542.00);
endPath();

// Right Ear Arc Detail
startPath(374.00, 520.00);
drawTo(380.00, 516.00);
drawTo(382.00, 532.00);
drawTo(376.00, 542.00);
endPath();


// --- DETAILED HAIR PROFILE LINES ---
// Outer Structural Silhouette Frame
startPath(246.00, 520.00);
drawTo(234.00, 460.00);
drawTo(260.00, 412.00); 
drawTo(310.00, 400.00); // Shifted down to Y:400
drawTo(360.00, 412.00); 
drawTo(386.00, 460.00);
drawTo(374.00, 520.00);
endPath();

// Left Side Hanging Length Extensions
startPath(246.00, 520.00);
drawTo(242.00, 560.00);
drawTo(248.00, 590.00);
drawTo(254.00, 600.00); 
endPath();

// Right Side Hanging Length Extensions
startPath(374.00, 520.00);
drawTo(378.00, 560.00);
drawTo(372.00, 590.00);
drawTo(366.00, 600.00); 
endPath();

// Front Strands (Left Parting Wave 1)
startPath(294.00, 406.00);
drawTo(278.00, 434.00);
drawTo(262.00, 472.00);
endPath();

// Front Strands (Left Parting Wave 2)
startPath(310.00, 400.00);
drawTo(302.00, 442.00);
drawTo(290.00, 486.00);
endPath();

// Front Strands (Right Side Sweep)
startPath(326.00, 406.00);
drawTo(346.00, 440.00);
drawTo(360.00, 486.00);
endPath();

// Crown Interior Texture Streams
startPath(310.00, 400.00); drawTo(314.00, 430.00); drawTo(322.00, 462.00); endPath();
startPath(282.00, 416.00); drawTo(286.00, 448.00); endPath();
startPath(338.00, 416.00); drawTo(334.00, 448.00); endPath();
