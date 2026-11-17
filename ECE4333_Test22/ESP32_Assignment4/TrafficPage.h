#pragma once
static const char TRAFFIC_PAGE[] PROGMEM=R"HTML(<!doctype html>
<html lang="en"><head><meta charset="utf-8"><meta name="viewport" content="width=device-width,initial-scale=1"><title>ECE 4333 Robot Car Test22</title>
<style>
*{box-sizing:border-box}body{margin:0;background:#101722;color:#edf3fa;font:16px system-ui,sans-serif}
main{max-width:980px;margin:auto;padding:0 18px 40px}
.topbar{position:sticky;top:0;z-index:5;background:#0d141f;border-bottom:1px solid #2a3a4f;padding:10px 18px 0}
.topbar>*{max-width:944px;margin-left:auto;margin-right:auto}
.toprow{display:flex;align-items:center;justify-content:space-between;gap:10px;flex-wrap:wrap}
h1{font-size:22px;margin:4px 0}h1 small{color:#8fa6bf;font-weight:500}h2{font-size:19px;margin:0 0 12px}
.pill{background:#1c2939;border:1px solid #3a4f68;border-radius:999px;padding:6px 12px;font-size:14px}
.actions,nav{display:flex;align-items:center;gap:10px;flex-wrap:wrap}.actions{margin:8px 0}
section,details{background:#1c2939;border:1px solid #304156;border-radius:12px;padding:16px;margin:14px 0}
button,input,select{font:inherit;border:1px solid #52677f;border-radius:8px;padding:9px 12px;background:#26394e;color:#fff}
button{cursor:pointer;min-height:44px}button:disabled{opacity:.5;cursor:default}button.primary{background:#287b67;border-color:#3fa98e}.danger{background:#bd3b3b;border-color:#e06060}
button.big{font-size:18px;font-weight:700;padding:12px 22px}
nav{margin:6px 0 0;gap:4px}nav button{border-radius:10px 10px 0 0;border-bottom:none;background:#1c2939;min-width:120px}
nav button[aria-selected=true]{background:#287b67;border-color:#55baa2}
label{display:block;margin:12px 0}input[type=number]{width:110px}input[type=checkbox]{width:auto;transform:scale(1.3);margin-right:8px}
p{line-height:1.5;margin:10px 0}.hint,small{color:#b5c5d8}#message,#cal{color:#ffcf91;min-height:22px;overflow-wrap:anywhere;margin:4px 0 8px}
#live{display:block;width:100%;height:auto;min-height:180px;background:#080d15;border-radius:8px}
table{width:100%;border-collapse:collapse;font-size:14px}td,th{padding:10px 5px;text-align:left;border-bottom:1px solid #354459}th{color:#b5c5d8;font-weight:500}
summary{cursor:pointer;font-weight:600}a{color:#8cd7ff}.tablewrap{overflow:auto}#savedCal{white-space:pre-wrap;font-size:14px;line-height:1.7}
.stats{display:grid;grid-template-columns:repeat(3,1fr);gap:12px;margin:6px 0 10px}.stats>div{background:#132031;border:1px solid #2c3f56;border-radius:10px;padding:10px}
.stats .num{display:block;font-size:28px;font-weight:700}.stats small{display:block}.red{color:#ff7b7b}.ok{color:#5be3a8}
.drivepad{display:grid;grid-template-columns:repeat(3,70px);gap:8px;margin:10px 0}.drivepad button{touch-action:none;user-select:none;min-height:60px;font-size:22px}
[hidden]{display:none!important}
@media(max-width:620px){main{padding:0 10px 30px}.topbar{padding:8px 10px 0}nav button{min-width:0;flex:1;padding:9px 6px}.stats{grid-template-columns:1fr 1fr}td,th{padding:9px 3px;font-size:12px}}
</style></head><body>
<div class="topbar">
<div class="toprow"><h1>Robot Car <small>Test22</small></h1><span id="runState" class="pill">Connecting…</span></div>
<div class="actions"><button class="danger big" onclick="stopCar()">STOP CAR</button><button id="startLine" class="primary big" onclick="startCar()">Start</button></div>
<p id="message" role="status"></p>
<p id="motionDiag" role="status" class="hint">Waiting for live motor diagnostics</p>
<nav role="tablist"><button id="driveTab" role="tab" aria-selected="true" onclick="switchTab('drive')">Drive</button><button id="mapTab" role="tab" aria-selected="false" onclick="switchTab('map')">Map</button><button id="cameraTab" role="tab" aria-selected="false" onclick="switchTab('camera')">Camera</button><button id="setupTab" role="tab" aria-selected="false" onclick="switchTab('setup')">Settings</button></nav>
</div>
<main>
<div id="drivePanel" role="tabpanel">
<section id="gyroSection"><h2>Gyroscope</h2>
<p id="mapGyro" style="font-weight:600">Gyro: waiting…</p>
<div style="display:flex;gap:18px;align-items:center;flex-wrap:wrap;margin:8px 0">
<svg width="128" height="128" viewBox="-64 -64 128 128" aria-label="heading dial"><circle r="58" fill="#0b1220" stroke="#4a5b70" stroke-width="2"/>
<text y="-44" text-anchor="middle" font-size="11" fill="#9fb3c8">0°</text><text x="-46" y="4" text-anchor="middle" font-size="11" fill="#9fb3c8">+90</text><text x="46" y="4" text-anchor="middle" font-size="11" fill="#9fb3c8">-90</text>
<g id="gyroNeedle"><polygon points="0,-48 7,4 0,-2 -7,4" fill="#ff6b6b"/><polygon points="0,30 5,4 0,-2 -5,4" fill="#6c7f95"/></g><circle r="4" fill="#edf3fa"/></svg>
<div><div id="gyroYaw" style="font-size:30px;font-weight:700">— °</div>
<div id="gyroDetail" class="hint">heading — · rate — °/s</div>
<div class="actions"><button id="gyroZeroBtn" onclick="mapZeroGyro()">Zero + recalibrate gyro  (car still)</button></div>
<p id="gyroMsg" class="hint">Turn the car LEFT by hand: yaw must INCREASE. If needed, change Gyro Z sign in Settings.</p></div></div>
</section>

<section data-profile="race" hidden><h2>Stop on red</h2>
<div class="stats"><div><span class="num" id="redNow">—</span><small>Red pixels now</small></div><div><span class="num" id="rpNow">—</span><small>Threshold RP</small></div><div><span class="num" id="trafficNow" style="font-size:18px">—</span><small>Traffic state</small></div></div>
<label><input id="RS" type="checkbox" onchange="saveSettings()">Stop on red </label>
<label>STOP: stop at red pixels RP  <input id="RP" onchange="saveSettings()" type="number" min="0" max="160000" value="100"></label>
<label>STOP: slow down at red pixels RPS  (0 = RP/3) <input id="RPS" onchange="saveSettings()" type="number" min="0" max="160000" value="0"></label>
<label>STOP by width: sign width at the stop point WST  (per mille of image, 0 = use RP pixels) <input id="WST" onchange="saveSettings()" type="number" min="0" max="1000" value="0"> <span class="hint">live: <b id="wpmNow">—</b>‰</span></label>
<label>Stop distance for WST  SDC (cm, 5-100) <input id="SDC" onchange="saveSettings()" type="number" min="5" max="100" value="20"></label>


<p class="hint">Put the car where it must stop (SDC cm from the sign), read the live width and enter it as WST. Slow-down starts at half that width (twice the distance). Width depends far less on lighting than the pixel count.</p>
<label>Traffic light: red light at red pixels RPL  (0 = any red region) <input id="RPL" onchange="saveSettings()" type="number" min="0" max="160000" value="0"></label>
<p class="hint">Race profile only. O-S and the start wait use red/green traffic-light detection. E-O and C-O use red size for slowing/stopping and request STOP AI after stopping. The car holds for at least 3 seconds and scans the intersection. AI confirmation is logged; it does not choose the road function. B-C enables obstacle avoidance. Other segments use line following only.</p>

</section>
<section><h2>Speed</h2>
<label>Cruise PWM  (50–150) <input id="CS" onchange="$('DS').value=Math.min(Number($('DS').value),Number(this.value));saveSettings()" type="number" min="50" max="150" value="60"></label>
<p class="hint">Corners pivot at PWM 100. Cruise PWM 55-70 is a starting point; tune on the actual floor.</p>
<p id="applied" class="hint"></p>
<h2 style="margin-top:14px">Live line-following speed  (Wi-Fi, also while driving)</h2>
<div class="actions"><button onclick="liveSpeed(-10)">−10</button><input id="LSP" type="number" min="40" max="180" value="80" style="width:90px"><button onclick="liveSpeed(10)">+10</button><button class="primary" onclick="liveSpeed(0)">Apply now</button><span class="hint">car now: <b id="lspNow">—</b>PWM · lap <b id="lapNow">—</b></span></div>
<p class="hint">Localizer (ESP32 EKF): <b id="locNow">—</b></p>
<p id="routeFeature" class="hint">Waiting for UNO</p>
<p id="buildNow" style="font-weight:700;padding:6px 10px;border-radius:8px;background:#fff3bf;color:#5f3f00">UNO build: —</p>
<p class="hint">UNO: <b id="a5Now">—</b>. Battery LED on the car: green = good, yellow = low, red = charge.</p>
<p id="lspMsg" class="hint">Changes the line-following speed immediately (40-180 PWM). It is not saved: the next Start uses Cruise PWM above.</p></section>
<section data-profile="mapping" hidden><h2>Figure-8 run: STOP sign</h2>
<p class="hint">Place the car on O-D, facing D, and press Start: line following begins at once. On E-O and C-O the car slows down when it sees the STOP sign (red), stops in front of it for 3 seconds, then continues. STOP CAR ends the run.</p>
<label>Start point: distance after O on O-D  (cm, 20-150) <input id="xSO" type="number" min="20" max="150" value="83"></label>
<label>Speed while a STOP sign is ahead  (% of the line-following speed, 20-100) <input id="xSP" type="number" min="20" max="100" value="50"></label>
<label>Map stop line: stop this far before O even if the sign was not seen  (cm, 0 = off, 0-60) <input id="xSL" type="number" min="0" max="60" value="20"></label>
</section>
<section><h2>Obstacle avoidance (no scanning)</h2>
<p class="hint">When the ultrasonic sees an obstacle at the detection distance, the car stops, turns by the turn angle to the first-turn side and drives the first leg, turns back parallel to the road and drives straight past the obstacle, turns towards the road and drives the return leg, then turns back onto the road and keeps following the line. Nothing is scanned. Default: right 90°, 15 cm, straight 10 cm, left 15 cm.</p>
<label><input id="xOV" type="checkbox" checked>Avoidance ON </label>
<label>Obstacle detected at  (cm, 15-120) <input id="xOT" type="number" min="15" max="120" value="15"></label>
<label>Obstacle length along the road  (cm, 5-150, used by the clearance check) <input id="xOL" type="number" min="5" max="150" value="10"></label>
<label>First turn <select id="xOD"><option value="1">Right (then left, left)</option><option value="0">Left (then right, right)</option></select></label>
<label>Turn angle  (°, 30-90) <input id="xOA" type="number" min="30" max="90" value="90"></label>
<label>First leg, away from the road  (cm, 5-60) <input id="xOS" type="number" min="5" max="60" value="15"></label>
<label>Straight, past the obstacle  (cm, 5-150) <input id="xOJ" type="number" min="5" max="150" value="10"></label>
<label>Return leg, back to the road  (cm, 5-60) <input id="xOR" type="number" min="5" max="60" value="15"></label>
<p id="ovtCheck" class="hint"></p>
<p id="ovtNow" class="hint">Car is using: —</p>
</section>
<section data-profile="mapping" hidden><h2>Target following</h2>
<p class="hint">The targets are not fixed. Auto: the car follows the first red, green or blue target the camera sees (the largest if there are several), slows down and stops in front of it, then turns to look for the next target, which may be any colour <b>except the colour it has just reached</b> (e.g. after red: green or blue; after green: red or blue). To find a target the car first stands still and turns the camera: straight ahead, then 20°, 40° and 60° to each side, so targets in front are found first; only if the camera finds nothing does the car turn slowly on the spot (up to about one turn). It keeps going until the number of targets below is reached, nothing is found, or STOP CAR. A single colour (Red / Green / Blue only) = drive to one target of that colour and stop. It uses the colour detection of the Camera tab.</p>
<div class="stats"><div><span class="num" id="fwSeen">—</span><small>Target seen</small></div><div><span class="num" id="fwW">—</span><small>Target width</small></div><div><span class="num" id="fwSt" style="font-size:18px">—</span><small>State</small></div></div>
<div class="actions"><button class="primary big" id="followNow" onclick="startFollow()">Start target following</button></div>
<label>Target colour  <select id="FC"><option value="a">Auto (first colour seen, then a different colour each time)</option><option value="g">Green only (one target)</option><option value="r">Red only (one target)</option><option value="b">Blue only (one target)</option></select></label>
<label>Targets to reach in Auto  (0 = keep going until STOP CAR, 0-9) <input id="FN" type="number" min="0" max="9" value="0"></label>
<label>Stop when the target is this wide  (pixels of the analysis image) <input id="FW" type="number" min="5" max="1600" value="75"></label>
<details><summary>Advanced tuning</summary>
<label>Camera horizontal field of view  (°, 20-140) <input id="FF" type="number" min="20" max="140" value="62"> <input id="FM" type="checkbox">image mirrored </label>
<label>Kp ×10  (PWM per degree ×10, 1-80) <input id="FKP" type="number" min="1" max="80" value="15"></label>
<label>Kd ×100  (PWM per °/s ×100, 0-50) <input id="FKD" type="number" min="0" max="50" value="10"></label>
<p class="hint">If the car turns AWAY from the target tick “image mirrored”. If it under-turns raise the field of view, if it overshoots lower it; if it wobbles raise Kd or lower Kp.</p>
<label>Cone width for the map  CWS (per mille of the image at CDC cm, 0 = estimated 8 cm cone) <input id="CWS" onchange="saveSettings()" type="number" min="0" max="1000" value="0"></label>
<label>Distance for CWS  CDC (cm, 5-150) <input id="CDC" onchange="saveSettings()" type="number" min="5" max="150" value="30"></label>
</details>
<p id="fwMsg" class="hint"></p></section>

</div>
<div id="mapPanel" role="tabpanel" hidden>
<section id="mapSection"><h2>Assignment 5 localization and mapping  (Test22)</h2>
<h2>Feature-based map (gyro + kinematics)</h2>
<div id="featureBox"></div><pre id="featureText" class="hint" style="white-space:pre-wrap"></pre>
<div class="actions"><button onclick="featureExport('csv')">Feature map CSV</button><button onclick="featureExport('svg')">Feature map SVG</button><button onclick="slamExportSVG()">EKF map SVG</button></div>
<h2>Measured robot ground truth</h2>
<p class="hint">Enter measured samples as time_s,x_cm,y_cm,heading_deg, relative to this run's start. Comparison uses the nearest recorded sample (maximum 0.2 s). Empty input means no measured accuracy claim.</p>
<textarea id="poseGT" rows="4" style="width:100%" placeholder="0,0,0,0&#10;5,100,0,0"></textarea>
<div class="actions"><button onclick="poseEvaluate()">Compare robot estimates</button><button onclick="poseExport()">Evaluation CSV</button><button onclick="noiseExport()">Noise analysis CSV</button></div>
<pre id="poseEvaluation" class="hint" style="white-space:pre-wrap"></pre>
<h2>EKF-SLAM map  (Assignment 5)</h2>
<div id="slamBox"></div>
<label>Time  (evolving map) <input id="slamT" type="range" min="0" max="1000" value="1000" style="width:100%" oninput="slamRender(mapRes)"></label>
<pre id="slamText" class="hint" style="white-space:pre-wrap"></pre>
<label>Ground truth landmarks  (one per line: colour x y name, cm in the start frame: x ahead of the start heading, y to the left), e.g. "r 150 0 cone1"
<textarea id="slamGT" rows="4" style="width:100%" oninput="try{localStorage.setItem('a5gt',this.value)}catch(e){};slamRender(mapRes)"></textarea></label>
<div class="actions"><button onclick="slamCSV()">EKF-SLAM CSV</button><button onclick="slamNoise()">Gaussian noise analysis</button></div>
<pre id="noiseOut" class="hint" style="white-space:pre-wrap"></pre>
<p class="hint">Assignment 5 feature map: origin is the robot start, +x is the start heading, +y is left. Landmark positions come from sensor range/bearing and the EKF. The road drawing below is a surveyed reference, not a measured feature map. Calibrate CWS/CDC before quantitative evaluation; CWS=0 uses an approximate 8 cm cone.</p>
<p class="hint">This tablet records the serial log and keeps it across reloads. The ESP32 has a finite line buffer; its retention time depends on the observation/log rate and free PSRAM. Keep the page open and download each run.</p>
<p id="mapStatus" role="status">Waiting for the car…</p>
<label>Map of <select id="mapRun" onchange="mapPickRun(this.value)"><option value="live">Latest run (live)</option></select></label>
<div style="display:flex;gap:14px;flex-wrap:wrap;align-items:center;margin:6px 0"><b>Layers :</b>
<label style="margin:0"><input id="mapLyD" type="checkbox" checked onchange="mapLayerChanged()">Drawing </label>
<label style="margin:0"><input id="mapLyP" type="checkbox" checked onchange="mapLayerChanged()"><span style="color:#c8a38f">Predicted  (wheel model only)</span></label>
<label style="margin:0"><input id="mapLyF" type="checkbox" checked onchange="mapLayerChanged()"><span style="color:#5be3a8">Fused  (gyro + wheel)</span></label>
<label style="margin:0"><input id="mapLyM" type="checkbox" checked onchange="mapLayerChanged()"><span style="color:#8cd7ff">Mapped  (scaled, by state)</span></label></div>
<h2>Lap times</h2><div id="mapLaps" class="tablewrap"></div>

<label style="display:block;margin:6px 0">Distance from the stopped car to the STOP sign / light, for the surveyed-road comparison (cm) <input id="SR" type="number" min="5" max="150" value="35" style="width:80px" onchange="MAPSIGNRANGE=+this.value||35;saveA4();mapRefresh(true)"></label>
<h2>Surveyed road reference and odometry comparison</h2><div id="mapView"></div>
<div id="mapHeading"></div>
<div id="mapStates"></div>
<div id="mapTrans" style="overflow-x:auto"></div>
<div class="actions"><button class="primary" onclick="mapDownload('csv')">Download data CSV</button><button onclick="mapDownload('raw')">Download all lines</button></div>
<div class="actions"><button onclick="mapDownload('png')">Map PNG</button><button onclick="mapDownload('svg')">Map SVG</button><button onclick="mapDownload('trans')">State transitions CSV</button><button onclick="mapDownload('spng')">State timeline PNG</button><button onclick="mapDownload('hpng')">Heading chart PNG</button><button onclick="mapDownload('hsvg')">Heading SVG</button><button onclick="mapDownload('seg')">Segments CSV</button><button onclick="mapDownload('points')">Track points CSV (fused + mapped + predicted)</button><button class="primary" onclick="mapDownload('laps')">Lap times CSV</button><button onclick="mapDownload('loc')">Localizer + objects CSV</button><button onclick="mapDownload('summary')">Summary TXT</button></div>
<pre id="mapSummary" style="white-space:pre-wrap;font-size:13px"></pre>
<div class="tablewrap"><table id="mapSegTable"></table></div>
<details><summary>Track lengths (cm) </summary>
<p class="hint">Measured centre-line length of each straight, corner to corner. A-B and B-C are estimated from the drawing; measure them. Saved on this tablet.</p>
<label>C-O-D <input id="mapLen0" type="number" min="1" max="2000" step="0.1"></label> <label>D-E <input id="mapLen1" type="number" min="1" max="2000" step="0.1"></label> <label>E-O-A <input id="mapLen2" type="number" min="1" max="2000" step="0.1"></label> <label>A-B <input id="mapLen3" type="number" min="1" max="2000" step="0.1"></label> <label>B-C <input id="mapLen4" type="number" min="1" max="2000" step="0.1"></label>
<div class="actions"><button onclick="mapSaveRef()">Save lengths</button><button onclick="mapResetRef()">Drawing values</button></div></details>
<div class="actions"><button class="danger" onclick="mapClear()">Delete recording from tablet</button></div>
<p class="hint">Predicted (brown dashed) = kinematic model only: wheel PWM → speed, midpoint integration from recorded PWM, no gyro. Fused (green) = gyro heading + the same wheel speed, as logged by the UNO (both unscaled, exactly what the car believed). Mapped = fused + this page's gyro offset/drift fit, distance scale and lap closure. The summary lists the distance of each track to the drawn centre line. Follow runs (vision target following) are listed in the run menu and show the same layers plus set-point vs gyro heading. Map: grey = assignment drawing; blue = line following, orange = crossing heading-hold, red = corner pivot, purple = search; black dots = all three sensors black (crossing); teal squares = detected corners; pink = stopped for red / STOP sign; red ring = car now. Start each line run at S on O-D, facing D. Distances are scaled automatically from complete straights.</p>
</section>

</div>
<div id="cameraPanel" role="tabpanel" hidden>
<section><div style="position:relative"><img id="live" width="320" height="240" alt="Live camera — connecting"><div id="boxes" style="position:absolute;inset:0;pointer-events:none"></div></div><small id="previewState">Connecting camera…</small> <button id="togglePreview" onclick="togglePreview()">Pause preview</button>
<div class="tablewrap"><table><thead><tr><th>Target</th><th>Detected</th><th>Pixels </th><th>Width (px)</th><th>Distance (cm)</th></tr></thead><tbody>
<tr><td>Red </td><td id="redSeen">—</td><td id="redPixels">—</td><td id="redWidth">—</td><td id="redDistance">—</td></tr>
<tr><td>Green </td><td id="greenSeen">—</td><td id="greenPixels">—</td><td id="greenWidth">—</td><td id="greenDistance">—</td></tr>
<tr><td>Blue </td><td id="blueSeen">—</td><td id="bluePixels">—</td><td id="blueWidth">—</td><td id="blueDistance">—</td></tr>
<tr><td>STOP sign</td><td id="stopSeen">—</td><td id="stopPixels">—</td><td id="stopWidth">—</td><td id="stopDistance">—</td></tr>
</tbody></table></div><small>Pixels = coloured area (compare with RP). The preview only streams while this tab is open; detection keeps running in the background.</small>
<p id="visionHealth" class="hint"></p></section>
<section><h2>Camera settings</h2><p class="hint">Changing settings stops the car.</p>
<label><input id="detect" type="checkbox" checked onchange="setDetection('detect',this.checked)">STOP AI (160×160, deploy 5)</label>
<label><input id="color" type="checkbox" checked onchange="setDetection('color',this.checked)">Red / green / blue detection </label>
<label>Resolution <select id="framesize" onchange="setCamera('framesize',this.value)"><option value="">Loading camera sizes...</option></select></label>
<label>White balance <select id="wb_mode" onchange="setCamera('wb_mode',this.value)"><option value="0">Auto (0)</option><option value="1">Sunny (1)</option><option value="2">Cloudy (2)</option><option value="3">Office (3)</option><option value="4">Home (4)</option></select></label>
<label><input id="awb" type="checkbox" checked onchange="setCamera('awb',+this.checked)">Automatic white balance</label>
<label><input id="awb_gain" type="checkbox" checked onchange="setCamera('awb_gain',+this.checked)">White balance gain</label>
<label>Brightness <select id="brightness" onchange="setCamera('brightness',this.value)"><option>-2</option><option>-1</option><option selected>0</option><option>1</option><option>2</option></select></label>
<label>Contrast <select id="contrast" onchange="setCamera('contrast',this.value)"><option>-2</option><option>-1</option><option selected>0</option><option>1</option><option>2</option></select></label>
<label>Saturation <select id="saturation" onchange="setCamera('saturation',this.value)"><option>-2</option><option>-1</option><option selected>0</option><option>1</option><option>2</option></select></label>
<label><input id="vflip" type="checkbox" onchange="setCamera('vflip',+this.checked)">Flip vertically</label><label><input id="hmirror" type="checkbox" onchange="setCamera('hmirror',+this.checked)">Mirror horizontally</label>
<details><summary>Exposure, gain and advanced</summary>
<label>JPEG quality <input id="quality" type="number" min="10" max="63" value="10"><button onclick="setCamera('quality',$('quality').value)">Apply</button></label>
<label><input id="aec" type="checkbox" checked onchange="setCamera('aec',+this.checked)">Auto exposure</label>
<label><input id="aec2" type="checkbox" onchange="setCamera('aec2',+this.checked)">DSP exposure</label>
<label>Exposure level <select id="ae_level" onchange="setCamera('ae_level',this.value)"><option>-2</option><option>-1</option><option selected>0</option><option>1</option><option>2</option></select></label>
<label>Manual exposure <input id="aec_value" type="number" min="0" max="1200" value="300"><button onclick="setCamera('aec_value',$('aec_value').value)">Apply</button></label>
<label><input id="agc" type="checkbox" checked onchange="setCamera('agc',+this.checked)">Auto gain</label>
<label>Manual gain <input id="agc_gain" type="number" min="0" max="30" value="0"><button onclick="setCamera('agc_gain',$('agc_gain').value)">Apply</button></label>
<label>Gain ceiling <select id="gainceiling" onchange="setCamera('gainceiling',this.value)"><option value="0">2×</option><option value="1">4×</option><option value="2">8×</option><option value="3">16×</option><option value="4">32×</option><option value="5">64×</option><option value="6">128×</option></select></label>
<label>Effect <select id="special_effect" onchange="setCamera('special_effect',this.value)"><option value="0">None</option><option value="1">Negative</option><option value="2">Grayscale</option><option value="3">Red tint</option><option value="4">Green tint</option><option value="5">Blue tint</option><option value="6">Sepia</option></select></label>
<label><input id="bpc" type="checkbox" onchange="setCamera('bpc',+this.checked)">Black pixel correction</label>
<label><input id="wpc" type="checkbox" onchange="setCamera('wpc',+this.checked)">White pixel correction</label>
<label><input id="raw_gma" type="checkbox" onchange="setCamera('raw_gma',+this.checked)">Gamma</label>
<label><input id="lenc" type="checkbox" onchange="setCamera('lenc',+this.checked)">Lens correction</label>
<label><input id="dcw" type="checkbox" onchange="setCamera('dcw',+this.checked)">Downsize</label>
<label>Model view <select id="view_mode" onchange="setCamera('view_mode',this.value)"><option value="1">Centre crop (trained)</option><option value="0">Full image letterbox</option><option value="2">2× crop</option><option value="3">4× crop</option></select></label>
<label>AI threshold (%) <input id="threshold" type="number" min="50" max="99" value="70"><button onclick="setCamera('threshold',$('threshold').value)">Apply</button></label>
<label><input id="colorbar" type="checkbox" onchange="setCamera('colorbar',+this.checked)">Sensor test colour bars (turn off before driving)</label>
<p id="cameraInfo" class="hint">Read actual camera settings to see uptime and frame age.</p>
</details>
<div class="actions"><button onclick="loadCamera()">Read actual camera settings</button><button id="restoreDefaults" onclick="restoreCameraDefaults()">Restore CIF defaults (restarts ESP32)</button></div>
<button id="capture" onclick="captureNew()">Capture training photo</button><span id="captureState"></span><p id="downloads" hidden><a id="jpgLink">Open original JPEG</a>· <a id="bmpLink">Model input BMP</a></p>
</section>

</div>
<div id="setupPanel" role="tabpanel" hidden>
<section><h2>Line sensors</h2><p id="floor">Waiting for floor sensors…</p>
<label>Black range  <input id="BL" type="number" min="0" max="1022" value="600">to <input id="BH" type="number" min="1" max="1023" value="1023"></label>
<div class="actions"><button onclick="sampleFloor('white')">Sample white floor</button><button onclick="sampleFloor('black')">Sample black line</button><button onclick="applyFloor()">Apply measured threshold</button></div>
<p id="floorCal" class="hint">Stop the car. Put ALL three sensors over each surface in turn at normal ride height. Each sample takes 6 fresh readings. The UNO adds ±40 hysteresis around the threshold.</p></section>
<section><h2>Gyroscope</h2>
<label>Gyro Z sign  <select id="GS"><option value="0">Negative raw Z = left</option><option value="1">Positive raw Z = left</option></select></label>
<p class="hint">Turning the car LEFT must make the yaw on the Drive tab INCREASE. Your car (GY-521 chip up): Positive raw Z = left.  Positive raw Z = left</p></section>
<div hidden><input id="DS" type="number" value="40"><input id="SC" type="number" value="25"><input id="RA" type="number" value="180"><input id="SA" type="number" value="360"><input id="CT" type="number" value="1500"></div>
<button class="primary" onclick="saveSettings()">Save car settings</button><p id="applied2" class="hint"></p>
</div>
</main>
<script>
const $=id=>document.getElementById(id),names=['OFF ','—','TRACK ','STOP AHEAD: SLOW ','STOP: AI CHECK ·','RESERVED ','STOP: HOLD / SCAN ·','CROSSING ','FAULT ','FOLLOW: TRACKING ','FOLLOW: ARRIVED ','FOLLOW: NO TARGET, STOPPED ·','FOLLOW: TURNING THE CAR TO SEARCH ','FOLLOW: CAMERA SEARCH '];
const keys=['SC','BL','BH','RA','SA','CT','GS','CS','DS','RP','RPS','RPL','WST','SDC','CWS','CDC'],TABS=['drive','map','camera','setup'];let driving=false,latest=null,scannedToken=null,sampleResolution=null,currentTab='drive',previewTimer=null,previewBusy=false,previewPaused=false,previewURL=null,previewStamp='',previewLastShown=0,cameraBusy=false,starting=false;
let floorSamples={},floorSampling=false;
async function sampleFloor(kind){
 if(floorSampling)return;floorSampling=true;
 try{const rows=[];let prev=null;
 for(let n=0;n<30&&rows.length<6;n++){
  const d=JSON.parse(await request('/traffic-status')),u=d.uno||{};
  if(d.age_ms>=1500||u.M!==0||u.PL!==0||u.PR!==0)throw Error('Stop the car and wait for fresh UNO OFF telemetry.');
  if(u.UT!==prev&&[u.L,u.C,u.R].every(Number.isFinite)){rows.push([u.L,u.C,u.R]);prev=u.UT;}
  if(rows.length<6)await delay(450);
 }
 if(rows.length<6)throw Error('Not enough fresh sensor samples.');
 floorSamples[kind]=rows.flat();$('floorCal').textContent=kind+' captured: '+Math.min(...floorSamples[kind])+' to '+Math.max(...floorSamples[kind]);
 }catch(e){$('floorCal').textContent=e.message;}finally{floorSampling=false;}
}
function floorThreshold(w,b){
 if(!w?.length||!b?.length)throw Error('Sample both surfaces first.');
 const wMin=Math.min(...w),wMax=Math.max(...w),bMin=Math.min(...b),bMax=Math.max(...b);
 if(bMin-wMax>=60)return [Math.round((bMin+wMax)/2),1023];
 if(wMin-bMax>=60)return [0,Math.round((wMin+bMax)/2)];
 throw Error('Black/white readings overlap or are less than 60 ADC counts apart. Check height, light and each sensor.');
}
async function applyFloor(){try{const [low,high]=floorThreshold(floorSamples.white,floorSamples.black);$('BL').value=low;$('BH').value=high;if(await saveSettings())$('floorCal').textContent='Saved black range '+low+' to '+high+'.';}catch(e){$('floorCal').textContent=e.message;}}
const delay=ms=>new Promise(resolve=>setTimeout(resolve,ms));
async function request(url,data){const controller=new AbortController(),timer=setTimeout(()=>controller.abort(),4000);try{const r=await fetch(url,{cache:'no-store',signal:controller.signal,...(data===undefined?{}:{method:'POST',body:new URLSearchParams(data)})});const t=await r.text();if(!r.ok){const e=Error(t);e.status=r.status;throw e;}return t;}finally{clearTimeout(timer);}}
async function post(url,data={}){return request(url,data);}
function switchTab(name){if(!TABS.includes(name))name='drive';currentTab=name;window.scrollTo(0,0);for(const t of TABS){$(t+'Panel').hidden=t!==name;$(t+'Tab').setAttribute('aria-selected',String(t===name));}try{localStorage.setItem('ece4333-test21-tab',name);}catch(e){}if(name==='camera'&&!previewPaused){previewStamp='';refreshPreview();}if(name==='map'&&typeof mapRefresh==='function')mapRefresh(true);}
async function stopCar(){driving=false;try{$('message').textContent=await post('/cmd',{action:'stop'});}catch(e){$('message').textContent='STOP was not delivered. Autonomous running may continue; use the physical power switch if needed.';}}
async function startCar(){if(starting||settingsBusy){$('message').textContent='Wait for the pending command.';return;}
 starting=true;
 try{const wanted=readSettingsForm(),current=JSON.parse(await request('/traffic-settings'));
 if(!settingsMatch(current,wanted)&&!await saveSettings())return;
 const before=JSON.parse(await request('/traffic-status'));
 saveA4();await post('/traffic-start',Object.assign({green:0,ovt:+$('xOV').checked},a4Options()));let confirmed=false;
 for(let i=0;i<16;i++){await delay(300);const d=JSON.parse(await request('/traffic-status'));
 if(d.age_ms<1500&&d.uno?.M===6&&d.uno.RN!==before.uno?.RN){confirmed=true;break;}}
 if(!confirmed)throw Error('Start confirmation missing; inspect car state before retrying.');
 driving=true;$('message').textContent='Start acknowledged. See live motor PWM and pause reason below.';
 }catch(e){$('message').textContent=e.message;}
 finally{starting=false;}}
async function loadSettings(){try{const d=JSON.parse(await request('/traffic-settings'));keys.forEach(k=>$(k).value=d[k]);$('RS').checked=!!d.RS;}catch(e){$('message').textContent=e.message;}}
let settingsBusy=false;
function readSettingsForm(){const d={};keys.forEach(k=>{const n=Number($(k).value);if(!Number.isInteger(n))throw Error('Use whole numbers');d[k]=n;});d.RS=+$('RS').checked;return d;}
function settingsMatch(current,wanted){return !!current.applied&&[...keys,'RS'].every(k=>Number(current[k])===wanted[k]);}
async function saveSettings(){if(settingsBusy)return false;settingsBusy=true;driving=false;$('RS').disabled=$('CS').disabled=$('RP').disabled=true;
 try{const wanted=readSettingsForm();$('applied').textContent=$('applied2').textContent='Stopping car and applying settings…';
 await post('/cmd',{action:'stop'});
 let saved=false;for(let n=0;n<16;n++){try{await post('/traffic-settings',wanted);saved=true;break;}catch(e){if(e.status!==409)throw e;await delay(400);}}
 if(!saved)throw Error('UNO did not acknowledge OFF. Settings not applied.');
 for(let n=0;n<20;n++){const actual=JSON.parse(await request('/traffic-settings'));if(settingsMatch(actual,wanted)){$('message').textContent=`UNO confirmed : stop on red ${wanted.RS?'ON':'OFF'}, RP ${wanted.RP} px, cruise PWM ${wanted.CS}, gyro sign ${wanted.GS}, black ${wanted.BL}-${wanted.BH}. Press Start.`;return true;}await delay(400);}
 throw Error('Settings saved but UNO confirmation missing. Do not start; check the Test22 UNO is uploaded.');
 }catch(e){$('message').textContent=e.message;return false;}finally{settingsBusy=false;$('RS').disabled=$('CS').disabled=$('RP').disabled=false;}}
function resetSample(){scannedToken=null;sampleResolution=null;}
function showObjects(objects){for(const k of ['red','green','blue','stop']){const v=objects[k]||{};$(k+'Seen').textContent=!v.valid?'—':v.seen?'Yes':'No';$(k+'Pixels').textContent=v.valid&&v.seen?v.pixels:'—';$(k+'Width').textContent=v.valid&&v.seen&&v.width>0?v.width:'—';$(k+'Distance').textContent=v.valid&&v.seen&&v.distance_cm>=0?v.distance_cm:'—';}}
const faultReasons={10:'The gyro turned the wrong way during a pivot — set Gyro Z sign GS (Settings) and check by hand',15:'Line not found after a stop',11:'Line not found after the full search (too fast / tape too narrow / lifted?)',14:'Object too close for the line search'};
function faultMessage(u){const code=Number(u.FC);return code>0?`FAULT ${code} — ${faultReasons[code]||'Unknown fault'}. Press STOP CAR, fix the cause, place the car on the line, then Start.`:'FAULT — press STOP CAR before restarting.';}
const pauseNames={0:'motor command active',1:'standby',2:'gyro calibration',3:'line-following fault',4:'object too close',5:'STOP sign hold',6:'target not acquired / lost',7:'target reached',8:'target search limit reached',9:'controller currently commands zero',10:'gyro feedback unavailable',11:'looking for a target with the camera (car still)'};
function motionDiagnostic(u,stale){
 if(stale)return 'UNO telemetry stale: motor state is unknown.';
 if(u.UV!==51)return 'Firmware mismatch: upload BOTH Test22 ESP32 and UV=51 UNO.';
 if(u.A5===1&&u.TS===5)return 'Unexpected state on A5: verify both boards and refresh this page.';
 const reason=pauseNames[u.BR]||'pause reason unavailable';
 return ({0:'Standby',6:'Line',7:'Target'}[u.M]||'Mode '+u.M)+' | motor PWM L '+(u.PL??'?')+' / R '+(u.PR??'?')+' | ultrasonic '+(u.US>=0?u.US+' cm':'no echo')+' | '+reason+'.';
}
async function poll(){try{const d=JSON.parse(await request('/traffic-status'));if(latest&&d.esp_ms<latest.esp_ms)$('message').textContent='ESP32 restart detected.';latest=d;
 const o=d.objects||{},u=d.uno||{},c=d.settings||{},red=o.red||{};
 showObjects(o);followLive(d);boxObjects=o;boxesAt=Date.now();drawBoxes();
 $('wpmNow').textContent=d.red_wpm??'—';if(d.loc&&$('locNow'))$('locNow').textContent='track position '+d.loc.P+' cm ± '+d.loc.S+' · scale '+d.loc.K+' · fixes '+d.loc.E+' · gyro drift '+d.loc.H+'°';
 const vh=d.vision_health||{};$('visionHealth').textContent=`Colour frame age: ${vh.age_ms??'—'} ms | Red regions: ${vh.red_regions??'—'} | Green regions: ${vh.green_regions??'—'} | Colour processing: ${vh.processing_ms??'—'} ms | STOP inference: ${o.stop?.inference_ms??'—'} ms`;
 const px=red.valid&&red.seen?Number(red.pixels):0,rp=Number(c.RP);
 $('redNow').textContent=red.valid?px:'—';$('redNow').className='num'+(red.valid&&px>=rp&&rp>=0?' red':'');$('rpNow').textContent=Number.isFinite(rp)?rp:'—';
 const stale=d.age_ms>=1500;applyProfile(u,stale);$('motionDiag').textContent=motionDiagnostic(u,stale);
 $('trafficNow').textContent=stale?'—':((u.M===6||u.M===7)?(names[u.TS]||'—'):'OFF ');
 $('runState').textContent=!stale?(Number(u.UV)===51?((u.M===6||u.M===7)?(names[u.TS]||'RUNNING'):'Stopped '):'UNO VERSION MISMATCH — upload Test22 UNO sketch'):'UNO telemetry stale — motor state unknown';
 $('floor').textContent=`Live floor sensors  L ${u.L??'—'} · M ${u.C??'—'} · R ${u.R??'—'} | threshold ${c.BL??'—'}–${c.BH??'—'}`;
 const ap=settingsBusy?'Applying settings…':c.applied?`UNO confirmed : stop on red ${c.RS?'ON':'OFF'} · RP ${c.RP} px · cruise PWM ${c.CS} · gyro sign ${c.GS}`:'Waiting for UNO settings confirmation';$('applied').textContent=$('applied2').textContent=ap;
 if($('buildNow'))$('buildNow').textContent=u.A5===1?'UNO build: Test22 (figure-8 run with STOP sign and obstacle avoidance, target following, battery LED).':u.A5===0?'UNO build: old race build. Upload the Test22 UNO_Mapping_Assignment5 sketch.':'UNO build: —';
 if($('a5Now'))$('a5Now').textContent=u.BV!=null?('battery '+(u.BV/10).toFixed(1)+' V ('+(u.BV>=76?'good':u.BV>=70?'low':'CHARGE')+')'):'—';
 $('lspNow').textContent=u.M===6&&u.CV!=null?u.CV:'—';$('lapNow').textContent=u.M===6&&u.LP!=null?(u.LP+1)+' · zone '+['LINE','LIGHT','STOP','OBSTACLE'][u.Z||0]+' · stops '+(u.SN??0)+' (AI ✓ '+(u.SA??0)+')':'—';
 $('detect').checked=!!d.detect_enabled;$('color').checked=!!d.color_enabled;
 if(u.TS===8){driving=false;$('message').textContent=faultMessage(u);}
 if(scannedToken!==null&&sampleResolution!==d.framesize)resetSample();
 }catch(e){showObjects({});boxObjects={};drawBoxes();$('runState').textContent='Web connection lost — motor state unknown';}finally{setTimeout(poll,500);}}
let boxObjects={},boxesAt=0;
function drawBoxes(){
 const layer=$('boxes');layer.innerHTML='';
 if(currentTab!=='camera'||previewPaused||!previewStamp||Date.now()-previewLastShown>1500||Date.now()-boxesAt>1500)return;
 for(const k of ['red','green','blue','stop']){const v=boxObjects[k];
  if(!v?.box_valid||!(v.image_w>0&&v.image_h>0&&v.w>0&&v.h>0))continue;
  const age=Math.abs((Number(previewStamp)-Number(v.captured))|0);if(age>(k==='stop'?5000:1200))continue;
  const x=Math.max(0,v.x),y=Math.max(0,v.y),w=Math.min(v.w,v.image_w-x),h=Math.min(v.h,v.image_h-y);if(w<=0||h<=0)continue;
  const el=document.createElement('div');el.style.cssText=`position:absolute;left:${100*x/v.image_w}%;top:${100*y/v.image_h}%;width:${100*w/v.image_w}%;height:${100*h/v.image_h}%;border:2px solid ${k==='red'?'#ff4545':k==='green'?'#39ff88':k==='blue'?'#5599ff':'#ffcf40'};color:white;font:bold 12px sans-serif;box-sizing:border-box`;
  const label=document.createElement('span');label.textContent=k==='stop'?'STOP':k.toUpperCase()+(k==='red'&&v.pixels?' '+v.pixels+' px':'');label.style.cssText=`position:absolute;${k==='stop'?'bottom:0':'top:0'};left:0;background:#101722cc;white-space:nowrap`;el.appendChild(label);layer.appendChild(el);
 }
}
async function refreshPreview(){
 if(previewBusy||previewPaused)return;
 clearTimeout(previewTimer);
 if(currentTab!=='camera'){previewTimer=setTimeout(refreshPreview,1000);return;} // no image traffic while driving/mapping
 previewBusy=true;
 const started=Date.now(),controller=new AbortController();
 const timeout=setTimeout(()=>controller.abort(),8000);let nextURL=null;
 try{
  const r=await fetch(`http://${location.hostname}:81/preview?after=${encodeURIComponent(previewStamp)}&t=${started}`,{cache:'no-store',signal:controller.signal});
  if(r.status===204){if(previewLastShown&&Date.now()-previewLastShown>2000)$('previewState').textContent='Waiting for a new frame — last image retained';return;}
  if(!r.ok)throw Error(`Camera temporarily unavailable (${r.status})`);
  const blob=await r.blob();if(!blob.size)throw Error('Empty camera frame');
  nextURL=URL.createObjectURL(blob);
  const probe=new Image();
  await new Promise((resolve,reject)=>{probe.onload=resolve;probe.onerror=()=>reject(Error('Incomplete camera frame'));probe.src=nextURL;});
  if(previewPaused)return;
  const old=previewURL;$('live').src=nextURL;previewURL=nextURL;nextURL=null;
  if(old)URL.revokeObjectURL(old);
  previewStamp=r.headers.get('X-Frame-Stamp')||'';previewLastShown=Date.now();drawBoxes();
  const age=Number(r.headers.get('X-Frame-Age')||0)+Date.now()-started;
  $('previewState').textContent=age>2000?'Delayed camera frame — updating…':'Live camera';
 }catch(e){if(!previewPaused)$('previewState').textContent=previewURL?'Preview paused — last image retained; retrying…':'Waiting for first camera frame…';}
 finally{if(nextURL)URL.revokeObjectURL(nextURL);clearTimeout(timeout);previewBusy=false;if(!previewPaused)previewTimer=setTimeout(refreshPreview,Math.max(250,Math.min(1500,Date.now()-started)));}
}
function previewAge(){drawBoxes();if(currentTab==='camera'&&!previewPaused&&previewLastShown&&Date.now()-previewLastShown>3000)$('previewState').textContent='Preview paused — last image retained; retrying…';setTimeout(previewAge,1000);}
function togglePreview(){previewPaused=!previewPaused;drawBoxes();$('togglePreview').textContent=previewPaused?'Start preview':'Pause preview';clearTimeout(previewTimer);if(previewPaused)$('previewState').textContent='Preview paused by you — detection continues';else{previewStamp='';refreshPreview();}}
const cameraNumbers=['framesize','wb_mode','brightness','contrast','saturation','quality','ae_level','aec_value','agc_gain','gainceiling','special_effect','view_mode','threshold'];
const cameraFlags=['colorbar','awb','awb_gain','aec','aec2','agc','vflip','hmirror','bpc','wpc','raw_gma','lenc','dcw'];
async function loadCamera(){try{const d=JSON.parse(await request('/status'));if(Array.isArray(d.framesizes)&&d.framesizes.length)fillFramesizes(d.framesizes,d.framesize);for(const k of cameraNumbers)if(d[k]!==undefined)$(k).value=d[k];for(const k of cameraFlags)if(d[k]!==undefined)$(k).checked=!!d[k];if(d.uptime_ms!==undefined)$('cameraInfo').textContent=`Uptime: ${Math.floor(d.uptime_ms/1000)} s | Last camera frame: ${d.preview_age_ms>=0?d.preview_age_ms+' ms ago':'unavailable'} | Frames: ${d.camera_frames??0} | Capture errors: ${d.camera_errors??0} | Sensor ID: ${d.sensor_pid}`;}catch(e){$('message').textContent=e.message;}}
// Test21: the size list comes from the ESP32 camera driver, so the numbers match whatever ESP32 core compiled the sketch
function fillFramesizes(list,cur){const sel=$('framesize');if(!sel||sel.dataset.filled===String(list.length))return;
 const nm={'96x96':'96×96','160x120':'QQVGA','128x160':'QQVGA2','176x144':'QCIF','240x176':'HQVGA','240x240':'240×240','320x240':'QVGA','400x296':'CIF','480x320':'HVGA','640x480':'VGA','800x600':'SVGA','1024x768':'XGA','1280x720':'HD','1280x1024':'SXGA','1600x1200':'UXGA'};
 sel.innerHTML=list.map(([i,w,h])=>'<option value="'+i+'">'+(nm[w+'x'+h]||'')+' '+w+'×'+h+(w===400?' (recommended: analysis size, most frames)':'')+'</option>').join('');sel.dataset.filled=String(list.length);if(cur!==undefined)sel.value=cur;}
async function applyCamera(key,value){for(let n=0;n<25;n++){try{return await post('/control',{var:key,val:value});}catch(e){if(e.status!==503||n===24)throw e;await delay(200);}}}
async function setCamera(key,value){if(cameraBusy){$('message').textContent='Wait for the current camera change to finish.';return;}cameraBusy=true;driving=false;resetSample();try{$('message').textContent=await applyCamera(key,value);if(key==='framesize'){previewStamp='';let completed=false;for(let n=0;n<40;n++){await delay(400);const d=JSON.parse(await request('/resolution-status'));if(d.state==='applied'||d.state==='failed'){$('message').textContent=d.message;completed=true;break;}}if(!completed)throw Error('Resolution is still pending; read camera settings before trying again.');}await loadCamera();}catch(e){$('message').textContent=e.message;}finally{cameraBusy=false;}}
async function restoreCameraDefaults(){if(cameraBusy)return;cameraBusy=true;driving=false;resetSample();$('restoreDefaults').disabled=true;try{$('message').textContent=await post('/camera-defaults');$('previewState').textContent='ESP32 restarting — reconnect Wi-Fi and refresh';previewStamp='';}catch(e){$('message').textContent=e.message;}finally{cameraBusy=false;$('restoreDefaults').disabled=false;}}
async function setDetection(action,value){driving=false;resetSample();try{$('message').textContent=await post('/cmd',{action,value:value?1:0});}catch(e){$('message').textContent=e.message;}}
async function captureNew(){await stopCar();$('capture').disabled=true;try{const target=JSON.parse(await post('/snapshot')).id;$('captureState').textContent=' Capturing…';for(let n=0;n<60;n++){await delay(500);const d=JSON.parse(await request('/snapshot'));if(d.id>=target){$('jpgLink').href=`/capture?id=${d.id}&download=1`;$('bmpLink').href=`/model-input?id=${d.id}&download=1`;$('downloads').hidden=false;$('captureState').textContent=' Ready';return;}}throw Error('Capture timed out');}catch(e){$('captureState').textContent=e.message;}finally{$('capture').disabled=false;}}
/*MAPCORE (Test21) - mapping algorithm of PC_Mapping/draw_map.py (tested by tests/test_map_core.js) + Test11 predicted/fused tracks, heading chart, target-following runs*/
// Figure 8 (course project): straights from corner C. The race starts at S on C-O-D, 150 cm (C-O) + start offset after C.
const MAPREF0=[{name:'C-O-D',h:0,len:317},{name:'D-E',h:-136.6,len:148},{name:'E-O-A',h:-240,len:217},{name:'A-B',h:-210,len:47},{name:'B-C',h:-120,len:125}];
function mapStartOff(){const e=$('xSO');return 150+((e&&+e.value)||83);} /* C ->S along C-O-D (cm): the map origin is S */
const MAPCOL={1:'#2a7de1',2:'#f08c00',3:'#d6336c',4:'#ae3ec9',5:'#000000',6:'#ff8fab'};
function mapDecode(d){const st=d[13],k=d[14],m=st>>4,o={q:d[0],t:d[1],L:d[2],C:d[3],R:d[4],PL:d[5],PR:d[6],g:d[7]/10,yaw:d[8]/10,dist:d[9]/10,x:d[10],y:d[11],u:d[12],mode:m,fs:st&7,gy:(st>>3)&1,k:k};
 if(d.length>=19){o.sm=d[18]&15;o.seg=d[18]>>4;}                                                // state-machine code, UV>=43
 if(d.length>=18){o.kh=d[15]/10;o.kx=d[16];o.ky=d[17];}                     // predicted pose (wheel model only), UV>=41
 if(m===7){o.tgtErr=d[2]/10;o.tgtW=d[3];o.tgtBear=d[4]/10;o.L=o.C=o.R=null;} // mode 7 reuses the L,C,R slots
 return o;}
function mapRuns(S){const runs=[];let cur=[],p=null;
 for(const r of S){if(r.mode!==6&&r.mode!==7){if(cur.length)runs.push(cur);cur=[];p=null;continue;}
  if(p&&(r.runId!==p.runId||r.mode!==p.mode||r.q<p.q||r.t<p.t||r.t-p.t>3000)){runs.push(cur);cur=[];}cur.push(r);p=r;}
 if(cur.length)runs.push(cur);const out=runs.filter(r=>r.length>=5);out.forEach(r=>{r.kind=r[0].mode;});return out;}
function mapStraight(run,yaw,dist){
 const fwd=run.map(r=>r.PL>0&&r.PR>0&&(r.fs===1||r.fs===2)),out=[],N=run.length;let i=0;
 while(i<N){if(!fwd[i]){i++;continue;}let j=i,sum=yaw[i],c=1;
  while(j+1<N&&fwd[j+1]&&Math.abs(yaw[j+1]-sum/c)<=8){j++;sum+=yaw[j];c++;}
  if(j-i+1>=6)out.push({i0:i,i1:j,head:sum/c,len:dist[j]-dist[i]});i=j+1;}
 // drop short transitional runs (heading still settling at the end of a pivot)
 const maxLen=out.reduce((a,r)=>Math.max(a,r.len),0),minLen=Math.max(12,0.06*maxLen);
 for(let i=out.length-1;i>=0;i--)if(out[i].len<minLen)out.splice(i,1);
 const m=[];for(const r of out){const L=m[m.length-1];
  if(L&&Math.abs(L.head-r.head)<12){const w0=L.i1-L.i0+1,w1=r.i1-r.i0+1;L.head=(L.head*w0+r.head*w1)/(w0+w1);L.i1=r.i1;L.len=dist[L.i1]-dist[L.i0];}
  else m.push(Object.assign({},r));}
 return m;}
function mapLabel(runs,ref){const H=ref.map(s=>s.h),n=H.length,turn=H.map((h,l)=>l+1<n?H[l+1]-h:H[0]-h);
 runs.forEach((r,k)=>{let lab=null;
  if(k===0)lab=Math.abs(r.head-H[0])<30?0:null;
  else{const p=runs[k-1],d=r.head-p.head;
   if(p.label!==null&&Math.abs(d-turn[p.label])<=25)lab=(p.label+1)%n;
   else{let off=0;for(let q=k-1;q>=0;q--)if(runs[q].label!==null){off=runs[q].head-H[runs[q].label];break;}
    let best=1e9,bi=null;H.forEach((h,i)=>{const e=Math.abs(r.head-off-h);if(e<best){best=e;bi=i;}});lab=best<12?bi:null;r.resync=true;}}
  r.label=lab;});
 return runs;}
function mapSolve3(A,b){const M=A.map((r,i)=>r.concat([b[i]]));
 for(let c=0;c<3;c++){let p=c;for(let r=c+1;r<3;r++)if(Math.abs(M[r][c])>Math.abs(M[p][c]))p=r;
  if(Math.abs(M[p][c])<1e-12)return null;[M[c],M[p]]=[M[p],M[c]];
  for(let r=0;r<3;r++)if(r!==c){const f=M[r][c]/M[c][c];for(let j=0;j<4;j++)M[r][j]-=f*M[c][j];}}
 return[0,1,2].map(i=>M[i][3]/M[i][i]);}
function mapWheelStep(r,p){
 let x=p?p.kx:0,y=p?p.ky:0,h=p?p.kh*Math.PI/180:0;
 if(p){const dt=Math.max(0,Math.min(0.5,(r.t-p.t)/1000)),vl=(p.PL+r.PL)*0.5*0.33,vr=(p.PR+r.PR)*0.5*0.33,
  v=(vl+vr)*0.5,dh=(vr-vl)/14*dt;
  x+=v*dt*Math.cos(h+dh*0.5);y+=v*dt*Math.sin(h+dh*0.5);h+=dh;
 }
 r.kh=h*180/Math.PI;r.kx=x;r.ky=y;
}
function mapWheelPrediction(run){run.forEach((r,i)=>mapWheelStep(r,i?run[i-1]:null));}

function mapBuildRun(run,ref,fixedScale){
 mapWheelPrediction(run);
 const n=ref.length,N=run.length,t=run.map(r=>(r.t-run[0].t)/1000),yaw=run.map(r=>r.yaw-run[0].yaw),dist=run.map(r=>r.dist-run[0].dist);
 let gaps=0;for(let i=1;i<N;i++)gaps+=Math.max(0,(run[i].q-run[i-1].q-1)&0xFFFF);
 const notes=[];
 const segs=mapLabel(mapStraight(run,yaw,dist),ref).filter(g=>g.label!==null);
 const labels=new Array(N).fill(null);segs.forEach(g=>{for(let i=g.i0;i<=g.i1;i++)labels[i]=g.label;});
 const nx=l=>(l+1)%n;
 segs.forEach((g,k)=>{const prevOk=k>0&&nx(segs[k-1].label)===g.label&&!g.resync,nextOk=k+1<segs.length&&nx(g.label)===segs[k+1].label&&!segs[k+1].resync;
  g.complete=prevOk&&nextOk; /* the first straight starts at S, not at a corner */g.inOrder=prevOk||nextOk||k===0;});
 const comp=segs.filter(g=>g.complete&&g.len>0);let scale,how;
 const lapEnds=[];for(let k=1;k<segs.length;k++)if(segs[k].label===0&&segs[k-1].label===n-1)lapEnds.push(segs[k].i0);
 const so=mapStartOff(),lapRef=ref.reduce((a,s)=>a+s.len,0),lapMeas=lapEnds.length?dist[lapEnds[lapEnds.length-1]]-dist[0]:0;
 if(fixedScale){scale=fixedScale;how='fixed';}
 else if(lapEnds.length&&lapMeas>0){scale=(lapEnds.length*lapRef-so)/lapMeas;how='auto from S to corner C after '+lapEnds.length+' lap(s) ('+(lapEnds.length*lapRef-so).toFixed(0)+' cm)';}
 else if(comp.length){scale=comp.reduce((a,g)=>a+ref[g.label].len,0)/comp.reduce((a,g)=>a+g.len,0);how='auto from '+comp.length+' complete segment(s)';}
 else{scale=1;how='NOT calibrated yet (needs a straight bounded by two corners) - UNO model units';notes.push('No complete segment yet: drive at least from C past corner E.');}
 const A=[[0,0,0],[0,0,0],[0,0,0]],bb=[0,0,0];let used=0;
 segs.forEach(g=>{if(!g.inOrder)return;const h=ref[g.label].h;
  for(let i=g.i0;i<=g.i1;i++){if(run[i].fs!==1)continue;const v=[1,t[i],yaw[i]],e=yaw[i]-h;
   for(let p=0;p<3;p++){bb[p]+=v[p]*e;for(let q=0;q<3;q++)A[p][q]+=v[p]*v[q];}used++;}});
 const fit=used>30&&new Set(segs.map(g=>g.label)).size>=3?mapSolve3(A,bb):null;let yawc,gyroTxt;
 if(fit){yawc=yaw.map((v,i)=>(v-fit[0]-fit[1]*t[i])/(1+fit[2]));gyroTxt='gyro correction: offset '+fit[0].toFixed(2)+' deg, drift '+fit[1].toFixed(3)+' deg/s, scale error '+(fit[2]*100).toFixed(2)+' %';}
 else{yawc=yaw.slice();gyroTxt='gyro correction: not fitted yet (needs 3 different straight segments)';}
 const xs=[0],ys=[0];
 for(let i=1;i<N;i++){const ds=(dist[i]-dist[i-1])*scale,h=(yawc[i]+yawc[i-1])/2*Math.PI/180;xs.push(xs[i-1]+ds*Math.cos(h));ys.push(ys[i-1]+ds*Math.sin(h));}
 const closure=[];let s0=0;
 for(const e of lapEnds){const ex=xs[e]+so,ey=ys[e]; /* corner C should be at (-so, 0) */closure.push(Math.hypot(ex,ey));const d0=dist[s0],d1=dist[e],dd=d1>d0?d1-d0:1;
  for(let i=s0;i<N;i++){const f=i>=e?1:(dist[i]-d0)/dd;xs[i]-=ex*f;ys[i]-=ey*f;}s0=e;}
 const cnt=b=>run.reduce((a,r)=>a+((r.k>>b)&1),0);
 const yawMoved=run.some(r=>r.yaw!==0),gyOk=run.filter(r=>r.gy).length;
 if(!yawMoved)notes.push('GYRO NOT WORKING: heading stayed exactly 0 for the whole run, so no map can be built and the car cannot recognise corners or the old track. Check the IMU line on this page (IM=0 means no chip found).');
 else if(gyOk===0&&run.some(r=>r.k!==undefined))notes.push('Samples do not carry the gyro flag (older UNO firmware).');
 if(segs.some(g=>!g.inOrder))notes.push('Segments marked (?) are out of the expected C>D>E>A>B order: the car took a wrong branch or reversed there.');
 return mapAddTracks({run,t,yaw,yawc,dist,xs,ys,segs,labels,scale,how,gyroTxt,laps:lapEnds.length,closure,lapEnds,notes,gaps,
  counts:{cross:cnt(4),turn:cnt(5),search:cnt(6),allBlack:cnt(3)}},ref);}

/* ---- Test11: predicted vs fused tracks ----
   fused     = gyro heading + wheel-speed model, exactly as the UNO logged it (x,y in cm, unscaled)
   predicted = wheel-speed model only (kinematics: v=(vl+vr)/2, w=(vr-vl)/b, midpoint integration from recorded PWM), no gyro
   mapped    = fused + this page's post-processing (gyro offset/drift fit, distance scale, lap closure) */
function mapDistSeg(px,py,a,b){const dx=b[0]-a[0],dy=b[1]-a[1],l2=dx*dx+dy*dy;let f=l2?((px-a[0])*dx+(py-a[1])*dy)/l2:0;f=Math.max(0,Math.min(1,f));return Math.hypot(px-a[0]-f*dx,py-a[1]-f*dy);}
function mapDistPoly(px,py,poly){let m=1e9;for(let i=1;i<poly.length;i++){const d=mapDistSeg(px,py,poly[i-1],poly[i]);if(d<m)m=d;}return m;}
function mapXtrack(xs,ys,poly){let s=0,q=0,mx=0;const n=xs.length;for(let i=0;i<n;i++){const d=mapDistPoly(xs[i],ys[i],poly);s+=d;q+=d*d;if(d>mx)mx=d;}return{mean:s/n,rms:Math.sqrt(q/n),max:mx};}
function mapUnwrap(deg){const o=[0];for(let i=1;i<deg.length;i++){let d=deg[i]-deg[i-1];d=((d+540)%360+360)%360-180;o.push(o[i-1]+d);}return o;}
// Test21: the UNO no longer integrates the wheel-only pose; rebuild it here from the logged PWM with the same model
// (v = 0.33 cm/s per PWM x (PL+PR)/2, w = 0.33 x (PR-PL)/14 cm, midpoint heading). Older logs keep their own values.
function mapPredictFromPwm(run){
 if(!run.length||!run.every(r=>!r.kx&&!r.ky))return;
 let x=0,y=0,h=0;run[0].kx=0;run[0].ky=0;run[0].kh=0;
 for(let i=1;i<run.length;i++){const p=run[i-1],dt=Math.min(Math.max((run[i].t-p.t)/1000,0),0.3),v=0.33*(p.PL+p.PR)/2,w=0.33*(p.PR-p.PL)/14;
  x+=v*Math.cos(h+w*dt/2)*dt;y+=v*Math.sin(h+w*dt/2)*dt;h+=w*dt;run[i].kx=x;run[i].ky=y;run[i].kh=h*180/Math.PI;}
}
function mapAddTracks(res,ref){
 mapPredictFromPwm(res.run);
 const run=res.run,N=run.length,poly=mapRefPoly(ref);
 res.fx=run.map(r=>r.x-run[0].x);res.fy=run.map(r=>r.y-run[0].y);
 res.hasPred=run.every(r=>r.kh!==undefined);
 if(res.hasPred){res.kyaw=mapUnwrap(run.map(r=>r.kh));res.px=run.map(r=>r.kx-run[0].kx);res.py=run.map(r=>r.ky-run[0].ky);}
 res.err={fused:mapXtrack(res.fx,res.fy,poly),mapped:mapXtrack(res.xs,res.ys,poly)};
 if(res.hasPred)res.err.pred=mapXtrack(res.px,res.py,poly);
 const hrms=a=>{let q=0,n=0;res.labels.forEach((l,i)=>{if(l===null)return;const e=a[i]-ref[l].h;q+=e*e;n++;});return n?Math.sqrt(q/n):null;};
 res.hdg={fused:hrms(res.yaw),pred:res.hasPred?hrms(res.kyaw):null};
 res.hdgEnd=res.hasPred?res.kyaw[N-1]-res.yaw[N-1]:null;
 res.hdgRms=res.hasPred?Math.sqrt(res.yaw.reduce((a,v,i)=>a+(res.kyaw[i]-v)*(res.kyaw[i]-v),0)/N):null;
 mapStateAnalysis(res);
 {const so=mapStartOff();res.closureRaw=res.lapEnds.map(e=>({fused:Math.hypot(res.fx[e]+so,res.fy[e]),pred:res.hasPred?Math.hypot(res.px[e]+so,res.py[e]):null}));}
 mapLaps(res);
 return res;}

/* ---- Test21: state machine log, overtaking episodes, object positions (Assignment 4) ---- */
const SMNAME=['STANDBY','TRACK','CROSSING','CORNER TURN','LINE SEARCH','STOP AHEAD: SLOW','STOP SIGN: STOP 3 s','RESERVED','RESERVED','AVOID: TURN + FIRST LEG','AVOID: STRAIGHT PAST','AVOID: RETURN + JOIN','FAULT','LAPS FINISHED','RESERVED','TARGET FOLLOWING'];
const ROUTESEG=['S-D','D-E','E-O (STOP)','O-A','A-B','B-C','C-O (STOP)','O-S (light)'];
const SMCOL=['#adb5bd','#2f9e44','#f08c00','#e64980','#ae3ec9','#e03131','#c92a2a','#fa5252','#845ef7','#7048e8','#5f3dc4','#4c6ef5','#000000','#868e96','#1098ad','#ced4da'];
function mapSignRange(){return typeof MAPSIGNRANGE==='number'?MAPSIGNRANGE:35;}
function mapStateAnalysis(res){
 const run=res.run,N=run.length;res.hasSm=run.every(r=>r.sm!==undefined);res.trans=[];res.episodes=[];res.objects=[];res.offroad=[];
 if(!res.hasSm)return;
 const rad=i=>res.yaw[i]*Math.PI/180;
 let st=run[0].sm,i0=0;
 const pushEp=(a,b,ok)=>{const o={i0:a,i1:b,dur:res.t[b]-res.t[a],ok:ok,x0:res.fx[a],y0:res.fy[a],x1:res.fx[b],y1:res.fy[b]};
  const u=run[a].u>0?run[a].u:(a>0&&run[a-1].u>0?run[a-1].u:-1);
  if(u>0)o.obstacle={x:res.fx[a]+u*Math.cos(rad(a)),y:res.fy[a]+u*Math.sin(rad(a)),u:u};res.episodes.push(o);};
 for(let i=1;i<=N;i++){
  if(i<N&&run[i].sm===st)continue;
  const dwell=res.t[Math.min(i,N-1)]-res.t[i0];
  res.trans.push({i:i0,t:res.t[i0],from:i0?run[i0-1].sm:null,to:st,x:res.fx[i0],y:res.fy[i0],h:res.yaw[i0],dwell:dwell});
  // object positions: where the car stood for a STOP sign / red light + nominal range ahead
  if(st===6||st===7||st===5){const r0=mapSignRange(),sx=res.fx[i0]+r0*Math.cos(rad(i0)),sy=res.fy[i0]+r0*Math.sin(rad(i0));
   if(st===6)res.objects.push({kind:'stop',x:sx,y:sy,i:i0});else if(st===7)res.objects.push({kind:'light',x:sx,y:sy,i:i0});}
  if(st===4&&dwell>3)res.offroad.push({t:res.t[i0],dur:dwell});
  i0=i;if(i<N)st=run[i].sm;
 }
 // overtaking episodes: samples with state 8..11 (wait behind counts as part of the manoeuvre), ended by TRACK = success, FAULT = failed
 let a=-1;for(let i=0;i<N;i++){const o=run[i].sm>=8&&run[i].sm<=11;if(o&&a<0)a=i;if(!o&&a>=0){pushEp(a,i-1,run[i].sm!==12);a=-1;}}
 if(a>=0)pushEp(a,N-1,null);
 res.episodes.forEach(e=>{if(e.obstacle)res.objects.push({kind:'obstacle',x:e.obstacle.x,y:e.obstacle.y,i:e.i0});});
 // Test21: localizer records of this run (q range) and objects placed at their EKF TRACK position + range ahead
 const q0=run[0].q,q1=run[N-1].q,inRun=o=>o.runId===run[0].runId&&o.epoch===run[0].epoch&&o.q>=q0&&o.q<=q1;
 res.loc=(typeof mapLoc!=='undefined'?mapLoc:[]).filter(inRun);
 const eo=(typeof mapObj!=='undefined'?mapObj:[]).filter(inRun);
 res.objectsFused=res.objects;res.ekfObjects=eo.map(o=>{const kind=['','stop','light','obstacle'][o.O]||'stop',r=o.R>0?o.R:(o.O===3?20:mapSignRange()),P=o.P/10+r,pt=mapTrackPoint(P);
  return {kind:kind,x:pt.x,y:pt.y,i:Math.max(0,run.findIndex(z=>z.q>=o.q)),P:P,R:o.R,ekf:true};});
 if(res.ekfObjects.length)res.objects=res.ekfObjects; // the map shows the localizer positions; the fused-estimate ones stay in the summary
}
// point of the drawn track at lap position P (cm after S, any lap)
function mapTrackPoint(P){const ref=typeof mapRef!=='undefined'?mapRef:MAPREF0,poly=mapRefPoly(ref),tot=ref.reduce((a,s)=>a+s.len,0);
 let a=((mapStartOff()+P)%tot+tot)%tot;
 for(let i=0;i<ref.length;i++){const L=ref[i].len;if(a<=L||i===ref.length-1){const f=Math.min(a/L,1);return {x:poly[i][0]+f*(poly[i+1][0]-poly[i][0]),y:poly[i][1]+f*(poly[i+1][1]-poly[i][1])};}a-=L;}
 return {x:0,y:0};}
// lap position ->UNO route segment name and distance to its end (same table as the UNO / ESP32)
function mapSegOf(P){const e=$('xSO'),so=(e&&+e.value)||83,L=[167-so,148,118,99,47,125,150,so],tot=L.reduce((a,b)=>a+b,0),END=['D','E','O','A','B','C','O','S'];
 let a=((P%tot)+tot)%tot;for(let i=0;i<8;i++){if(a<L[i]||i===7)return ROUTESEG[i]+', '+Math.max(0,L[i]-a).toFixed(0)+' cm before '+END[i];a-=L[i];}}
function mapStateLines(res){const L=[];
 if(!res.hasSm){L.push('state machine: not logged (UNO older than Test21)');return L;}
 const dur={};res.trans.forEach(t=>{dur[t.to]=(dur[t.to]||0)+t.dwell;});
 L.push('state machine: '+res.trans.length+' transitions; time in each state: '+Object.keys(dur).map(k=>SMNAME[k]+' '+dur[k].toFixed(1)+' s').join(', '));
 const ep=res.episodes;
 L.push('overtaking: '+ep.length+' manoeuvre(s)'+(ep.length?': '+ep.map((e,i)=>(i+1)+') '+e.dur.toFixed(1)+' s, '+(e.ok===null?'not finished':e.ok?'finished, back on the line':'FAILED')+(e.obstacle?', obstacle at ('+e.obstacle.x.toFixed(0)+', '+e.obstacle.y.toFixed(0)+') cm':'')).join('; '):' (none in this run)'));
 if(res.loc&&res.loc.length){const fx=res.loc.filter(r=>r.E>=0),inn=fx.map(r=>Math.abs(r.I/10)),last=res.loc[res.loc.length-1],LM=['D','E','O','A','B','C','O'];
  L.push('track localizer (EKF on the ESP32): '+fx.length+' landmark fixes ('+fx.map(r=>LM[r.E]).join(' ')+'), mean |innovation| '+(inn.length?(inn.reduce((a,b)=>a+b,0)/inn.length).toFixed(1):'—')+' cm, max '+(inn.length?Math.max(...inn).toFixed(1):'—')+' cm; final position ±'+(last.S/10).toFixed(1)+' cm, odometer scale '+(last.K/1000).toFixed(3)+', gyro drift '+(last.H/10).toFixed(1)+'°, map/segment disagreements '+last.M);}
 if(res.ekfObjects&&res.ekfObjects.length)L.push('objects at their localizer track position: '+res.ekfObjects.map(o=>o.kind+' '+mapSegOf(o.P)+' (track '+o.P.toFixed(0)+' cm'+(o.R>0?', range '+o.R+' cm':', nominal range')+', x '+o.x.toFixed(0)+' y '+o.y.toFixed(0)+')').join('; '));
 const sg=(res.objectsFused||res.objects).filter(o=>o.kind==='stop'),lt=(res.objectsFused||res.objects).filter(o=>o.kind==='light');
 L.push('estimated STOP sign position(s) (stopping pose + '+mapSignRange()+' cm ahead): '+(sg.length?sg.map(o=>'('+o.x.toFixed(0)+', '+o.y.toFixed(0)+')').join(', '):'none this run')+'; traffic light: '+(lt.length?lt.map(o=>'('+o.x.toFixed(0)+', '+o.y.toFixed(0)+')').join(', '):'start point (0, 0)'));
 const holds=[];res.trans.forEach((t,k)=>{if(t.to!==6)return;let d=0,j=k;for(;j<res.trans.length&&(res.trans[j].to===6||res.trans[j].to===14);j++)d+=res.trans[j].dwell;if(j<res.trans.length)holds.push(d);});
 const shortHold=holds.filter(d=>d<2.9).length;L.push('STOP holds (stop + AI + intersection scan): '+(holds.length?holds.map(d=>d.toFixed(1)+' s').join(', '):'none'));
 L.push('self-check of traffic rules: STOP holds shorter than 3 s: '+shortHold+'; off-road episodes longer than 3 s (line search): '+res.offroad.length+(res.offroad.length?' ('+res.offroad.map(o=>o.dur.toFixed(1)+' s at t='+o.t.toFixed(0)).join(', ')+')':''));
 return L;}
function mapTransRows(res){return (res.trans||[]).map(t=>({t_s:+t.t.toFixed(2),from:t.from===null?'':SMNAME[t.from],to:SMNAME[t.to],fused_x_cm:+t.x.toFixed(1),fused_y_cm:+t.y.toFixed(1),fused_heading_deg:+t.h.toFixed(1),time_in_state_s:+t.dwell.toFixed(2)}));}
function mapStateSVG(res){
 if(!res.hasSm)return '';
 const W=900,ml=150,mr=12,mt=30,rowH=20,used=[...new Set(res.run.map(r=>r.sm))].sort((a,b)=>a-b),H=mt+used.length*rowH+36,T=res.t[res.t.length-1]||1,X=t=>(ml+t/T*(W-ml-mr)).toFixed(1),o=[];
 o.push('<svg xmlns="http://www.w3.org/2000/svg" width="'+W+'" height="'+H+'" viewBox="0 0 '+W+' '+H+'" font-family="sans-serif"><rect width="100%" height="100%" fill="#ffffff"/>');
 o.push('<text x="10" y="18" font-size="15" fill="#222">State machine: current state vs time</text>');
 used.forEach((u,k)=>{const y=mt+k*rowH;o.push('<line x1="'+ml+'" y1="'+(y+rowH-1)+'" x2="'+(W-mr)+'" y2="'+(y+rowH-1)+'" stroke="#eee"/><text x="'+(ml-6)+'" y="'+(y+14)+'" font-size="11" text-anchor="end" fill="#333">'+SMNAME[u]+'</text>');});
 res.trans.forEach(t=>{const k=used.indexOf(t.to),y=mt+k*rowH,w=Math.max(1.5,(t.dwell/T)*(W-ml-mr));o.push('<rect x="'+X(t.t)+'" y="'+(y+3)+'" width="'+w.toFixed(1)+'" height="'+(rowH-7)+'" fill="'+SMCOL[t.to]+'"/>');});
 for(let g=0;g<=T;g+=Math.max(5,Math.ceil(T/10/5)*5))o.push('<text x="'+X(g)+'" y="'+(H-16)+'" font-size="11" text-anchor="middle" fill="#555">'+g+'</text>');
 o.push('<text x="'+((ml+W-mr)/2)+'" y="'+(H-3)+'" font-size="12" text-anchor="middle" fill="#333">time (s)</text></svg>');return o.join('\n');}
function mapNum(v,d){return v===null||v===undefined||!isFinite(v)?'n/a':v.toFixed(d===undefined?1:d);}
function mapTrackLines(res){const L=[],e=res.err,f=k=>e[k]?'mean '+mapNum(e[k].mean)+', rms '+mapNum(e[k].rms)+', max '+mapNum(e[k].max)+' cm':'n/a (UNO older than Test11: no predicted pose logged)';
 L.push('track error = distance to the drawn centre line (grey):');
 L.push('  predicted (wheel model only): '+f('pred'));L.push('  fused (gyro + wheel model)  : '+f('fused'));L.push('  mapped (corrected + scaled) : '+f('mapped'));
 L.push('heading error on the straights vs the drawing (rms): predicted '+mapNum(res.hdg.pred)+' deg, fused/gyro '+mapNum(res.hdg.fused)+' deg');
 if(res.hasPred)L.push('predicted heading minus gyro heading: at the end '+mapNum(res.hdgEnd)+' deg, rms over the run '+mapNum(res.hdgRms)+' deg');
 mapStateLines(res).forEach(x=>L.push(x));
 if(res.closureRaw.length)L.push('lap closure (end of lap vs start, raw): '+res.closureRaw.map((c,i)=>'lap '+(i+1)+' fused '+mapNum(c.fused)+' cm'+(c.pred===null?'':', predicted '+mapNum(c.pred)+' cm')).join('; '));
 return L;}
/* generic line chart (SVG string). o={title,xl,yl,series:[{name,color,x,y,dash,w}],bars:[{x0,x1,y,color}],zero:true} */
function mapChartSVG(o){
 const W=900,H=300,ml=58,mr=14,mt=34,mb=44,xs=[],ys=[];
 o.series.forEach(s=>{s.x.forEach(v=>xs.push(v));s.y.forEach(v=>{if(isFinite(v))ys.push(v);});});(o.bars||[]).forEach(b=>{xs.push(b.x0,b.x1);ys.push(b.y);});
 if(!xs.length)return '';
 let x0=Math.min(...xs),x1=Math.max(...xs),y0=Math.min(...ys),y1=Math.max(...ys);if(o.zero){y0=Math.min(y0,0);y1=Math.max(y1,0);}
 if(x1-x0<1e-9)x1=x0+1;if(y1-y0<1e-9){y1=y0+1;}const pad=(y1-y0)*0.06;y0-=pad;y1+=pad;
 const step=v=>{const r=v/6,p=Math.pow(10,Math.floor(Math.log10(r))),f=r/p;return(f<1.5?1:f<3?2:f<7?5:10)*p;};
 const sx=step(x1-x0),sy=step(y1-y0),X=v=>(ml+(v-x0)/(x1-x0)*(W-ml-mr)).toFixed(1),Y=v=>(H-mb-(v-y0)/(y1-y0)*(H-mt-mb)).toFixed(1),out=[];
 out.push('<svg xmlns="http://www.w3.org/2000/svg" width="'+W+'" height="'+H+'" viewBox="0 0 '+W+' '+H+'" font-family="sans-serif"><rect width="100%" height="100%" fill="#ffffff"/>');
 out.push('<text x="'+ml+'" y="18" font-size="15" fill="#222">'+o.title+'</text>');
 for(let g=Math.ceil(x0/sx)*sx;g<=x1+1e-9;g+=sx)out.push('<line x1="'+X(g)+'" y1="'+mt+'" x2="'+X(g)+'" y2="'+(H-mb)+'" stroke="#eee"/><text x="'+X(g)+'" y="'+(H-mb+16)+'" font-size="11" text-anchor="middle" fill="#555">'+(+g.toFixed(6))+'</text>');
 for(let g=Math.ceil(y0/sy)*sy;g<=y1+1e-9;g+=sy)out.push('<line x1="'+ml+'" y1="'+Y(g)+'" x2="'+(W-mr)+'" y2="'+Y(g)+'" stroke="'+(Math.abs(g)<1e-9?'#bbb':'#eee')+'"/><text x="'+(ml-6)+'" y="'+(+Y(g)+4)+'" font-size="11" text-anchor="end" fill="#555">'+(+g.toFixed(6))+'</text>');
 out.push('<text x="'+((ml+W-mr)/2)+'" y="'+(H-8)+'" font-size="12" text-anchor="middle" fill="#333">'+o.xl+'</text><text transform="translate(14 '+((mt+H-mb)/2)+') rotate(-90)" font-size="12" text-anchor="middle" fill="#333">'+o.yl+'</text>');
 (o.bars||[]).forEach(b=>out.push('<line x1="'+X(b.x0)+'" y1="'+Y(b.y)+'" x2="'+X(b.x1)+'" y2="'+Y(b.y)+'" stroke="'+b.color+'" stroke-width="7" stroke-opacity="0.35"/>'));
 o.series.forEach(s=>{const pts=[];s.x.forEach((v,i)=>{if(isFinite(s.y[i]))pts.push(X(v)+','+Y(s.y[i]));});
  out.push('<polyline fill="none" stroke="'+s.color+'" stroke-width="'+(s.w||2)+'"'+(s.dash?' stroke-dasharray="'+s.dash+'"':'')+' stroke-linejoin="round" points="'+pts.join(' ')+'"/>');});
 let lx=ml+4;const ly=H-mb-6;
 o.series.concat((o.bars||[]).length?[{name:o.barName||'drawing',color:o.bars[0].color,w:7}]:[]).forEach(s=>{out.push('<rect x="'+lx+'" y="'+(ly-9)+'" width="16" height="4" fill="'+s.color+'"/><text x="'+(lx+21)+'" y="'+(ly-4)+'" font-size="12" fill="#222">'+s.name+'</text>');lx+=24+s.name.length*7;});
 out.push('</svg>');return out.join('\n');}
function mapHeadingSVG(res,ref){
 const bars=res.segs.map(g=>({x0:res.t[g.i0],x1:res.t[g.i1],y:ref[g.label].h,color:'#868e96'})),S=[];
 if(res.hasPred)S.push({name:'predicted (wheel model)',color:'#8d6e63',x:res.t,y:res.kyaw,dash:'6 4',w:2});
 S.push({name:'gyro / fused',color:'#2f9e44',x:res.t,y:res.yaw,w:2});S.push({name:'mapped (corrected)',color:'#2a7de1',x:res.t,y:res.yawc,w:1.2});
 return mapChartSVG({title:'Heading vs time (deg, + = left, 0 = along C-O-D)',xl:'time (s)',yl:'heading (deg)',series:S,bars:bars,barName:'drawing heading',zero:true});}
/* ---- Test21: lap times. The UNO logs its map segment in the high nibble of the state code; a lap ends when the
   segment wraps O-S ->S-D (the car passes the start S). Race time starts when the car leaves RESERVED. ---- */
function mapLaps(res){
 const run=res.run,N=run.length,rows=[];res.lapRows=rows;res.lapNo=new Array(N).fill('');res.race=null;
 if(N<2||run[0].seg===undefined)return;
 let s0=0;while(s0<N-1&&(run[s0].sm===7||(run[s0].PL===0&&run[s0].PR===0)))s0++;
 const sc=res.scale||1,mk=(a,b,done)=>{const T=res.t[b]-res.t[a],D=(res.dist[b]-res.dist[a])*sc;let stopT=0,stops=0,avoid=0,red=0;
  for(let i=a+1;i<=b;i++){const dt=res.t[i]-res.t[i-1];if(run[i].PL===0&&run[i].PR===0)stopT+=dt;const p=run[i-1].sm,c=run[i].sm;if(c!==p){if(c===6)stops++;if(c===9)avoid++;if(c===7)red++;}}
  return{lap:rows.length+1,complete:done?1:0,t_start_s:+res.t[a].toFixed(2),t_end_s:+res.t[b].toFixed(2),lap_time_s:+T.toFixed(2),distance_cm:+D.toFixed(1),
   avg_speed_cm_s:T>0?+(D/T).toFixed(1):'',moving_time_s:+(T-stopT).toFixed(2),avg_moving_speed_cm_s:T-stopT>0?+(D/(T-stopT)).toFixed(1):'',
   stop_sign_stops:stops,red_light_stops:red,avoid_manoeuvres:avoid,start_fused_x_cm:+res.fx[a].toFixed(1),start_fused_y_cm:+res.fy[a].toFixed(1),end_fused_x_cm:+res.fx[b].toFixed(1),end_fused_y_cm:+res.fy[b].toFixed(1),
   end_mapped_x_cm:+res.xs[b].toFixed(1),end_mapped_y_cm:+res.ys[b].toFixed(1)};};
 let a=s0;
 for(let i=s0+1;i<N;i++){const p=run[i-1].seg,c=run[i].seg;if(p>=6&&c<=1&&c<p){for(let j=a;j<i;j++)res.lapNo[j]=rows.length+1;rows.push(mk(a,i,true));a=i;}}
 const done=rows.length;
 if(a<N-1){for(let j=a;j<N;j++)res.lapNo[j]=rows.length+1;rows.push(mk(a,N-1,false));}
 if(done){const e=rows[done-1],T=e.t_end_s-res.t[s0],D=rows.slice(0,done).reduce((x,r)=>x+r.distance_cm,0);
  res.race={laps:done,time:T,dist:D,avg:T>0?D/T:0,best:Math.min(...rows.slice(0,done).map(r=>r.lap_time_s))};}
}
function mapLapLines(res){const L=[],R=res.lapRows||[];if(!R.length)return L;
 R.forEach(r=>L.push('lap '+r.lap+(r.complete?'':' (in progress)')+': '+r.lap_time_s.toFixed(2)+' s, '+r.distance_cm.toFixed(0)+' cm, average '+r.avg_speed_cm_s+' cm/s (moving '+r.avg_moving_speed_cm_s+' cm/s), STOP holds '+r.stop_sign_stops+', red-light stops '+r.red_light_stops+', avoidance '+r.avoid_manoeuvres));
 if(res.race)L.push('RACE: '+res.race.laps+' lap(s) in '+res.race.time.toFixed(2)+' s (from the green light), '+res.race.dist.toFixed(0)+' cm, average speed '+res.race.avg.toFixed(1)+' cm/s, best lap '+res.race.best.toFixed(2)+' s');
 return L;}
function mapLapHTML(res){const R=res.lapRows||[];if(!R.length)return '<p class="hint">Lap times appear after the car passes the start point S (needs UNO Test21).</p>';
 let h='<table style="width:100%;font-size:14px"><tr><th>Lap </th><th>Time  (s)</th><th>Distance  (cm)</th><th>Avg speed  (cm/s)</th><th>Moving avg (cm/s)</th><th>STOP holds</th><th>Red stops</th><th>Avoid</th></tr>';
 R.forEach(r=>{h+='<tr'+(r.complete?'':' style="opacity:.6"')+'><td>'+r.lap+(r.complete?'':' …')+'</td><td>'+r.lap_time_s.toFixed(2)+'</td><td>'+r.distance_cm.toFixed(0)+'</td><td>'+r.avg_speed_cm_s+'</td><td>'+r.avg_moving_speed_cm_s+'</td><td>'+r.stop_sign_stops+'</td><td>'+r.red_light_stops+'</td><td>'+r.avoid_manoeuvres+'</td></tr>';});
 if(res.race)h+='<tr style="font-weight:700"><td>Race</td><td>'+res.race.time.toFixed(2)+'</td><td>'+res.race.dist.toFixed(0)+'</td><td>'+res.race.avg.toFixed(1)+'</td><td colspan="4">best lap '+res.race.best.toFixed(2)+' s</td></tr>';
 return h+'</table>';}
function mapRefPoly(ref){const so=mapStartOff(),p=[[-so,0]];let x=-so,y=0;for(const s of ref){const h=s.h*Math.PI/180;x+=s.len*Math.cos(h);y+=s.len*Math.sin(h);p.push([x,y]);}return p;}
function mapSummary(res,ref,runNo,runCount){const L=[];
 L.push('Test11 map summary (run '+runNo+' of '+runCount+', '+res.run.length+' samples, '+res.t[res.t.length-1].toFixed(1)+' s, UNO sample gaps '+res.gaps+')');
 L.push('distance scale: x'+res.scale.toFixed(3)+' ('+res.how+')');
 L.push('  ->measured speed factor ~ '+(0.33*res.scale).toFixed(3)+' cm/s per PWM (UNO uses 0.33; MapConfig.h LEFT/RIGHT_CM_S_PER_PWM)');
 L.push(res.gyroTxt);
 mapLapLines(res).forEach(x=>L.push(x));const lt=$('mapLaps');if(lt)lt.innerHTML=mapLapHTML(res);if(typeof slamRender==='function'&&typeof document!=='undefined')setTimeout(()=>slamRender(res),0);
 L.push('laps completed (map, at corner C): '+res.laps+(res.closure.length?'; closure error before correction: '+res.closure.map(e=>e.toFixed(1)+' cm').join(', '):''));
 L.push('segment order seen: '+(res.segs.map(g=>ref[g.label].name+(g.inOrder?'':'(?)')).join(' >')||'(none yet)'));
 L.push('crossing holds: '+res.counts.cross+', corner turns: '+res.counts.turn+', searches: '+res.counts.search+', all-black hits: '+res.counts.allBlack);
 mapTrackLines(res).forEach(x=>L.push(x));res.notes.forEach(s=>L.push('NOTE: '+s));return L.join('\n');}
function mapSegRows(res,ref){let prevM=null,prevR=null;return res.segs.map((g,k)=>{const s=ref[g.label];let hs=0;for(let i=g.i0;i<=g.i1;i++)hs+=res.yawc[i];const len=g.len*res.scale;
 const mh=hs/(g.i1-g.i0+1),nrm=a=>((a+540)%360+360)%360-180,et=prevR===null?null:nrm(s.h-prevR),mt=prevM===null?null:nrm(mh-prevM);prevM=mh;prevR=s.h;
 const wrong=et!==null&&Math.abs(et)>30&&mt!==null&&Math.sign(et)!==Math.sign(mt);
 return{order:k+1,segment:s.name,complete:g.complete?1:0,expected_turn_deg:et===null?'':+et.toFixed(0),measured_turn_deg:mt===null?'':+mt.toFixed(0),turn_check:et===null?'':(wrong?'WRONG WAY':'ok'),ref_heading_deg:s.h,meas_heading_deg:+mh.toFixed(1),ref_length_cm:s.len,meas_length_cm:+len.toFixed(1),
  length_error_cm:g.complete?+(len-s.len).toFixed(1):'',t_start_s:+res.t[g.i0].toFixed(1),t_end_s:+res.t[g.i1].toFixed(1)};});}
function mapCSV(rows){if(!rows.length)return'';const k=Object.keys(rows[0]);return k.join(',')+'\n'+rows.map(r=>k.map(c=>r[c]===undefined||r[c]===null?'':r[c]).join(',')).join('\n')+'\n';}
function mapPointRows(res,ref){return res.run.map((r,i)=>({t_s:+res.t[i].toFixed(2),s_cm:+(res.dist[i]*res.scale).toFixed(1),x_cm:+res.xs[i].toFixed(1),y_cm:+res.ys[i].toFixed(1),
 heading_deg:+res.yawc[i].toFixed(1),fused_x_cm:+res.fx[i].toFixed(1),fused_y_cm:+res.fy[i].toFixed(1),fused_heading_deg:+res.yaw[i].toFixed(1),pred_x_cm:res.hasPred?+res.px[i].toFixed(1):'',pred_y_cm:res.hasPred?+res.py[i].toFixed(1):'',pred_heading_deg:res.hasPred?+res.kyaw[i].toFixed(1):'',state:res.hasSm?SMNAME[res.run[i].sm]:'',lap:res.lapNo?res.lapNo[i]:'',route_segment:r.seg!==undefined?ROUTESEG[r.seg]:'',segment:res.labels[i]===null?'corner':ref[res.labels[i]].name,follow_state:r.fs,L:r.L,C:r.C,R:r.R,all_black:(r.k>>3)&1,ev_cross:(r.k>>4)&1}));}
function mapSVG(res,ref,title,opt){
 opt=Object.assign({drawing:true,pred:true,fused:true,mapped:true},opt||{});
 const rp=mapRefPoly(ref),ax=[].concat(rp.map(p=>p[0])),ay=[].concat(rp.map(p=>p[1]));
 if(opt.mapped){ax.push(...res.xs);ay.push(...res.ys);}if(opt.fused){ax.push(...res.fx);ay.push(...res.fy);}if(opt.pred&&res.hasPred){ax.push(...res.px);ay.push(...res.py);}
 const ent=[];
 if(opt.drawing)ent.push(['b','#999','drawing',85]);
 if(opt.pred&&res.hasPred)ent.push(['d','#8d6e63','predicted (wheel model)',170]);
 if(opt.fused)ent.push(['l','#2f9e44','fused (gyro + wheel)',150]);
 if(opt.mapped)[[1,'FOLLOW'],[2,'CROSS hold'],[3,'corner TURN'],[4,'SEARCH']].forEach(([st,nm])=>ent.push(['l',MAPCOL[st],'mapped '+nm,145]));
 if(res.hasSm&&res.episodes.length)ent.push(['l','#7048e8','overtaking path',120]);
 ent.push(['r','#e03131','car now',70]);
 {let lx=10,row=0;ent.forEach(e=>{if(lx+e[3]>900){lx=10;row++;}e.push(lx,row);lx+=e[3];});} // legend wraps onto new rows (own x and row per entry)
 const rows=ent[ent.length-1][5]+1;
 const x0=Math.min(...ax)-30,x1=Math.max(...ax)+30,y0=Math.min(...ay)-30,y1=Math.max(...ay)+30,W=900,k=W/(x1-x0),H=Math.round((y1-y0)*k)+70+(rows-1)*18;
 const X=x=>((x-x0)*k).toFixed(1),Y=y=>((y1-y)*k+40).toFixed(1),o=[];
 const line=(xs,ys,c,w,dash,op)=>'<polyline fill="none" stroke="'+c+'" stroke-width="'+w+'"'+(dash?' stroke-dasharray="'+dash+'"':'')+(op?' stroke-opacity="'+op+'"':'')+' stroke-linecap="round" stroke-linejoin="round" points="'+xs.map((x,i)=>X(x)+','+Y(ys[i])).join(' ')+'"/>';
 o.push('<svg xmlns="http://www.w3.org/2000/svg" width="'+W+'" height="'+H+'" viewBox="0 0 '+W+' '+H+'" font-family="sans-serif">');
 o.push('<rect width="100%" height="100%" fill="#ffffff"/>');
 o.push('<text x="10" y="22" font-size="16" fill="#222">'+title+' - grid 50 cm'+(opt.mapped?' - mapped scale x'+res.scale.toFixed(2):'')+'</text>');
 for(let g=Math.floor(x0/50)*50;g<=x1;g+=50)o.push('<line x1="'+X(g)+'" y1="40" x2="'+X(g)+'" y2="'+H+'" stroke="#eee"/>');
 for(let g=Math.floor(y0/50)*50;g<=y1;g+=50)o.push('<line x1="0" y1="'+Y(g)+'" x2="'+W+'" y2="'+Y(g)+'" stroke="#eee"/>');
 if(opt.drawing)o.push('<polyline fill="none" stroke="#999" stroke-width="10" stroke-opacity="0.35" stroke-linejoin="round" points="'+rp.map(p=>X(p[0])+','+Y(p[1])).join(' ')+'"/>');
 if(opt.mapped){let i=1;while(i<res.xs.length){const c=MAPCOL[res.run[i].fs]||MAPCOL[1],pts=[X(res.xs[i-1])+','+Y(res.ys[i-1])];
  while(i<res.xs.length&&(MAPCOL[res.run[i].fs]||MAPCOL[1])===c){pts.push(X(res.xs[i])+','+Y(res.ys[i]));i++;}
  o.push('<polyline fill="none" stroke="'+c+'" stroke-width="2.5" stroke-linecap="round" stroke-linejoin="round" points="'+pts.join(' ')+'"/>');}
  res.run.forEach((r,j)=>{if((r.k>>3)&1)o.push('<circle cx="'+X(res.xs[j])+'" cy="'+Y(res.ys[j])+'" r="2.5" fill="#212529"/>');});
  res.segs.slice(1).forEach(g=>o.push('<rect x="'+(X(res.xs[g.i0])-3.5)+'" y="'+(Y(res.ys[g.i0])-3.5)+'" width="7" height="7" fill="#1098ad"/>'));}
 if(opt.fused)o.push(line(res.fx,res.fy,'#2f9e44',2.2,'',0.95));
 if(opt.pred&&res.hasPred)o.push(line(res.px,res.py,'#8d6e63',2.4,'7 4',1));
 if(res.hasSm){res.episodes.forEach(e=>{const xs=[],ys=[];for(let i=e.i0;i<=e.i1;i++){xs.push(res.fx[i]);ys.push(res.fy[i]);}if(xs.length>1)o.push(line(xs,ys,'#7048e8',5,'',0.55));});
  res.objects.forEach(g=>{const cx=+X(g.x),cy=+Y(g.y);
   if(g.kind==='obstacle')o.push('<circle cx="'+cx+'" cy="'+cy+'" r="9" fill="#795548" stroke="#fff" stroke-width="1.5"/><text x="'+(cx+12)+'" y="'+(cy-6)+'" font-size="12" fill="#5d4037">obstacle</text>');
   else if(g.kind==='stop'){const pts=[0,1,2,3,4,5,6,7].map(j=>(cx+9*Math.cos(Math.PI/8+j*Math.PI/4)).toFixed(1)+','+(cy+9*Math.sin(Math.PI/8+j*Math.PI/4)).toFixed(1)).join(' ');o.push('<polygon points="'+pts+'" fill="#e03131" stroke="#fff" stroke-width="1.5"/><text x="'+cx+'" y="'+(cy+3.5)+'" font-size="7" font-weight="bold" fill="#fff" text-anchor="middle">STOP</text>');}
   else o.push('<rect x="'+(cx-5)+'" y="'+(cy-10)+'" width="10" height="20" rx="3" fill="#212529"/><circle cx="'+cx+'" cy="'+(cy-4)+'" r="3" fill="#fa5252"/><circle cx="'+cx+'" cy="'+(cy+4)+'" r="3" fill="#40c057"/>');});}
 const ex=opt.fused?res.fx:(opt.pred&&res.hasPred?res.px:res.xs),ey=opt.fused?res.fy:(opt.pred&&res.hasPred?res.py:res.ys),e=ex.length-1;
 o.push('<circle cx="'+X(ex[e])+'" cy="'+Y(ey[e])+'" r="5" fill="none" stroke="#e03131" stroke-width="2"/>');
 o.push('<circle cx="'+X(0)+'" cy="'+Y(0)+'" r="6" fill="#2b8a3e"/><text x="'+(+X(0)+8)+'" y="'+(+Y(0)+16)+'" font-size="13" fill="#2b8a3e">start S</text>');
 ent.forEach(([t,c,nm,w,lx,row])=>{const ly=H-12-(rows-1-row)*18;
  if(t==='r')o.push('<circle cx="'+(lx+5)+'" cy="'+(ly-3)+'" r="4" fill="none" stroke="'+c+'" stroke-width="2"/>');
  else if(t==='b')o.push('<rect x="'+lx+'" y="'+(ly-7)+'" width="14" height="8" fill="'+c+'" fill-opacity="0.35"/>');
  else o.push('<rect x="'+lx+'" y="'+(ly-4)+'" width="14" height="4" fill="'+c+'"/>');
  o.push('<text x="'+(lx+18)+'" y="'+ly+'" font-size="12">'+nm+'</text>');});
 o.push('</svg>');return o.join('\n');}
/* ---- target-following runs (mode 7): the L,C,R slots carry heading error, target width, bearing ---- */
function mapBuildFollow(run){
 mapWheelPrediction(run);
 const N=run.length,t=run.map(r=>(r.t-run[0].t)/1000),yaw=run.map(r=>r.yaw-run[0].yaw),set=run.map((r,i)=>yaw[i]+r.tgtErr),
  fx=run.map(r=>r.x-run[0].x),fy=run.map(r=>r.y-run[0].y),hasPred=run.every(r=>r.kh!==undefined),res={run,t,yaw,set,fx,fy,hasPred,width:run.map(r=>r.tgtW),bear:run.map(r=>r.tgtBear),err:run.map(r=>r.tgtErr)};
 if(hasPred){res.kyaw=mapUnwrap(run.map(r=>r.kh));res.px=run.map(r=>r.kx-run[0].kx);res.py=run.map(r=>r.ky-run[0].ky);}
 const trk=run.map((r,i)=>r.fs===1||r.fs===2?i:-1).filter(i=>i>=0),arr=run.findIndex(r=>r.fs===2),first=run.findIndex(r=>r.fs===1);
 res.firstTrack=first>=0?t[first]:null;res.arrived=arr>=0?t[arr]:null;
 res.errRms=trk.length?Math.sqrt(trk.reduce((a,i)=>a+res.err[i]*res.err[i],0)/trk.length):null;
 res.finalWidth=res.width[N-1];res.pathCm=(run[N-1].dist-run[0].dist);
 res.lostCount=run.reduce((a,r,i)=>a+(i>0&&(r.fs===3||r.fs===4)&&run[i-1].fs!==3&&run[i-1].fs!==4?1:0),0);
 res.hdgRms=hasPred?Math.sqrt(yaw.reduce((a,v,i)=>a+(res.kyaw[i]-v)*(res.kyaw[i]-v),0)/N):null;
 res.endGap=hasPred?Math.hypot(res.px[N-1]-fx[N-1],res.py[N-1]-fy[N-1]):null;
 return res;}
function mapFollowRows(res){return res.run.map((r,i)=>({t_s:+res.t[i].toFixed(2),state:['','TRACK','ARRIVED','LOST','SEARCH','','RESERVED'][r.fs]||r.fs,fused_x_cm:res.fx[i],fused_y_cm:res.fy[i],fused_heading_deg:+res.yaw[i].toFixed(1),
 pred_x_cm:res.hasPred?res.px[i]:'',pred_y_cm:res.hasPred?res.py[i]:'',pred_heading_deg:res.hasPred?+res.kyaw[i].toFixed(1):'',target_heading_deg:+res.set[i].toFixed(1),heading_error_deg:res.err[i],target_width_permille:res.width[i],target_bearing_deg:res.bear[i],PL:r.PL,PR:r.PR}));}
function mapFollowSummary(res,runNo,runCount){const L=[];
 L.push('Target-following summary (run '+runNo+' of '+runCount+', '+res.run.length+' samples, '+res.t[res.t.length-1].toFixed(1)+' s)');
 L.push('first target lock: '+(res.firstTrack===null?'never':res.firstTrack.toFixed(1)+' s')+'; arrived at stop width: '+(res.arrived===null?'no':res.arrived.toFixed(1)+' s')+'; target lost '+res.lostCount+' time(s)');
 L.push('heading error of the inner loop while tracking (rms): '+mapNum(res.errRms)+' deg; final target width '+res.finalWidth+' per mille of the image');
 L.push('distance driven (odometer): '+mapNum(res.pathCm)+' cm; end point fused ('+res.fx[res.fx.length-1]+', '+res.fy[res.fy.length-1]+') cm'+(res.hasPred?', predicted ('+res.px[res.px.length-1]+', '+res.py[res.py.length-1]+') cm, gap '+mapNum(res.endGap)+' cm':''));
 if(res.hasPred)L.push('predicted heading minus gyro heading: at the end '+mapNum(res.kyaw[res.kyaw.length-1]-res.yaw[res.yaw.length-1])+' deg, rms '+mapNum(res.hdgRms)+' deg');
 return L.join('\n');}
function mapFollowCharts(res){
 const H=[{name:'target heading (set point)',color:'#e8590c',x:res.t,y:res.set,dash:'6 4',w:2},{name:'gyro / fused heading',color:'#2f9e44',x:res.t,y:res.yaw,w:2}];
 if(res.hasPred)H.push({name:'predicted (wheel model)',color:'#8d6e63',x:res.t,y:res.kyaw,dash:'2 3',w:2});
 return [mapChartSVG({title:'Target following: heading vs time (deg, + = left)',xl:'time (s)',yl:'heading (deg)',series:H,zero:true}),
  mapChartSVG({title:'Target following: target width in the image (per mille) and camera bearing (deg)',xl:'time (s)',yl:'width / bearing',series:[{name:'width (per mille)',color:'#1c7ed6',x:res.t,y:res.width,w:2},{name:'bearing (deg, + = left)',color:'#ae3ec9',x:res.t,y:res.bear,w:1.5}],zero:true})];}
function mapFollowSVG(res,title,opt){
 opt=Object.assign({pred:true,fused:true},opt||{});
 const ax=[0],ay=[0];if(opt.fused){ax.push(...res.fx);ay.push(...res.fy);}if(opt.pred&&res.hasPred){ax.push(...res.px);ay.push(...res.py);}
 const x0=Math.min(...ax)-30,x1=Math.max(...ax)+30,y0=Math.min(...ay)-30,y1=Math.max(...ay)+30,W=900,k=W/(x1-x0),H=Math.round((y1-y0)*k)+70,X=x=>((x-x0)*k).toFixed(1),Y=y=>((y1-y)*k+40).toFixed(1),o=[];
 const line=(xs,ys,c,w,dash)=>'<polyline fill="none" stroke="'+c+'" stroke-width="'+w+'"'+(dash?' stroke-dasharray="'+dash+'"':'')+' stroke-linecap="round" stroke-linejoin="round" points="'+xs.map((x,i)=>X(x)+','+Y(ys[i])).join(' ')+'"/>';
 o.push('<svg xmlns="http://www.w3.org/2000/svg" width="'+W+'" height="'+H+'" viewBox="0 0 '+W+' '+H+'" font-family="sans-serif"><rect width="100%" height="100%" fill="#ffffff"/>');
 o.push('<text x="10" y="22" font-size="16" fill="#222">'+title+' - grid 50 cm</text>');
 for(let g=Math.floor(x0/50)*50;g<=x1;g+=50)o.push('<line x1="'+X(g)+'" y1="40" x2="'+X(g)+'" y2="'+H+'" stroke="#eee"/>');
 for(let g=Math.floor(y0/50)*50;g<=y1;g+=50)o.push('<line x1="0" y1="'+Y(g)+'" x2="'+W+'" y2="'+Y(g)+'" stroke="#eee"/>');
 if(opt.fused)o.push(line(res.fx,res.fy,'#2f9e44',2.4,''));if(opt.pred&&res.hasPred)o.push(line(res.px,res.py,'#8d6e63',2.4,'7 4'));
 res.run.forEach((r,i)=>{if(r.fs===2&&(i===0||res.run[i-1].fs!==2))o.push('<rect x="'+(X(res.fx[i])-4)+'" y="'+(Y(res.fy[i])-4)+'" width="8" height="8" fill="#e8590c"/>');});
 const e=res.fx.length-1;o.push('<circle cx="'+X(res.fx[e])+'" cy="'+Y(res.fy[e])+'" r="5" fill="none" stroke="#e03131" stroke-width="2"/>');
 o.push('<circle cx="'+X(0)+'" cy="'+Y(0)+'" r="6" fill="#2b8a3e"/><text x="'+(+X(0)+8)+'" y="'+(+Y(0)+16)+'" font-size="13" fill="#2b8a3e">start</text>');
 o.push('<text x="10" y="'+(H-12)+'" font-size="12">green = fused (gyro + wheel model) · brown dashed = predicted (wheel model only) · orange square = reached stop width · red ring = car now</text></svg>');return o.join('\n');}
/*END MAPCORE*/

/*MAPUI - Test21 tablet recording / live map (predicted + fused + mapped layers, heading chart, target-following runs) / downloads (no PC needed)*/
let mapLines=[],mapSamples=[],mapNext=null,mapEspTotal=0,mapBufferCapacity=0,mapLost=0,mapBusyPoll=false,mapLastOk=0,mapDirty=false,mapSavedAt=0,
 mapLastBuild=0,mapBuiltCount=-1,mapRes=null,mapRunsCache=[],mapRunPick='live',mapStoreWarn='',mapDownloadedCount=0;
let mapStatus221=null,mapEpoch=0,mapRunId=0,mapLastSample=null,mapLastLogIdx=-1,mapLastLogEsp=-1,mapLoc=[],mapObj=[],mapSlam={pose:[],obs:[],lm:[],cone:[]}; // Test21: N250-N253 EKF-SLAM // Test21: N240 localizer records, N241 objects
let mapRef=MAPREF0.map(s=>Object.assign({},s));
try{const r=JSON.parse(localStorage.getItem('ece4333-test21-reference')||'null');if(Array.isArray(r)&&r.length===5)r.forEach((v,i)=>{if(v>0)mapRef[i].len=+v;});}catch(e){}
function mapAddLine(idx,esp,text,pc){
 if(idx<mapLastLogIdx||esp<mapLastLogEsp){mapEpoch++;mapLastSample=null;mapRunId++;}
 mapLastLogIdx=idx;mapLastLogEsp=esp;
 mapLines.push(idx+'\t'+esp+'\t'+pc+'\t'+text);
 if(text.startsWith('{"N":221,')){try{mapStatus221=JSON.parse(text);}catch(e){}}
 if(text.startsWith('{"N":240,')){try{mapLoc.push(Object.assign(JSON.parse(text),{runId:mapRunId,epoch:mapEpoch}));}catch(e){}}
 if(text.startsWith('{"N":241,')){try{mapObj.push(Object.assign(JSON.parse(text),{runId:mapRunId,epoch:mapEpoch}));}catch(e){}}
 if(text.startsWith('{"N":25')){try{const o=JSON.parse(text),k={250:'pose',251:'obs',252:'lm',253:'cone'}[o.N];if(k)mapSlam[k].push(Object.assign(o,{runId:mapRunId,epoch:mapEpoch}));}catch(e){}}
 if(text.startsWith('{"N":230,')){try{const o=JSON.parse(text);if(Array.isArray(o.d)&&o.d.length>=15){const s=mapDecode(o.d);if(!mapLastSample||s.mode!==mapLastSample.mode||s.q<mapLastSample.q||s.t<mapLastSample.t||s.t-mapLastSample.t>3000)mapRunId++;
 s.runId=mapRunId;s.epoch=mapEpoch;mapWheelStep(s,mapLastSample&&mapLastSample.runId===s.runId?mapLastSample:null);s.idx=idx;s.esp=esp;s.pc=pc;mapSamples.push(s);mapLastSample=s;}}catch(e){}}
}
function mapSave(force){
 if(!mapDirty&&!force)return;if(!force&&Date.now()-mapSavedAt<4000)return;
 try{localStorage.setItem('ece4333-test21-map',JSON.stringify({v:1,next:mapNext,lost:mapLost,lines:mapLines}));mapStoreWarn='';mapDirty=false;mapSavedAt=Date.now();}
 catch(e){mapStoreWarn='Tablet storage full: download the data now (it is still kept in this page until you close it).';}
}
function mapRestore(){
 try{const o=JSON.parse(localStorage.getItem('ece4333-test21-map')||'null');if(!o||!Array.isArray(o.lines))return;
  mapNext=o.next;mapLost=o.lost||0;
  for(const l of o.lines){const p=l.split('\t');if(p.length>=4)mapAddLine(+p[0],+p[1],p.slice(3).join('\t'),+p[2]);}
 }catch(e){}
}
async function mapPoll(){
 if(mapBusyPoll)return;mapBusyPoll=true;
 try{
  for(let round=0;round<8;round++){
   const txt=await request('/map-log?from='+(mapNext===null?0:mapNext)+'&max=200');
   const lines=txt.split('\n'),h={};(lines[0]||'').replace(/^#/,'').split(' ').forEach(p=>{const i=p.indexOf('=');if(i>0)h[p.slice(0,i)]=p.slice(i+1);});
   const total=+h.lines||0,oldest=+h.oldest||0;mapEspTotal=total;mapBufferCapacity=+h.capacity||0;mapLastOk=Date.now();
   if(h.error){$('mapStatus').textContent='ESP32: '+h.error;break;}
   if(mapNext===null)mapNext=oldest;              // first visit: take everything the ESP32 still holds
   if(total<mapNext){mapNext=0;continue;}          // ESP32 restarted: its counter began again at 0
   const pc=Date.now();let got=0;
   for(const l of lines.slice(1)){if(!l)continue;const a=l.indexOf('\t'),b=l.indexOf('\t',a+1);if(a<0||b<0)continue;
    const idx=+l.slice(0,a);if(idx<mapNext)continue;if(idx>mapNext)mapLost+=idx-mapNext;
    mapAddLine(idx,+l.slice(a+1,b),l.slice(b+1),pc);mapNext=idx+1;got++;}
   if(got)mapDirty=true;
   if(mapNext>=total||!got)break;
  }
 }catch(e){}
 finally{mapBusyPoll=false;}
 mapSave(false);mapRefresh(false);
}
function mapGyroReadout(){
 const s=mapSamples[mapSamples.length-1],u=mapStatus221;if(!s)return;
 const y=s.yaw,h=((y%360)+360)%360;
 $('gyroYaw').textContent=y.toFixed(1)+' °';
 $('gyroDetail').textContent='heading '+h.toFixed(0)+'° (0-360) · rate '+s.g.toFixed(1)+' °/s'+(u&&u.CB?' · CALIBRATING, keep still':'')+' · updated '+Math.max(0,(Date.now()-s.pc)/1000).toFixed(1)+' s ago';
 $('gyroNeedle').setAttribute('transform','rotate('+(-y).toFixed(1)+')');
}
async function mapZeroGyro(){
 const b=$('gyroZeroBtn'),zr0=mapStatus221?mapStatus221.ZR:null;b.disabled=true;
 $('gyroMsg').textContent='Keep the car completely still for ~2 s… ';
 try{await post('/cmd',{action:'gyro_navigation',value:2});}
 catch(e){$('gyroMsg').textContent='Not accepted: '+(e.message||e)+' — press STOP CAR first, then zero. ';b.disabled=false;return;}
 const t0=Date.now();
 while(Date.now()-t0<6000){await delay(250);const u=mapStatus221;
  if(u&&zr0!==null&&u.ZR!==zr0&&!u.CB){$('gyroMsg').textContent='Zeroed ✓ (bias re-measured). Now turn the car LEFT by hand: yaw must increase. ';b.disabled=false;mapGyroReadout();return;}}
 $('gyroMsg').textContent='No confirmation from the car within 6 s — stop the car and try again. ';b.disabled=false;
}
function mapRunLabel(r,i){const d=((r[r.length-1].t-r[0].t)/1000).toFixed(0);return (r.kind===7?'Follow run ':'Run ')+(i+1)+' - '+r.length+' samples, '+d+' s';}
const MAPLY=['mapLyD','mapLyP','mapLyF','mapLyM'];
function mapLayers(){return{drawing:$('mapLyD').checked,pred:$('mapLyP').checked,fused:$('mapLyF').checked,mapped:$('mapLyM').checked};}
function mapLayerChanged(){try{localStorage.setItem('ece4333-test21-layers',JSON.stringify(MAPLY.map(id=>$(id).checked)));}catch(e){}mapRefresh(true);}
function mapLayerRestore(){try{const v=JSON.parse(localStorage.getItem('ece4333-test21-layers')||'null');if(Array.isArray(v)&&v.length===4)MAPLY.forEach((id,i)=>{$(id).checked=!!v[i];});}catch(e){}}
function mapStyle(svg){return svg.replace('<svg ','<svg style="width:100%;height:auto;border-radius:8px;margin:6px 0" ');}
function mapCurSVG(){const L=mapLayers();return mapRes.kind===7?mapFollowSVG(mapRes,'Follow run '+mapRes.no,L):mapSVG(mapRes,mapRef,'Run '+mapRes.no,L);}
function mapCurHeading(){return mapRes.kind===7?mapFollowCharts(mapRes)[0]:mapHeadingSVG(mapRes,mapRef);}
function mapRefresh(force){
 mapGyroReadout();
 const age=mapLastOk?((Date.now()-mapLastOk)/1000).toFixed(0):'-',live=mapSamples[mapSamples.length-1];
 const st=['IDLE','FOLLOW','CROSS','TURN','SEARCH','FAIL','RED STOP'];
 $('mapStatus').textContent='Recorded '+mapLines.length+' lines / '+mapSamples.length+' samples on this tablet'+
  (mapLost?' | LOST '+mapLost+' log lines':' | nothing lost')+' | ESP buffer '+mapBufferCapacity+' lines'+' | last contact '+age+' s ago'+
  (live?' | car: '+(live.mode===6?'AUTO '+(st[live.fs]||live.fs):live.mode===7?'FOLLOW '+(['','TRACK','ARRIVED','TARGET LOST','SEARCHING','','RESERVED'][live.fs]||live.fs)+', target width '+live.tgtW+' per mille':live.mode===5?'MANUAL':'stopped')+', yaw '+live.yaw.toFixed(0)+' deg'+(live.mode===7?'':', L/C/R '+live.L+'/'+live.C+'/'+live.R):'')+(mapStatus221&&mapStatus221.LP?' | laps done '+mapStatus221.LP:'')+
  (live&&live.mode===6&&mapStatus221&&mapStatus221.SG!=null?' | route: on '+ROUTESEG[mapStatus221.SG]+', next '+['corner D RIGHT','corner E RIGHT','O: STOP + cross','corner A LEFT','corner B LEFT','corner C LEFT','O: STOP + cross','S: lap line'][mapStatus221.SG]+', lap '+((mapStatus221.LP||0)+1):'')+
  (mapLines.length>mapDownloadedCount?' | NOT downloaded yet':' | downloaded');
 if(mapStoreWarn)$('mapStatus').textContent+=' | '+mapStoreWarn;
 const u=mapStatus221;let g='Gyro: waiting for UNO status';
 if(u){const chip=u.IM===1?'MPU6050 / GY-521 (WHO_AM_I '+u.IW+')':u.IM===2?'QMI8658':'NO CHIP FOUND';
  g=u.GY?'Gyro OK ('+chip+')'+(u.CB?' — calibrating, keep still':'')+(u.GF?' — GYRO DIRECTION WAS REVERSED and was auto-corrected (until power-off). In settings set Gyro Z sign GS = '+(u.GL>0?1:0)+' and Save.  GS  '+(u.GL>0?1:0):''):
   'GYRO NOT WORKING ('+chip+') — Start is blocked. '+
   (u.IW===255?'Nothing answers on I2C: check the GY-521 board is pushed fully into its header, then switch power OFF and ON with the car still.':'Switch power OFF and ON with the car still; see README.');}
 $('mapGyro').textContent=g;$('mapGyro').style.color=u&&(!u.GY||u.GF)?'#ff6b6b':'';
 if(!force&&(mapSamples.length===mapBuiltCount||Date.now()-mapLastBuild<1500))return;
 mapLastBuild=Date.now();mapBuiltCount=mapSamples.length;
 mapRunsCache=mapRuns(mapSamples);
 const sel=$('mapRun'),keep=mapRunPick;sel.innerHTML='';
 const o0=document.createElement('option');o0.value='live';o0.textContent='Latest run (live)';sel.appendChild(o0);
 mapRunsCache.forEach((r,i)=>{const o=document.createElement('option');o.value=String(i);o.textContent=mapRunLabel(r,i);sel.appendChild(o);});
 sel.value=keep==='live'||+keep<mapRunsCache.length?keep:'live';
 if(!mapRunsCache.length){$('mapView').innerHTML='<p class="hint">No driving data yet. Press Start continuous run; the map appears here while the car drives.</p>';$('mapSummary').textContent='';$('mapSegTable').innerHTML='';$('mapHeading').innerHTML='';$('mapStates').innerHTML='';$('mapTrans').innerHTML='';mapRes=null;return;}
 const idx=sel.value==='live'?mapRunsCache.length-1:+sel.value,run=mapRunsCache[idx],lay=mapLayers();
 if(run.kind===7){
  mapRes=mapBuildFollow(run);mapRes.kind=7;mapRes.no=idx+1;
  $('mapView').innerHTML=mapStyle(mapFollowSVG(mapRes,'Follow run '+(idx+1),lay));
  $('mapHeading').innerHTML=mapFollowCharts(mapRes).map(mapStyle).join('');
  $('mapSummary').textContent=mapFollowSummary(mapRes,idx+1,mapRunsCache.length);
  $('mapSegTable').innerHTML='';$('mapStates').innerHTML='';$('mapTrans').innerHTML='';slamRender(mapRes);return; // Test21: the EKF-SLAM map of cone runs
 }
 mapRes=mapBuildRun(run,mapRef);mapRes.kind=6;mapRes.no=idx+1;
 $('mapView').innerHTML=mapStyle(mapSVG(mapRes,mapRef,'Run '+(idx+1),lay));
 $('mapHeading').innerHTML=mapStyle(mapHeadingSVG(mapRes,mapRef));
 $('mapSummary').textContent=mapSummary(mapRes,mapRef,idx+1,mapRunsCache.length);
 $('mapStates').innerHTML=mapStyle(mapStateSVG(mapRes));
 $('mapTrans').innerHTML=mapRes.trans&&mapRes.trans.length?'<details><summary><b>State transitions  ('+mapRes.trans.length+')</b></summary><table style="width:100%;font-size:13px"><tr><th>t (s)</th><th>from</th><th>to</th><th>x, y (cm)</th><th>in state (s)</th></tr>'+mapRes.trans.map(t=>'<tr><td>'+t.t.toFixed(1)+'</td><td>'+(t.from===null?'':SMNAME[t.from])+'</td><td><b>'+SMNAME[t.to]+'</b></td><td>'+t.x.toFixed(0)+', '+t.y.toFixed(0)+'</td><td>'+t.dwell.toFixed(1)+'</td></tr>').join('')+'</table></details>':'';
 const rows=mapSegRows(mapRes,mapRef);
 $('mapSegTable').innerHTML=rows.length?'<tr><th>#</th><th>Segment</th><th>Complete</th><th>Turn expected/measured</th><th>Heading ref/meas</th><th>Length ref/meas (cm)</th></tr>'+
  rows.map(r=>'<tr><td>'+r.order+'</td><td>'+r.segment+'</td><td>'+(r.complete?'yes':'—')+'</td><td'+(r.turn_check==='WRONG WAY'?' style="color:#fa5252;font-weight:bold"':'')+'>'+(r.expected_turn_deg===''?'—':r.expected_turn_deg+' / '+r.measured_turn_deg+(r.turn_check==='WRONG WAY'?' WRONG WAY':''))+'</td><td>'+r.ref_heading_deg+' / '+r.meas_heading_deg+'</td><td>'+r.ref_length_cm+' / '+r.meas_length_cm+'</td></tr>').join(''):'';
}
function mapPickRun(v){mapRunPick=v;mapRefresh(true);}
function mapStamp(){const d=new Date(),p=n=>String(n).padStart(2,'0');return d.getFullYear()+p(d.getMonth()+1)+p(d.getDate())+'_'+p(d.getHours())+p(d.getMinutes())+p(d.getSeconds());}
function mapSaveFile(name,text,type){
 const blob=new Blob([text],{type:type||'text/plain'}),url=URL.createObjectURL(blob),a=document.createElement('a');
 a.href=url;a.download=name;document.body.appendChild(a);a.click();a.remove();setTimeout(()=>URL.revokeObjectURL(url),60000);
 $('message').textContent='Saved '+name+' ('+Math.round(text.length/1024)+' kB). If nothing appeared, check the browser Downloads list.';
}
function mapDownload(kind){
 const st=mapStamp();
 if(kind==='csv'||kind==='raw'){
  if(!mapLines.length){$('message').textContent='Nothing recorded yet.';return;}
  if(kind==='csv'){
   const cols=['tablet_ms','esp_idx','esp_ms','q','t_ms','L','C','R','PL','PR','gyro_rate_dps','yaw_deg','dist_cm','x_cm','y_cm','ultrasonic_cm','mode','follow_state','gyro_ok','L_seen','C_seen','R_seen','all_black','ev_cross','ev_turn','ev_search','ev_estop','pred_heading_deg','pred_x_cm','pred_y_cm','tgt_heading_err_deg','tgt_width_permille','tgt_bearing_deg','state_code','state','route_seg'];
   const csv=cols.join(',')+'\n'+mapSamples.map(s=>[s.pc,s.idx,s.esp,s.q,s.t,s.L,s.C,s.R,s.PL,s.PR,s.g,s.yaw,s.dist,s.x,s.y,s.u,s.mode,s.fs,s.gy].concat([0,1,2,3,4,5,6,7].map(b=>(s.k>>b)&1)).concat([s.kh,s.kx,s.ky,s.tgtErr,s.tgtW,s.tgtBear,s.sm,s.sm!==undefined?SMNAME[s.sm]:'',s.seg!==undefined?ROUTESEG[s.seg]:'']).map(v=>v===undefined||v===null?'':v).join(',')).join('\n')+'\n';
   mapSaveFile('map_samples_'+st+'.csv',csv,'text/csv');
  }else mapSaveFile('map_all_lines_'+st+'.txt','esp_idx\tesp_ms\ttablet_ms\tuno_line\n'+mapLines.join('\n')+'\n');
  mapDownloadedCount=mapLines.length;mapRefresh(false);return;
 }
 if(!mapRes){$('message').textContent='No driving run to export yet.';return;}
 const tag='run'+mapRes.no+'_'+st;
 if(kind==='laps'){if(!(mapRes.lapRows||[]).length){$('message').textContent='No lap data in this run yet.';return;}mapSaveFile('laps_'+tag+'.csv',mapCSV(mapRes.lapRows),'text/csv');}
 if(kind==='loc'){const rows=(mapRes.loc||[]).map(r=>({q:r.q,lap_pos_cm:r.P/10,segment:ROUTESEG[r.G]??'',odometer_scale:r.K/1000,sigma_cm:r.S/10,landmark:r.E>=0?['D','E','O','A','B','C','O'][r.E]:'',innovation_cm:r.E>=0?r.I/10:'',gyro_drift_deg:r.H/10,disagreements:r.M}))
  .concat((mapRes.ekfObjects||[]).map(o=>({q:mapRes.run[o.i].q,object:o.kind,lap_pos_cm:+o.P.toFixed(1),where:mapSegOf(o.P),range_cm:o.R,x_cm:+o.x.toFixed(1),y_cm:+o.y.toFixed(1)})));
  if(!rows.length){$('message').textContent='No localizer data in this run (needs ESP32 Test21).';return;}mapSaveFile('localizer_'+tag+'.csv',mapCSV(rows),'text/csv');}
 if(kind==='svg')mapSaveFile('map_'+tag+'.svg',mapCurSVG(),'image/svg+xml');
 if(kind==='trans'){if(!mapRes.trans||!mapRes.trans.length){$('message').textContent='No state transitions in this run (needs the Test21 UNO).';return;}mapSaveFile('state_transitions_'+tag+'.csv',mapCSV(mapTransRows(mapRes)),'text/csv');}
 if(kind==='spng'){if(mapRes.kind===7||!mapRes.hasSm){$('message').textContent='No state log in this run.';return;}mapSavePNG(mapStateSVG(mapRes),'state_timeline_'+tag+'.png');}
 if(kind==='hsvg')mapSaveFile('heading_'+tag+'.svg',mapCurHeading(),'image/svg+xml');
 if(kind==='seg'){if(mapRes.kind===7){$('message').textContent='Segments table is for line-following runs only.';return;}mapSaveFile('map_segments_'+tag+'.csv',mapCSV(mapSegRows(mapRes,mapRef)),'text/csv');}
 if(kind==='points')mapSaveFile('map_points_'+tag+'.csv',mapCSV(mapRes.kind===7?mapFollowRows(mapRes):mapPointRows(mapRes,mapRef)),'text/csv');
 if(kind==='summary')mapSaveFile('map_summary_'+tag+'.txt',$('mapSummary').textContent+'\n');
 if(kind==='png')mapSavePNG(mapCurSVG(),'map_'+tag+'.png');
 if(kind==='hpng')mapSavePNG(mapCurHeading(),'heading_'+tag+'.png');
}
function mapSavePNG(svg,name){
 const img=new Image(),url=URL.createObjectURL(new Blob([svg],{type:'image/svg+xml'}));
 img.onload=()=>{try{const c=document.createElement('canvas');c.width=img.width*2;c.height=img.height*2;const g=c.getContext('2d');g.scale(2,2);g.drawImage(img,0,0);URL.revokeObjectURL(url);
  c.toBlob(b=>{if(!b){$('message').textContent='PNG export not supported here: use the SVG button.';return;}const u=URL.createObjectURL(b),a=document.createElement('a');a.href=u;a.download=name;document.body.appendChild(a);a.click();a.remove();setTimeout(()=>URL.revokeObjectURL(u),60000);$('message').textContent='Saved '+name;},'image/png');}
  catch(e){$('message').textContent='PNG export failed on this browser: use the SVG button.';}};
 img.onerror=()=>{$('message').textContent='PNG export failed on this browser: use the SVG button.';};img.src=url;
}
function mapClear(){
 if(mapLines.length>mapDownloadedCount&&!confirm('Recording has NOT been downloaded. Delete it from this tablet anyway?'))return;
 if(!confirm('Delete all recorded data from this tablet? (The car itself is not affected.)'))return;
 mapLoc=[];mapObj=[];mapSlam={pose:[],obs:[],lm:[],cone:[]};mapLastSample=null;mapRunId++;mapLines=[];mapSamples=[];mapLost=0;mapNext=mapEspTotal;mapDownloadedCount=0;mapRes=null;
 try{localStorage.removeItem('ece4333-test21-map');}catch(e){}
 mapBuiltCount=-1;mapRefresh(true);
}
function mapLoadRefInputs(){mapRef.forEach((s,i)=>{$('mapLen'+i).value=s.len;});}
function mapSaveRef(){
 const v=mapRef.map((s,i)=>+$('mapLen'+i).value);
 if(v.some(x=>!(x>0&&x<2000))){$('message').textContent='Enter lengths in cm (1-2000).';return;}
 v.forEach((x,i)=>mapRef[i].len=x);try{localStorage.setItem('ece4333-test21-reference',JSON.stringify(v));}catch(e){}
 $('message').textContent='Track lengths saved on this tablet.';mapRefresh(true);
}
function mapResetRef(){mapRef=MAPREF0.map(s=>Object.assign({},s));try{localStorage.removeItem('ece4333-test21-reference');}catch(e){}mapLoadRefInputs();mapRefresh(true);}
window.addEventListener('beforeunload',e=>{mapSave(true);if(mapLines.length>mapDownloadedCount){e.preventDefault();e.returnValue='';}});
document.addEventListener('visibilitychange',()=>{if(document.hidden)mapSave(true);else mapPoll();});
mapRestore();mapLoadRefInputs();mapLayerRestore();mapRefresh(true);setInterval(mapPoll,500);
/*END MAPUI*/


// ---- Assignment 4 race options (Test21) ----
const A4IDS=['xSO','xSP','xSL','xOT','xOL','xOD','xOA','xOS','xOJ','xOR'];
function a4Options(){const v=i=>+$(i).value;return{so:v('xSO')||83,slow:v('xSP')||50,line:v('xSL'),trig:v('xOT')||15,right:v('xOD'),ang:v('xOA')||90,out:v('xOS')||15,pass:v('xOJ')||10,ret:v('xOR')||15};}
// Clearance check (same formula as tests/overtake_test.cpp): the axle must get past the far end of the obstacle (detection
// distance + 11 cm sensor-to-axle + obstacle length + 8 cm car rear + 4 cm) and far enough sideways (length / 2 + 10 cm),
// assuming a square obstacle centred on the road.
function ovtCheck(){const e=$('ovtCheck');if(!e)return;const o=a4Options(),len=+$('xOL').value||10,a=o.ang*Math.PI/180,c=Math.cos(a);
 if(!$('xOV').checked){e.textContent='Avoidance is off: the car waits behind an obstacle.';e.style.color='';return;}
 const fwd=(o.out+o.ret)*c+o.pass,need=o.trig+11+len+12,side=o.out*Math.sin(a),sideNeed=len/2+10,msgs=[];
 if(fwd<need)msgs.push('the car comes back onto the road '+Math.round(need-fwd)+' cm before the far end of the obstacle: make the straight at least '+Math.ceil(need-(o.out+o.ret)*c)+' cm');
 if(side<sideNeed)msgs.push('the first leg takes the car only '+Math.round(side)+' cm sideways: about '+Math.ceil(sideNeed)+' cm is needed');
 e.textContent=msgs.length?'WARNING: with these values the car will probably hit a '+len+' cm obstacle — '+msgs.join('; ')+'.':'Clearance check OK for a '+len+' cm obstacle.';
 e.style.color=msgs.length?'#c92a2a':'';}
function ovtShow(u){const e=$('ovtNow');if(!e)return;if(!u||u.OT===undefined){e.textContent='Car is using: —';return;}
 e.textContent='Car is using: '+(u.OV?'avoidance ON':'avoidance OFF')+', detect '+u.OT+' cm, first turn '+(u.OD?'right':'left')+' '+u.OA+'°, legs '+u.OS+' / '+u.OL+' / '+u.OR+' cm'+(u.OV>1?' — now: '+['','turning out','first leg','turning back','straight past','turning to the road','return leg','joining the road'][u.OV-1]:'')+' (sent with the last Start).';}
function saveA4(){try{localStorage.setItem('a4race22',JSON.stringify(A4IDS.map(i=>$(i).value).concat([+$('xOV').checked,$('SR')?$('SR').value:'35'])));}catch(e){}}
function loadA4(){try{const v=JSON.parse(localStorage.getItem('a4race22')||'null');if(Array.isArray(v)&&v.length===A4IDS.length+2){A4IDS.forEach((i,k)=>{$(i).value=v[k];});$('xOV').checked=!!v[A4IDS.length];}}catch(e){}}
async function liveSpeed(step){const n=Math.max(40,Math.min(180,(+$('LSP').value||80)+step));$('LSP').value=n;
 try{await post('/cmd',{action:'live_speed',value:n});$('lspMsg').textContent='Line-following speed '+n+' PWM sent; "car now" confirms it.';}catch(e){$('lspMsg').textContent=e.message;}}
A4IDS.concat(['xOV']).forEach(i=>$(i).addEventListener('change',()=>{saveA4();ovtCheck();if(i==='xSO')mapRefresh(true);}));
// ---- race options and target following (Test11) ----
// Legacy browser wait-for-green settings are intentionally ignored.
const FOLLOWIDS=['FC','FN','FW','FF','FKP','FKD'];
function saveFollow(){try{localStorage.setItem('ece4333-test22-follow',JSON.stringify(FOLLOWIDS.map(i=>$(i).value).concat([+$('FM').checked])));}catch(e){}}
function loadFollow(){try{const v=JSON.parse(localStorage.getItem('ece4333-test22-follow')||'null');if(Array.isArray(v)&&v.length===FOLLOWIDS.length+1){FOLLOWIDS.forEach((i,k)=>{$(i).value=v[k];});$('FM').checked=!!v[FOLLOWIDS.length];}}catch(e){}}
FOLLOWIDS.concat(['FM']).forEach(i=>$(i).addEventListener('change',saveFollow));
function beforeWidth(){return Number(latest?.vision_health?.image_w)||0;}
function followStopWidth(value,unit,imageWidth){
 if(!Number.isInteger(value)||value<=0)throw Error('Enter a positive whole stop width.');
 if(unit==='px'&&!(imageWidth>0))throw Error('Wait for a fresh analysis frame before using pixels.');
 const w=unit==='px'?Math.round(1000*value/imageWidth):value;
 if(w<40||w>900)throw Error('Stop width must be 40-900 per mille of the analysis image.');
 return w;
}
function applyProfile(u,stale){
 const race=!stale&&u.A5===0,mapping=!stale&&u.A5===1;
 for(const e of document.querySelectorAll('[data-profile]'))e.hidden=e.dataset.profile==='race'?!race:!mapping;
 ovtShow(stale?null:u);
 $('startLine').disabled=stale||(!race&&!mapping);
 $('startLine').textContent='Start figure-8 run';
 if($('routeFeature'))$('routeFeature').textContent=stale?'Waiting for UNO':mapping?'Figure-8 run: line following, STOP sign (3 s), obstacle avoidance; target following below':
  'Segment '+(['S-D','D-E','E-O','O-A','A-B','B-C','C-O','O-S'][u.SG]||'?')+' | Active feature: '+(['LINE ONLY','LINE ONLY','STOP SIGN','LINE ONLY'][u.Z]||'LINE ONLY');
}
async function startFollow(){
 if(starting||settingsBusy){$('message').textContent='Wait for the pending command.';return;}
 starting=true;
 try{
  const p={color:$('FC').value,n:Number($('FN').value),w:followStopWidth(Number($('FW').value),'px',beforeWidth()),fov:Number($('FF').value),mirror:+$('FM').checked,kp:Number($('FKP').value),kd:Number($('FKD').value)};
  if(![p.n,p.w,p.fov,p.kp,p.kd].every(Number.isInteger)||p.n<0||p.n>9)throw Error('Use whole numbers for the target following settings (targets 0-9)');
  saveFollow();
  const wanted=readSettingsForm(),current=JSON.parse(await request('/traffic-settings'));
  if(!settingsMatch(current,wanted)&&!await saveSettings())return;
  const before=JSON.parse(await request('/traffic-status'));
  await post('/follow-start',p);let confirmed=false;
  for(let i=0;i<16;i++){await delay(300);const d=JSON.parse(await request('/traffic-status'));
   if(d.age_ms<1500&&d.uno?.M===7&&d.uno.RN!==before.uno?.RN){confirmed=true;break;}}
  if(!confirmed)throw Error('Follow confirmation missing; inspect car state before retrying.');
  driving=true;$('message').textContent='Follow start acknowledged. Target state and live motor PWM are shown below.';
 }catch(e){$('message').textContent=e.message;}
 finally{starting=false;}}
function followLive(d){
 const f=d.follow||{},color=(f.color==='a'?f.locked:f.color)||'',k={r:'red',g:'green',b:'blue'}[color];
 const t=(d.objects||{})[k]||{},u=d.uno||{},pm=t.valid&&t.seen&&t.image_w>0?Math.round(1000*t.width/t.image_w):null;
 $('fwSeen').textContent=!k?(f.color==='a'?'auto: none yet':'—'):k+': '+(!t.valid?'not sampled':t.seen?'Yes':'No');
 $('fwW').textContent=pm===null?'—':t.width+' px / '+pm+' per mille (image '+t.image_w+' px)';
 const lastK={r:'red',g:'green',b:'blue'}[f.last];
 $('fwSt').textContent=d.age_ms<1500&&u.M===7?(names[u.TS]||'—')+' | reached '+(f.reached||0)+(f.targets?' / '+f.targets:'')+(lastK&&f.color==='a'?' | next: not '+lastK:''):'—';
}

let savedTab='drive';try{savedTab=localStorage.getItem('ece4333-test21-tab')||'drive';}catch(e){}
loadFollow();loadA4();ovtCheck();switchTab(savedTab);refreshPreview();previewAge();loadSettings();loadCamera();poll();
/* ---- Test21 EKF-SLAM view (Assignment 5): the ESP32 logs N250 pose, N251 observation, N252 landmark, N253 cone reached ---- */
function slamRunData(res){const q0=res.run[0].q,q1=res.run[res.run.length-1].q,f=o=>o.runId===res.run[0].runId&&o.epoch===res.run[0].epoch&&o.q>=q0&&o.q<=q1;
 return {q0:q0,q1:q1,pose:mapSlam.pose.filter(f),obs:mapSlam.obs.filter(f),lm:mapSlam.lm.filter(f),cone:mapSlam.cone.filter(f)};}
function slamGroundTruth(){const out=[];for(const ln of ($('slamGT')?.value||'').split('\n')){const p=ln.trim().split(/[\s,]+/);if(p.length>=3&&!isNaN(+p[1])&&!isNaN(+p[2]))out.push({c:p[0][0].toLowerCase()==='g'?2:p[0][0].toLowerCase()==='b'?3:1,x:+p[1],y:+p[2],name:p[3]||''});}return out;}
function slamLatest(d,qs){const L={};for(const r of d.lm)if(r.q<=qs)L[r.l]=r;return Object.values(L);}
function slamEllipse(r,k){const a=r.a,b=r.b,d=r.d,t=(a+d)/2,s=Math.sqrt(Math.max(((a-d)/2)**2+b*b,0)),l1=t+s,l2=Math.max(t-s,0),ang=0.5*Math.atan2(2*b,a-d);
 return {rx:k*Math.sqrt(l1),ry:k*Math.sqrt(l2),deg:ang*180/Math.PI};}
function slamRender(res){const box=$('slamBox');if(!box||!res||!res.run.length)return;featureRender(res);
 const d=slamRunData(res);if(!d.pose.length){box.innerHTML='<p class="hint">No EKF-SLAM data in this run (no camera landmark yet: check that Red / green / blue detection is ON in the Camera tab; CWS/CDC in Target following calibrate cone distance).</p>';$('slamText').textContent='';return;}
 const fr=Number($('slamT').value)/1000,qs=d.q0+fr*(d.q1-d.q0),gt=slamGroundTruth(),lms=slamLatest(d,qs),pose=d.pose.filter(p=>p.q<=qs);
 const xs=pose.map(p=>p.x).concat(lms.map(l=>l.x),gt.map(g=>g.x)),ys=pose.map(p=>p.y).concat(lms.map(l=>l.y),gt.map(g=>g.y));
 const x0=Math.min(...xs)-40,x1=Math.max(...xs)+40,y0=Math.min(...ys)-40,y1=Math.max(...ys)+40,W=640,H=420,sc=Math.min((W-60)/(x1-x0),(H-60)/(y1-y0));
 const X=x=>(30+(x-x0)*sc).toFixed(1),Y=y=>(H-30-(y-y0)*sc).toFixed(1),col=c=>c===2?'#2f9e44':c===3?'#1c7ed6':'#e03131';
 let o=['<svg xmlns="http://www.w3.org/2000/svg" viewBox="0 0 '+W+' '+H.toFixed(0)+'" style="width:100%;background:#fff;border-radius:8px">'];
 for(let g=Math.ceil(x0/50)*50;g<x1;g+=50)o.push('<line x1="'+X(g)+'" y1="0" x2="'+X(g)+'" y2="'+H+'" stroke="#eee"/>');
 for(let g=Math.ceil(y0/50)*50;g<y1;g+=50)o.push('<line x1="0" y1="'+Y(g)+'" x2="'+W+'" y2="'+Y(g)+'" stroke="#eee"/>');
 const raw=res.run.filter(r=>r.q<=qs);
 if(raw.length>1)o.push('<polyline fill="none" stroke="#8d6e63" stroke-dasharray="5 4" points="'+raw.map(r=>X(r.kx)+','+Y(r.ky)).join(' ')+'"/>');
 if(pose.length>1)o.push('<polyline fill="none" stroke="#1c7ed6" stroke-width="2" points="'+pose.map(p=>X(p.x)+','+Y(p.y)).join(' ')+'"/>');
 const pAt=q=>{let b=d.pose[0];for(const p of d.pose){if(p.q<=q)b=p;else break;}return b;};
 for(const ob of d.obs.filter(z=>z.q<=qs&&z.q>qs-15&&z.l>=0)){const p=pAt(ob.q),l=lms.find(m=>m.l===ob.l);if(l)o.push('<line x1="'+X(p.x)+'" y1="'+Y(p.y)+'" x2="'+X(l.x)+'" y2="'+Y(l.y)+'" stroke="#66d9e8" stroke-width="1.2"/>');}
 for(const g of gt)o.push('<path d="M'+(+X(g.x)-6)+' '+(+Y(g.y)-6)+'l12 12m0-12l-12 12" stroke="'+col(g.c)+'" stroke-width="2" opacity="0.6"/>');
 for(const l of lms){const e=slamEllipse(l,2);o.push('<ellipse cx="'+X(l.x)+'" cy="'+Y(l.y)+'" rx="'+(e.rx*sc).toFixed(1)+'" ry="'+(e.ry*sc).toFixed(1)+'" transform="rotate('+(-e.deg).toFixed(1)+' '+X(l.x)+' '+Y(l.y)+')" fill="none" stroke="#364fc7"/><circle cx="'+X(l.x)+'" cy="'+Y(l.y)+'" r="4" fill="'+col(l.c)+'"/><text x="'+(+X(l.x)+7)+'" y="'+(+Y(l.y)-7)+'" font-size="11">L'+l.l+'</text>');}
 const p=pose[pose.length-1]||d.pose[0],h=p.h*Math.PI/180;
 o.push('<circle cx="'+X(p.x)+'" cy="'+Y(p.y)+'" r="6" fill="#1c7ed6"/><line x1="'+X(p.x)+'" y1="'+Y(p.y)+'" x2="'+X(p.x+25*Math.cos(h))+'" y2="'+Y(p.y+25*Math.sin(h))+'" stroke="#1c7ed6" stroke-width="3"/>');
 o.push('</svg>');
 box.innerHTML='<p class="hint">Grid 50 cm. Blue: EKF pose. Brown dashed: wheel prediction. Dots: landmarks and 2-sigma uncertainty. Crosses: measured ground truth. Cyan: recent observations.</p>'+o.join('');
 const L=[],fin=slamLatest(d,d.q1),last=d.pose[d.pose.length-1];
 L.push('EKF-SLAM at t = '+((pAt(qs).q-d.q0)/10).toFixed(1)+' s: pose ('+p.x.toFixed(0)+', '+p.y.toFixed(0)+') cm, heading '+p.h.toFixed(1)+'°, 1σ '+p.sx.toFixed(1)+' / '+p.sy.toFixed(1)+' cm, '+p.sh.toFixed(1)+'°; landmarks '+lms.length);
 L.push('run: '+d.obs.length+' observations ('+d.obs.filter(z=>z.l>=0&&!z.new).length+' updates, '+d.obs.filter(z=>z.new).length+' new landmarks, '+d.obs.filter(z=>z.l<0).length+' ignored); final pose 1σ '+last.sx.toFixed(1)+' / '+last.sy.toFixed(1)+' cm'+(d.cone.length?'; cones reached: '+d.cone.length:''));
 const errs=[],matches=landmarkMatches(fin,gt);for(const l of fin){const e=slamEllipse(l,1),best=matches.get(l.l);
  L.push('L'+l.l+' ('+(l.c===3?'blue':l.c===2?'green':'red')+') at ('+l.x.toFixed(1)+', '+l.y.toFixed(1)+') cm, 1σ '+e.rx.toFixed(1)+' × '+e.ry.toFixed(1)+' cm'+(best?', ground truth '+(best.g.name||'')+' ('+best.g.x+', '+best.g.y+') → error '+best.d.toFixed(1)+' cm':''));if(best)errs.push(best.d);}
 if(gt.length)L.push('Ground truth comparison: '+matches.size+' one-to-one pairs; '+(gt.length-matches.size)+' unmatched true landmarks; '+(fin.length-matches.size)+' unmatched estimates.');
 if(errs.length)L.push('map error vs ground truth: mean '+(errs.reduce((a,b)=>a+b,0)/errs.length).toFixed(1)+' cm, max '+Math.max(...errs).toFixed(1)+' cm');
 $('slamText').textContent=L.join('\n');
}
function slamCSV(){if(!mapRes)return;const d=slamRunData(mapRes),fin=slamLatest(d,d.q1),gt=slamGroundTruth(),matches=landmarkMatches(fin,gt);
 const rows=d.pose.map(p=>({type:'pose',q:p.q,t_s:((p.q-d.q0)/10).toFixed(1),x_cm:p.x,y_cm:p.y,heading_deg:p.h,sigma_x_cm:p.sx,sigma_y_cm:p.sy,sigma_h_deg:p.sh,landmarks:p.n}))
  .concat(d.obs.map(z=>({type:'observation',q:z.q,t_s:((z.q-d.q0)/10).toFixed(1),class:z.c===3?'blue':z.c===2?'green':'red',range_cm:z.r,bearing_deg:z.b,landmark:z.l,new:z.new})))
  .concat(fin.map(l=>{const best=matches.get(l.l);
   return {type:'landmark_final',landmark:l.l,class:l.c===3?'blue':l.c===2?'green':'red',x_cm:l.x,y_cm:l.y,var_xx:l.a,cov_xy:l.b,var_yy:l.d,truth_x_cm:best?best.g.x:'',truth_y_cm:best?best.g.y:'',error_cm:best?best.d.toFixed(1):''};}));
 mapSaveFile('slam_run'+mapRes.no+'.csv',mapCSV(rows),'text/csv');}

// Task 5 baseline: project measured range/bearing using the gyro/kinematic pose.
function featureData(res){
 const d=slamRunData(res),byQ=new Map(res.run.map(r=>[r.q,r])),points=[],landmarks=[];
 for(const ob of d.obs){const r=byQ.get(ob.q);if(!r||!(ob.r>0))continue;
  const a=(r.yaw+ob.b)*Math.PI/180,p={q:r.q,t:(r.t-res.run[0].t)/1000,c:ob.c,x:r.x+ob.r*Math.cos(a),y:r.y+ob.r*Math.sin(a)};
  points.push(p);let best=null,dist=30;
  for(const l of landmarks){if(l.c!==p.c)continue;const dd=Math.hypot(l.x-p.x,l.y-p.y);if(dd<dist){dist=dd;best=l;}}
  if(best){best.n++;best.x+=(p.x-best.x)/best.n;best.y+=(p.y-best.y)/best.n;}
  else landmarks.push({l:landmarks.length,c:p.c,x:p.x,y:p.y,n:1});
 }
 return {points,landmarks};
}
function landmarkMatches(landmarks,truth){
 const edges=[];landmarks.forEach(l=>truth.forEach((g,i)=>{if(l.c===g.c)edges.push({l:l.l,i,g,d:Math.hypot(g.x-l.x,g.y-l.y)});}));
 edges.sort((a,b)=>a.d-b.d);const used=new Set(),out=new Map();
 for(const e of edges)if(!out.has(e.l)&&!used.has(e.i)){out.set(e.l,e);used.add(e.i);}
 return out; // one-to-one nearest-distance pairing; unmatched counts are reported
}
function featureRender(res){
 const d=featureData(res),all=res.run.concat(d.landmarks),x0=Math.min(0,...all.map(p=>p.x))-30,x1=Math.max(0,...all.map(p=>p.x))+30,
  y0=Math.min(0,...all.map(p=>p.y))-30,y1=Math.max(0,...all.map(p=>p.y))+30,W=640,H=420,sc=Math.min((W-40)/(x1-x0),(H-40)/(y1-y0)),
  X=x=>20+(x-x0)*sc,Y=y=>H-20-(y-y0)*sc,col=c=>c===2?'#2f9e44':c===3?'#1c7ed6':'#e03131';
 const o=['<svg xmlns="http://www.w3.org/2000/svg" viewBox="0 0 '+W+' '+H+'" style="width:100%;background:#fff;border-radius:8px">',
  '<text x="12" y="18" fill="#222" font-size="12">Feature map (cm, start frame)</text>',
  '<polyline fill="none" stroke="#2f9e44" stroke-width="2" points="'+res.run.map(r=>X(r.x)+','+Y(r.y)).join(' ')+'"/>'];
 for(const p of d.points)o.push('<circle cx="'+X(p.x)+'" cy="'+Y(p.y)+'" r="1.5" opacity="0.3" fill="'+col(p.c)+'"/>');
 for(const l of d.landmarks)o.push('<circle cx="'+X(l.x)+'" cy="'+Y(l.y)+'" r="5" fill="'+col(l.c)+'"/><text x="'+(X(l.x)+7)+'" y="'+Y(l.y)+'" font-size="12" fill="#111">F'+l.l+'</text>');
 o.push('<circle cx="'+X(0)+'" cy="'+Y(0)+'" r="5" fill="#111"/></svg>');$('featureBox').innerHTML=o.join('');
 $('featureText').textContent=d.points.length+' valid range/bearing observations; '+d.landmarks.length+' feature clusters (30 cm association gate). Green path = gyro + wheel pose. These landmarks use the sensor observations, without fitting the surveyed road drawing.';
}
function featureExport(kind){if(!mapRes)return;const d=featureData(mapRes);
 if(kind==='svg')return mapSaveFile('feature_map_run'+mapRes.no+'.svg',$('featureBox').querySelector('svg').outerHTML,'image/svg+xml');
 mapSaveFile('feature_map_run'+mapRes.no+'.csv',mapCSV(d.points.map(p=>({type:'observation',...p})).concat(d.landmarks.map(l=>({type:'landmark',...l})))),'text/csv');
}
function slamExportSVG(){const svg=$('slamBox').querySelector('svg');if(svg)mapSaveFile('ekf_map_run'+mapRes.no+'.svg',svg.outerHTML,'image/svg+xml');}
let poseEvaluationRows=[];
function poseEvaluate(){
 if(!mapRes)return;poseEvaluationRows=[];const run=mapRes.run,d=slamRunData(mapRes),wrap=a=>((a+180)%360+360)%360-180;
 const truth=$('poseGT').value.split(/\n/).map(l=>l.trim().split(/[\s,]+/).map(Number)).filter(a=>a.length===4&&a.every(Number.isFinite));
 for(const a of truth){let r=null,dt=1e9;for(const z of run){const e=Math.abs((z.t-run[0].t)/1000-a[0]);if(e<dt){dt=e;r=z;}}if(dt>0.2)continue;
  const candidates=[{name:'wheel',x:r.kx,y:r.ky,h:r.kh},{name:'gyro_wheel',x:r.x,y:r.y,h:r.yaw}];
  let ekf=null,qgap=1e9;for(const z of d.pose){const e=Math.abs(z.q-r.q);if(e<qgap){qgap=e;ekf=z;}}
  if(ekf&&qgap<=2)candidates.push({name:'ekf',x:ekf.x,y:ekf.y,h:ekf.h});
  for(const e of candidates)poseEvaluationRows.push({model:e.name,time_s:a[0],matched_dt_s:dt,true_x_cm:a[1],true_y_cm:a[2],true_heading_deg:a[3],estimated_x_cm:e.x,estimated_y_cm:e.y,estimated_heading_deg:e.h,position_error_cm:Math.hypot(e.x-a[1],e.y-a[2]),heading_error_deg:wrap(e.h-a[3])});
 }
 if(!poseEvaluationRows.length){$('poseEvaluation').textContent='No matched measured ground truth. No measured error can be reported.';return;}
 const L=[];for(const name of ['wheel','gyro_wheel','ekf']){const a=poseEvaluationRows.filter(r=>r.model===name);if(!a.length)continue;
  L.push(name+': '+a.length+' matched samples; position RMSE '+Math.sqrt(a.reduce((v,r)=>v+r.position_error_cm**2,0)/a.length).toFixed(2)+' cm; heading RMSE '+Math.sqrt(a.reduce((v,r)=>v+r.heading_error_deg**2,0)/a.length).toFixed(2)+' deg');}
 const seconds=(run[run.length-1].t-run[0].t)/1000,distance=run[run.length-1].dist-run[0].dist;
 L.push('Recorded duration '+seconds.toFixed(1)+' s; modelled distance '+distance.toFixed(1)+' cm; modelled average speed '+(distance/Math.max(seconds,0.001)).toFixed(1)+' cm/s. Speed is derived from PWM, not an encoder measurement.');
 $('poseEvaluation').textContent=L.join('\n');
}
function poseExport(){poseEvaluate();if(poseEvaluationRows.length)mapSaveFile('robot_evaluation_run'+mapRes.no+'.csv',mapCSV(poseEvaluationRows),'text/csv');}
function noiseExport(){if(window.noiseRows?.length)mapSaveFile('noise_run'+mapRes.no+'.csv',mapCSV(window.noiseRows),'text/csv');else $('noiseOut').textContent='Run Gaussian noise analysis first.';}

// Gaussian noise analysis (A5 task 3): re-integrate the recorded run with white Gaussian noise added to the gyro rate
// and to the wheel speed, many times, and compare with the noise-free gyro + wheel estimate of the same run.
function slamNoise(){if(!mapRes)return;const r=mapRes.run,N=r.length;if(N<20){$('noiseOut').textContent='Run too short.';return;}
 const gauss=()=>{let u=0,v=0;while(!u)u=Math.random();v=Math.random();return Math.sqrt(-2*Math.log(u))*Math.cos(2*Math.PI*v);};
 const integ=(sg,sv)=>{let x=0,y=0,h=r[0].yaw*Math.PI/180,sum=0;const fx=[0],fy=[0];let ex=0,ey=0,eh=h;
  for(let i=1;i<N;i++){const dt=Math.max((r[i].t-r[i-1].t)/1000,0),dd=r[i].dist-r[i-1].dist,dh=(r[i].yaw-r[i-1].yaw)*Math.PI/180;
   ex+=dd*Math.cos(eh+dh/2);ey+=dd*Math.sin(eh+dh/2);eh+=dh;                                    // noise-free reference
   const dh2=dh+sg*dt*gauss()*Math.PI/180,dd2=dd*(1+sv/100*gauss()); // Gaussian noise on the gyro rate (sigma deg/s) and the wheel speed (sigma %)
   x+=dd2*Math.cos(h+dh2/2);y+=dd2*Math.sin(h+dh2/2);h+=dh2;sum+=Math.hypot(x-ex,y-ey);}
  return {fin:Math.hypot(x-ex,y-ey),mean:sum/(N-1),head:Math.abs((h-eh)*180/Math.PI)};};
 const M=40,rows=[],L=['Gaussian noise analysis of run '+mapRes.no+' ('+((r[N-1].t-r[0].t)/1000).toFixed(1)+' s, '+M+' trials each; error = distance from the noise-free gyro + wheel estimate)','gyro σ (°/s) | speed σ (%) | mean pos. error (cm) | final pos. error RMS (cm) | final heading error RMS (°)'];
 for(const sg of [0,0.5,1,2])for(const sv of [0,5,10]){if(!sg&&!sv)continue;let a=0,b=0,c=0;for(let k=0;k<M;k++){const e=integ(sg,sv);a+=e.mean;b+=e.fin*e.fin;c+=e.head*e.head;}
  const row={gyro_sigma_dps:sg,speed_sigma_pct:sv,mean_pos_err_cm:+(a/M).toFixed(1),final_pos_err_rms_cm:+Math.sqrt(b/M).toFixed(1),final_heading_err_rms_deg:+Math.sqrt(c/M).toFixed(2)};rows.push(row);
  L.push(String(sg).padStart(6)+' | '+String(sv).padStart(5)+' | '+String(row.mean_pos_err_cm).padStart(8)+' | '+String(row.final_pos_err_rms_cm).padStart(8)+' | '+String(row.final_heading_err_rms_deg).padStart(7));}
 L.push('Heading noise grows the position error with distance (each heading error turns every later step); speed noise alone mostly averages out.');
 $('noiseOut').textContent=L.join('\n');window.noiseRows=rows;}

try{const g=localStorage.getItem('a5gt');if(g&&$('slamGT'))$('slamGT').value=g;}catch(e){}
</script></body></html>
)HTML";
