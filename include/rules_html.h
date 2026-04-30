// rules_html — Rules Editor WebView
// Included by web_assets.cpp
// Served at GET /rules-editor

#ifndef RULES_HTML_H
#define RULES_HTML_H

static const char rules_html[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="es">
<head>
<meta charset="UTF-8">
<meta name="viewport" content="width=device-width,initial-scale=1">
<title>Reglas de Automatización · Monitoreo</title>
<style>
  :root {
    --bg: #0f1117;
    --surface: #1a1d27;
    --surface2: #222536;
    --accent: #6c63ff;
    --accent2: #38d9a9;
    --danger: #ff6b6b;
    --text: #e2e8f0;
    --muted: #8892a4;
    --border: #2d3249;
    --radius: 12px;
    --font: 'Inter', system-ui, sans-serif;
  }
  * { box-sizing: border-box; margin: 0; padding: 0; }
  body {
    font-family: var(--font);
    background: var(--bg);
    color: var(--text);
    min-height: 100vh;
    padding: 24px 16px;
  }
  h1 { font-size: 1.6rem; font-weight: 700; margin-bottom: 6px; }
  .subtitle { color: var(--muted); font-size: .9rem; margin-bottom: 24px; }
  .card {
    background: var(--surface);
    border: 1px solid var(--border);
    border-radius: var(--radius);
    padding: 20px;
    margin-bottom: 20px;
  }
  .card h2 { font-size: 1rem; font-weight: 600; margin-bottom: 16px; color: var(--accent2); }
  label { display: block; font-size: .8rem; color: var(--muted); margin-bottom: 4px; margin-top: 12px; }
  select, input[type=number], input[type=text] {
    width: 100%; padding: 9px 12px;
    background: var(--surface2); border: 1px solid var(--border);
    border-radius: 8px; color: var(--text); font-size: .9rem;
    outline: none; transition: border .2s;
  }
  select:focus, input:focus { border-color: var(--accent); }
  .row { display: flex; gap: 12px; flex-wrap: wrap; }
  .row > * { flex: 1; min-width: 120px; }
  .btn {
    padding: 10px 20px; border: none; border-radius: 8px;
    font-size: .9rem; font-weight: 600; cursor: pointer; transition: opacity .2s;
  }
  .btn:hover { opacity: .85; }
  .btn-primary { background: var(--accent); color: #fff; }
  .btn-success { background: var(--accent2); color: #0f1117; }
  .btn-danger  { background: var(--danger); color: #fff; }
  .btn-sm { padding: 6px 14px; font-size: .8rem; }
  .actions { display: flex; gap: 10px; margin-top: 16px; flex-wrap: wrap; }
  /* Rule list */
  .rule-item {
    background: var(--surface2);
    border: 1px solid var(--border);
    border-radius: 10px;
    padding: 14px 16px;
    margin-bottom: 10px;
    position: relative;
  }
  .rule-item .rule-meta { font-size: .78rem; color: var(--muted); margin-top: 6px; }
  .rule-item .delete-btn {
    position: absolute; top: 12px; right: 12px;
  }
  .badge {
    display: inline-block; padding: 2px 8px; border-radius: 20px;
    font-size: .72rem; font-weight: 600;
  }
  .badge-rule { background: rgba(108,99,255,.25); color: var(--accent); }
  .badge-pri1 { background: rgba(56,217,169,.15); color: var(--accent2); }
  .badge-pri2 { background: rgba(255,184,0,.15);  color: #ffd43b; }
  .badge-pri3 { background: rgba(255,107,107,.2); color: var(--danger); }
  /* Status panel */
  .actuator-grid { display: grid; grid-template-columns: repeat(auto-fill,minmax(160px,1fr)); gap: 12px; }
  .actuator-card {
    background: var(--surface2); border: 1px solid var(--border);
    border-radius: 10px; padding: 14px; text-align: center;
  }
  .actuator-card .name { font-size: .85rem; font-weight: 600; margin-bottom: 8px; }
  .actuator-card .state {
    font-size: 1.3rem; font-weight: 700;
    color: var(--danger);
  }
  .actuator-card .state.on { color: var(--accent2); }
  .actuator-card .ctl { display: flex; gap: 6px; margin-top: 10px; justify-content: center; }
  /* Toast */
  #toast {
    position: fixed; bottom: 24px; right: 24px;
    background: var(--surface); border: 1px solid var(--border);
    border-radius: 10px; padding: 12px 20px;
    font-size: .88rem; color: var(--text);
    opacity: 0; transition: opacity .3s;
    pointer-events: none; z-index: 999;
  }
  #toast.show { opacity: 1; }
  .cond-block { background: var(--bg); border: 1px solid var(--border); border-radius: 8px; padding: 12px; margin-top: 10px; }
  .cond-block label { color: var(--muted); }
</style>
<link rel="preconnect" href="https://fonts.googleapis.com">
<link href="https://fonts.googleapis.com/css2?family=Inter:wght@400;600;700&display=swap" rel="stylesheet">
</head>
<body>

<h1>⚡ Reglas de Automatización</h1>
<p class="subtitle">Configura condiciones de sensores para activar actuadores automáticamente.</p>

<!-- Actuator status panel -->
<div class="card">
  <h2>🔌 Estado de Actuadores</h2>
  <div class="actuator-grid" id="actuatorGrid">
    <p style="color:var(--muted);font-size:.85rem">Cargando...</p>
  </div>
</div>

<!-- Rule list -->
<div class="card">
  <h2>📋 Reglas Activas</h2>
  <div id="ruleList"><p style="color:var(--muted);font-size:.85rem">Cargando...</p></div>
</div>

<!-- Rule builder -->
<div class="card">
  <h2>➕ Nueva Regla</h2>

  <div class="cond-block">
    <label>Tipo de expresión</label>
    <select id="exprType" onchange="toggleExprType()">
      <option value="LEAF">Condición simple (LEAF)</option>
      <option value="AND">AND — ambas condiciones</option>
      <option value="OR">OR — cualquier condición</option>
    </select>

    <!-- LEAF -->
    <div id="leafSection">
      <div class="row">
        <div>
          <label>Device ID (0=local)</label>
          <input type="number" id="leafDevice" value="0" min="0" max="255">
        </div>
        <div>
          <label>Sensor ID</label>
          <input type="number" id="leafSensor" value="0" min="0" max="65535">
        </div>
        <div>
          <label>Variable (0=Temp)</label>
          <input type="number" id="leafVariable" value="0" min="0" max="255">
        </div>
        <div>
          <label>Operador</label>
          <select id="leafOp">
            <option value="GT">GT (&gt;)</option>
            <option value="LT">LT (&lt;)</option>
            <option value="GTE">GTE (&ge;)</option>
            <option value="LTE">LTE (&le;)</option>
            <option value="EQ">EQ (=)</option>
          </select>
        </div>
        <div>
          <label>Umbral</label>
          <input type="number" id="leafValue" value="30" step="0.1">
        </div>
      </div>
    </div>

    <!-- AND / OR -->
    <div id="compoundSection" style="display:none">
      <p style="color:var(--muted);font-size:.8rem;margin-top:10px">
        Condición IZQUIERDA
      </p>
      <div class="row">
        <div><label>Device</label><input type="number" id="leftDevice" value="0" min="0" max="255"></div>
        <div><label>Sensor</label><input type="number" id="leftSensor" value="0" min="0" max="65535"></div>
        <div><label>Var</label><input type="number" id="leftVariable" value="0" min="0" max="255"></div>
        <div><label>Op</label>
          <select id="leftOp">
            <option>GT</option><option>LT</option><option>GTE</option><option>LTE</option><option>EQ</option>
          </select>
        </div>
        <div><label>Umbral</label><input type="number" id="leftValue" value="30" step="0.1"></div>
      </div>
      <p style="color:var(--muted);font-size:.8rem;margin-top:10px">Condición DERECHA</p>
      <div class="row">
        <div><label>Device</label><input type="number" id="rightDevice" value="0" min="0" max="255"></div>
        <div><label>Sensor</label><input type="number" id="rightSensor" value="0" min="0" max="65535"></div>
        <div><label>Var</label><input type="number" id="rightVariable" value="0" min="0" max="255"></div>
        <div><label>Op</label>
          <select id="rightOp">
            <option>GT</option><option>LT</option><option>GTE</option><option>LTE</option><option>EQ</option>
          </select>
        </div>
        <div><label>Umbral</label><input type="number" id="rightValue" value="30" step="0.1"></div>
      </div>
    </div>
  </div>

  <div class="row" style="margin-top:16px">
    <div>
      <label>ID Actuador</label>
      <input type="number" id="ruleActuator" value="0" min="0" max="255">
    </div>
    <div>
      <label>Estado al activar</label>
      <select id="ruleState">
        <option value="true">ON (true)</option>
        <option value="false">OFF (false)</option>
      </select>
    </div>
    <div>
      <label>Prioridad</label>
      <select id="rulePriority">
        <option value="1">1 — Timer</option>
        <option value="2" selected>2 — Regla</option>
        <option value="3">3 — Manual</option>
      </select>
    </div>
    <div>
      <label>Duración (ms, 0=indefinido)</label>
      <input type="number" id="ruleDuration" value="0" min="0">
    </div>
  </div>

  <div class="actions">
    <button class="btn btn-primary" onclick="addRule()">💾 Agregar Regla</button>
    <button class="btn btn-success" onclick="saveRules()">☁️ Guardar en dispositivo</button>
  </div>

  <!-- REFERENCE PANEL -->
  <div style="margin-top: 20px; padding: 15px; background: var(--bg-card); border-radius: 8px;">
    <h3 style="margin-top:0; font-size:1.1rem; color:var(--primary)">🔑 Referencia de IDs (En vivo)</h3>
    <button class="btn btn-sm" style="margin-bottom:10px" onclick="loadReferencePanel()">🔄 Actualizar Referencias</button>
    <div id="refPanelContent" style="font-size:0.85rem; max-height: 200px; overflow-y: auto; background:#111; padding:10px; border-radius:4px; font-family:monospace;">
      Cargando referencias...
    </div>
  </div>
</div>

<div id="toast">✅ Operación exitosa</div>

<script>
let currentRules = [];

function toast(msg, ms=2500) {
  const t = document.getElementById('toast');
  t.textContent = msg;
  t.classList.add('show');
  setTimeout(() => t.classList.remove('show'), ms);
}

function toggleExprType() {
  const v = document.getElementById('exprType').value;
  document.getElementById('leafSection').style.display = (v === 'LEAF') ? '' : 'none';
  document.getElementById('compoundSection').style.display = (v !== 'LEAF') ? '' : 'none';
}

function priLabel(p) {
  return ['','Timer','Regla','Manual'][p] || p;
}
function priBadge(p) {
  return `<span class="badge badge-pri${p}">${priLabel(p)}</span>`;
}

function exprSummary(expr) {
  if (!expr) return '?';
  const op = expr.op || 'LEAF';
  if (op === 'AND' || op === 'OR') {
    return `(${exprSummary(expr.left)} ${op} ${exprSummary(expr.right)})`;
  }
  const s = expr.sensor || {};
  return `[D${s.device||0}:S${s.id||0}:V${s.var||0}] ${expr.cond||'?'} ${expr.value}`;
}

async function loadRules() {
  try {
    const r = await fetch('/rules');
    const d = await r.json();
    currentRules = d.rules || [];
  } catch(e) { currentRules = []; }
  renderRules();
}

async function loadReferencePanel() {
  const el = document.getElementById('refPanelContent');
  el.innerHTML = "Cargando...";
  try {
    const r = await fetch('/actual');
    const d = await r.json();
    let html = "<div style='display:flex; gap:20px; flex-wrap:wrap;'>";
    
    // Sensores
    html += "<div style='flex:1; min-width:200px'><b>📡 Sensores</b><br>";
    if (d.sensors && d.sensors.length > 0) {
      d.sensors.forEach(s => {
        if(s.readings) {
          s.readings.forEach(rd => {
            if(rd.key_device !== undefined) {
              html += `<span style="color:#0f0">Device:</span> ${rd.key_device} | <span style="color:#0ff">Sensor:</span> ${rd.key_sensor} | <span style="color:#f0f">Var:</span> ${rd.key_var} &rarr; ${rd.label} (${rd.value} ${rd.unit})<br>`;
            }
          });
        }
      });
    } else {
      html += "Ninguno<br>";
    }
    html += "</div>";

    // Actuadores
    html += "<div style='flex:1; min-width:200px'><b>⚡ Actuadores</b><br>";
    if (d.actuators && d.actuators.length > 0) {
      d.actuators.forEach(a => {
         html += `<span style="color:#ff0">ID:</span> ${a.id} &rarr; ${a.name} (Estado: ${a.state})<br>`;
      });
    } else {
      html += "Ninguno<br>";
    }
    html += "</div></div>";

    el.innerHTML = html;
  } catch(e) {
    el.innerHTML = "<span style='color:red'>Error cargando referencias</span>";
  }
}

window.addEventListener('load', () => {
  loadRules();
  loadReferencePanel();
});

function renderRules() {
  const el = document.getElementById('ruleList');
  if (!currentRules.length) {
    el.innerHTML = '<p style="color:var(--muted);font-size:.85rem">Sin reglas. Crea una con el formulario.</p>';
    return;
  }
  el.innerHTML = currentRules.map((r, i) => `
    <div class="rule-item">
      <span class="badge badge-rule">Regla ${i+1}</span>
      ${priBadge(r.priority||2)}
      <span style="font-size:.82rem;margin-left:8px">Actuador <b>${r.actuator}</b> → <b>${r.state ? 'ON':'OFF'}</b>
        ${r.duration_ms ? `(${r.duration_ms}ms)` : '(hold)'}
      </span>
      <div class="rule-meta">Condición: <code>${exprSummary(r.expr)}</code></div>
      <button class="btn btn-danger btn-sm delete-btn" onclick="deleteRule(${i})">🗑</button>
    </div>
  `).join('');
}

function deleteRule(idx) {
  currentRules.splice(idx, 1);
  renderRules();
  toast('🗑 Regla eliminada (guarda para aplicar)');
}

function buildLeafExpr(device, sensor, variable, op, value) {
  return { sensor: { device: parseInt(device), id: parseInt(sensor), var: parseInt(variable) }, cond: op, value: parseFloat(value) };
}

function addRule() {
  const exprType = document.getElementById('exprType').value;
  let expr;

  if (exprType === 'LEAF') {
    expr = buildLeafExpr(
      document.getElementById('leafDevice').value,
      document.getElementById('leafSensor').value,
      document.getElementById('leafVariable').value,
      document.getElementById('leafOp').value,
      document.getElementById('leafValue').value
    );
  } else {
    const left  = buildLeafExpr(
      document.getElementById('leftDevice').value,
      document.getElementById('leftSensor').value,
      document.getElementById('leftVariable').value,
      document.getElementById('leftOp').value,
      document.getElementById('leftValue').value
    );
    const right = buildLeafExpr(
      document.getElementById('rightDevice').value,
      document.getElementById('rightSensor').value,
      document.getElementById('rightVariable').value,
      document.getElementById('rightOp').value,
      document.getElementById('rightValue').value
    );
    expr = { op: exprType, left, right };
  }

  const rule = {
    expr,
    actuator:    parseInt(document.getElementById('ruleActuator').value),
    state:       document.getElementById('ruleState').value === 'true',
    priority:    parseInt(document.getElementById('rulePriority').value),
    duration_ms: parseInt(document.getElementById('ruleDuration').value)
  };

  currentRules.push(rule);
  renderRules();
  toast('✅ Regla agregada (guarda para aplicar)');
}

async function saveRules() {
  const payload = JSON.stringify({ rules: currentRules });
  try {
    const r = await fetch('/rules/save', {
      method: 'POST',
      headers: { 'Content-Type': 'application/json' },
      body: payload
    });
    const txt = await r.text();
    toast(r.ok ? '✅ Reglas guardadas y aplicadas' : '❌ Error: ' + txt);
  } catch(e) { toast('❌ Error de conexión'); }
}

async function loadActuators() {
  try {
    const r = await fetch('/actuator/status');
    const d = await r.json();
    renderActuators(d);
  } catch(e) {
    document.getElementById('actuatorGrid').innerHTML =
      '<p style="color:var(--muted);font-size:.85rem">No disponible.</p>';
  }
}

function renderActuators(list) {
  const g = document.getElementById('actuatorGrid');
  if (!list || !list.length) {
    g.innerHTML = '<p style="color:var(--muted);font-size:.85rem">Sin actuadores registrados.</p>';
    return;
  }
  g.innerHTML = list.map(a => `
    <div class="actuator-card">
      <div class="name">${a.name || 'Act '+a.id}</div>
      <div class="state ${a.state ? 'on':''}">
        ${a.state ? '● ON' : '○ OFF'}
      </div>
      <div class="ctl">
        <button class="btn btn-success btn-sm" onclick="sendCmd(${a.id},true,0,3)">ON</button>
        <button class="btn btn-danger  btn-sm" onclick="sendCmd(${a.id},false,0,3)">OFF</button>
        <button class="btn btn-primary btn-sm" onclick="sendCmd(${a.id},false,0,0)">AUTO</button>
      </div>
    </div>
  `).join('');
}

async function sendCmd(id, state, duration, priority = 3) {
  try {
    const r = await fetch('/actuator/command', {
      method: 'POST',
      headers: { 'Content-Type': 'application/json' },
      body: JSON.stringify({ id, state, duration, priority })
    });
    toast(r.ok ? `✅ Actuador ${id} → ${state?'ON':'OFF'}` : '❌ Error');
    setTimeout(loadActuators, 500);
  } catch(e) { toast('❌ Error de conexión'); }
}

// Init
loadRules();
loadActuators();
setInterval(loadActuators, 5000);
</script>
</body>
</html>
)rawliteral";

#endif // RULES_HTML_H
